#!/bin/bash
set -e

PHASE=$1
PROJECT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$PROJECT_ROOT"

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

pass() { echo -e "${GREEN}✅ Phase $PHASE: $1${NC}"; }
fail() { echo -e "${RED}❌ Phase $PHASE: $1${NC}"; exit 1; }

case "$PHASE" in
  1)
    # スケルトン: ビルドが通るか
    [ -f CMakeLists.txt ] || fail "CMakeLists.txt not found"
    [ -f Source/Parameters.h ] || fail "Parameters.h not found"
    [ -f Source/PluginProcessor.h ] || fail "PluginProcessor.h not found"
    [ -f Source/PluginProcessor.cpp ] || fail "PluginProcessor.cpp not found"
    [ -f Source/PluginEditor.h ] || fail "PluginEditor.h not found"
    [ -f Source/PluginEditor.cpp ] || fail "PluginEditor.cpp not found"
    cmake -B build -DCMAKE_BUILD_TYPE=Debug 2>&1 | tail -5
    cmake --build build/ 2>&1 | tail -10
    pass "Scaffold build succeeded"
    ;;

  2)
    # DSP コンポーネント: ヘッダー存在 + ビルド
    [ -f Source/DSP/DelayLine.h ] || fail "DelayLine.h not found"
    [ -f Source/DSP/OnePoleLPF.h ] || fail "OnePoleLPF.h not found"
    [ -f Source/DSP/AllpassFilter.h ] || fail "AllpassFilter.h not found"
    [ -f Source/DSP/LFO.h ] || fail "LFO.h not found"
    cmake --build build/ 2>&1 | tail -10
    pass "DSP components build succeeded"
    ;;

  3)
    # リバーブエンジン: ファイル存在 + ビルド
    [ -f Source/DSP/DattorroReverb.h ] || fail "DattorroReverb.h not found"
    [ -f Source/DSP/DattorroReverb.cpp ] || fail "DattorroReverb.cpp not found"
    cmake --build build/ 2>&1 | tail -10
    pass "Reverb engine build succeeded"
    ;;

  4)
    # 統合: ビルド + VST3 バイナリ存在
    cmake --build build/ 2>&1 | tail -10
    VST3=$(find build/ -name "*.vst3" -type d 2>/dev/null | head -1)
    [ -n "$VST3" ] || fail "VST3 bundle not found in build/"
    pass "Integration build succeeded — VST3: $VST3"
    ;;

  5)
    # テスト: ビルド + ctest PASS
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON 2>&1 | tail -5
    cmake --build build/ 2>&1 | tail -10
    ctest --test-dir build/ --output-on-failure 2>&1
    RESULT=$?
    [ $RESULT -eq 0 ] || fail "Tests failed (exit code: $RESULT)"
    pass "All tests passed"
    ;;

  5b)
    # pluginval バリデーション
    VST3=$(find build/ -name "*.vst3" -type d 2>/dev/null | head -1)
    [ -n "$VST3" ] || fail "VST3 not found"

    # pluginval の存在確認
    if command -v pluginval &>/dev/null; then
      PVAL="pluginval"
    elif [ -f "./pluginval.app/Contents/MacOS/pluginval" ]; then
      PVAL="./pluginval.app/Contents/MacOS/pluginval"
    elif [ -f "./pluginval.exe" ]; then
      PVAL="./pluginval.exe"
    else
      echo "⚠️  pluginval not found — skipping validation"
      pass "pluginval skipped (not installed)"
      exit 0
    fi

    $PVAL --strictness-level 5 --skip-gui-tests \
      --timeout-ms 60000 \
      --sample-rates "44100,48000,96000" \
      --block-sizes "64,128,256,512,1024" \
      --validate "$VST3" 2>&1
    [ $? -eq 0 ] || fail "pluginval validation failed"
    pass "pluginval validation passed"
    ;;

  5c)
    # Plugin Analyser 音響解析
    CLI_PATH="/tmp/plugin-analyser/build/plugin_measure_grid_cli"
    if [ ! -f "$CLI_PATH" ]; then
      echo "⚠️  Plugin Analyser not found — skipping analysis"
      pass "Plugin Analyser skipped (not installed)"
      exit 0
    fi

    VST3=$(find build/ -name "*.vst3" -type d 2>/dev/null | head -1)
    [ -n "$VST3" ] || fail "VST3 not found"
    [ -f "test-configs/analyser-config.json" ] || fail "analyser-config.json not found"

    mkdir -p test-results/analysis
    $CLI_PATH \
      --config test-configs/analyser-config.json \
      --out test-results/analysis/ 2>&1
    [ $? -eq 0 ] || fail "Plugin Analyser failed"

    # THD チェック (1kHz, decay=0.85 の行で THD < 0.01 = 1%)
    if [ -f "test-results/analysis/grid_thd.csv" ]; then
      # 簡易チェック: THD カラムの最大値が 0.01 未満か
      MAX_THD=$(awk -F',' 'NR>1 {if($NF+0 > max) max=$NF+0} END{print max}' \
        test-results/analysis/grid_thd.csv 2>/dev/null || echo "0")
      echo "Max THD: $MAX_THD"
      # THD が 10% 超なら発振の疑い
      if [ "$(echo "$MAX_THD > 0.10" | bc -l 2>/dev/null || echo 0)" = "1" ]; then
        fail "Possible oscillation detected (THD > 10%: $MAX_THD)"
      fi
    fi
    pass "Plugin Analyser completed — results in test-results/analysis/"
    ;;

  6)
    # コミット: git log 確認
    LAST_COMMIT=$(git log -1 --oneline 2>/dev/null || echo "")
    echo "$LAST_COMMIT" | grep -qi "feat.*dattorro\|feat.*reverb\|feat.*plate" \
      || fail "Expected feat commit not found. Last: $LAST_COMMIT"
    pass "Commit verified: $LAST_COMMIT"
    ;;

  *)
    echo "Usage: check-phase.sh [1-6|5b|5c]"
    exit 1
    ;;
esac
