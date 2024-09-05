## What is this?

When you get bc or ll file, this program extracts a CFG dot file from what you wanted.

### Build (SVF)
This version use SVF 3.0 - 2024.04.12 build
```
git clone https://github.com/SVF-tools/SVF
cd SVF
git reset --hard a7c5c803980e4e05b7ade7361ac08b0a6f0e0754
./build.sh
```

### Build
```bash
# source ./env.sh
cmake .
make
```

### Usage

1. Make ll or bc code
```bash
clang -S -emit-llvm example.c -o example.ll
clang -c -emit-llvm example.c -o example.bc
```

2. Extract dot CFG, and getting `visited_nodes`
```
./src/svf-example example.bc
```

(Then, you'll got `new_icfg.dot.dot`) file

3. Run `postprocessing.py`
```
python3 postprocessing.py
```

