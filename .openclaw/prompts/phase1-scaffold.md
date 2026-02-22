# Phase 1: スケルトン作成

## 参照ファイル

まず以下を読んでください:
- `.copilot/handoff/spec.md` — 仕様
- `.copilot/handoff/design.md` — 設計

## タスク

### 1. プロジェクトルートに `CMakeLists.txt` を作成する
- design.md のテンプレートに従う
- JUCE 7 を FetchContent で取得
- VST3 + Standalone フォーマット
- juce_audio_utils + juce_dsp をリンク

### 2. `Source/Parameters.h` を作成する
- spec.md の全パラメータを APVTS 用に定義
- createParameterLayout() を static メンバとして実装
- パラメータ ID は spec.md の通り (小文字スネークケース)
- oversampling は juce::AudioParameterChoice で Off/2x/4x

### 3. `Source/PluginProcessor.h` / `Source/PluginProcessor.cpp` を作成する
- APVTS を Parameters.h の layout で初期化
- processBlock は空 (入力をそのまま出力にコピー)
- prepareToPlay / releaseResources は空実装

### 4. `Source/PluginEditor.h` / `Source/PluginEditor.cpp` を作成する
- juce::GenericAudioProcessorEditor を使用
- サイズ: 400 x 600

## 完了条件

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build/
```

このコマンドがエラーなしで完了すること。

## 注意

- JUCE の FetchContent は時間がかかる。タイムアウトしないよう注意
- JucePlugin_Code は "Dplt" (4文字)
- COMPANY_NAME は spec.md に合わせる
