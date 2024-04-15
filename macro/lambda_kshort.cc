//
// Created by Misha on 3/15/2023.
//
using namespace ROOT;
using namespace ROOT::Math;
using namespace ROOT::RDF;
using fourVector=LorentzVector<PtEtaPhiE4D<double>>;

void lambda_kshort(std::string list){

  TStopwatch timer;
  timer.Start();
  
  TFileCollection collection( "collection", "", list.c_str() );
  auto* chain = new TChain( "t" );
  chain->AddFileInfoList( collection.GetList() );
  ROOT::RDataFrame d( *chain );

  auto dd = d
          .Define("primary_vtx", [](float x, float y, float z){
            return std::vector<float>{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
            }, { "recoPrimVtxX", "recoPrimVtxY", "recoPrimVtxZ"})
          .Define("centrality", [](ROOT::VecOps::RVec<fourVector> mom){
            float centrality{-1.f};
            std::vector<float> centrality_percentage{ 10, 100 };
            std::vector<int> multiplicity_edges{ 5000, 0 };
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
            }, { "recoGlobalMom" })
          .Define("pdg_vector", [](ROOT::VecOps::RVec<int> sim_index, ROOT::VecOps::RVec<int> sim_pdg, ROOT::VecOps::RVec<short> charge){
            std::vector<int> pdg;
            for (int i=0; i<sim_index.size(); ++i) {
            //for( auto idx : sim_index ) {
              auto idx = sim_index.at(i);
              auto q = charge.at(i);
              if( idx < 0 ) {
                pdg.push_back(-999);
                continue;
              }
              if( idx >= sim_pdg.size() ) {
                pdg.push_back(-999);
                continue;
              }
              //if ( q < 0 )
              //  pdg.push_back(-211);
              //else
              pdg.push_back(sim_pdg.at(idx));
            }
            return pdg;
            }, { "recoGlobalSimIndex", "simPdg", "recoGlobalCharge" })
            .Define("mId_vector", [](ROOT::VecOps::RVec<int> sim_index, ROOT::VecOps::RVec<int> motherIds) {
             std::vector<int> mId;
             //for ( auto idx : sim_index ) {
             for (int i=0; i<sim_index.size(); ++i) {
               auto idx = sim_index.at(i);
               if (idx < 0) {
                 mId.push_back(-999);
                 continue;
               }
               if (idx >= motherIds.size()) {
                 mId.push_back(-999);
                 continue;
               }
               mId.push_back(motherIds.at(idx));
             }
             return mId;
            }, { "recoGlobalSimIndex", "simMotherId" })
            .Define("recoKalmanParamVertexBgPion", [](ROOT::VecOps::RVec<std::vector<float>> tracks){
              std::vector<std::vector<float>> bg_tracks;
              for (int i=0; i<tracks.size(); ++i){
                auto track = tracks.at(i);
                auto sign = (track.at(6)<0) ? (float)-1. : (float)1.;
                auto bg_track = track;
                bg_track.at(3) *= sign;
                bg_track.at(4) *= sign;
                bg_track.at(5) *= sign;
                bg_tracks.push_back( bg_track );
              }
              return bg_tracks;
            },{"recoKalmanParamVertex"})
            .Define("is_good_track_lambda", [](std::vector<int> pdg_vector,
                                               std::vector<int> mId_vector,
                                               ROOT::VecOps::RVec<int> nhits,
                                               ROOT::VecOps::RVec<ROOT::Math::XYZVector> dca_vector){
            std::vector<int> is_good;
            for( int i=0; i<pdg_vector.size(); ++i ) {
              auto pid = pdg_vector.at(i);
              auto nhit = nhits.at(i);
              auto motherId = mId_vector.at(i);
              auto dca = dca_vector.at(i).R();
              if( motherId == -1 ) {
                is_good.push_back(0);
                continue;
              }
              // if (dca < 20.){
              //   is_good.push_back(0);
              //   continue;
              // }
              if( nhit < 10 ){
                is_good.push_back(0);
                continue;
              }
              if(pid == 2212) {
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
            }, { "pdg_vector", "mId_vector", "recoGlobalNhits", "recoGlobalDca" })
            .Define("is_good_track_kshort", [](std::vector<int> pdg_vector,
                                               std::vector<int> mId_vector,
                                               ROOT::VecOps::RVec<int> nhits,
                                               ROOT::VecOps::RVec<ROOT::Math::XYZVector> dca_vector){
            std::vector<int> is_good;
            for( int i=0; i<pdg_vector.size(); ++i ) {
              auto pid = pdg_vector.at(i);
              auto nhit = nhits.at(i);
              auto motherId = mId_vector.at(i);
              auto dca = dca_vector.at(i).R();
              if( motherId == -1 ) {
                is_good.push_back(0);
                continue;
              }
              // if (dca < 20.){
              //   is_good.push_back(0);
              //   continue;
              // }
              if( nhit < 10 ){
                is_good.push_back(0);
                continue;
              }
              if(pid == 211) {
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
            }, { "pdg_vector", "mId_vector", "recoGlobalNhits", "recoGlobalDca" })
            ;

  Finder finder;
  finder.AddDecay("lambda",
//                  MotherConfig().SetPdg(3122).SetL(2.25).SetLdL(6.25).SetChi2Geo(40).SetDca(0.728),
                  MotherConfig().SetPdg(3122),
                  {
//    DaughterConfig().SetPdg(-211).SetChi2Prim(535),
    DaughterConfig().SetPdg(-211),
//    DaughterConfig().SetPdg(2212).SetChi2Prim(50).SetCos(0.99),
    DaughterConfig().SetPdg(2212),
    });
  finder.AddDecay("kshort",
                  MotherConfig().SetPdg(310),
                  {
                    DaughterConfig().SetPdg(-211),
                    DaughterConfig().SetPdg(211)
                  });
//  finder.AddDecay("lambda", 3122, {-211, 2212});
  auto ddd = dd.Filter("0 < centrality && centrality < 100")
          .Define( "lambda_candidates", finder, {"primary_vtx",
                                           "recoKalmanParamVertex",
                                           "recoKalmanCovMtxVertex",
                                           "recoKalmanMagField",
                                           "pdg_vector",
                                           "is_good_track_lambda"} )
          .Define("lambda_candidate_momenta", Getters::GetMomenta, {"lambda_candidates"} )
          .Define("lambda_candidate_mass", Getters::GetMass, {"lambda_candidates"} )
          .Define("lambda_candidate_momentum_errors", Getters::GetMomentumErr, {"lambda_candidates"} )
          .Define("lambda_candidate_pid", Getters::GetPDG, {"lambda_candidates"} )
          .Define("lambda_daughter_cosines", Getters::GetDaughterCosines, {"lambda_candidates"} )
          .Define("lambda_daughter_chi2_prim", Getters::GetDaughterChi2PV, {"lambda_candidates"} )
          .Define("lambda_candidate_cos_topo", Getters::GetCosineTopo, {"lambda_candidates"} )
          .Define("lambda_daughter_dca", Getters::GetDaughterDCA, {"lambda_candidates"} )
          .Define("lambda_distance_to_sv", Getters::GetDistanceToSV, {"lambda_candidates"} )
          .Define("lambda_candidate_L", Getters::GetL, {"lambda_candidates"} )
          .Define("lambda_candidate_LdL", Getters::GetLdL, {"lambda_candidates"} )
          .Define("lambda_candidate_chi2_geo", Getters::GetChi2Geo, {"lambda_candidates"} )
          .Define("lambda_candidate_chi2_topo", Getters::GetChi2Topo, {"lambda_candidates"} )
          .Define("lambda_daughter_id", Getters::GetDaughterId, {"lambda_candidates"} )
          .Define("lambda_candidate_true_pid", Getters::GetTruePDG, {
                  "lambda_candidates",
                  "simMotherId",
                  "recoGlobalSimIndex",
                  "simPdg"
          } )
          .Define( "lambda_bg_candidates", finder, {"primary_vtx",
                                           "recoKalmanParamVertexBgPion",
                                           "recoKalmanCovMtxVertex",
                                           "recoKalmanMagField",
                                           "pdg_vector",
                                           "is_good_track_lambda"} )
          .Define("lambda_bg_candidate_momenta", Getters::GetMomenta, {"lambda_bg_candidates"} )
          .Define("lambda_bg_candidate_mass", Getters::GetMass, {"lambda_bg_candidates"} )
          .Define("lambda_bg_candidate_momentum_errors", Getters::GetMomentumErr, {"lambda_bg_candidates"} )
          .Define("lambda_bg_candidate_pid", Getters::GetPDG, {"lambda_bg_candidates"} )
          .Define("lambda_bg_daughter_cosines", Getters::GetDaughterCosines, {"lambda_bg_candidates"} )
          .Define("lambda_bg_daughter_chi2_prim", Getters::GetDaughterChi2PV, {"lambda_bg_candidates"} )
          .Define("lambda_bg_candidate_cos_topo", Getters::GetCosineTopo, {"lambda_bg_candidates"} )
          .Define("lambda_bg_daughter_dca", Getters::GetDaughterDCA, {"lambda_bg_candidates"} )
          .Define("lambda_bg_distance_to_sv", Getters::GetDistanceToSV, {"lambda_bg_candidates"} )
          .Define("lambda_bg_candidate_L", Getters::GetL, {"lambda_bg_candidates"} )
          .Define("lambda_bg_candidate_LdL", Getters::GetLdL, {"lambda_bg_candidates"} )
          .Define("lambda_bg_candidate_chi2_geo", Getters::GetChi2Geo, {"lambda_bg_candidates"} )
          .Define("lambda_bg_candidate_chi2_topo", Getters::GetChi2Topo, {"lambda_bg_candidates"} )
          .Define("lambda_bg_daughter_id", Getters::GetDaughterId, {"lambda_bg_candidates"} )
          .Define("lambda_bg_candidate_true_pid", Getters::GetTruePDG, {
                  "lambda_bg_candidates",
                  "simMotherId",
                  "recoGlobalSimIndex",
                  "simPdg"
          } )
          .Define( "kshort_candidates", finder, {"primary_vtx",
                                           "recoKalmanParamVertex",
                                           "recoKalmanCovMtxVertex",
                                           "recoKalmanMagField",
                                           "pdg_vector",
                                           "is_good_track_kshort"} )
          .Define("kshort_candidate_momenta", Getters::GetMomenta, {"kshort_candidates"} )
          .Define("kshort_candidate_mass", Getters::GetMass, {"kshort_candidates"} )
          .Define("kshort_candidate_momentum_errors", Getters::GetMomentumErr, {"kshort_candidates"} )
          .Define("kshort_candidate_pid", Getters::GetPDG, {"kshort_candidates"} )
          .Define("kshort_daughter_cosines", Getters::GetDaughterCosines, {"kshort_candidates"} )
          .Define("kshort_daughter_chi2_prim", Getters::GetDaughterChi2PV, {"kshort_candidates"} )
          .Define("kshort_candidate_cos_topo", Getters::GetCosineTopo, {"kshort_candidates"} )
          .Define("kshort_daughter_dca", Getters::GetDaughterDCA, {"kshort_candidates"} )
          .Define("kshort_distance_to_sv", Getters::GetDistanceToSV, {"kshort_candidates"} )
          .Define("kshort_candidate_L", Getters::GetL, {"kshort_candidates"} )
          .Define("kshort_candidate_LdL", Getters::GetLdL, {"kshort_candidates"} )
          .Define("kshort_candidate_chi2_geo", Getters::GetChi2Geo, {"kshort_candidates"} )
          .Define("kshort_candidate_chi2_topo", Getters::GetChi2Topo, {"kshort_candidates"} )
          .Define("kshort_daughter_id", Getters::GetDaughterId, {"kshort_candidates"} )
          .Define("kshort_candidate_true_pid", Getters::GetTruePDG, {
                  "kshort_candidates",
                  "simMotherId",
                  "recoGlobalSimIndex",
                  "simPdg"
          } )
          .Define( "kshort_bg_candidates", finder, {"primary_vtx",
                                           "recoKalmanParamVertexBgPion",
                                           "recoKalmanCovMtxVertex",
                                           "recoKalmanMagField",
                                           "pdg_vector",
                                           "is_good_track_kshort"} )
          .Define("kshort_bg_candidate_momenta", Getters::GetMomenta, {"kshort_bg_candidates"} )
          .Define("kshort_bg_candidate_mass", Getters::GetMass, {"kshort_bg_candidates"} )
          .Define("kshort_bg_candidate_momentum_errors", Getters::GetMomentumErr, {"kshort_bg_candidates"} )
          .Define("kshort_bg_candidate_pid", Getters::GetPDG, {"kshort_bg_candidates"} )
          .Define("kshort_bg_daughter_cosines", Getters::GetDaughterCosines, {"kshort_bg_candidates"} )
          .Define("kshort_bg_daughter_chi2_prim", Getters::GetDaughterChi2PV, {"kshort_bg_candidates"} )
          .Define("kshort_bg_candidate_cos_topo", Getters::GetCosineTopo, {"kshort_bg_candidates"} )
          .Define("kshort_bg_daughter_dca", Getters::GetDaughterDCA, {"kshort_bg_candidates"} )
          .Define("kshort_bg_distance_to_sv", Getters::GetDistanceToSV, {"kshort_bg_candidates"} )
          .Define("kshort_bg_candidate_L", Getters::GetL, {"kshort_bg_candidates"} )
          .Define("kshort_bg_candidate_LdL", Getters::GetLdL, {"kshort_bg_candidates"} )
          .Define("kshort_bg_candidate_chi2_geo", Getters::GetChi2Geo, {"kshort_bg_candidates"} )
          .Define("kshort_bg_candidate_chi2_topo", Getters::GetChi2Topo, {"kshort_bg_candidates"} )
          .Define("kshort_bg_daughter_id", Getters::GetDaughterId, {"kshort_bg_candidates"} )
          .Define("kshort_bg_candidate_true_pid", Getters::GetTruePDG, {
                  "kshort_bg_candidates",
                  "simMotherId",
                  "recoGlobalSimIndex",
                  "simPdg"
          } )
          ;

  //ddd.Foreach([](ULong64_t evtId){if (evtId % 100 == 0) cout << "\t" << evtId << "\n";}, {"rdfentry_"});
  
  auto fields = ddd.GetColumnNames();
  std::vector<std::string> exclude_fields{"centrality", "mId_vector", "pdg_vector", "lambda_candidates", "kshort_candidates", "lambda_bg_candidates", "kshort_bg_candidates", "recoKalmanParamVertexBgPion", "primary_vtx", "primary_vtx", "is_good_track_lambda", "is_good_track_kshort"};
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

  timer.Stop();
  timer.Print();
}
