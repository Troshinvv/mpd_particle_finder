//
// Created by Misha on 3/15/2023.
//
using namespace ROOT;
using namespace ROOT::Math;
using namespace ROOT::RDF;
using fourVector=LorentzVector<PtEtaPhiE4D<double>>;

void rec30_lambda_kshort(std::string list){

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
          .Filter("recoPrimVtxZ<130 && recoPrimVtxZ>-130")
          .Define("centrality", [](ROOT::VecOps::RVec<fourVector> mom, ROOT::VecOps::RVec<int> nhits, ROOT::VecOps::RVec<ROOT::Math::XYZVector> dca){
            float centrality{-1.f};
            std::vector<float> centrality_percentage{ 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
            std::vector<int> multiplicity_edges{ 271, 183, 126, 84, 55, 34, 21, 12, 6, 1, 0 };
            int multiplicity = 0;
            for(int i=0; i< mom.size();i++){
             if(dca.at(i).R()>2 || nhits.at(i)<10 || mom.at(i).Pt()<0.1 || mom.at(i).Eta()>0.5)
                 continue;
             else
               multiplicity++;
            }
            int idx = 0;
            float bin_edge = multiplicity_edges[idx];
            while( multiplicity < bin_edge &&
                   idx < multiplicity_edges.size()-1 ){
              idx++;
              bin_edge = multiplicity_edges[idx];
            }
            centrality = (centrality_percentage[idx-1] + centrality_percentage[idx])/2.0f;
            return centrality;
            }, { "recoGlobalMom","recoGlobalNhits","recoGlobalDca" })
          .Define("pdg_vector", []( ROOT::VecOps::RVec<short> charge){
            std::vector<int> pdg;
            for (int i=0; i<charge.size(); ++i) {
              auto q = charge.at(i);
              if( q > 0 ) {
                pdg.push_back(2212);
                continue;
              }
              if ( q < 0 )
                pdg.push_back(-211);
            }
            return pdg;
            }, {  "recoGlobalCharge" })
            .Define("is_good_track_lambda", [](std::vector<int> pdg_vector,
                                               ROOT::VecOps::RVec<int> nhits){
            std::vector<int> is_good;
            for( int i=0; i<pdg_vector.size(); ++i ) {
              auto pid = pdg_vector.at(i);
              auto nhit = nhits.at(i);
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
            }, { "pdg_vector", "recoGlobalNhits" })
            ;

  Finder finder;
  finder.AddDecay("lambda",
                  MotherConfig().SetPdg(3122),
                  {
    DaughterConfig().SetPdg(-211),
    DaughterConfig().SetPdg(2212),
    });
  auto ddd = dd.Filter("10 < centrality && centrality < 50")
          .Define( "candidates", finder, {"primary_vtx",
                                           "recoKalmanParamVertex",
                                           "recoKalmanCovMtxVertex",
                                           "recoKalmanMagField",
                                           "pdg_vector",
                                           "is_good_track_lambda"} )
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
          .Define("candidate_true_id", Getters::GetTrueId, {
                  "candidates",
                  "simMotherId",
                  "recoGlobalSimIndex",
                  "simPdg"
          } )
          .Define("candidate_origin_flag", Getters::GetFlag, {
                  "candidates",
                  "simMotherId",
                  "recoGlobalSimIndex",
                  "simPdg"
          } )
          ;

  //ddd.Foreach([](ULong64_t evtId){if (evtId % 100 == 0) cout << "\t" << evtId << "\n";}, {"rdfentry_"});
  
  auto fields = ddd.GetColumnNames();
  std::vector<std::string> exclude_fields{"pdg_vector", "candidates", "primary_vtx", "is_good_track_lambda"};
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
