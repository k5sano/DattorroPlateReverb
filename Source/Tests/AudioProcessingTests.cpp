#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "../PluginProcessor.h"

class AudioProcessingTests : public juce::UnitTest {
public:
    AudioProcessingTests() : UnitTest("AudioProcessingTests") {}

    void runTest() override {
        beginTest("Silence Input -> Silence Output (mix=0)");
        {
            DattorroPlateReverbProcessor plugin;
            plugin.prepareToPlay(44100.0, 512);
            
            // mix = 0 に設定
            if (auto* param = plugin.apvts.getParameter("mix"))
                param->setValueNotifyingHost(0.0f);

            juce::AudioBuffer<float> buffer(2, 512);
            buffer.clear();
            
            // 入力にインパルスを入れる
            buffer.setSample(0, 0, 1.0f);
            buffer.setSample(1, 0, 1.0f);

            juce::MidiBuffer midi;
            plugin.processBlock(buffer, midi);

            // mix=0 なので、入力がそのまま出力されるはず (Dryのみ)
            expectWithinAbsoluteError(buffer.getSample(0, 0), 1.0f, 0.0001f);
            expectWithinAbsoluteError(buffer.getSample(1, 0), 1.0f, 0.0001f);
        }

        beginTest("Crash Test: Silence Input 10 blocks");
        {
            DattorroPlateReverbProcessor plugin;
            plugin.prepareToPlay(44100.0, 512);
            
            juce::AudioBuffer<float> buffer(2, 512);
            buffer.clear();
            juce::MidiBuffer midi;

            for (int i = 0; i < 10; ++i) {
                plugin.processBlock(buffer, midi);
            }
            // クラッシュしなければOK
            expect(true); 
        }

        beginTest("Different Sample Rates");
        {
             DattorroPlateReverbProcessor plugin;
             std::vector<double> rates = {44100.0, 48000.0, 96000.0};
             for (auto rate : rates) {
                 plugin.prepareToPlay(rate, 512);
                 juce::AudioBuffer<float> buffer(2, 512);
                 buffer.clear();
                 juce::MidiBuffer midi;
                 plugin.processBlock(buffer, midi);
             }
             expect(true);
        }
        
        beginTest("Different Buffer Sizes");
        {
             DattorroPlateReverbProcessor plugin;
             std::vector<int> sizes = {64, 128, 256, 512, 1024};
             plugin.prepareToPlay(44100.0, 1024); // 最大サイズでprepare
             
             for (auto size : sizes) {
                 juce::AudioBuffer<float> buffer(2, size);
                 buffer.clear();
                 juce::MidiBuffer midi;
                 plugin.processBlock(buffer, midi);
             }
             expect(true);
        }

        beginTest("Decay=0 attenuation to -60dB after 20 blocks");
        {
            DattorroPlateReverbProcessor plugin;
            plugin.prepareToPlay(44100.0, 512);
            
            // decay = 0 に設定（最小値）
            if (auto* param = plugin.apvts.getParameter("decay"))
                param->setValueNotifyingHost(0.0f);
            
            juce::AudioBuffer<float> buffer(2, 512);
            juce::MidiBuffer midi;
            
            // 初期インパルス入力
            buffer.clear();
            buffer.setSample(0, 0, 1.0f);
            buffer.setSample(1, 0, 1.0f);
            plugin.processBlock(buffer, midi);
            
            // さらに 19 ブロック処理（合計 20 ブロック）
            for (int i = 1; i < 20; ++i) {
                buffer.clear();
                plugin.processBlock(buffer, midi);
            }
            
            // 出力の最大振幅を確認（-60dB = 0.001）
            float maxSample = 0.0f;
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
                for (int i = 0; i < buffer.getNumSamples(); ++i) {
                    maxSample = std::max(maxSample, std::abs(buffer.getSample(ch, i)));
                }
            }
            
            // decay=0 なので出力は -60dB 以下に減衰しているはず
            expect(maxSample < 0.001f, 
                   "Output should be below -60dB after 20 blocks with decay=0. Max: " 
                   + juce::String(maxSample));
        }

        beginTest("Oversampling settings Off/2x/4x");
        {
            DattorroPlateReverbProcessor plugin;
            
            // oversampling パラメータの値: 0=Off, 1=2x, 2=4x を想定
            std::vector<std::pair<int, juce::String>> modes = {
                {0, "Off"}, {1, "2x"}, {2, "4x"}
            };
            
            for (const auto& [value, name] : modes) {
                if (auto* param = plugin.apvts.getParameter("oversampling")) {
                    // Normalized value (0.0, 0.5, 1.0 for 3 choices)
                    float normalized = value / 2.0f;
                    param->setValueNotifyingHost(normalized);
                }
                
                plugin.prepareToPlay(48000.0, 512);
                
                juce::AudioBuffer<float> buffer(2, 512);
                buffer.clear();
                buffer.setSample(0, 0, 0.5f);
                buffer.setSample(1, 0, 0.5f);
                
                juce::MidiBuffer midi;
                plugin.processBlock(buffer, midi);
                
                // クラッシュしなければOK
                expect(true, "Oversampling " + name + " should not crash");
            }
        }
    }
};

static AudioProcessingTests audioProcessingTests;
