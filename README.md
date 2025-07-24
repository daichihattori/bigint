# bigint
BigInt implementation in C++ using GMP

## Prepare
linux
```bash
sudo apt install libgmp-dev
```

mac
```bash
brew install gmp
```

Debug build
```
cmake -S . -B build
cmake --build build -j
```

Release build
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

```
cd build
ctest
```