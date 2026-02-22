# Phase 5c: Plugin Analyser 音響解析

## 事前準備

Plugin Analyser がインストールされていない場合:

```bash
# リポジトリクローン & ビルド
git clone https://github.com/lucaromagnoli/plugin-analyser.git /tmp/plugin-analyser
cd /tmp/plugin-analyser
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
cd -
```

## タスク

### 1. 解析設定ファイルを作成

`test-configs/analyser-config.json` を作成:

```json
{
  "pluginPath": "<VST3_PATH>",
  "sampleRate": 96000,
  "blockSize": 512,
  "durationSeconds": 5.0,
  "signalType": "sweep",
  "signalFrequency": 1000.0,
  "inputLevels": [-60, -40, -20, -12, -6, 0],
  "analyzers": [
    "LinearResponse",
    "Thd",
    "RmsPeak",
    "TransferCurve"
  ],
  "parameters": [
    { "name": "decay", "strategy": "ExplicitValues", "values": [0.0, 0.5, 0.85, 0.95, 0.999] },
    { "name": "damping", "strategy": "ExplicitValues", "values": [0.0, 0.3, 0.7, 1.0] },
    { "name": "mix", "strategy": "ExplicitValues", "values": [1.0] }
  ]
}
```

### 2. 解析実行

```bash
VST3_PATH=$(find build/ -name "*.vst3" -type d | head -1)

# config 内の <VST3_PATH> を置換
sed -i.bak "s|<VST3_PATH>|$VST3_PATH|g" test-configs/analyser-config.json

# 実行
mkdir -p test-results/analysis
/tmp/plugin-analyser/build/plugin_measure_grid_cli \
  --config test-configs/analyser-config.json \
  --out test-results/analysis/
```

### 3. 結果の検証

出力された CSV を読み、以下を確認:

**周波数応答 (grid_linear_response.csv):**
- damping=0 のとき: フラットに近い応答 (±3dB 以内 20Hz-20kHz)
- damping=1.0 のとき: 高域が明確にロールオフ (-6dB 以上 @ 10kHz)

**THD (grid_thd.csv):**
- decay=0.85, 1kHz 入力: THD < 1% (リバーブはリニアなので低い)
- 発振の兆候 (THD > 10%) がないこと

**ダイナミクス (grid_rms_peak.csv):**
- decay=0 のとき: 出力 RMS が入力より十分小さい (テイルが消えている)
- decay=0.999 のとき: 出力 RMS が入力に近い (テイルが長い)

**トランスファーカーブ (grid_transfer_curves.csv):**
- リニアなカーブ (リバーブはノンリニア処理なし)

### 4. 結果ファイルをコミット対象に含める

```bash
mkdir -p test-results/
# CSV を保存 (git で追跡)
git add test-configs/ test-results/
```

## 完了条件

- 全解析が正常終了 (exit code 0)
- THD < 1% (1kHz, decay=0.85)
- damping による高域ロールオフが確認できる
- decay=0 で信号が減衰する
- 発振の兆候がない

## Plugin Analyser がない場合

インストールできない場合はこの Phase をスキップして Phase 6 に進む。
結果レポートに「Plugin Analyser 未実行」と記載すること。
