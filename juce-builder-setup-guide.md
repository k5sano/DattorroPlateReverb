# JUCE Builder エージェント - セットアップガイド

Claude Code を使って JUCE オーディオプラグインを自動実装するための構成一式。

## ディレクトリ構成

```
project/
├── .github/
│   ├── agents/
│   │   └── juce-builder.agent.md      # カスタムエージェント定義
│   ├── hooks/
│   │   └── validation.hooks.json      # セッション・ツールフック
│   └── skills/
│       ├── handoff-reader/SKILL.md    # handoff ファイル読み込み
│       ├── juce-impl/SKILL.md         # JUCE 実装ルール
│       ├── juce-testing/SKILL.md      # テストルール
│       ├── juce-builder/SKILL.md      # メインエージェント
│       └── build-check/SKILL.md       # CMake ビルド確認
├── .copilot/
│   └── handoff/
│       ├── .gitkeep                   # 空ディレクトリ保持用
│       └── （spec.md, design.md 等）  # 実装指示ファイル
├── scripts/
│   └── fix-counter.sh                 # 修正回数カウンター
├── scaffold/
│   └── .gitkeep                       # スケルトン配置場所
└── setup-copilot.sh                   # 初回セットアップスクリプト
```

---

## 1. .github/agents/juce-builder.agent.md

```markdown
# juce-builder エージェント

あなたは JUCE オーディオプラグイン専門のビルド・実装エージェントです。

## 基本ルール

1. `.copilot/handoff/` ディレクトリのファイル（spec.md, design.md, copilot-handoff.md）を読み、指示に従って実装する
2. 修正は根拠必須。推測だけで変更しない
3. ビルドは `cmake --build build/` で確認する
4. テストは `ctest --test-dir build/` で実行する
5. 3回修正しても解決しない場合はロールバックし、レポートを作成する

## ワークフロー

1. `/handoff-reader` スキルで handoff ファイルを読み込む
2. `/juce-impl` スキルのルールに従って実装する
3. `/build-check` スキルでビルドを確認する
4. `/juce-testing` スキルのルールに従ってテストする
5. 成功したら `git add -A && git commit -m "feat: <内容>" && git push`

## 許可された操作

- cmake ビルド・テスト
- ソースファイルの編集・作成
- git add / commit / push
- MCP 経由の JUCE API 問い合わせ

## 禁止事項

- handoff に記載のない機能の追加
- 既存のテストを削除・無効化すること
- build/ ディレクトリ以外のバイナリ生成
```

---

## 2. .github/hooks/validation.hooks.json

```json
{
  "hooks": [
    {
      "event": "onSessionStart",
      "steps": [
        {
          "name": "check-handoff-exists",
          "command": "test -d .copilot/handoff && ls .copilot/handoff/*.md 2>/dev/null | head -5 || echo 'WARNING: No handoff files found'"
        }
      ]
    },
    {
      "event": "onToolCall",
      "tools": ["editFiles"],
      "steps": [
        {
          "name": "increment-fix-counter",
          "command": "bash scripts/fix-counter.sh increment"
        },
        {
          "name": "check-fix-limit",
          "command": "bash scripts/fix-counter.sh check"
        }
      ]
    }
  ]
}
```

---

## 3. .github/skills/juce-builder/SKILL.md

```markdown
---
name: juce-builder
description: "JUCE オーディオプラグイン専門のビルド・実装エージェント"
user-invokable: true
---

# juce-builder エージェント

あなたは JUCE オーディオプラグイン専門のビルド・実装エージェントです。

## 基本ルール

1. `.copilot/handoff/` ディレクトリのファイル（spec.md, design.md, copilot-handoff.md）を読み、指示に従って実装する
2. 修正は根拠必須。推測だけで変更しない
3. ビルドは `cmake --build build/` で確認する
4. テストは `ctest --test-dir build/` で実行する
5. 3回修正しても解決しない場合はロールバックし、レポートを作成する

## ワークフロー

1. `/handoff-reader` スキルで handoff ファイルを読み込む
2. `/juce-impl` スキルのルールに従って実装する
3. `/build-check` スキルでビルドを確認する
4. `/juce-testing` スキルのルールに従ってテストする
5. 成功したら `git add -A && git commit -m "feat: <内容>" && git push`

## 許可された操作

- cmake ビルド・テスト
- ソースファイルの編集・作成
- git add / commit / push
- MCP 経由の JUCE API 問い合わせ

## 禁止事項

- handoff に記載のない機能の追加
- 既存のテストを削除・無効化すること
- build/ ディレクトリ以外のバイナリ生成
```

---

## 4. .github/skills/handoff-reader/SKILL.md

```markdown
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
```

---

## 5. .github/skills/juce-impl/SKILL.md

```markdown
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

Source/
├── PluginProcessor.h
├── PluginProcessor.cpp
├── PluginEditor.h
├── PluginEditor.cpp
└── Parameters.h （パラメータ定義）
```

---

## 6. .github/skills/juce-testing/SKILL.md

```markdown
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

Tests/
├── PluginTests.cpp
└── AudioProcessingTests.cpp

## 失敗時の対応

1. エラーメッセージを記録
2. 該当コードを特定
3. 修正を試みる（最大3回）
4. 3回失敗したらロールバックしてレポート作成
```

---

## 7. .github/skills/build-check/SKILL.md

```markdown
---
name: build-check
description: "CMake ビルドを実行し結果を確認するスキル"
user-invokable: true
---

# ビルド確認スキル

## 手順

1. プロジェクトルートに `CMakeLists.txt` が存在するか確認
2. `build/` ディレクトリがなければ作成し、cmake configure を実行:
   cmake -B build -DCMAKE_BUILD_TYPE=Debug
3. ビルドを実行:
   cmake --build build/ --config Debug
4. ビルド結果を解析:
   - 成功: 「✅ ビルド成功」と報告
   - 警告あり: 警告一覧を報告し、修正を提案
   - 失敗: エラーメッセージを解析し、修正を試みる

## エラー解析ルール

- `undefined reference` → ヘッダーのインクルード漏れまたはリンク設定を確認
- `no matching function` → JUCE API のバージョン差異を確認
- `CMake Error` → CMakeLists.txt の記述を確認

## レポートフォーマット

🔨 ビルド結果

ステータス: [成功/警告あり/失敗]
所要時間: [秒]
警告数: [n]
エラー数: [n]
詳細: [エラーメッセージ]
推奨修正: [修正案]
```

---

## 8. scripts/fix-counter.sh

```bash
#!/bin/bash

COUNTER_FILE=".copilot/handoff/.fix-count"
MAX_FIXES=3

case "$1" in
  increment)
    if [ -f "$COUNTER_FILE" ]; then
      count=$(cat "$COUNTER_FILE")
      echo $((count + 1)) > "$COUNTER_FILE"
    else
      echo 1 > "$COUNTER_FILE"
    fi
    echo "Fix count: $(cat "$COUNTER_FILE")/$MAX_FIXES"
    ;;
  check)
    if [ -f "$COUNTER_FILE" ]; then
      count=$(cat "$COUNTER_FILE")
      if [ "$count" -ge "$MAX_FIXES" ]; then
        echo "ERROR: Fix limit reached ($count/$MAX_FIXES). Rolling back."
        echo "Please review the handoff files and try a different approach."
        exit 1
      fi
    fi
    echo "Fix count OK: $(cat "$COUNTER_FILE" 2>/dev/null || echo 0)/$MAX_FIXES"
    ;;
  reset)
    echo 0 > "$COUNTER_FILE"
    echo "Fix counter reset."
    ;;
  *)
    echo "Usage: fix-counter.sh [increment|check|reset]"
    ;;
esac
```

---

## 9. setup-copilot.sh（初回セットアップ用）

```bash
#!/bin/bash
set -e

echo "=== Copilot Setup Script ==="

# Create directories
mkdir -p .github/agents
mkdir -p .github/skills/handoff-reader
mkdir -p .github/skills/juce-impl
mkdir -p .github/skills/juce-testing
mkdir -p .github/skills/build-check
mkdir -p .github/skills/juce-builder
mkdir -p .github/hooks
mkdir -p .copilot/handoff
mkdir -p scaffold
mkdir -p scripts

# Create .gitkeep files
touch .copilot/handoff/.gitkeep
touch scaffold/.gitkeep

echo "=== Setup complete ==="
echo "Files created. Run 'git add -A && git commit' to record."
```

---

## 使い方

### 1. セットアップ

```bash
# プロジェクトディレクトリで setup-copilot.sh を実行
./setup-copilot.sh

# または手動でディレクトリ作成
mkdir -p .github/{agents,skills/{handoff-reader,juce-impl,juce-testing,build-check,juce-builder},hooks}
mkdir -p .copilot/handoff scaffold scripts
```

### 2. Handoff ファイルの作成

`.copilot/handoff/` に以下のファイルを配置：

- **spec.md** - プラグインの仕様（パラメータ、機能、レンジ等）
- **design.md** - 設計詳細（クラス構成、シグナルフロー等）
- **copilot-handoff.md** - Claude Code への直接指示

### 3. Claude Code で実行

```bash
# プロジェクトディレクトリで Claude Code を起動
claude

# またはエージェントを指定
claude --agent juce-builder

# ヘッドレスモード（自動化用）
claude -p "handoff ファイルを読み込んで実装してください"
```

### 4. ワークフロー

1. Claude Code が `.copilot/handoff/` を読み込む
2. spec.md/design.md に従って実装
3. `cmake --build build/` でビルド確認
4. `ctest --test-dir build/` でテスト実行
5. 成功したら `git commit && git push`
6. 3回修正しても失敗したらロールバック＆レポート作成

---

## Claude Code インストール・認証

```bash
# インストール
npm install -g @anthropic-ai/claude-code

# 認証（Claude Pro/Max サブスクが必要）
claude auth login

# 認証状態確認
claude auth status
```

---

## 注意事項

- 修正回数は3回まで（`scripts/fix-counter.sh` で管理）
- handoff に記載のない機能は追加禁止
- 既存のテストを削除・無効化してはいけない
- ビルド成果物は `build/` ディレクトリのみ
