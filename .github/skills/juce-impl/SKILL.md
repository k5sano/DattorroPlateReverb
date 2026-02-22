---
name: juce-impl
description: "JUCE プラグイン実装時のコーディングルールとベストプラクティス"
user-invokable: true
disable-model-invocation: false
---

# JUCE 実装ルール

## コーディング規約

- C++17 以上を使用
- JUCE のコーディングスタイルに従う（camelCase、juce:: namespace）
- ヘッダーには `#pragma once` を使用
- AudioProcessor と AudioProcessorEditor を分離する

## パラメータ管理

- `juce::AudioProcessorValueTreeState` (APVTS) を必ず使用する
- パラメータ ID は小文字スネークケース（例: `gain_db`, `bypass`）
- レンジとデフォルト値は spec.md の指定に従う

## オーディオ処理

- `processBlock` 内でメモリアロケーションしない
- バイパス機能は `processBlockBypassed` をオーバーライドする
- サンプルレート変更は `prepareToPlay` で処理する
- バッファサイズの仮定をしない

## ビルド設定

- CMake を使用する（Projucer 不可）
- `CMakeLists.txt` は `juce_add_plugin` を使用する
- ビルドタイプ: Debug と Release
- 出力フォーマット: VST3, AU（macOS のみ）, Standalone

## ファイル構成

```
Source/
├── PluginProcessor.h
├── PluginProcessor.cpp
├── PluginEditor.h
├── PluginEditor.cpp
└── Parameters.h （パラメータ定義）
```
