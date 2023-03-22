//
// Created by Misha on 3/15/2023.
//

#ifndef BMN_PARTICLE_FINDER_UTILS_H
#define BMN_PARTICLE_FINDER_UTILS_H

#include <vector>
#include <array>
#include <cmath>

class Utils {
public:
  Utils() = default;
  ~Utils() = default;

  static std::array<std::array<float, 5>, 6> CalculateJacobian( std::vector<float> parameters );
  static inline std::array<std::array<float, 6>, 5> TransposeJacobian( std::array<std::array<float, 5>, 6> J );
  static std::vector<float> CalculateCovariance( std::vector<float> cov,
                                                                  std::array<std::array<float, 5>, 6> Jacobian );
};


#endif //BMN_PARTICLE_FINDER_UTILS_H
