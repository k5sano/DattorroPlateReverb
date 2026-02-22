#!/bin/bash
set -e

echo "=== Full Setup: OpenClaw + Claude Code JUCE Builder ==="

# --- Claude Code 側 ---
mkdir -p .github/{agents,skills/{handoff-reader,juce-impl,juce-testing,build-check,juce-builder},hooks}
mkdir -p .copilot/handoff
mkdir -p scaffold
mkdir -p scripts

# --- OpenClaw 側 ---
mkdir -p .openclaw/agents/juce-orchestrator/agent
mkdir -p .openclaw/agents/juce-worker/agent
mkdir -p .openclaw/prompts
mkdir -p .openclaw/scripts

# --- .gitkeep ---
touch .copilot/handoff/.gitkeep
touch scaffold/.gitkeep

# --- スクリプトに実行権限 ---
chmod +x scripts/fix-counter.sh 2>/dev/null || true
chmod +x .openclaw/scripts/*.sh 2>/dev/null || true
chmod +x setup-all.sh 2>/dev/null || true

echo ""
echo "=== Directory structure created ==="
echo ""
echo "次のステップ:"
echo "  1. .copilot/handoff/ に spec.md, design.md, copilot-handoff.md を配置"
echo "  2. 各ファイルを配置"
echo ""
echo "実行方法:"
echo "  A) OpenClaw 経由 (推奨):"
echo "     → OpenClaw チャットで 'Dattorro リバーブを実装して' と指示"
echo ""
echo "  B) 直接 Claude Code:"
echo "     → bash .openclaw/scripts/spawn-claude-code.sh 1"
echo "     → bash .openclaw/scripts/spawn-claude-code.sh 2"
echo "     → ... (Phase 1-6 を順に)"
echo ""
echo "  C) Claude Code 単発:"
echo "     → claude -p 'handoff ファイルを読み込んで実装してください'"
echo ""
echo "=== Setup complete ==="
