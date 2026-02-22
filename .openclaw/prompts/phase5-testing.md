# Phase 5: テスト

## 参照ファイル
- `.copilot/handoff/spec.md` — パラメータ一覧
- `.copilot/handoff/design.md` — テストファイル配置

## タスク

### 1. `Source/Tests/PluginTests.cpp`

JUCE の UnitTest フレームワークを使用:

```cpp
class DattorroPluginTests : public juce::UnitTest {
  DattorroPluginTests() : UnitTest("DattorroPlugin") {}
  void runTest() override {
    // テストケースをここに
  }
};
static DattorroPluginTests dattorroPluginTests;
```

テストケース:
- プラグインインスタンス生成が成功すること
- 全パラメータ ID が APVTS に存在すること
  (pre_delay, decay, damping, bandwidth, size, mix,
   input_diffusion_1, input_diffusion_2, decay_diffusion_1,
   decay_diffusion_2, mod_rate, mod_depth, oversampling)
- 各パラメータのデフォルト値が spec.md の通りであること
- 各パラメータのレンジが spec.md の通りであること

### 2. `Source/Tests/AudioProcessingTests.cpp`

テストケース:
- mix=0 で無音入力 → 入力がそのまま出力されること
- 無音入力を 10 ブロック処理してクラッシュしないこと
- sampleRate 44100/48000/96000 で prepareToPlay が通ること
- bufferSize 64/128/256/512/1024 で processBlock が通ること
- decay=0 で 20 ブロック処理後、出力が -60dB 以下に減衰すること
- oversampling Off/2x/4x 各設定で prepareToPlay → processBlock が通ること

### 3. main 関数

テストアプリの main:

```cpp
int main() {
  juce::UnitTestRunner runner;
  runner.runAllTests();
  int failures = 0;
  for (int i = 0; i < runner.getNumResults(); ++i)
    failures += runner.getResult(i)->failures;
  return failures > 0 ? 1 : 0;
}
```

## 完了条件

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build/
ctest --test-dir build/ --output-on-failure
```

全テストが PASS すること。
