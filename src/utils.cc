//
// Created by Misha on 3/15/2023.
//

#include "utils.h"

// 6x5 Jacobian of transformation {x, y, z, tx, ty, qp} -> {x, y, z, px, py, pz}
std::array<std::array<float, 5>, 6> Utils::CalculateJacobian( std::vector<float> parameters ){
  auto tx = parameters.at(3);
  auto ty = parameters.at(4);
  auto qp = parameters.at(5);

  auto p = fabsf( 1.0f/qp );
  auto q = qp * p;

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

std::vector<float> Utils::CalculateCovariance( std::vector<float> cov,
                                               std::array<std::array<float, 5>, 6> Jacobian ){
  // Covariance matrix (5x5) in {x, y, z, tx, ty, qp} variables
  std::array<std::array<float, 5>, 5> C{
          std::array<float, 5>{cov[0],  cov[1],   cov[3],   cov[6],   cov[10] },
          std::array<float, 5>{cov[1],  cov[2],   cov[4],   cov[7],   cov[11] },
          std::array<float, 5>{cov[3],  cov[4],   cov[5],   cov[8],   cov[12] },
          std::array<float, 5>{cov[6],  cov[7],   cov[8],   cov[9],   cov[13] },
          std::array<float, 5>{cov[10], cov[11],  cov[12],  cov[13],  cov[14] },
  };
//    for( int i=0; i<5; ++i ){
//      for( int j=0; j<5; ++j ){
//        C.at(i).at(j) = cov.at(std::min(i, j) + std::max(i, j) * (std::max(i, j) + 1) / 2);
//      }
//    }
  auto JT = TransposeJacobian(Jacobian);  // (5x6)
  std::array<std::array<float, 6>, 5> C_JT{};
  for( int i=0; i<5; ++i ){
    for( int j=0; j<6; ++j ){
      for( int k=0; k<5; ++k )
        C_JT.at(i).at(j)+= C.at(i).at(k) * JT.at(k).at(j);
    }
  }
  // matrix 6x6
  std::array<std::array<float, 6>, 6> J_C_JT{};
  for( int i=0; i<6; ++i ){
    for( int j=0; j<6; ++j ){
      for( int k=0; k<5; ++k )
        J_C_JT.at(i).at(j)+= Jacobian.at(i).at(k) * C_JT.at(k).at(j);
    }
  }
  std::vector<float> new_covariance;
  new_covariance.reserve(21);
  for( int i=0; i<6; ++i ){
    for( int j=0; j<=i; ++j ){
      new_covariance.push_back(J_C_JT.at(i).at(j) );
    }
  }
  return new_covariance;
}

// (5x6) Jacobian of transformation {x, y, z, tx, ty, qp} -> {x, y, z, px, py, pz}
std::array<std::array<float, 6>, 5> Utils::TransposeJacobian(std::array<std::array<float, 5>, 6> J) {
  std::array<std::array<float, 6>, 5> JT{};
  for( size_t i=0; i<5; ++i ){
    for( size_t j=0; j<6; ++j ){
      JT.at(i).at(j) = J.at(j).at(i);
    }
  }
  return JT;
}
