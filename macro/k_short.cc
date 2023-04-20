//
// Created by Misha on 3/15/2023.
//


void k_short(std::string list){
  TFileCollection collection( "collection", "", list.c_str() );
  auto* chain = new TChain( "t" );
  chain->AddFileInfoList( collection.GetList() );
  ROOT::RDataFrame d( *chain );

  auto dd = d
          .Define("primary_vtx", [](double x, double y, double z){
            return std::vector<float>{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
            }, { "vtxX", "vtxY", "vtxZ"})
          .Define("centrality", [](ROOT::VecOps::RVec<float> mom){
            float centrality{-1.f};
            std::vector<float> centrality_percentage{ 0, 5, 10, 15, 20, 25, 30, 35, 40, 50, 60, 70, 80, 90, 100 };
            std::vector<int> multiplicity_edges{ 249, 118, 102, 89, 77, 67, 57, 49, 42, 29, 20, 13, 8, 4, 0 };
            auto multiplicity = mom.size();
            int idx = 0;
            float bin_edge = multiplicity_edges[idx];
            while( multiplicity < bin_edge &&
                   idx < multiplicity_edges.size()-1 ){
              idx++;
              bin_edge = multiplicity_edges[idx];
            }
            centrality = (centrality_percentage[idx-1] + centrality_percentage[idx])/2.0f;
            return centrality;
            }, { "trP" })
          .Define("pdg_vector", [](ROOT::VecOps::RVec<int> sim_index, ROOT::VecOps::RVec<int> sim_pdg){
            std::vector<int> pdg;
            for( auto idx : sim_index ) {
              if( idx < 0 ) {
                pdg.push_back(-999);
                continue;
              }
              if( idx >= sim_pdg.size() ) {
                pdg.push_back(-999);
                continue;
              }
              pdg.push_back(sim_pdg.at(idx));
            }
            return pdg;
            }, { "trSimIndex", "simPdg" })
            .Define("is_good_track", [](std::vector<int> pdg_vector){
            std::vector<int> is_good;
            for( auto pid : pdg_vector ) {
              if( pid == 211 ) {
                is_good.push_back(1);
                continue;
              }
              if( pid == -211 ) {
                is_good.push_back(1);
                continue;
              }
              is_good.push_back(0);
            }
            return is_good;
            }, { "pdg_vector" })
            ;

  Finder finder;
  finder.AddDecay("k_short", 310, {-211, 211});
  auto ddd = dd
          .Define( "candidates", finder, {"primary_vtx",
                                           "stsTrackParameters",
                                           "stsTrackCovMatrix",
                                           "stsTrackMagField",
                                           "pdg_vector",
                                           "is_good_track"} )
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
          .Define( "candidate_true_pid", Getters::GetTruePDG, {
                  "candidates",
                  "simMotherId",
                  "trSimIndex",
                  "simPdg"
          } )
          ;
  auto fields = ddd.GetColumnNames();
  std::vector<std::string> exclude_fields{"candidates", "primary_vtx", "primary_vtx", "is_good_track"};
  std::vector<std::string> white_list{};
  for(const auto& field : fields){
    if( std::find(exclude_fields.begin(), exclude_fields.end(), field) != exclude_fields.end() )
      continue;
    if( auto dot_pos = field.find("."); dot_pos != std::string::npos )
      continue;
    white_list.push_back( field );
  }
  std::cout << "Fields to be written to the output tree: " << "\n";
  for( auto field : white_list )
    std::cout << field << "\n";
  ddd.Snapshot("t", "candidates.root", white_list );

}