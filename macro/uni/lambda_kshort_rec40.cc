//
// Created by Misha on 3/15/2023.
//
using namespace ROOT;
using namespace ROOT::Math;
using namespace ROOT::RDF;
using fourVector=LorentzVector<PtEtaPhiE4D<double>>;

void lambda_kshort_rec40(std::string list){

  TStopwatch timer;
  timer.Start();
  TFileCollection collection( "collection", "", list.c_str() );
  auto* chain = new TChain( "t" );
  chain->AddFileInfoList( collection.GetList() );
  ROOT::RDataFrame d( *chain );
std::vector<float> cent_bins{2.5, 7.5, 12.5, 17.5, 22.5, 27.5, 32.5, 37.5, 42.5, 47.5, 52.5, 57.5, 62.5, 67.5, 72.5, 77.5, 82.5, 87.5, 95.};
std::vector<int> cent_mult{202, 132, 112, 95, 81, 68, 57, 47, 39, 32, 26, 21, 16, 12, 9, 6, 4, 2, 1};
  std::vector<float> cent_b{ 1.43172, 2.87406, 4.05374, 5.0333, 5.86304, 6.58249, 7.22197, 7.80409, 8.34523, 8.8571, 9.34824, 9.8255, 10.2956, 10.7666, 11.2494, 11.7595, 12.3179, 12.9533, 13.7034};
  auto dd = d
          .Define("primary_vtx", [](float x, float y, float z){
            return std::vector<float>{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
            }, { "recoPrimVtxX", "recoPrimVtxY", "recoPrimVtxZ"})
          .Filter("recoPrimVtxZ<130 && recoPrimVtxZ>-130")
	  .Define( "simPt", " std::vector<float> pT; for( auto mom : simMom ){ pT.push_back( mom.Pt() ); } return pT; " )
	  .Define( "refMult", [](vector<fourVector> _p, RVec<int> _nhits){
      int Mult = 0;
      for (int i=0; i<_p.size(); ++i) {
        auto mom = _p.at(i);
        auto nhits = _nhits.at(i);
        if (nhits<=16) continue;
        if(mom.Eta()>0. && mom.Eta()<2.)
          Mult++;
      }
      return Mult;
    },{"recoGlobalMom", "recoGlobalNhits"})
    .Define( "centrality", [cent_mult,cent_bins](int refmult){
      float cent = -1.;
      if (cent_mult.size() == 0) return (float)-1.;
      if (cent_bins.size() == 0) return (float)-1.;
      if (refmult >= cent_mult.at(0))
        cent = cent_bins.at(0);
      for (int i=1; i<cent_mult.size(); ++i){
        if (refmult >= cent_mult.at(i) && refmult < cent_mult.at(i-1))
          cent = cent_bins.at(i);
      }
      return cent;
    },{"refMult"})
          .Define("pdg_vector", []( ROOT::VecOps::RVec<short> charge,ROOT::VecOps::RVec<int> sim_pdg,ROOT::VecOps::RVec<int> sim_index){
            std::vector<int> pdg;
            for (int i=0; i<charge.size(); ++i) {
              auto q = charge.at(i);
              if( q > 0 ) {
                pdg.push_back(2212);
                continue;
              }
              if ( q < 0 )
                pdg.push_back(-211);
	/*	if(sim_index.at(i)<0){
			pdg.push_back(-999);
			continue;
			}
		if(sim_index.at(i)>=sim_pdg.size()){
			pdg.push_back(-999);
			continue;
			}
		pdg.push_back(sim_pdg.at(sim_index.at(i)));
*/
            }
            return pdg;
            }, {  "recoGlobalCharge","simPdg","recoGlobalSimIndex" })
            .Define("is_good_track_lambda", [](std::vector<int> pdg_vector,
                                               ROOT::VecOps::RVec<int> nhits){
            std::vector<int> is_good;
            for( int i=0; i<pdg_vector.size(); ++i ) {
              auto pid = pdg_vector.at(i);
              auto nhit = nhits.at(i);
              if( nhit < 16 ){
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
  auto ddd = dd.Define( "candidates", finder, {"primary_vtx",
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
                  "simPdg",
		  "simMatchMotherIdSize"
          } )
          .Define("candidate_true_id", Getters::GetTrueId, {
                  "candidates",
                  "simMotherId",
                  "recoGlobalSimIndex",
                  "simPdg",
		  "simMatchMotherIdSize"
          } )
          .Define("candidate_origin_flag", Getters::GetFlag, {
                  "candidates",
                  "simMotherId",
                  "recoGlobalSimIndex",
                  "simPdg",
		  "simMatchMotherIdSize"
          } )
	  .Define("candidate_UniGen_flag", Getters::GetUniGenFlag, {
                  "candidates",
                  "simMotherId",
                  "recoGlobalSimIndex",
                  "simPdg",
		  "AssocMcMap",
		  "simMatchMotherIdSize"
          } )
	  .Define("candidate_phiprot", Getters::GetPhiProt,{
			"candidates",
			"candidate_momenta",
			"recoGlobalMom",
			"simMom",
			"recoGlobalSimIndex"
		})
 	  .Define("candidate_true_pt", Getters::GetTruepT, {
                  "candidates",
                  "simMotherId",
                  "recoGlobalSimIndex",
                  "simPt",
		  "simMatchMotherIdSize"
          } ) 
	  .Define("candidate_thetaprot", Getters::GetThetaProt,{
                        "candidates",
                        "candidate_momenta",
                        "recoGlobalMom",
                        "simMom",
                        "recoGlobalSimIndex"
                })
	 .Define("candidate_mc_phiprot", Getters::GetMCPhiProt,{
                        "candidates",
                        "candidate_momenta",
                        "recoGlobalMom",
                        "simMom",
                        "recoGlobalSimIndex"
                })
	 .Define("candidate_mc_thetaprot", Getters::GetMCThetaProt,{
                        "candidates",
                        "candidate_momenta",
                        "recoGlobalMom",
                        "simMom",
                        "recoGlobalSimIndex"
                })
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
