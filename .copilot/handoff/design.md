# DattorroPlateReverb – 設計書

## クラス構成

```
Source/
├── PluginProcessor.h / .cpp   ← AudioProcessor 本体
├── PluginEditor.h / .cpp      ← GUI (最小限: Generic Editor でも可)
├── Parameters.h               ← パラメータ ID / レイアウト定義
├── DSP/
│   ├── DattorroReverb.h / .cpp  ← リバーブエンジン本体
│   ├── AllpassFilter.h          ← ラティス型オールパスフィルタ
│   ├── DelayLine.h              ← 補間付きディレイライン
│   ├── OnePoleLPF.h             ← 1次ローパスフィルタ
│   └── LFO.h                    ← 直交 LFO (sin/cos)
└── Tests/
    ├── PluginTests.cpp          ← パラメータ・基本テスト
    └── AudioProcessingTests.cpp ← オーディオ処理テスト
```

## クラス詳細

### 1. DelayLine (テンプレート)

```cpp
// DSP/DelayLine.h
template <typename SampleType>
class DelayLine {
public:
    void setMaxDelay(int maxSamples);
    void setDelay(int samples);
    void reset();
    void write(SampleType sample);
    SampleType read() const;              // 末尾読み出し
    SampleType readAt(int offset) const;  // 任意位置読み出し
    SampleType readCubic(SampleType fractional) const;  // cubic 補間読み出し

private:
    std::vector<SampleType> buffer_;
    int writeIndex_ = 0;
    int delayLength_ = 0;
    int mask_ = 0;  // power-of-2 マスク
};
```

- バッファサイズは2のべき乗に切り上げ、ビットマスクでラップ
- readCubic は Niemitalo の 4-point cubic 補間

### 2. AllpassFilter

```cpp
// DSP/AllpassFilter.h
template <typename SampleType>
class AllpassFilter {
public:
    void prepare(int delaySamples);
    void reset();
    SampleType process(SampleType input, SampleType coefficient);
    SampleType readAt(int offset) const;    // 出力タップ用
    SampleType readCubic(SampleType frac) const;  // モジュレーション用

private:
    DelayLine<SampleType> delay_;
};
```

- ラティス構造: `output = -coeff * input + delay_out; delay_in = input + coeff * delay_out`
- 符号反転は呼び出し側で coefficient の符号を変えて制御

### 3. OnePoleLPF

```cpp
// DSP/OnePoleLPF.h
template <typename SampleType>
class OnePoleLPF {
public:
    void setCoefficient(SampleType coeff);
    SampleType process(SampleType input);
    void reset();

private:
    SampleType z1_ = 0;
    SampleType coeff_ = 0;
};
```

- `y[n] = y[n-1] + coeff * (x[n] - y[n-1])`
- bandwidth フィルタ: coeff = bandwidth パラメータ
- damping フィルタ: coeff = 1.0 - damping パラメータ

### 4. LFO (Quadrature Oscillator)

```cpp
// DSP/LFO.h
template <typename SampleType>
class QuadratureLFO {
public:
    void prepare(SampleType sampleRate);
    void setFrequency(SampleType hz);

    struct Output {
        SampleType sin;
        SampleType cos;
    };

    Output tick();
    void reset();

private:
    SampleType phase_ = 0;
    SampleType phaseInc_ = 0;
};
```

- 位相アキュムレータ方式
- sin/cos を同時出力し、タンク左右で直交変調

### 5. DattorroReverb (エンジン本体)

```cpp
// DSP/DattorroReverb.h
class DattorroReverb {
public:
    void prepare(double sampleRate, int oversampleFactor);
    void reset();
    void process(const float* input, float* outputL, float* outputR, int numSamples);

    // パラメータセッター (スムージングなし — 呼び出し側で処理)
    void setPreDelay(float ms);
    void setDecay(float value);
    void setDamping(float value);
    void setBandwidth(float value);
    void setInputDiffusion1(float value);
    void setInputDiffusion2(float value);
    void setDecayDiffusion1(float value);
    void setDecayDiffusion2(float value);
    void setModRate(float hz);
    void setModDepth(float ms);
    void setSize(float ratio);

private:
    double sampleRate_ = 44100.0;
    float sizeFactor_ = 1.0f;

    // Pre-delay
    DelayLine<float> preDelay_;

    // Input bandwidth filter
    OnePoleLPF<float> bandwidthFilter_;

    // Input diffusers (4 allpass)
    AllpassFilter<float> inputDiffuser_[4];

    // Tank — Left half
    AllpassFilter<float> tankDiffuserL1_;  // modulated
    DelayLine<float> tankDelayL1_;
    OnePoleLPF<float> dampingFilterL_;
    AllpassFilter<float> tankDiffuserL2_;
    DelayLine<float> tankDelayL2_;

    // Tank — Right half
    AllpassFilter<float> tankDiffuserR1_;  // modulated
    DelayLine<float> tankDelayR1_;
    OnePoleLPF<float> dampingFilterR_;
    AllpassFilter<float> tankDiffuserR2_;
    DelayLine<float> tankDelayR2_;

    // LFO
    QuadratureLFO<float> lfo_;

    // Parameters (current)
    float decay_ = 0.85f;
    float damping_ = 0.3f;
    float bandwidth_ = 0.9995f;
    float inputDiff1_ = 0.75f;
    float inputDiff2_ = 0.625f;
    float decayDiff1_ = 0.70f;
    float decayDiff2_ = 0.50f;
    float modDepthSamples_ = 0.0f;

    // Delay length scaling
    float scaleDelay(int originalSamples) const;

    // Original delay lengths (Fs=29761)
    static constexpr int kInputDiffDelay[4] = {142, 107, 379, 277};
    static constexpr int kTankDiffDelay[4] = {672, 908, 1800, 2656};
    static constexpr int kTankLineDelay[4] = {4453, 4217, 3720, 3163};

    // Output taps (Fs=29761 基準)
    // Left taps: {delayIndex, tapOffset} ...
    // Right taps: {delayIndex, tapOffset} ...
    // → 詳細は spec.md Table 2 参照
};
```

### 6. PluginProcessor

```cpp
class DattorroPlateReverbProcessor : public juce::AudioProcessor {
public:
    // APVTS
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    DattorroReverb reverbEngine_;
    juce::dsp::Oversampling<float> oversampling_;

    // Smoothed parameters
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothDecay_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothDamping_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothMix_;
    // ... (全パラメータ分)

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
};
```

### processBlock フロー

1. APVTS からパラメータ読み出し → SmoothedValue 更新
2. ステレオ入力をモノサムに変換 → monoBuffer
3. `oversampling_.processSamplesUp(block)`
4. オーバーサンプリングされたブロックを `DattorroReverb::process()` へ
5. `oversampling_.processSamplesDown(block)`
6. Dry/Wet ミックス適用
7. 出力バッファへ書き込み

### オーバーサンプリング初期化

```cpp
// prepareToPlay 内
int osFactorIndex = getOversamplingIndex();  // 0=off, 1=2x, 2=4x
oversampling_ = std::make_unique<juce::dsp::Oversampling<float>>(
    2,  // チャンネル数
    osFactorIndex,
    juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,
    true  // isMaxQuality
);
oversampling_->initProcessing(samplesPerBlock);

double effectiveSampleRate = sampleRate * oversampling_->getOversamplingFactor();
reverbEngine_.prepare(effectiveSampleRate, (int)oversampling_->getOversamplingFactor());
```

### ディレイ長スケーリング

```cpp
float DattorroReverb::scaleDelay(int originalSamples) const {
    return static_cast<float>(originalSamples)
           * static_cast<float>(sampleRate_ / 29761.0)
           * sizeFactor_;
}
```

### CMakeLists.txt 構成

```cmake
cmake_minimum_required(VERSION 3.22)
project(DattorroPlateReverb VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE を FetchContent または add_subdirectory で導入
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(DattorroPlateReverb
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE Dplt
    FORMATS VST3 Standalone
    PRODUCT_NAME "Dattorro Plate Reverb"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(DattorroPlateReverb
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DSP/DattorroReverb.cpp
)

target_include_directories(DattorroPlateReverb PRIVATE Source)

target_compile_definitions(DattorroPlateReverb
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(DattorroPlateReverb
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# --- Tests ---
if(BUILD_TESTING)
    enable_testing()

    juce_add_console_app(DattorroPlateReverbTests
        PRODUCT_NAME "DattorroPlateReverbTests"
    )

    target_sources(DattorroPlateReverbTests
        PRIVATE
            Source/Tests/PluginTests.cpp
            Source/Tests/AudioProcessingTests.cpp
            Source/DSP/DattorroReverb.cpp
    )

    target_include_directories(DattorroPlateReverbTests PRIVATE Source)

    target_link_libraries(DattorroPlateReverbTests
        PRIVATE
            juce::juce_audio_utils
            juce::juce_dsp
    )

    target_compile_definitions(DattorroPlateReverbTests
        PUBLIC
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_STANDALONE_APPLICATION=1
    )

    add_test(NAME PluginTests COMMAND DattorroPlateReverbTests)
endif()
```

### GUI (最小構成)

初期リリースは `juce::GenericAudioProcessorEditor` を使用する。 全パラメータが自動的にスライダーとして表示される。 カスタム GUI は後続フェーズで実装する。

### スレッドセーフティ

- パラメータは APVTS 経由で読み書き (lock-free atomic)
- SmoothedValue はオーディオスレッドで `.getNextValue()` を呼ぶ
- オーバーサンプリング倍率変更時は prepareToPlay を再呼び出しで処理
- DattorroReverb 内部にロック/アロケーションなし
