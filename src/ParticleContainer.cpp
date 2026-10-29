/*
 * ParticleContainer.cpp
 *
 *  Created on: 22.10.2026
 *      Author: hutj
 */

#include "ParticleContainer.h"

void ParticleContainer::reserve(std::size_t n) { particles.reserve(n); }

std::size_t ParticleContainer::size() const { return particles.size(); }

bool ParticleContainer::empty() const { return particles.empty(); }

ParticleContainer::iterator ParticleContainer::begin() { return particles.begin(); }

ParticleContainer::iterator ParticleContainer::end() { return particles.end(); }

ParticleContainer::const_iterator ParticleContainer::begin() const { return particles.begin(); }

ParticleContainer::const_iterator ParticleContainer::end() const { return particles.end(); }
