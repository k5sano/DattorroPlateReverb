# CLAUDE.md — Dattorro Plate Reverb

## プロジェクト概要

Dattorro (1997) プレートリバーブアルゴリズムの JUCE プラグイン実装。
オーバーサンプリング搭載。バイオリン向けロングディケイ。

## ビルド

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build/
ctest --test-dir build/ --output-on-failure
```

## 重要ファイル

- `.copilot/handoff/spec.md` — 完全な仕様（ディレイ長、タップ位置等）
- `.copilot/handoff/design.md` — クラス設計
- `Source/DSP/DattorroReverb.h` — リバーブエンジン本体

## ルール

- processBlock 内でヒープアロケーション禁止
- spec.md のディレイ長・タップ位置を改変しない
- 修正は根拠必須
- テストを削除・無効化しない

---

## MCP サーバー

以下を Claude Code の設定に追加:

```json
{
  "mcpServers": {
    "juce-docs": {
      "command": "node",
      "args": ["/path/to/juce-docs-mcp-server/dist/index.js"]
    }
  }
}
```

JUCE API が不明なときは以下のツールを使うこと:

- `/search-juce-classes Oversampling` — クラス検索
- `/get-juce-class-docs AudioProcessorValueTreeState` — ドキュメント取得
