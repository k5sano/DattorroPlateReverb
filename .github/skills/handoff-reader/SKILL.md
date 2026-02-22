---
name: handoff-reader
description: ".copilot/handoff/ ディレクトリからハンドオフファイルを読み込み、実装指示を解析する"
argument-hint: "handoff ディレクトリのパス（省略時: .copilot/handoff/）"
user-invokable: true
---

# Handoff Reader スキル

## 手順

1. `.copilot/handoff/` ディレクトリ内のファイル一覧を取得する
2. 以下の優先順でファイルを読み込む:
   - `copilot-handoff.md` （最優先：Copilot への直接指示）
   - `spec.md` （仕様）
   - `design.md` （設計）
   - `scaffold.md` （スケルトン定義）
3. 各ファイルの内容を解析し、以下を抽出する:
   - 実装すべき機能の一覧
   - 使用する JUCE モジュール
   - ファイル構成
   - ビルド手順
   - テスト要件
4. 抽出結果をまとめてユーザーに報告する

## 出力フォーマット

📋 ハンドオフ解析結果

機能: [機能一覧]
JUCE モジュール: [モジュール一覧]
ファイル構成: [ファイル一覧]
ビルド: [ビルドコマンド]
テスト: [テスト要件]
注意事項: [特記事項]

## エラー処理

- `.copilot/handoff/` が空の場合: 「ハンドオフファイルがありません」と報告
- ファイルの形式が不正な場合: 読めた部分だけ報告し、問題箇所を明示
