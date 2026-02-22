#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "../PluginProcessor.h"

// テスト用のメイン関数をここで定義
// 注意: JUCE_STANDALONE_APPLICATION=1 が定義されている必要があります

class DattorroPluginTests : public juce::UnitTest {
public:
    DattorroPluginTests() : UnitTest("DattorroPluginTests") {}

    void runTest() override {
        beginTest("Plugin Instance Creation");
        
        std::unique_ptr<DattorroPlateReverbProcessor> plugin(new DattorroPlateReverbProcessor());
        expect(plugin != nullptr, "Plugin instance should not be null");

        beginTest("Parameter Existence");
        
        // パラメータIDのリスト (spec.mdに基づく)
        const std::vector<juce::String> paramIDs = {
            "pre_delay", "decay", "damping", "bandwidth", "size", "mix",
            "input_diffusion_1", "input_diffusion_2", "decay_diffusion_1",
            "decay_diffusion_2", "mod_rate", "mod_depth", "oversampling"
        };

        auto& apvts = plugin->getParameters(); // ここは実装に合わせて調整が必要かも
        // 通常は processor から getParameters() で AudioProcessorParameter* の配列が返る
        // または APVTS を公開メンバとして持っているか確認が必要

        // 今回は APVTS を直接触るか、getParameters() 経由で確認する
        // DattorroPlateReverbAudioProcessor の実装詳細が不明なので、
        // 一般的な getParameters() を使ってIDを確認する

        for (const auto& id : paramIDs) {
            bool found = false;
            for (auto* param : plugin->getParameters()) {
                if (auto* p = dynamic_cast<juce::AudioProcessorParameterWithID*>(param)) {
                    if (p->paramID == id) {
                        found = true;
                        break;
                    }
                }
            }
            expect(found, "Parameter not found: " + id);
        }

        beginTest("Parameter Ranges & Defaults");
        // ここに各パラメータのデフォルト値と範囲のテストを追加
        // spec.md の内容に基づいて実装する
        // 例:
        // checkParam(plugin.get(), "pre_delay", 0.0f, 0.0f, 200.0f);
    }
    
    // ヘルパー関数
    void checkParam(juce::AudioProcessor* plugin, const juce::String& id, float defaultVal, float minVal, float maxVal) {
        for (auto* param : plugin->getParameters()) {
             if (auto* p = dynamic_cast<juce::AudioProcessorParameterWithID*>(param)) {
                if (p->paramID == id) {
                    // NormalisableRange を取得するのは難しい場合がある（RangedAudioParameterなら可能）
                    // ここでは簡易的にデフォルト値だけチェックするか、キャストして確認する
                    if (auto* rangedParam = dynamic_cast<juce::AudioParameterFloat*>(param)) {
                         expectWithinAbsoluteError(rangedParam->get(), defaultVal, 0.001f, id + " default value");
                         expectWithinAbsoluteError(rangedParam->range.start, minVal, 0.001f, id + " min value");
                         expectWithinAbsoluteError(rangedParam->range.end, maxVal, 0.001f, id + " max value");
                    }
                    return;
                }
            }
        }
    }
};

static DattorroPluginTests dattorroPluginTests;
