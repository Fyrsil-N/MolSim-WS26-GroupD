# TODO

## Documentation

* list prerequisites and dependencies somewhere
* (starting from task 2) add documentation on how to use the ctest as well

___

## Bugs

---

## Features

* maintain the vtk reader inside of the class dont regen it every time
* maybe add a runtime selection between vtk,xyz and other writers.
* deduplicate the interval checking by having it sit in outputWriter and then calling the actual private virtual write
  function.

---

## Assignment1

* add the doxygen make target
* add the binary vtk support
* presentation
