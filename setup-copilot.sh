#!/bin/bash
set -e

echo "=== Copilot Setup Script ==="

# Create directories
mkdir -p .github/agents
mkdir -p .github/skills/handoff-reader
mkdir -p .github/skills/juce-impl
mkdir -p .github/skills/juce-testing
mkdir -p .github/skills/build-check
mkdir -p .github/hooks
mkdir -p .copilot/handoff
mkdir -p scaffold
mkdir -p scripts

# Create .gitkeep files
touch .copilot/handoff/.gitkeep
touch scaffold/.gitkeep

echo "=== Setup complete ==="
echo "Files created. Run 'git add -A && git commit' to record."
