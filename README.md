# MolSim Group D

Implementation for the course **Bachelor-Praktikum - Scientific Computing (PSE) Molekulardynamik (IN0012, IN4229)** of
Group D (Tjorben Huber, Luka Maric, Florian Mathis).

## Architecture

## Usage

### Compilation

Builds are stored out-of-source in a gitignored `build/` folder.

Configure and build with the following commands (from the project root):

```bash
cmake -S . -B build # configure (rerun whenever a new file is added to include it in the build)
cd build 
make -j # compile the project
```

Configuration flags for the first command:

* `-DCMAKE_BUILD_TYPE=Debug` for a debug build
* `-DENABLE_VTK_OUTPUT=On` to output the simulation in VTK format instead of XYZ files

If a configuration breaks or needs to be changed, remove the old build by running from project root:

```bash
rm -rf build 
```

### Make targets

* `make format` uses clang-format and the `.clang-format` template to ensure consistent formatting of the project (run
  before any commit)
* `make doc_doxygen` is used to build the doxygen documentation (not yet implemented)

### Execution

Run the final executable inside `build/` to keep the repo clean and all output files contained in `build/`.

```bash
./MolSim ../input/eingabe-sonne.txt
./MolSim -d 0.014 -e 1000 ../input/eingabe-sonne.txt
```

```
Usage: ./MolSim [options] <input_file>

Options:
  -d, --delta-t <double>         time step size (default: 0.014)
  -e, --t-end <double>           end time (default: 1000)
  -o, --output-interval <int>    output is written every N iterations (default: 10)
  -h, --help                     print this help and exit
```
