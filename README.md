# MolSim Group D

Implementation for the course **Bachelor-Praktikum - Scientific Computing (PSE) Molekulardynamik (IN0012, IN4229)** of
Group D (Tjorben Huber, Luka Maric, Florian Mathis).

## Architecture

The project is split into a library `MolSimCore` and an actual executable `MolSim`. This way functionality and
instantiation are separated and the testing framework can make direct use of all code as well.

### Simulation core: `Simulator`, `ParticleContainer`, `Particle`

* `Simulator` owns all parts of the program related to physical updates and the main simulation loop.
    * It is the only part of the project that is intended to write the attributes of particles.
    * The main simulation loop follows the Störmer–Verlet order (x → F → v).
    * The specific force used for the update can be passed in via an interface (see `forces` namespace).
    * To make the pairwise force calculation more efficient, we make use of Newton's third law and update the forces of
      both particles at the same time with opposite signs. This halves the number of force calculations.
* `ParticleContainer` is based on a `std::vector`, as it provides an extendable but still cache-friendly data structure.
    * We pass through the iterators of the vector for use in range-based for loops and provide a `forEachPair` method
      that visits every distinct pair of particles exactly once and applies a given function to it.
    * The `reserve` method can be used to reserve space for large amounts of particles to reduce relocations of the
      vector.
* `Particle` stores the state of a particle together with getters and setters. It implements a `noexcept` move
  constructor to avoid expensive copy operations. The simulation currently runs without ever copying a particle.
* **Pattern**: Iterator, so implementation details of the Container are hidden.

### Physical forces: `namespace forces`

* `namespace forces` groups all types of forces behind a unified interface that allows them to be plugged into the
  simulation directly.
* `PairwiseForce` is an abstract class defining an interface for forces acting antisymmetrically and pairwise on
  particles.
    * It only requires the `calculateForce` method between two particles.
    * Currently `Gravity` implementing gravitational attraction and `LennardJones` implementing the Lennard-Jones force
      are valid options.
* An `ExternalForce` interface to consider forces acting on each particle individually is possible and would follow a
  similar pattern.
* Combining multiple forces is also possible in theory. As long as the `calculateForce` signature is maintained multiple
  forces can be grouped into one and passed to the simulator.
* **Pattern**: Strategy, so a new force is just another subclass and doesn't change the Simulator.

### Input and output: `namespace io`

* To allow for different methods of output, we provide an interface `OutputWriter` and adapt the `VTKWriter` and
  `XYZWriter` to follow it.
    * Each `OutputWriter` has its own `output_interval` (even if we currently only use a global interval), which states
      at which iterations it writes output.
    * We currently only implement two file-based writers for VTK and XYZ output, which are mutually exclusive. However,
      the simulation loop supports an arbitrary number of writers (passed in as a vector), so simultaneous live
      visualizations are possible.
    * Writers need to implement an `onStep` method called at every iteration and may override `onEnd` for final
      summaries.
    * Writers can only read from the `ParticleContainer` but never write to it.
* To allow for different input methods, we define the interface `InputReader` and adapt the `FileReader` to follow it.
    * Readers have to implement the method `readInParticles`, which fills a provided `ParticleContainer`.
    * They should also make use of the `reserve` method before filling the container to reduce relocations.
* **Pattern**: Strategy, so new readers share a unified interface; Observer, so OutputWriters can get notified without
  knowing their implementation.

### Parsing input parameters: `namespace cli` and `SimulationParameters`

* `CommandLineInterface` is responsible for reading in and validating all options and arguments of the program.
    * We use `getopt_long` to safely get all parameters and parsing functions to read out numerical options.
    * We validate input with basic sanity checks (positive time step and end time etc.).
    * A simple help message is provided with the `-h` option.
* `SimulationParameters` is a simple struct to store and pass back the command line arguments to the rest of the
  program.

### `MolSim.cpp`

* `MolSim.cpp` is the main file of the program. It is not supposed to touch any logic or do any computations.
* It creates all the necessary components like readers, writers, force and the simulation, combines them and starts the
  simulation run.

## Usage

### Compilation

The project uses C++20 and was developed and tested with **clang 21.1.8**.

Builds are stored out-of-source in a gitignored `build/` folder.

Configure and build with the following commands (from the project root):

```bash
CC=clang CXX=clang++ cmake -S . -B build # configure (rerun whenever a new file is added to include it in the build)
cd build
make -j # compile the project
```

Configuration flags for different builds (passed to cmake):

* `-DCMAKE_BUILD_TYPE=Debug` for a debug build (default: `Release`)
* `-DENABLE_VTK_OUTPUT=OFF` to build without VTK output. Required on machines without VTK installed, otherwise the
  configuration fails. (default: `ON`)
* `-DENABLE_OPENMP=OFF` to build without OpenMP for parallelization. Required on machines without OpenMp installed,
  otherwise the configuration fails. (default: `ON`)
* `-DBUILD_DOC_DOXYGEN=OFF` to disable the doc_doxygen target. (default: `ON`)
* `-DUSE_GOOGLETEST=OFF` for google test to not be built. Speeds up compilation. (default: `ON`)

If a configuration breaks or needs to be changed, remove the old build by running from project root:

```bash
rm -rf build
```

### Make targets

* `make format` uses clang-format and the `.clang-format` template to ensure consistent formatting of the project (run
  before any commit).
* `make doc_doxygen` is used to build the Doxygen documentation. (Not available if disabled in cmake configuration)

### Execution

Run the final executable inside `build/` to keep the repo clean and all output files contained in `build/`.

```bash
./MolSim ../input/Assignment2_params -o 50 -e 5 -d 0.0002
```

Parameters and arguments can also be checked via the `-h` or `--help` option.

```
Usage: ./MolSim [options] <input_file>

Options:
  -d, --delta-t <double>         time step size (default: 0.014)
  -e, --t-end <double>           end time (default: 1000)
  -o, --output-interval <int>    output is written every N iterations (default: 10)
  -l, --log-level <level>        trace|debug|info|warn|err|critical|off (default: info)
  -v, --vtk                      output simulation files using vtk (default: xyz)
  -p, --parallel                 calculate the forces in parallel (default: serial)
  -b, --benchmark                run a benchmark (disables all output and logs)
  -h, --help                     print this help and exit
```

Note `-v` and `-p` are not always available which is also shown in the help message:

* `-v` is only available if the project was configured with `ENABLE_VTK_OUTPUT=ON` and produces an error otherwise.
* `-p` is only available if the project was configured with `ENABLE_OPENMP=ON` and produces an error otherwise.

### Doxygen Documentation

When the target doc_doxygen is executed, Doxygen creates a subfolder within the project root: `doxygen-output/`.

This folder contains documentation generated by Doxygen as HTML and LaTeX.

The HTML Website can be viewed by opening `doxygen-output/html/index.html` in a browser.

The LATEX Code can be used to generate a pdf, which contains the documentation, by executing `make` within
the `doxygen-output/latex` folder (only if LATEX is installed). This creates a refman.pdf, which contains
the full documentation.

### Tests

The project uses the googletest framework for unit testing. All tests can be found in the `tests/` directory.

After building the project, run tests from the `build/` folder using:

```bash
ctest
ctest --output-on-failure #Output anything outputted by the test program if the test should fail.
```

### Performance Measurements

Performance of the simulation can be measured by setting the `-b` flag during execution. This will set the log level to
`off` and disable all OutputWriters except the `PerformanceWriter` which is used for the actual measurements. Careful
this also means using the `-l`, `-v`, `-o` flag in combination has **no** effect.

Benchmarks should only be run on a release build.

During a benchmark run the simulation will be executed normally on the input file (parallel or serial depending on the
`-p` flag). In the end a summary over total runtime, cpu runtime, iterations and other statistics will be printed to
`std::cout`.

```bash
./MolSim ../input/Assignment2_params -b -p -e 5 -d 0.0002
```
