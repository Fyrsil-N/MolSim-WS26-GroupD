MolSim Group D
===

Implementation for the course **Bachelor-Praktikum - Scientific Computing (PSE) Molekulardynamik (IN0012, IN4229)** of Group D (Tjorben Huber, Luka Maric, Florian Mathis).

## Architecture

## Usage 
### Compilation using CMake

Builds are stored out-of-source in a gitignored `/build` folder.

Configure and build using the CMakeLists.txt with the following commands:
```bash
cmake -S . -B build # configure (rerun whenever a new file is added)
cmake --build build -j # compile
cmake --build build --target format # use the clang auto formater
```

Configuration flags for the first command:
* `-DCMAKE_BUILD_TYPE=Debug` for a debug build

To get a fresh rebuild (when a configuration breaks), remove old builds by running 
```bash
rm -rf build 
```

### Execution
Run the final executable inside `build` to keep the repo clean.
```bash
cd build
./MolSim ../input/eingabe-sonne.txt
```
