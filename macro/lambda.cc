//
// Created by Misha on 3/15/2023.
//
using namespace ROOT;
using namespace ROOT::Math;
using namespace ROOT::RDF;
using fourVector=LorentzVector<PtEtaPhiE4D<double>>;

void lambda(std::string list){

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
            std::vector<int> multiplicity_edges{ 1000, 0 };
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
            .Define("is_good_track", [](std::vector<int> pdg_vector,
                                        std::vector<int> mId_vector,
                                        ROOT::VecOps::RVec<int> nhits,
                                        ROOT::VecOps::RVec<ROOT::Math::XYZVector> dca_vector){
            std::vector<int> is_good;
            for( int i=0; i<pdg_vector.size(); ++i ) {
              auto pid = pdg_vector.at(i);
              auto nhit = nhits.at(i);
              auto motherId = mId_vector.at(i);
              auto dca = dca_vector.at(i).R();
              // if( motherId == -1 ) {
              //   is_good.push_back(0);
              //   continue;
              // }
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
//  finder.AddDecay("lambda", 3122, {-211, 2212});
  auto ddd = dd.Filter("0 < centrality && centrality < 100")
          .Define( "candidates", finder, {"primary_vtx",
                                           "recoKalmanParamVertex",
                                           "recoKalmanCovMtxVertex",
                                           "recoKalmanMagField",
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
          .Define("daughter_id", Getters::GetDaughterId, {"candidates"} )
          .Define("candidate_true_pid", Getters::GetTruePDG, {
                  "candidates",
                  "simMotherId",
                  "recoGlobalSimIndex",
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

  timer.Stop();
  timer.Print();
}
