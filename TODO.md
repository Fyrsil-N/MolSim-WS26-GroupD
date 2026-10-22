# TODO

## Documentation

* add a clear statement to the readme which compiler and c++ version are being used
* list prerequisites and dependencies somewhere
* fill out the architecture subheading
* (starting from task 2) add documentation on how to use the ctest as well
* cleanly document command line arguments in help and in the readme

___

## Bugs

---

## Features

* optimize the force calculation (two particles acting on each other have the same force so we dont need to calculate it
  twice)

---

## Assignment1

* add the doxygen make target
* add the binary vtk support
* task 4 everything
    * would be cool to visualize the size of the particles due to their mass?
* presentation
* group simulation things into a simulator class
* maybe add an interface to plug in different forces in the simulator
