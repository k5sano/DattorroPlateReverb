#!/bin/bash

PHASE=$1
TIMESTAMP=$(date +%Y%m%d-%H%M%S)
LOG_DIR=".openclaw/logs"

mkdir -p "$LOG_DIR"

# パターンマッチでプロンプトファイルを探す
PROMPT_FILE=$(ls .openclaw/prompts/phase${PHASE}-*.md 2>/dev/null | head -1)

if [ -z "$PROMPT_FILE" ]; then
  echo "❌ Prompt file not found for phase $PHASE"
  exit 1
fi

echo "=== Phase $PHASE: $(basename $PROMPT_FILE) ==="
echo "📄 Log: ${LOG_DIR}/phase${PHASE}-${TIMESTAMP}.log"
echo ""

# Use Opus for complex phases
MODEL_FLAG=""
if [[ "$PHASE" -ge 3 ]]; then
  MODEL_FLAG="--model opus"
  echo "🧠 Using Claude Opus 4.6"
fi

claude $MODEL_FLAG -p "$(cat "$PROMPT_FILE")" \
  2>&1 | tee "${LOG_DIR}/phase${PHASE}-${TIMESTAMP}.log"

echo ""
echo "=== Running check-phase.sh $PHASE ==="
bash .openclaw/scripts/check-phase.sh "$PHASE" 2>&1 | tee -a "${LOG_DIR}/phase${PHASE}-${TIMESTAMP}.log"
