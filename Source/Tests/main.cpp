#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

// テストランナーのメイン関数
int main(int argc, char* argv[]) {
    // JUCEの初期化（必要であれば）
    juce::ScopedJuceInitialiser_GUI libraryInitialiser;
    
    juce::UnitTestRunner runner;
    runner.runAllTests();

    int failures = 0;
    for (int i = 0; i < runner.getNumResults(); ++i) {
        failures += runner.getResult(i)->failures;
    }

    return failures > 0 ? 1 : 0;
}
