# Phase 2: DSP コンポーネント

## 参照ファイル
- `.copilot/handoff/design.md` — クラス詳細セクション

## タスク

以下の 4 つのヘッダーオンリークラスを作成する（全て Source/DSP/ に配置）:

### 1. `Source/DSP/DelayLine.h`
- テンプレートクラス `DelayLine<SampleType>`
- バッファサイズは 2 のべき乗に切り上げ
- ビットマスクでラップアラウンド
- メソッド: setMaxDelay, setDelay, reset, write, read, readAt, readCubic
- readCubic: Niemitalo の 4-point cubic 補間
  ```
  x0, x1, x2, x3 = 4連続サンプル
  a = (3*(x1-x2) - x0 + x3) / 2
  b = 2x2 + x0 - (5x1+x3) / 2
  c = (x2-x0) / 2
  result = ((a*frac + b)*frac + c)*frac + x1
  ```

### 2. `Source/DSP/OnePoleLPF.h`
- テンプレートクラス `OnePoleLPF<SampleType>`
- `y[n] = y[n-1] + coeff * (x[n] - y[n-1])`
- メソッド: setCoefficient, process, reset

### 3. `Source/DSP/AllpassFilter.h`
- テンプレートクラス `AllpassFilter<SampleType>`
- 内部に DelayLine を持つ
- ラティス構造: `out = -coeff*in + delay_out; write(in + coeff*delay_out)`
- メソッド: prepare, reset, process, readAt, readCubic (タンクタップ/モジュレーション用)

### 4. `Source/DSP/LFO.h`
- テンプレートクラス `QuadratureLFO<SampleType>`
- 位相アキュムレータ方式
- struct Output { sin, cos } を返す tick()
- メソッド: prepare, setFrequency, tick, reset

## 完了条件

```bash
cmake --build build/
```

ビルドが通ること。ヘッダーオンリーなので #include するだけでテスト可能。
PluginProcessor.cpp の先頭に各ヘッダーの #include を追加して
ビルドが通ることを確認する。
