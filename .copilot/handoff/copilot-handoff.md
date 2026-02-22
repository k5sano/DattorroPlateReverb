# Claude Code への実装指示

## ミッション

Dattorro プレートリバーブの JUCE プラグインを新規実装する。 `spec.md` と `design.md` に従い、ビルドが通り、テストが PASS する状態まで完成させること。

## 実装順序 (厳守)

### Phase 1: スケルトン作成

1. `CMakeLists.txt` を作成する (design.md のテンプレート通り)
2. `Source/Parameters.h` を作成 — 全パラメータ ID と createParameterLayout()
3. `Source/PluginProcessor.h/.cpp` を最小実装 (空の processBlock)
4. `Source/PluginEditor.h/.cpp` を GenericAudioProcessorEditor で作成
5. **ビルド確認**: `cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build/`

### Phase 2: DSP コンポーネント

6. `Source/DSP/DelayLine.h` を実装 — power-of-2 バッファ、cubic 補間
7. `Source/DSP/OnePoleLPF.h` を実装
8. `Source/DSP/AllpassFilter.h` を実装 — DelayLine を内蔵
9. `Source/DSP/LFO.h` を実装 — QuadratureLFO
10. **ビルド確認**

### Phase 3: リバーブエンジン

11. `Source/DSP/DattorroReverb.h/.cpp` を実装
    - prepare(): 全ディレイバッファを確保、ディレイ長スケーリング
    - reset(): 全バッファゼロクリア
    - process(): spec.md の信号フロー通りに実装
    - 出力タップ: spec.md Table 2 の通り正確に実装
12. **ビルド確認**

### Phase 4: プロセッサ統合

13. PluginProcessor に DattorroReverb と Oversampling を統合
    - prepareToPlay: Oversampling 初期化 → reverbEngine.prepare()
    - processBlock: パラメータ読み出し → mono sum → oversample up → reverb → oversample down → dry/wet mix
14. **ビルド確認**

### Phase 5: テスト

15. `Source/Tests/PluginTests.cpp` を作成
    - 全パラメータの ID 存在確認
    - パラメータのレンジ・デフォルト値検証
    - プラグインのインスタンス生成テスト

16. `Source/Tests/AudioProcessingTests.cpp` を作成
    - 無音入力 → 無音出力テスト (mix=0)
    - 無音入力 → 無音出力テスト (bypass)
    - サンプルレート 44100/48000/96000 で prepare が通ること
    - バッファサイズ 64/128/256/512/1024 で processBlock が通ること
    - decay=0 で数ブロック後にテイルが十分に減衰すること

17. **テスト実行**: `cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON && cmake --build build/ && ctest --test-dir build/`

### Phase 6: コミット

18. 全ビルド・テスト PASS を確認
19. `git add -A && git commit -m "feat: Dattorro plate reverb with oversampling" && git push`

## 重要な制約

- **各 Phase のビルド確認を飛ばさないこと**
- **spec.md のディレイ長・タップ位置の数値を改変しないこと**
- **processBlock 内で new / malloc / vector::push_back 等を呼ばないこと**
- **既存テストを削除・スキップしないこと**
- **3回ビルド修正しても通らない場合はロールバックしてレポートを書くこと**
- **オーバーサンプリングの on/off でクラッシュしないこと (prepareToPlay で再初期化)**

## デバッグのヒント

- `undefined reference to juce::` → target_link_libraries に juce::juce_dsp が抜けていないか
- `no matching function for call to 'Oversampling'` → テンプレート引数 <float> を確認
- タンクが発振する → decay_diffusion_1 の符号反転を確認 (spec.md: 負の係数)
- 出力が無音 → 出力タップの delayIndex が正しいか、mix が 0 になっていないか
- 出力が DC オフセット → OnePoleLPF の reset() が prepareToPlay で呼ばれているか

## 成果物チェックリスト

- [ ] CMakeLists.txt がある
- [ ] VST3 ビルドが通る (Debug)
- [ ] Standalone ビルドが通る (Debug)
- [ ] 全パラメータが APVTS に登録されている
- [ ] オーバーサンプリング Off/2x/4x が切り替えられる
- [ ] 全ユニットテストが PASS
- [ ] processBlock 内にヒープアロケーションがない
- [ ] git commit & push 済み
