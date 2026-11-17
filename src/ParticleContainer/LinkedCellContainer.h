//
// Created by hutj on 15.11.26.
//

#pragma once
#include <array>
#include <cstddef>
#include <vector>

#include "ParticleContainer.h"

/**
 * This version of the ParticleContainer implements calculating the forces for the particles via the linked-cell
 * algorithm in O(n)
 */
class LinkedCellContainer : public ParticleContainer {
   private:
    /**
     * The simulation domain starts at these (x, y, z) coordinates.
     */
    std::array<double, 3> domain_anchor;

    /**
     * how many cells exist in all 3 spacial dimensions (x,y,z), including one layer of halo cells on each side
     * if we are in 2d the z direction has exactly size = 1
     */
    std::array<int, 3> cell_counts;

    /**
     * dimensions in (x,y,z) direction of a single cell
     */
    std::array<double, 3> cell_dimensions;

    /**
     * Whether the third spatial dimension is enabled
     */
    bool third_dim;

    /**
     * for every cell (inner and halo) we store the indices of the particles belonging to that cell
     */
    std::vector<std::vector<size_t>> cells;

    /**
     * Indices (in cells) of all inner cells
     */
    std::vector<size_t> inner_cells;

    /**
     * Indices (in cells) of all halo cells
     */
    std::vector<size_t> halo_cells;

    /**
     * Flat index offsets to the forward half of the neighbouring cells (set depending on third_dim).
     */
    std::vector<size_t> forward_stencil;

    /**
     * Based on the position of the cell in the grid (x, y, z) return the index of said cell in the cells vector
     * @param cell_position (x, y, z) position of the cell all going form -1 (helo cell), inner cells, cell_counts[i] -2
     * @return index of the cell in the cells vector
     */
    inline size_t cellPositionToIndex(std::array<int, 3> cell_position) const {
        // make sure to also iterate over z outest loop to x inner loop for cache friendliness
        if (third_dim) {
            cell_position[2] += 1;
        }
        return (cell_position[0] + 1) + cell_counts[0] * (cell_position[1] + 1) +
               cell_counts[0] * cell_counts[1] * cell_position[2];
    }

    /**
     * Based on the index of a cell in the cells vector return the (x, y, z) coordinate of the cell in the grid
     * @param index index of the cell
     * @return (x, y, z) position of the cell all going form -1 (helo cell), inner cells, cell_counts[i] -2
     */
    inline std::array<int, 3> indexToCellPosition(size_t index) const {
        std::array<int, 3> cell_position{};
        cell_position[0] = index % cell_counts[0] - 1;
        cell_position[1] = (index / cell_counts[0]) % cell_counts[1] - 1;
        if (third_dim) {
            cell_position[2] = index / (cell_counts[0] * cell_counts[1]) - 1;
        } else {
            cell_position[2] = 0;
        }
        return cell_position;
    }

    /**
     * Based on the index of a cell in the cells vector return if it is a halo cell or not
     * @param index index of the cell
     * @return true if the cell is a halo cell
     */
    inline bool isHaloCell(size_t index) const {
        auto cell_position = indexToCellPosition(index);
        for (size_t i = 0; i < cell_position.size(); ++i) {
            if (cell_position[i] == -1 || cell_position[i] == cell_counts[i] - 2) {
                return true;
            }
        }
        return false;
    }

    /**
     * Iterate over all particles once O(n) and sort them into their respective cells.
     * Particles outside the halo are put into the closest halo cell.
     */
    void sortParticles();

    /**
     * Apply the force to every pair of unique particles within one cell (using Newton's third law).
     * @param cell particle indices of the cell
     * @param force PairwiseForce that acts between the particles
     */
    void applyForcesInCell(const std::vector<size_t> &cell, const forces::PairwiseForce &force);

    /**
     * Apply the force to every pair of unique particles with one particle from each cell (using Newton's third law).
     * @param cell_1 particle indices of the first cell
     * @param cell_2 particle indices of the second cell (must be a different cell)
     * @param force PairwiseForce that acts between the particles of both cells
     */
    void applyForcesBetweenCells(const std::vector<size_t> &cell_1, const std::vector<size_t> &cell_2,
                                 const forces::PairwiseForce &force);

   public:
    /**
     * Constructor for the LinkedCellContainer
     * @param domain_anchor coordinates of the start of the simulation domain
     * @param domain_dimensions dimensions of the simulation domain
     * @param third_dim set if the third dimension is enabled in the simulation
     * @param r_cutoff cut off radius of the pairwise force and minimum size of linked cells
     */
    LinkedCellContainer(std::array<double, 3> domain_anchor, std::array<double, 3> domain_dimensions, bool third_dim,
                        double r_cutoff);

    /**
     * Calculate the forces on the particles in the next timestep and move the current force into old_force.
     * The algorithm to do this is the linked_cells_algorithm so this takes O(n).
     * Every inner cell is paired with itself and its forward neighbours.
     * @param force PairwiseForce that acts between all pairs of unique particles
     */
    void updateForce(const forces::PairwiseForce &force) override;
};
