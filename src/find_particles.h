//
// Created by Misha on 3/15/2023.
//

#ifndef BMN_PARTICLE_FINDER_FIND_PARTICLES_H
#define BMN_PARTICLE_FINDER_FIND_PARTICLES_H

#include <vector>
#include <array>
#include <cmath>
#include <iostream>

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
#include <TFile.h>
#include <TTree.h>
#include <TDatabasePDG.h>

#include "utils.h"
#include "tree_manager.h"

class FindParticles {
public:
  explicit FindParticles(const std::string& out_file_name_ );
  ~FindParticles();

  void AddDecay( const std::string& name, int mother_pdg, std::vector<int> daughter_pdg );
  void inline Clear();
  void Fill(std::vector<float> primary_vertex,
            std::vector<std::vector<float>> track_parameters,
            std::vector<std::vector<float>> covariance_matrix,
            std::vector<std::vector<float>> magnetic_field,
            std::vector<int> pid_vector,
            std::vector<bool> is_good_track_={});
  size_t Find();
  std::vector<float> GetCandidateMass();
  std::vector<ROOT::Math::PtEtaPhiMVector> GetCandidateMomenta();
  std::vector<std::vector<float>> GetCandidateMomentumErr();
  std::vector<int> GetCandidatePDG();
  std::vector<int> GetIsTrue(ROOT::VecOps::RVec<int> mother_ids,
                             ROOT::VecOps::RVec<int> sim_ids,
                             ROOT::VecOps::RVec<int> sim_pid);
  std::vector<std::vector<float>> GetCandidateCosines();
  std::vector<std::vector<float>> GetCandidateCosTopo();
  std::vector<std::vector<float>> GetDaughterChi2Prim();
  std::vector<double> GetDaughterDCA();
  std::vector<double> GetDaughterDistanceToSV();
  std::vector<double> GetCandidateL();
  std::vector<double> GetCandidateLdL();
  std::vector<double> GetPrimaryToSecondaryVertexDistance();
  std::vector<std::vector<float>> GetChi2Geo();
  std::vector<std::vector<float>> GetChi2Topo();
  std::vector<std::vector<float>> GetCosTopo();
  void WriteDaughterInfo(std::vector<std::vector<float>> track_parameters, std::vector<int> pid_vector);
  void inline SetDecays(const std::vector<Decay> &decays);
  void FillOutTree(){
    tree_manager_.Fill();
  }
private:
  TreeManager tree_manager_;

  std::vector<Decay> decays_;
  InputContainer input_container_;
  std::vector<OutputContainer> candidates_;
};


#endif //BMN_PARTICLE_FINDER_FIND_PARTICLES_H
