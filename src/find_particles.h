//
// Created by Misha on 3/15/2023.
//

#ifndef BMN_PARTICLE_FINDER_FIND_PARTICLES_H
#define BMN_PARTICLE_FINDER_FIND_PARTICLES_H

#include <utility>
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

#include <ROOT/RDataFrame.hxx>
#include <Math/LorentzVector.h>
#include <Math/PtEtaPhiM4D.h>
#include <Math/Cartesian3D.h>
#include <ROOT/RVec.hxx>
#include <ROOT/RDataFrame.hxx>
#include <Math/GenVector/PtEtaPhiE4D.h>
#include <TFile.h>
#include <TTree.h>
#include <TDatabasePDG.h>

#include "finder.h"
#include "getters.h"

//template<typename DataFrame>
class FindParticles {
public:
  FindParticles( ROOT::RDF::RInterface<ROOT::Detail::RDF::RLoopManager, void> data_frame,
                          std::string  out_file_name_ )  : data_frame_(std::move(data_frame)), out_file_name_(std::move(out_file_name_)) {
  };
  ~FindParticles() = default;

  void AddDecay( const std::string& name, int mother_pdg, const std::vector<int>& daughter_pdg ) {
    std::vector<Daughter> daughters;
    for( auto pdg : daughter_pdg ){
      daughters.emplace_back( pdg );
      daughters.back().SetCutChi2Prim(18.42);
    }
    Mother mother(mother_pdg);
    mother.SetCutChi2Geo(3);
    mother.SetCutDistance(1);
    mother.SetCutLdL(5);
    decays_.emplace_back( name, mother, daughters );
  };
  void InitFields(){
    finder_.SetDecays( decays_ );
    data_frame_ = data_frame_
    .Define( "candidates", finder_, {primary_vertex_field_,
                                    track_parameters_field_,
                                    covariance_matrix_field_,
                                    magnetic_field_field_,
                                    pid_vector_field_,
                                    is_good_track_field_} )
    .Define("candidate_momenta", Getters::GetMomenta, {"candidates"} )
    .Define("candidate_mass", Getters::GetMass, {"candidates"} )
    .Define("candidate_momentum_errors", Getters::GetMomentumErr, {"candidates"} )
    .Define("candidate_pid", Getters::GetPDG, {"candidates"} )
    .Define("daughter_cosines", Getters::GetDaughterCosines, {"candidates"} )
    .Define("daughter_chi2_prim", Getters::GetDaughterChi2PV, {"candidates"} )
    .Define("candidate_cos_topo", Getters::GetCosineTopo, {"candidates"} )
    .Define("daughter_dca", Getters::GetDaughterDCA, {"candidates"} )
    .Define("distance_to_sv", Getters::GetDistanceToSV, {"candidates"} )
    .Define("candidate_L", Getters::GetL, {"candidates"} )
    .Define("candidate_LdL", Getters::GetLdL, {"candidates"} )
    .Define("candidate_chi2_geo", Getters::GetChi2Geo, {"candidates"} )
    .Define("candidate_chi2_topo", Getters::GetChi2Topo, {"candidates"} )
    ;
    if( !mother_id_field_.empty() )
      data_frame_ = data_frame_.Define( "candidate_true_pid", Getters::GetTruePDG, {
        "candidates",
        mother_id_field_,
        sim_id_field_,
        sim_pid_field_
      } );
  }
  void DumpTree(std::vector<std::string> exclude_fields = {}){
    auto fields = data_frame_.GetColumnNames();
    exclude_fields.emplace_back("candidates");
    std::vector<std::string> white_list{};
    for(const auto& field : fields){
      if( std::find(exclude_fields.begin(), exclude_fields.end(), field) != exclude_fields.end() )
        continue;
      if( auto dot_pos = field.find("."); dot_pos != std::string::npos )
        continue;
      white_list.push_back( field );
    }
    for( auto field : white_list )
      std::cout << field << "\n";
    data_frame_.Snapshot("t", out_file_name_, white_list );
  }

  void SetPrimaryVertexField(const std::string &primary_vertex_field) {
    primary_vertex_field_ = primary_vertex_field;
  }

  void SetTrackParametersField(const std::string &track_parameters_field) {
    track_parameters_field_ = track_parameters_field;
  }

  void SetCovarianceMatrixField(const std::string &covariance_matrix_field) {
    covariance_matrix_field_ = covariance_matrix_field;
  }

  void SetMagneticFieldField(const std::string &magnetic_field_field) {
    magnetic_field_field_ = magnetic_field_field;
  }

  void SetPidVectorField(const std::string &pid_vector_field) {
    pid_vector_field_ = pid_vector_field;
  }

  void SetIsGoodTrackField(const std::string &is_good_track_field) {
    is_good_track_field_ = is_good_track_field;
  }

  void SetMotherIdField(const std::string &mother_id_field) {
    mother_id_field_ = mother_id_field;
  }

  void SetSimIdField(const std::string &sim_id_field) {
    sim_id_field_ = sim_id_field;
  }

  void SetSimPidField(const std::string &sim_pid_field) {
    sim_pid_field_ = sim_pid_field;
  }

private:
  ROOT::RDF::RInterface<ROOT::Detail::RDF::RLoopManager, void> data_frame_;
  std::string out_file_name_;
//  TreeManager tree_manager_;
  Finder finder_{};
  std::string primary_vertex_field_{};
  std::string track_parameters_field_{};
  std::string covariance_matrix_field_{};
  std::string magnetic_field_field_{};
  std::string pid_vector_field_{};
  std::string is_good_track_field_{};
  std::string mother_id_field_{};
  std::string sim_id_field_{};
  std::string sim_pid_field_{};

  std::vector<Decay> decays_{};
  InputContainer input_container_{};
  std::vector<OutputContainer> candidates_{};
};


#endif //BMN_PARTICLE_FINDER_FIND_PARTICLES_H
