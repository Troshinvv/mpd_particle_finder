//
// Created by Misha on 4/14/2023.
//

#ifndef BMN_PARTICLE_FINDER_FINDER_H
#define BMN_PARTICLE_FINDER_FINDER_H


#include <utility>
#include <vector>
#include <InputContainer.hpp>
#include <Decay.hpp>
#include <OutputContainer.hpp>
#include <ROOT/RVec.hxx>

class DaughterConfig{
public:
  friend class Finder;
  DaughterConfig() : pdg_(2212), chi2_prim_(-1.0), cos_(-1.0) {}
  DaughterConfig& SetPdg(int pdg) { pdg_ = pdg; return *this; }
  DaughterConfig& SetChi2Prim(float chi_2_prim) { chi2_prim_ = chi_2_prim; return *this; }
  DaughterConfig& SetCos(float cos) { cos_ = cos; return *this; }
private:
  int pdg_;
  float chi2_prim_;
  float cos_;
};

class MotherConfig{
public:
  friend class Finder;
  MotherConfig() : pdg_(2212), L_(-1.0), LdL_(-1.0), cos_topo_(-1.0),
  chi2_geo_(999), chi2_topo_(-1), dca_(999) {}
  MotherConfig& SetPdg(int pdg) { pdg_ = pdg; return *this; }
  MotherConfig& SetL(float l) { L_ = l; return *this; }
  MotherConfig& SetLdL(float ld_l) { LdL_ = ld_l; return *this; }
  MotherConfig& SetCosTopo(float cos_topo) { cos_topo_ = cos_topo; return *this; }
  MotherConfig& SetChi2Geo(float chi_2_geo) { chi2_geo_ = chi_2_geo; return *this; }
  MotherConfig& SetChi2Topo(float chi_2_topo) { chi2_topo_ = chi_2_topo; return *this; }
  MotherConfig& SetDca(float dca) { dca_ = dca; return *this;}
private:
  int pdg_;
  float L_;
  float LdL_;
  float cos_topo_;
  float chi2_topo_;
  float chi2_geo_;
  float dca_;
};

class Finder {
public:
  Finder() = default;
  explicit Finder( std::vector<Decay>  decays ) : decays_(std::move(decays)) {}
  ~Finder() = default;
  void SetDecays(const std::vector<Decay> &decays) {
    decays_ = decays;
  }
  void AddDecay( const std::string& name, int mother_pdg, const std::vector<int>& daughter_pdg ) {
    std::vector<Daughter> daughters;
    for( auto pdg : daughter_pdg ){
      daughters.emplace_back( pdg );
      daughters.back().CancelCuts();
    }
    Mother mother(mother_pdg);
    mother.CancelCuts();
    decays_.emplace_back( name, mother, daughters );
  };
  void AddDecay( const std::string& name, MotherConfig mother_config, const std::vector<DaughterConfig>& daughter_configs ) {
    std::vector<Daughter> daughters;
    for( const auto& conf : daughter_configs ){
      daughters.emplace_back( conf.pdg_ );
      daughters.back().SetCutChi2Prim( conf.chi2_prim_ );
      daughters.back().SetCutCos( conf.cos_ );
    }
    Mother mother(mother_config.pdg_);
    mother.CancelCuts();
    mother.SetCutLdL( mother_config.LdL_ );
    mother.SetCutDecayLength( mother_config.L_ );
    mother.SetCutCosTopo( mother_config.cos_topo_ );
    mother.SetCutChi2Geo( mother_config.chi2_geo_ );
    mother.SetCutChi2Topo( mother_config.chi2_topo_ );
    mother.SetCutDistance( mother_config.dca_ );

    decays_.emplace_back( name, mother, daughters );
  };
  ROOT::RVec<OutputContainer> operator()(const std::vector<float>& primary_vertex,
                                          const std::vector<std::vector<float>>& track_parameters,
                                          const std::vector<std::vector<float>>& covariance_matrix,
                                          const std::vector<std::vector<float>>& magnetic_field,
                                          const std::vector<int>& pid_vector,
                                          const std::vector<int>& is_good_track_);
private:
  ROOT::RVec<OutputContainer> Find();
  std::vector<Decay> decays_;
  InputContainer input_container_{};

};


#endif //BMN_PARTICLE_FINDER_FINDER_H
