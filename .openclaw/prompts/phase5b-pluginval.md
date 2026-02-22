# Phase 5b: pluginval バリデーション

## 事前準備

pluginval がインストールされていない場合:

### macOS
```bash
curl -L "https://github.com/Tracktion/pluginval/releases/latest/download/pluginval_macOS.zip" -o pluginval.zip
unzip pluginval.zip
chmod +x pluginval.app/Contents/MacOS/pluginval
```

### Windows
```bash
curl -L "https://github.com/Tracktion/pluginval/releases/latest/download/pluginval_Windows.zip" -o pluginval.zip
unzip pluginval.zip
```

## タスク

### 1. VST3 バンドルのパスを特定

```bash
VST3_PATH=$(find build/ -name "*.vst3" -type d | head -1)
echo "VST3: $VST3_PATH"
```

### 2. pluginval を実行 (strictness level 5)

```bash
# macOS
./pluginval.app/Contents/MacOS/pluginval \
  --strictness-level 5 \
  --skip-gui-tests \
  --timeout-ms 60000 \
  --sample-rates "44100,48000,96000" \
  --block-sizes "64,128,256,512,1024" \
  --validate "$VST3_PATH"

# Windows
./pluginval.exe \
  --strictness-level 5 \
  --skip-gui-tests \
  --timeout-ms 60000 \
  --sample-rates "44100,48000,96000" \
  --block-sizes "64,128,256,512,1024" \
  --validate "$VST3_PATH"
```

### 3. 結果を解析

- exit code 0 → PASS
- exit code 1 → FAIL → エラーログを読んで修正

## pluginval が検証する内容 (level 5)

- プラグインの正常ロード/アンロード
- パラメータの列挙・設定・取得
- prepareToPlay / releaseResources のサイクル
- processBlock の各サンプルレート・バッファサイズ組み合わせ
- ステート保存/復元の一貫性
- エディタ生成/破棄 (--skip-gui-tests 時はスキップ)
- NaN / Inf / サブノーマル値の検出

## 完了条件

pluginval が exit code 0 で終了すること。
