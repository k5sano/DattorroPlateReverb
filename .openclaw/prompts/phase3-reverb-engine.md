# Phase 3: リバーブエンジン

## 参照ファイル
- `.copilot/handoff/spec.md` — アルゴリズム構造定義、ディレイ長テーブル、出力タップ構造
- `.copilot/handoff/design.md` — DattorroReverb クラス詳細

## タスク

`Source/DSP/DattorroReverb.h` と `Source/DSP/DattorroReverb.cpp` を作成する。

### 構造 (spec.md の通り厳密に)

**入力セクション:**
- Bandwidth LPF (OnePoleLPF)
- Pre-delay (DelayLine)
- Input Diffuser 1-4 (AllpassFilter × 4)

**タンク (Figure-8):**
- Left: DecayDiffuser1(mod) → DelayLine1 → DampLPF_L → ×decay → DecayDiffuser2 → DelayLine2 → ×decay
- Right: DecayDiffuser3(mod) → DelayLine3 → DampLPF_R → ×decay → DecayDiffuser4 → DelayLine4 → ×decay
- クロスフィードバック: Left 末尾 → Right 入力に加算、Right 末尾 → Left 入力に加算

**ディレイ長 (Fs=29761 基準、スケーリング必須):**
```
Input Diffuser: 142, 107, 379, 277
Tank Diffuser: 672, 908, 1800, 2656
Tank Delay: 4453, 4217, 3720, 3163
```

**出力タップ (spec.md Table 2 を正確に実装):**
```
YL = + DelayLine3[266] + DelayLine3[2974] - DecayDiff4[1913]
     + DelayLine4[1996] - DelayLine1[1990] - DecayDiff2[187]
     - DelayLine2[1066]
全タップ ×0.6

YR = + DelayLine1[353] + DelayLine1[3627] - DecayDiff2[1228]
     + DelayLine2[2673] - DelayLine3[2111] - DecayDiff4[335]
     - DelayLine4[121]
全タップ ×0.6
```

**モジュレーション:**
- QuadratureLFO で sin/cos を生成
- DecayDiffuser1 に cos、DecayDiffuser3 に sin
- readCubic で cubic 補間読み出し

**スケーリング:**
```cpp
float scaleDelay(int original) const {
  return float(original) * float(sampleRate_ / 29761.0) * sizeFactor_;
}
```

### メソッド
- prepare(double sampleRate, int oversampleFactor) — 全バッファ確保
- reset() — 全バッファゼロクリア
- process(const float* in, float* outL, float* outR, int numSamples) — メイン処理
- 各パラメータの setter

## 完了条件

```bash
cmake --build build/
```

ビルドが通ること。
