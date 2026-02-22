# Dattorro Plate Reverb

A JUCE audio plugin implementing Jon Dattorro's plate reverb algorithm from "Effect Design Part 1: Reverberator and Other Filters" (JAES 1997). Features 2x/4x oversampling for high-fidelity sound, optimized for violin and string instruments with beautiful long decay tails.

## Features

- **Dattorro plate reverb** — faithful implementation of the 1997 paper
- **Oversampling** — Off / 2x / 4x for reduced aliasing
- **Full parameter control** — Pre-delay, Decay, Damping, Bandwidth, Size, Mix, Input/Decay Diffusion, Modulation Rate/Depth
- **Formats** — VST3, Standalone
- **Platforms** — macOS 12+, Windows 10+

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Pre Delay | 0–200 ms | 15 ms | Pre-delay time |
| Decay | 0.0–0.9999 | 0.85 | Tank decay coefficient (≥0.95 for long decay) |
| Damping | 0.0–1.0 | 0.3 | High-frequency damping in tank |
| Input BW | 0.0–1.0 | 0.9995 | Input bandwidth limiting |
| Size | 0.5–2.0 | 1.0 | Delay length scaling |
| Mix | 0.0–1.0 | 0.35 | Dry/Wet mix |
| Mod Rate | 0.0–3.0 Hz | 0.5 Hz | Tank delay modulation rate |
| Mod Depth | 0.0–2.0 ms | 0.7 ms | Tank delay modulation depth |
| Oversampling | Off / 2x / 4x | 2x | Oversampling factor |

## Build

Requires CMake 3.22+ and a C++17 compiler. JUCE 7 is fetched automatically.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build/
```

### Run tests

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build/
ctest --test-dir build/ --output-on-failure
```

## References

- Dattorro, J. (1997). "Effect Design Part 1: Reverberator and Other Filters." *Journal of the Audio Engineering Society*, 45(9), 660–684.

## License

All rights reserved.
