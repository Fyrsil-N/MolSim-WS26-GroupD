//
// Created by hutj on 15.11.26.
//

#include "LinkedCellContainer.h"

#include <algorithm>
#include <cmath>

LinkedCellContainer::LinkedCellContainer(std::array<double, 3> domain_anchor, std::array<double, 3> domain_dimensions,
                                         bool third_dim, double r_cutoff)
    : domain_anchor(domain_anchor), third_dim(third_dim) {
    for (size_t i = 0; i < domain_dimensions.size(); ++i) {
        const int inner_count = std::max(static_cast<int>(domain_dimensions[i] / r_cutoff), 1);
        cell_dimensions[i] = domain_dimensions[i] / inner_count;
        // add halo cells
        cell_counts[i] = inner_count + 2;
    }
    // without a third dimension we have exactly one cell in z direction with an (arbitrary) size of r_cutoff
    // with only 2D all particles should be in the same plane so the size of the cell won't matter.
    if (!third_dim) {
        cell_counts[2] = 1;
        cell_dimensions[2] = r_cutoff;
    }

    // create one empty particle index vector per cell (including the halo cells)
    cells.resize(static_cast<size_t>(cell_counts[0] * cell_counts[1] * cell_counts[2]));

    // collect the inner cells and halo cells separately to make calculations easier later on (removes some nested
    // loops)
    // @TODO later on we probably need to collect halo and boundary cells per boundary not just in general
    for (size_t cell_index = 0; cell_index < cells.size(); ++cell_index) {
        if (isHaloCell(cell_index)) {
            halo_cells.push_back(cell_index);
        } else {
            inner_cells.push_back(cell_index);
        }
    }

    // build the stencil we will use to make newton's third law possible
    // these are indices into the cells vector to avoid having another loop in the force update
    const int max_dz = third_dim ? 1 : 0;

    for (int dz = -max_dz; dz <= max_dz; ++dz) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                // the stencils are all the cells with a positive offset since we build a forward stencil
                const int offset = dx + cell_counts[0] * dy + cell_counts[0] * cell_counts[1] * dz;
                if (offset > 0) {
                    forward_stencil.push_back(static_cast<std::size_t>(offset));
                }
            }
        }
    }
}

void LinkedCellContainer::sortParticles() {
    // delete the old entries
    for (auto &cell : cells) {
        cell.clear();
    }

    // iterate over the position of all particles and store their index to assign them to the cell they are in
    const size_t dims = third_dim ? 3 : 2;
    std::array<int, 3> cell_position{};

    for (std::size_t i = 0; i < data.positions.size(); ++i) {
        for (std::size_t j = 0; j < dims; ++j) {
            const int tmp =
                static_cast<int>(std::floor((data.positions[i][j] - domain_anchor[j]) / cell_dimensions[j]));
            // clamp to cells that exist so we can at most write in the halo
            cell_position[j] = std::clamp(tmp, -1, cell_counts[j] - 2);
        }

        cells[cellPositionToIndex(cell_position)].push_back(i);
    }
}

void LinkedCellContainer::updateForce(const forces::PairwiseForce &force) {
    // reset the old force
    resetForces();

    // sort the particles in their new cells
    sortParticles();

    // compute the force of particles using a forward stencil and newtons third law.
    for (const auto cell : inner_cells) {
        applyForcesInCell(cells[cell], force);
        for (const auto offset : forward_stencil) {
            // @TODO implement boundary condition for now we just dont compute interactions with halo cells
            // @TODO this check is super expensive so get rid of it asap
            if (!isHaloCell(cell + offset)) {
                applyForcesBetweenCells(cells[cell], cells[cell + offset], force);
            }
        }
    }

    // @TODO handle other boundary conditions by iterating over halo cells (and deleting all particles for outflow)
}

void LinkedCellContainer::applyForcesInCell(const std::vector<size_t> &cell, const forces::PairwiseForce &force) {
    for (size_t i = 0; i < cell.size(); ++i) {
        for (size_t j = i + 1; j < cell.size(); ++j) {
            std::array<double, 3> delta_f = force.calculateForce(getConstHandle(cell[i]), getConstHandle(cell[j]));
            data.forces[cell[i]] = data.forces[cell[i]] + delta_f;
            data.forces[cell[j]] = data.forces[cell[j]] - delta_f;
        }
    }
}

void LinkedCellContainer::applyForcesBetweenCells(const std::vector<size_t> &cell_1, const std::vector<size_t> &cell_2,
                                                  const forces::PairwiseForce &force) {
    for (const size_t i : cell_1) {
        for (const size_t j : cell_2) {
            const std::array<double, 3> delta_f = force.calculateForce(getConstHandle(i), getConstHandle(j));
            data.forces[i] = data.forces[i] + delta_f;
            data.forces[j] = data.forces[j] - delta_f;
        }
    }
}
