# MolSim Group D

Implementation for the course **Bachelor-Praktikum - Scientific Computing (PSE) Molekulardynamik (IN0012, IN4229)** of
Group D (Tjorben Huber, Luka Maric, Florian Mathis).

## Architecture

The project is split into a library `MolSimCore` and an actual executable `MolSim`. This way functionality and
instantiation are separated and the testing framework can make direct use of all code as well.

### Simulation core: `Simulator`, `ParticleContainer`, `Particle`

* `Simulator` owns all parts of the program related to physical updates and the main simulation loop.
    * The main simulation loop follows the Störmer–Verlet order (x → F → v).
    * The specific force used for the update can be passed in via an interface (see `forces` namespace).
* `ParticleContainer` is a Struct Of Arrays data-structure to allow for vectorized and cache friendly operations.
    * The particle class was fully eliminated to get rid of an expensive indirection in hot loop
    * `ParticleContainer` now wraps the `PartileData` struct which stores all particle attributes directly in vectors
    * `ParticleContainer` still provides easy for-range style loop over a `ParticleHandle` and an Iterator
    * The `ParticleHandle` doesn't store any data itself and instead directly reads into the Data struct because it is a
      `friend` class of the `ParticleContainer`
    * The `reserve` method can be used to reserve space for large amounts of particles to reduce relocations
* The child classes of `ParticleContainer` `DirectSumContainer` and `LinkedCellContainer` provide different
  implementations of updating the pairwise forces acting on the particles
    * `DirectSum` loops over all unique pairs of particles and updates the force in O (n^2)
    * `LinkedCell` instead makes use of the linked cell algorithm and a cutoff radius of the pairwise force to reduce
      the loop to ~O (n)
    * Both implementations make use of Newton's third law and update the forces of both particles at the same time with
      opposite signs. This halves the number of force calculations.
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
* To make using the linked cell algorithm possible `Lennard Jones` supports a cutoff radius after which all forces are 0
* **Pattern**: Strategy, so a new force is just another subclass and doesn't change the Simulator.

### Input and output: `namespace io`

* To allow for different methods of output, we provide an interface `OutputWriter`.
    * Each `OutputWriter` has its own `output_interval`, which states at which iterations it performs its action.
    * We currently implement two file-based writers for VTK and XYZ output, which are mutually exclusive.
    * The `ProgressWriter` logs the progress of the simulation in 10% increments.
    * The `PerformanceWriter` allows us to measure the performance of a given simulation run.
    * Writers need to implement an `onStep` method called at every iteration and may override `onEnd` for final
      summaries.
    * Writers can only read from the `ParticleContainer` but never write to it.
    * Output is written to an output folder in the project root.
* To allow for different input methods, we define the interface `InputReader`.
    * Readers have to implement the methods `readInParticles` and `readInSimulation`, which fill provided
      `ParticleContainer`s and `SimulationParameter`s.
    * They should also make use of the `reserve` method before filling the container to reduce relocations.
    * The `YamlReader` initializes the simulation with arbitrary arrangements of particles into shapes and single
      particles.
* **Pattern**: Strategy, so new readers share a unified interface; Observer, so OutputWriters can get notified without
  knowing their implementation.

### Parsing input parameters: `namespace cli` and `SimulationParameters`

* `CommandLineInterface` is responsible for reading in and validating all options and arguments of the program.
    * We use `getopt_long` to safely get all parameters.
    * A simple help message is provided with the `-h` option.
* `SimulationParameters` is a simple struct to store and pass back the command line arguments to the rest of the
  program.

### `MolSim.cpp`

* `MolSim.cpp` is the main file of the program. It is not supposed to touch any logic or do any computations.
* It creates all the necessary components like readers, writers, force and the simulation, combines them and starts the
  simulation run.

## Input File Format .yml

### Scene building

Defining simulation parameters and particle positions happens in YAML input files. A demonstration file can be found
inside the input folder which demonstrates most of the functionality (`input/YamlReaderDemo.yml`).

The input is split into two main parts:

#### Simulation:

Inside the simulation map, we define:

* size of each timestep (`delta_t`)
* end time (`end_t`)
* output frequency (`out_interval`)
* spatial dimensions (`3d`: true/false)

as well as choose and (if desired) parametrize forces acting upon the particles. Available pairwise forces are Gravity
and Lennard-Jones. Lennard-Jones can be parametrized by changing its `sigma`, `epsilon` and `cutoff` variables.

#### Scene:

Inside the scene map, we can generate different arrangements of particles:

* Cuboids
* Ellipsoids
* Singular particles

They all take the same parameters (`anchor_point`, `dimensions`, `velocity`, `spacing`, `mass`, `type`). Multiple
versions of one shape type can be generated using sequences.

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

Run the final executable inside `build/`.

```bash
./MolSim ../input/Assignment3_Input2.yml -v
./MolSim ../input/Assignment3_Input2.yml -v -s # is calculated serially
```

Parameters and arguments can also be checked via the `-h` or `--help` option.

```
Usage: ./MolSim [options] <input_file>

Options:
  -l, --log-level <level>        trace|debug|info|warn|err|critical|off (default: info)
  -v, --vtk                      output simulation files using vtk (default: xyz)
  -s, --serial                   calculate the forces serially (currently does nothing, forces are always calculated serially)
  -b, --benchmark                run a benchmark (disables all output and logs)
  -h, --help                     print this help and exit
```

Note `-v` is not always available which is also shown in the help message:

* `-v` is only available if the project was configured with `ENABLE_VTK_OUTPUT=ON` and produces an error otherwise.

### Doxygen Documentation

When the target doc_doxygen is executed, Doxygen creates a subfolder within the project root: `doxygen-output/`.

This folder contains documentation generated by Doxygen as HTML and LaTeX.

The HTML Website can be viewed by opening `doxygen-output/html/index.html` in a browser.

The LATEX Code can be used to generate a PDF, which contains the documentation, by executing `make` within
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
this also means using the `-l` and `-v` flags in combination has **no** effect.

Benchmarks should only be run on a release build.

During a benchmark run the simulation will be executed normally on the input file (parallel or serial depending on the
`-s` flag; currently always serial, so the reported thread count is only correct with `-s`). In the end a summary over
total runtime, cpu runtime, iterations and other statistics will be printed to
`std::cout`.

```bash
./MolSim ../input/Assignment3_Benchmark.yml -b
```
