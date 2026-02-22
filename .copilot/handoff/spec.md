# DattorroPlateReverb – プラグイン仕様書

## 概要

Jon Dattorro "Effect Design Part 1: Reverberator and Other Filters" (JAES 1997) に基づくプレート・リバーブ。バイオリン等の弦楽器で美しいロングディケイと 深みのあるニュアンスを実現する。2x オーバーサンプリングを搭載。

## ターゲット

| 項目 | 値 |
|---|---|
| OS | macOS (12+), Windows (10+) |
| フォーマット | VST3, Standalone |
| C++ 規格 | C++17 |
| ビルドシステム | CMake + JUCE 7 (juce_add_plugin) |
| サンプルレート | 44100, 48000, 88200, 96000 Hz 対応 |

## パラメータ一覧

### メインコントロール

| ID | 表示名 | 型 | 範囲 | デフォルト | 単位 | 説明 |
|---|---|---|---|---|---|---|
| `pre_delay` | Pre Delay | float | 0 – 200 | 15 | ms | プリディレイ |
| `decay` | Decay | float | 0.0 – 0.9999 | 0.85 | ratio | タンク減衰係数。0.95 以上でロングディケイ |
| `damping` | Damping | float | 0.0 – 1.0 | 0.3 | ratio | タンク内高域ダンピング (0=ダンプなし, 1=最大ダンプ) |
| `bandwidth` | Input BW | float | 0.0 – 1.0 | 0.9995 | ratio | 入力信号帯域制限 (1=フルBW) |
| `size` | Size | float | 0.5 – 2.0 | 1.0 | ratio | ディレイ長全体のスケーリング係数 |
| `mix` | Mix | float | 0.0 – 1.0 | 0.35 | ratio | Dry/Wet ミックス (0=Dry, 1=Wet) |

### ディフュージョン

| ID | 表示名 | 型 | 範囲 | デフォルト | 単位 | 説明 |
|---|---|---|---|---|---|---|
| `input_diffusion_1` | Input Diff 1 | float | 0.0 – 1.0 | 0.750 | ratio | 入力ディフューザー 1,2 の係数 |
| `input_diffusion_2` | Input Diff 2 | float | 0.0 – 1.0 | 0.625 | ratio | 入力ディフューザー 3,4 の係数 |
| `decay_diffusion_1` | Decay Diff 1 | float | 0.0 – 0.999 | 0.70 | ratio | タンクディフューザー 1,3 の係数 (符号反転で使用) |
| `decay_diffusion_2` | Decay Diff 2 | float | 0.0 – 0.999 | 0.50 | ratio | タンクディフューザー 2,4 の係数 |

### モジュレーション

| ID | 表示名 | 型 | 範囲 | デフォルト | 単位 | 説明 |
|---|---|---|---|---|---|---|
| `mod_rate` | Mod Rate | float | 0.0 – 3.0 | 0.5 | Hz | タンク内ディレイ変調レート |
| `mod_depth` | Mod Depth | float | 0.0 – 2.0 | 0.7 | ms | タンク内ディレイ変調デプス |

### オーバーサンプリング

| ID | 表示名 | 型 | 範囲 | デフォルト | 説明 |
|---|---|---|---|---|---|
| `oversampling` | Oversampling | choice | Off / 2x / 4x | 2x | オーバーサンプリング倍率 |

## Dattorro アルゴリズム — 構造定義

### 基準サンプルレート

原論文の全ディレイ長は **Fs = 29761 Hz** 基準。 実行時サンプルレートへ `(Fs_actual / 29761.0) * size` でスケーリングする。

### 信号フロー

```
Input(L+R mono sum)
  → Bandwidth LPF (1-pole)
  → Pre-Delay
  → Input Diffuser 1 (allpass lattice, coeff = +input_diffusion_1)
  → Input Diffuser 2 (allpass lattice, coeff = +input_diffusion_1)
  → Input Diffuser 3 (allpass lattice, coeff = +input_diffusion_2)
  → Input Diffuser 4 (allpass lattice, coeff = +input_diffusion_2)
  → split into Tank Left / Tank Right (figure-8 feedback)
```

### タンク構造 (Figure-8)

```
=== Left Half ===
  → Decay Diffuser 1 (modulated allpass, coeff = -decay_diffusion_1)
    delay length = 672 + excursion
  → Delay Line 1 (length = 4453)
  → Damping LPF 1 (1-pole, coeff = 1 - damping)
  → × decay
  → Decay Diffuser 2 (allpass, coeff = +decay_diffusion_2)
    delay length = 1800
  → Delay Line 2 (length = 3720)
  → × decay
  → feeds into Right Half input

=== Right Half ===
  → Decay Diffuser 3 (modulated allpass, coeff = -decay_diffusion_1)
    delay length = 908 + excursion
  → Delay Line 3 (length = 4217)
  → Damping LPF 2 (1-pole, coeff = 1 - damping)
  → × decay
  → Decay Diffuser 4 (allpass, coeff = +decay_diffusion_2)
    delay length = 2656
  → Delay Line 4 (length = 3163)
  → × decay
  → feeds into Left Half input
```

### ディレイ長テーブル (Fs = 29761 Hz 基準, サンプル数)

```
--- Input Diffusers ---
Input Diffuser 1: 142 samples
Input Diffuser 2: 107 samples
Input Diffuser 3: 379 samples
Input Diffuser 4: 277 samples

--- Tank (Left Half) ---
Decay Diffuser 1: 672 samples (+ modulated excursion)
Delay Line 1: 4453 samples
Decay Diffuser 2: 1800 samples
Delay Line 2: 3720 samples

--- Tank (Right Half) ---
Decay Diffuser 3: 908 samples (+ modulated excursion)
Delay Line 3: 4217 samples
Decay Diffuser 4: 2656 samples
Delay Line 4: 3163 samples
```

### 出力タップ構成 (Table 2)

出力はタンク内の複数地点からタップし、ステレオ合成する。 `nodeA_B[n]` = ノードA からノードB の間のディレイラインの n サンプル目。 全タップに 0.6 の係数を乗じる。

```
=== Left Output (YL) ===
node48_54[ 266]  (Delay Line 3 の 266 サンプル目)
node48_54[2974]  (Delay Line 3 の 2974 サンプル目)
node55_59[1913]  (Decay Diffuser 4 の 1913 サンプル目)
node59_63[1996]  (Delay Line 4 の 1996 サンプル目)
node24_30[1990]  (Delay Line 1 の 1990 サンプル目)
node31_33[ 187]  (Decay Diffuser 2 の 187 サンプル目)
node33_39[1066]  (Delay Line 2 の 1066 サンプル目)

=== Right Output (YR) ===
node24_30[ 353]  (Delay Line 1 の 353 サンプル目)
node24_30[3627]  (Delay Line 1 の 3627 サンプル目)
node31_33[1228]  (Decay Diffuser 2 の 1228 サンプル目)
node33_39[2673]  (Delay Line 2 の 2673 サンプル目)
node48_54[2111]  (Delay Line 3 の 2111 サンプル目)
node55_59[ 335]  (Decay Diffuser 4 の 335 サンプル目)
node59_63[ 121]  (Delay Line 4 の 121 サンプル目)
```

### モジュレーション

- Decay Diffuser 1 と 3 のディレイ長を正弦波で変調する
- LFO は直交位相 (quadrature) — sin / cos で左右を分ける
- レート: `mod_rate` Hz (デフォルト ≈ 1 Hz)
- デプス: `mod_depth` ms をサンプル数に変換
- 補間: **cubic interpolation** (リニア補間は高域にアーティファクトを生む)

### オーバーサンプリング

- `juce::dsp::Oversampling<float>` を使用
- Off / 2x / 4x を選択可能
- リバーブ処理全体をオーバーサンプリング内で実行
- ディレイ長は `(Fs_actual * oversample_factor / 29761.0) * size` でスケーリング
- prepareToPlay でオーバーサンプリング係数変更時にタンクを再初期化

### バイオリン向けチューニングのポイント

1. **ロングディケイ**: decay デフォルト 0.85。0.95+ で 10 秒超のテイルを実現
2. **ダンピング控えめ**: damping デフォルト 0.3。弦の倍音を殺さない
3. **モジュレーション微量**: mod_depth 0.7ms / mod_rate 0.5Hz。 ビブラートと干渉しない程度の揺らぎ
4. **オーバーサンプリング 2x**: 高域の折り返しノイズを抑え、 倍音構造のディテールを保持
5. **プリディレイ 15ms**: 原音の明瞭度を維持しつつ空間を付加

## 非機能要件

- processBlock 内でヒープアロケーション禁止
- 全ディレイバッファは prepareToPlay で確保
- パラメータ変更はスムージング (SmoothedValue, 20ms) を適用
- ステートフルフィルタの係数切り替えはクリックノイズを発生させない
- CPU 使用率: 2x オーバーサンプリング時、バッファ 512 samples で シングルコア 15% 以下 (M1 Mac 基準)
