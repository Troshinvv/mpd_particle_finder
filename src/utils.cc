//
// Created by Misha on 3/15/2023.
//

#include "utils.h"

std::array<std::array<float, 6>, 6> Utils::CalculateJacobian( std::vector<float> parameters ){
  
  std::array<std::array<float, 6>, 6> J = {
          std::array<float, 6>{1.f, 0.f, 0.f, 0.f,     0.f,    0.f},
          std::array<float, 6>{0.f, 1.f, 0.f, 0.f,     0.f,    0.f},
          std::array<float, 6>{0.f, 0.f, 1.f, 0.f,     0.f,    0.f},
          std::array<float, 6>{0.f, 0.f, 0.f, 1.f,     0.f,    0.f},
          std::array<float, 6>{0.f, 0.f, 0.f, 0.f,     1.f,    0.f},
          std::array<float, 6>{0.f, 0.f, 0.f, 0.f,     0.f,    1.f}
  };

  return J;
}

std::vector<float> Utils::CalculateCovariance( std::vector<float> cov,
                                               std::array<std::array<float, 6>, 6> Jacobian ){
  // Covariance matrix (5x5) in {x, y, z, tx, ty, qp} variables
  std::array<std::array<float, 6>, 6> C{
    //                          x,      y,        z,    tx,       ty,       Qp
          std::array<float, 6>{cov[0],  cov[1],   0.f,  cov[3],   cov[6],   cov[10] }, // x
          std::array<float, 6>{cov[1],  cov[2],   0.f,  cov[4],   cov[7],   cov[11] }, // y
          std::array<float, 6>{0.f,     0.f,      0.f,  0.f,      0.f,      0.f },     // z
          std::array<float, 6>{cov[3],  cov[4],   0.f,  cov[5],   cov[8],   cov[12] }, // tx
          std::array<float, 6>{cov[6],  cov[7],   0.f,  cov[8],   cov[9],   cov[13] }, // ty
          std::array<float, 6>{cov[10], cov[11],  0.f,  cov[12],  cov[13],  cov[14] }, // Qp
  };
//    for( int i=0; i<5; ++i ){
//      for( int j=0; j<5; ++j ){
//        C.at(i).at(j) = cov.at(std::min(i, j) + std::max(i, j) * (std::max(i, j) + 1) / 2);
//      }
//    }
  auto JT = TransposeJacobian(Jacobian);  // (5x6)
  std::array<std::array<float, 6>, 6> C_JT{};
  for( int i=0; i<6; ++i ){
    for( int j=0; j<6; ++j ){
      C_JT.at(i).at(j) = 0;
      for( int k=0; k<6; ++k )
        C_JT.at(i).at(j)+= C.at(i).at(k) * JT.at(k).at(j);
    }
  }
  // matrix 6x6
  std::array<std::array<float, 6>, 6> J_C_JT{};
  for( int i=0; i<6; ++i ){
    for( int j=0; j<6; ++j ){
      J_C_JT.at(i).at(j) = 0;
      for( int k=0; k<6; ++k )
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
std::array<std::array<float, 6>, 6> Utils::TransposeJacobian(std::array<std::array<float, 6>, 6> J) {
  std::array<std::array<float, 6>, 6> JT{};
  for( size_t i=0; i<6; ++i ){
    for( size_t j=0; j<6; ++j ){
      JT.at(i).at(j) = J.at(j).at(i);
    }
  }
  return JT;
}
