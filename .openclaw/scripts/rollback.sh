#!/bin/bash
set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$PROJECT_ROOT"

echo "=== Rollback ==="

# 未コミットの変更を全て取り消す
git checkout -- .
git clean -fd --exclude=.openclaw/ --exclude=.copilot/

# fix counter リセット
bash scripts/fix-counter.sh reset 2>/dev/null || true

echo "=== Rollback complete ==="
echo "Working tree restored to last commit."
git log -1 --oneline
