//
// Created by hutj on 25.10.26.
//

#pragma once
#include "ParticleContainer.h"

namespace io {
/**
 * This class defines a unified interface for all types of output methods to use (both output file based and live
 * visualizations). OutputWriters follow the observer pattern and don't store any data.
 */
class OutputWriter {
 protected:
  /**
   * Output is written every output_interval iterations
   */
  int output_interval;

 public:
  /**
   * Create an OutputWriter with a given output_interval.
   * @param output_interval_arg Output is written every output_interval iterations
   */
  explicit OutputWriter(int output_interval_arg) : output_interval(output_interval_arg) {}

  virtual ~OutputWriter() = default;

  /**
   * Write outputs based on the current state of the simulation as stored in the ParticleContainer.
   * This method is invoked at every step of the iteration, the output writer has to keep track of its output interval.
   * @param particles Particles to add to the output
   * @param iteration Current iteration number
   * @param time Current in-simulation time
   */
  virtual void onStep(const ParticleContainer &particles, int iteration, double time) = 0;

  /**
   * Optional method to write any final state or summary on the simulation.
   * This method is called once at the end of the simulation.
   * @param particles Final state of the simulation
   */
  virtual void onEnd(const ParticleContainer &particles [[maybe_unused]]) {}
};
}  // namespace io