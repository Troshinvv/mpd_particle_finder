//
// Created by Misha on 3/21/2023.
//

#include "tree_manager.h"

TreeManager::TreeManager(const std::string &name_out_file) : name_out_file_(name_out_file) {
  file_out_ = std::make_unique<TFile>( name_out_file_.c_str(), "RECREATE"  );
  file_out_->cd();
  tree_out_ = std::make_unique<TTree>( "t", "candidates" );

  tree_out_->Branch( "candidate_momenta", &candidate_momenta_ );
  tree_out_->Branch( "candidate_momentum_errors", &candidate_momentum_errors_ );
  tree_out_->Branch( "candidate_mass", &candidate_mass_ );
  tree_out_->Branch( "candidate_pid", &candidate_pid_ );
  tree_out_->Branch( "candidate_true_pid", &candidate_true_pid_ );
  tree_out_->Branch( "daughter_chi2_prim", &daughter_chi2_prim_ );
  tree_out_->Branch( "candidate_cosines", &candidate_cosines_ );
  tree_out_->Branch( "candidate_cos_topo", &candidate_cos_topo_ );
  tree_out_->Branch( "daughter_dca", &daughter_dca_ );
  tree_out_->Branch( "daughter_distance_to_sv", &daughter_distance_to_sv_ );
  tree_out_->Branch( "candidate_L", &candidate_L_ );
  tree_out_->Branch( "candidate_LdL", &candidate_LdL_ );
  tree_out_->Branch( "primary_to_secondary_vertex_distance", &primary_to_secondary_vertex_distance_ );
  tree_out_->Branch( "chi2_geo", &chi2_geo_ );
  tree_out_->Branch( "chi2_topo", &chi2_topo_ );
  tree_out_->Branch( "cos_topo_", &cos_topo_ );
}

TreeManager::~TreeManager() {
  file_out_->cd();
  tree_out_->Write();
  file_out_->Close();
}
