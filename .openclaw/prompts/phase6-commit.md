# Phase 6: コミット

## タスク

### 1. 最終ビルド確認:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build/
ctest --test-dir build/ --output-on-failure
```

全テスト PASS を確認

### 2. コミット:

```bash
git add -A
git commit -m "feat: Dattorro plate reverb with oversampling (VST3/Standalone)"
```

### 3. プッシュ:

```bash
git push
```

### 4. 確認:

```bash
git log -1 --oneline
```

## 完了条件

- git log -1 に "feat: Dattorro plate reverb" が含まれること
- リモートにプッシュ済みであること
