---
name: build-check
description: "CMake ビルドを実行し結果を確認するスキル"
user-invokable: true
---

# ビルド確認スキル

## 手順

1. プロジェクトルートに `CMakeLists.txt` が存在するか確認
2. `build/` ディレクトリがなければ作成し、cmake configure を実行:
   ```
   cmake -B build -DCMAKE_BUILD_TYPE=Debug
   ```
3. ビルドを実行:
   ```
   cmake --build build/ --config Debug
   ```
4. ビルド結果を解析:
   - 成功: 「✅ ビルド成功」と報告
   - 警告あり: 警告一覧を報告し、修正を提案
   - 失敗: エラーメッセージを解析し、修正を試みる

## エラー解析ルール

- `undefined reference` → ヘッダーのインクルード漏れまたはリンク設定を確認
- `no matching function` → JUCE API のバージョン差異を確認
- `CMake Error` → CMakeLists.txt の記述を確認

## レポートフォーマット

```
🔨 ビルド結果

ステータス: [成功/警告あり/失敗]
所要時間: [秒]
警告数: [n]
エラー数: [n]
詳細: [エラーメッセージ]
推奨修正: [修正案]
```
