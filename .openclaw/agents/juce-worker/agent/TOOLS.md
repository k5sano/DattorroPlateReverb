# 許可されたツール

## ファイル操作
- read: ソースファイル、handoff ファイルの読み取り
- write: 新規ファイルの作成
- edit: 既存ファイルの編集
- apply_patch: パッチの適用

## コマンド実行
- exec: cmake, ctest, git status, ls 等
- process: バックグラウンドプロセス

## 禁止事項
- browser: 不要
- canvas: 不要
- sessions_spawn: ワーカーはサブエージェントをスポーンしない
- cron: 不要
- gateway: 不要
