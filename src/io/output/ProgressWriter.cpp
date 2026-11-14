//
// Created by hutj on 08.11.26.
//

#include "ProgressWriter.h"

#include <spdlog/spdlog.h>

namespace io {
ProgressWriter::ProgressWriter(int total_iterations_arg)
    : OutputWriter(1), total_iterations(total_iterations_arg), old_progress(0) {}

void ProgressWriter::onStep(const ParticleContainer &particles [[maybe_unused]], int iteration,
                            double time [[maybe_unused]]) {
    if (total_iterations <= 0) {
        return;
    }

    const int progress = 100 * iteration / total_iterations;
    if (progress >= old_progress + 10) {
        old_progress = progress;
        spdlog::info("{0:d}%", progress);
    }
}
}  // namespace io