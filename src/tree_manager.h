//
// Created by Misha on 3/21/2023.
//

#ifndef BMN_PARTICLE_FINDER_TREE_MANAGER_H
#define BMN_PARTICLE_FINDER_TREE_MANAGER_H


#include <string>
#include <TFile.h>
#include <TTree.h>
#include <Math/Vector4D.h>
#include <Math/LorentzVector.h>

class TreeManager {
public:
  explicit TreeManager(std::string name_out_file);
  virtual ~TreeManager();

  void SetCandidateMomenta(const std::vector<ROOT::Math::PtEtaPhiMVector> &candidate_momenta) {
    candidate_momenta_ = candidate_momenta;
  }

  void SetCandidateMomentumErrors(const std::vector<std::vector<float>> &candidate_momentum_errors) {
    candidate_momentum_errors_ = candidate_momentum_errors;
  }

  void SetCandidateMass(const std::vector<float> &candidate_mass) {
    candidate_mass_ = candidate_mass;
  }

  void SetCandidatePid(const std::vector<int> &candidate_pid) {
    candidate_pid_ = candidate_pid;
  }

  void SetCandidateTruePid(const std::vector<int> &candidate_true_pid) {
    candidate_true_pid_ = candidate_true_pid;
  }

  void SetCandidateCosines(const std::vector<std::vector<float>> &candidate_cosines) {
    candidate_cosines_ = candidate_cosines;
  }

  void SetCandidateCosTopo(const std::vector<std::vector<float>> &candidate_cos_topo) {
    candidate_cos_topo_ = candidate_cos_topo;
  }

  void SetDaughterDca(const std::vector<double> &daughter_dca) {
    daughter_dca_ = daughter_dca;
  }

  void SetDaughterDistanceToSv(const std::vector<double> &daughter_distance_to_sv) {
    daughter_distance_to_sv_ = daughter_distance_to_sv;
  }

  void SetCandidateL(const std::vector<double> &candidate_l) {
    candidate_L_ = candidate_l;
  }

  void SetCandidateLdL(const std::vector<double> &candidate_ld_l) {
    candidate_LdL_ = candidate_ld_l;
  }

  void SetPrimaryToSecondaryVertexDistance(const std::vector<double> &primary_to_secondary_vertex_distance) {
    primary_to_secondary_vertex_distance_ = primary_to_secondary_vertex_distance;
  }

  void SetChi2Geo(const std::vector<std::vector<float>> &chi_2_geo) {
    chi2_geo_ = chi_2_geo;
  }

  void SetChi2Topo(const std::vector<std::vector<float>> &chi_2_topo) {
    chi2_topo_ = chi_2_topo;
  }

  void SetCosTopo(const std::vector<std::vector<float>> &cos_topo) {
    cos_topo_ = cos_topo;
  }

  void SetDaughterChi2Prim(const std::vector<std::vector<float>> &daughter_chi_2_prim) {
    daughter_chi2_prim_ = daughter_chi_2_prim;
  }

  void SetDaughter1Momenta(const std::vector<ROOT::Math::PtEtaPhiMVector> &daughter_1_momenta) {
    daughter1_momenta_ = daughter_1_momenta;
  }

  void SetDaughter2Momenta(const std::vector<ROOT::Math::PtEtaPhiMVector> &daughter_2_momenta) {
    daughter2_momenta_ = daughter_2_momenta;
  }

  void SetDaughterPid(const std::vector<std::vector<int>> &daughter_pid) {
    daughter_pid_ = daughter_pid;
  }

  void SetCentrality(float centrality) {
    centrality_ = centrality;
  }

  void SetPrimaryVertex(const std::vector<float> &primary_vertex) {
    primary_vertex_ = primary_vertex;
  }

  void Fill(){
    tree_out_->Fill();
  }

private:
  std::string name_out_file_;
  TFile* file_out_{nullptr};
  TTree* tree_out_{nullptr};

  float centrality_{};
  std::vector<float> primary_vertex_{};
  std::vector<ROOT::Math::PtEtaPhiMVector> candidate_momenta_{};
  std::vector<ROOT::Math::PtEtaPhiMVector> daughter1_momenta_{};
  std::vector<ROOT::Math::PtEtaPhiMVector> daughter2_momenta_{};
  std::vector<std::vector<int>> daughter_pid_{};
  std::vector<std::vector<float>> candidate_momentum_errors_{};
  std::vector<float> candidate_mass_{};
  std::vector<int> candidate_pid_{};
  std::vector<int> candidate_true_pid_{};
  std::vector<std::vector<float>> daughter_chi2_prim_{};
  std::vector<std::vector<float>> candidate_cosines_{};
  std::vector<std::vector<float>> candidate_cos_topo_{};
  std::vector<double> daughter_dca_{};
  std::vector<double> daughter_distance_to_sv_{};
  std::vector<double> candidate_L_{};
  std::vector<double> candidate_LdL_{};
  std::vector<double> primary_to_secondary_vertex_distance_{};
  std::vector<std::vector<float>> chi2_geo_{};
  std::vector<std::vector<float>> chi2_topo_{};
  std::vector<std::vector<float>> cos_topo_{};
};


#endif //BMN_PARTICLE_FINDER_TREE_MANAGER_H
