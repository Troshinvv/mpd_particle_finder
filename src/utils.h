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
  Utils() = delete;
  ~Utils() = delete;

  static std::array<std::array<float, 5>, 6> CalculateJacobian( std::vector<float> parameters ){
    auto x = parameters.at(0);
    auto y = parameters.at(1);
    auto z = parameters.at(2);
    auto tx = parameters.at(3);
    auto ty = parameters.at(4);
    auto qp = parameters.at(5);

    auto p = fabsf( 1.0f/qp );
    auto q = qp * p;
    auto pz = sqrtf( p*p / ( tx*tx + ty*ty +1 ) );
    auto px = tx/pz;
    auto py = ty/pz;

    auto t = sqrtf(1.f + tx * tx + ty * ty);
    auto t3 = t * t * t;
    auto dpxdtx = q / qp * (1.f + ty * ty) / t3;
    auto dpxdty = -q / qp * tx * ty / t3;
    auto dpxdqp = -q / (qp * qp) * tx / t;
    auto dpydtx = -q / qp * tx * ty / t3;
    auto dpydty = q / qp * (1.f + tx * tx) / t3;
    auto dpydqp = -q / (qp * qp) * ty / t;
    auto dpzdtx = -q / qp * tx / t3;
    auto dpzdty = -q / qp * ty / t3;
    auto dpzdqp = -q / (qp * qp * t3);

    std::array<std::array<float, 5>, 6> J = {
            std::array<float, 5>{1.f, 0.f, 0.f,     0.f,    0.f},
            std::array<float, 5>{0.f, 1.f, 0.f,     0.f,    0.f},
            std::array<float, 5>{0.f, 0.f, 0.f,     0.f,    0.f},
            std::array<float, 5>{0.f, 0.f, dpxdtx,  dpxdty, dpxdqp},
            std::array<float, 5>{0.f, 0.f, dpydtx,  dpydty, dpydqp},
            std::array<float, 5>{0.f, 0.f, dpzdtx,  dpzdty, dpzdqp}
    };

    return J;
  }

  static std::vector<float> CalculateCovariance( std::vector<float> covariance_vector,
                                                                  std::array<std::array<float, 5>, 6> Jacobian ){
    std::array<std::array<float, 5>, 5> C{};
    for( int i=0; i<5; ++i ){
      for( int j=0; j<5; ++j ){
        C.at(i).at(j) = covariance_vector.at(std::min(i, j) + std::max(i, j) * (std::max(i, j) + 1) / 2);
      }
    }
    // matrix 5x6
    std::array<std::array<float, 6>, 5> CJT{};
    for( int i=0; i<5; ++i ){
      for( int j=0; j<6; ++j ){
        for( int k=0; k<5; ++k )
          CJT.at(i).at(j)+= C.at(i).at(k) + Jacobian.at(j).at(k);
      }
    }
    // matrix 6x6
    std::array<std::array<float, 6>, 6> JCJT{};
    for( int i=0; i<6; ++i ){
      for( int j=0; j<6; ++j ){
        for( int k=0; k<5; ++k )
          JCJT.at(i).at(j)+= Jacobian.at(i).at(k)*CJT.at(k).at(j);
      }
    }
    std::vector<float> new_covariance;
    new_covariance.reserve(21);
    for( int i=0; i<6; ++i ){
      for( int j=0; j<=i; ++j ){
        new_covariance.push_back( JCJT.at(i).at(j) );
      }
    }
    return new_covariance;
  }


};


#endif //BMN_PARTICLE_FINDER_UTILS_H
