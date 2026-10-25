/*
 * Particle.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <array>
#include <string>

class Particle {
 private:
  /**
   * Position of the particle
   */
  std::array<double, 3> x;

  /**
   * Velocity of the particle
   */
  std::array<double, 3> v;

  /**
   * Force effective on this particle
   */
  std::array<double, 3> f;

  /**
   * Force which was effective on this particle
   */
  std::array<double, 3> old_f;

  /**
   * Mass of this particle
   */
  double m;

  /**
   * Type of the particle. Use it for whatever you want (e.g. to separate
   * molecules belonging to different bodies, matters, and so on)
   */
  int type;

 public:
  /**
   * Default Constructor to create a particle with all members = 0
   * @param type field to distinguish between different particle types
   */
  explicit Particle(int type = 0);

  /**
   * Copy constructor
   * @param other Reference to the particle to be copied
   */
  Particle(const Particle &other);

  /**
   * Move constructor to avoid copies when reallocating in a larger data structure.
   */
  Particle(Particle &&other) noexcept = default;

  Particle &operator=(const Particle &other) = default;

  Particle &operator=(Particle &&other) noexcept = default;

  /**
   * Constructor of the particle class.
   * @param x_arg initial (x, y, z) position of the particle
   * @param v_arg initial velocity in 3D space
   * @param m_arg weight of the particle
   * @param type field to distinguish between different particle types
   */
  Particle(
      // for visualization, we need always 3 coordinates
      // -> in case of 2d, we use only the first and the second
      const std::array<double, 3> &x_arg, const std::array<double, 3> &v_arg, double m_arg, int type = 0);

  virtual ~Particle();

  const std::array<double, 3> &getX() const;

  const std::array<double, 3> &getV() const;

  const std::array<double, 3> &getF() const;

  const std::array<double, 3> &getOldF() const;

  double getM() const;

  int getType() const;

  void setX(const std::array<double, 3> &x);

  void setV(const std::array<double, 3> &v);

  void setF(const std::array<double, 3> &f);

  void setOldF(const std::array<double, 3> &old_f);

  /**
   * Applies a new force to the particle, adding it to the current f
   * @param delta_f new force acting on the particle
   */
  void addToF(const std::array<double, 3> &delta_f);

  /**
   * move f into old_f and start a new force calculation step with f = {0.0, 0.0, 0.0}
   */
  void resetF();

  /**
   * Compares two particles for equality based on the value of their members NOT actual identity.
   * @param other other particle to compare to
   * @return True if two particles are equal in all members
   */
  bool operator==(Particle &other);

  std::string toString() const;
};

std::ostream &operator<<(std::ostream &stream, Particle &p);
