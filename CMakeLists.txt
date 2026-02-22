cmake_minimum_required(VERSION 3.22)
project(DattorroPlateReverb VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE 7 via FetchContent
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(DattorroPlateReverb
    COMPANY_NAME "DattorroAudio"
    PLUGIN_MANUFACTURER_CODE Datt
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
            Source/PluginProcessor.cpp
            Source/PluginEditor.cpp
            Source/Tests/PluginTests.cpp
            Source/Tests/AudioProcessingTests.cpp
            Source/Tests/main.cpp
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
            JucePlugin_Name="Dattorro Plate Reverb"
            JucePlugin_Manufacturer="DattorroAudio"
            JucePlugin_ManufacturerWebsite=""
            JucePlugin_ManufacturerEmail=""
            JucePlugin_Version=1.0.0
            JucePlugin_VersionCode=0x10000
            JucePlugin_VersionString="1.0.0"
            JucePlugin_Desc="Dattorro Plate Reverb"
            JucePlugin_PluginCode=0x44706c74
            JucePlugin_ManufacturerCode=0x44617474
    )

    add_test(NAME PluginTests COMMAND DattorroPlateReverbTests)
endif()
