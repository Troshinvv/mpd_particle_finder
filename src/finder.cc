//
// Created by Misha on 4/14/2023.
//

#include <SimpleFinder.hpp>
#include "finder.h"
#include "utils.h"

ROOT::RVec<OutputContainer>
Finder::operator()(const std::vector<float>& primary_vertex,
                   const std::vector<std::vector<float>>& track_parameters,
                   const std::vector<std::vector<float>>& covariance_matrix,
                   const std::vector<std::vector<float>>& magnetic_field,
                   const std::vector<int>& pid_vector,
                   const std::vector<int>& is_good_track_)
                   try {
  input_container_.Clear();
  input_container_.SetPV( primary_vertex.at(0), primary_vertex.at(1), primary_vertex.at(2) );
  const auto n_particles = track_parameters.size();
  input_container_.Reserve( n_particles );
  for( int i=0; i<n_particles; ++i ){
    if( is_good_track_.at(i) == 0 )
      continue;
    const auto trk_parameters = track_parameters.at(i);
    const auto trk_cov = covariance_matrix.at(i);
    const auto trk_field = magnetic_field.at(i);
    const auto trk_pid = pid_vector.at(i);

    const auto x = trk_parameters.at(0);
    const auto y = trk_parameters.at(1);
    const auto z = trk_parameters.at(2);
    const auto tx = trk_parameters.at(3);
    const auto ty = trk_parameters.at(4);
    const auto qp = trk_parameters.at(5);

    const auto p = fabsf( 1.0f / qp );
    const auto q = qp * p;
    const auto pz = sqrtf( p*p / ( tx*tx + ty*ty + 1 ) );
    const auto px = tx*pz;
    const auto py = ty*pz;

    const auto J = Utils::CalculateJacobian(trk_parameters);
    const auto new_cov = Utils::CalculateCovariance( trk_cov, J );
    const std::vector<float> par{ x, y, z, px, py, pz };
    input_container_.AddTrack( par, new_cov, trk_field, static_cast<int>(q), trk_pid, i );
  }
  return Find();
} catch ( std::exception &e ){
  std::cout << __func__ << "(): Exception is issued" << "\n";
  throw e;
}

ROOT::RVec<OutputContainer> Finder::Find() try {

  std::unique_ptr<SimpleFinder> finder = std::make_unique<SimpleFinder>();
  finder->SetDecays(decays_);
  finder->Init(input_container_);
  finder->FindParticles();
  auto candidates = ROOT::RVec(finder->GetCandidates());
  return candidates;
} catch ( std::exception &e ){
  std::cout << __func__ << "(): Exception is issued" << "\n";
  throw e;
}
