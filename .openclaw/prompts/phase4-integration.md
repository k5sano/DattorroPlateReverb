# Phase 4: プロセッサ統合

## 参照ファイル
- `.copilot/handoff/design.md` — PluginProcessor セクション、processBlock フロー

## タスク

`Source/PluginProcessor.cpp` を完全実装する。

### prepareToPlay
1. APVTS から oversampling パラメータを読む
2. juce::dsp::Oversampling<float> を生成
   - チャンネル数: 2
   - factorIndex: 0(off), 1(2x), 2(4x)
   - filterHalfBandPolyphaseIIR, maxQuality
3. oversampling.initProcessing(samplesPerBlock)
4. effectiveSampleRate = sampleRate * oversampling.getOversamplingFactor()
5. reverbEngine_.prepare(effectiveSampleRate, factor)
6. 全 SmoothedValue を reset(sampleRate, 0.02) — 20ms スムージング

### processBlock
1. APVTS パラメータ読み出し → SmoothedValue にセット
2. 入力をモノにダウンミックス (L+R)/2 → monoBuffer
3. juce::dsp::AudioBlock でラップ
4. auto oversampledBlock = oversampling_.processSamplesUp(block)
5. オーバーサンプリングされたブロック内でサンプルごとに:
   - SmoothedValue から現在値取得 → reverbEngine_ にセット
   - reverbEngine_.process() を呼ぶ
6. oversampling_.processSamplesDown(block)
7. Dry/Wet ミックス: out = dry*input + wet*reverb
8. 出力バッファの L/R に書き込み

### 注意事項
- processBlock 内で new / malloc / push_back 禁止
- oversampling_ はメンバ変数として保持（unique_ptr<Oversampling<float>>）
- oversampling 切り替え時は prepareToPlay 再呼び出しで処理
- monoBuffer は prepareToPlay で確保

## 完了条件

```bash
cmake --build build/
find build/ -name "*.vst3" | head -1
```

ビルドが通り、build/ 内に VST3 バイナリが生成されていること。
