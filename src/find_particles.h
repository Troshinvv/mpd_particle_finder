//
// Created by Misha on 3/15/2023.
//

#ifndef BMN_PARTICLE_FINDER_FIND_PARTICLES_H
#define BMN_PARTICLE_FINDER_FIND_PARTICLES_H

#include <vector>
#include <array>
#include <cmath>

#include <Decay.hpp>
#include <Daughter.hpp>
#include <Mother.hpp>
#include <InputContainer.hpp>
#include <OutputContainer.hpp>
#include <SimpleFinder.hpp>

#include <Math/LorentzVector.h>
#include <Math/PtEtaPhiM4D.h>
#include <Math/Cartesian3D.h>
#include <ROOT/RVec.hxx>
#include <Math/GenVector/PtEtaPhiE4D.h>

#include "utils.h"

class FindParticles {
public:
  FindParticles() = default;
  FindParticles(const FindParticles& ) = default;
  FindParticles(FindParticles&& ) = default;
  ~FindParticles();
  FindParticles& operator=(FindParticles&&) = default;
  FindParticles& operator=(const FindParticles&) = default;

  void AddDecay( const std::string& name, int mother_pdg, std::vector<int> daughter_pdg );
  void inline Clear();
  void Fill(std::vector<float> primary_vertex,
            std::vector<std::vector<float>> track_parameters,
            std::vector<std::vector<float>> covariance_matrix,
            std::vector<std::vector<float>> magnetic_field,
            std::vector<int> pid_vector);
  size_t Find();
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<double>>> GetCandidateMomenta();
  std::vector<std::vector<float>> GetCandidateMomentumErr();
  std::vector<int> GetCandidatePDG();
  std::vector<std::vector<float>> GetCandidateCosines();
  std::vector<std::vector<float>> GetCandidateCosTopo();
  std::vector<double> GetDaughterDCA();
  std::vector<double> GetDaughterDistanceToSV();
  std::vector<double> GetCandidateL();
  std::vector<double> GetCandidateLdL();
  std::vector<double> GetPrimaryToSecondaryVertexDistance();
  std::vector<std::vector<float>> GetChi2Geo();
  std::vector<std::vector<float>> GetChi2Topo();
  std::vector<std::vector<float>> GetCosTopo();
  void inline SetDecays(const std::vector<Decay> &decays);

private:
  std::vector<Decay> decays_;
  InputContainer input_container_;
  std::vector<OutputContainer> candidates_;
};


#endif //BMN_PARTICLE_FINDER_FIND_PARTICLES_H
