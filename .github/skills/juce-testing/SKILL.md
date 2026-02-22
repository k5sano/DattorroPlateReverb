---
name: juce-testing
description: "JUCE プラグインのテストルールとテスト手順"
user-invokable: true
---

# JUCE テストルール

## テスト種別

### ビルドテスト
- `cmake --build build/` が警告なしで成功すること
- Debug と Release の両方でビルドが通ること

### ユニットテスト
- `ctest --test-dir build/` で全テストが PASS すること
- JUCE の `UnitTest` クラスを使用する
- 各パラメータのレンジ・デフォルト値を検証する

### オーディオテスト
- 無音入力 → 無音出力（ゲイン 0dB、バイパス時）
- サンプルレート 44100, 48000, 96000 で動作すること
- バッファサイズ 64, 128, 256, 512, 1024 で動作すること

## テストファイル配置

```
Tests/
├── PluginTests.cpp
└── AudioProcessingTests.cpp
```

## 失敗時の対応

1. エラーメッセージを記録
2. 該当コードを特定
3. 修正を試みる（最大3回）
4. 3回失敗したらロールバックしてレポート作成
