//
// Created by Misha on 3/15/2023.
//

void test(std::string list){
  TFileCollection collection( "collection", "", list.c_str() );
  auto* chain = new TChain( "t" );
  chain->AddFileInfoList( collection.GetList() );
  ROOT::RDataFrame d( *chain );

  std::shared_ptr<FindParticles> particles = std::make_shared<FindParticles>();
  particles->AddDecay( "lambda", 3122, {-211, 2212} );

  auto dd = d.Define("primary_vtx", [](double x, double y, double z){
    return std::vector<float>{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
    }, { "vtxX", "vtxY", "vtxZ"})
          .Define("pdg_vector", [](unsigned int event_id, ROOT::VecOps::RVec<float> sim_index, ROOT::VecOps::RVec<int> sim_pdg){
            std::vector<int> pdg;
            for( auto idx : sim_index ) {
              if( idx < 0 ) {
                pdg.push_back(-1);
                continue;
              }
              if( idx > sim_pdg.size() ) {
                pdg.push_back(-1);
                continue;
              }
              pdg.push_back(sim_pdg.at(idx));
            }
//            std::cout << event_id << std::endl;
            return pdg;
            }, { "evtId", "trLength", "simPdg" })
          .Define( "n_candidates", [particles](unsigned int event_id,
                                               std::vector<float>& primary_vertex,
                                               std::vector<std::vector<float>> track_parameters,
                                               std::vector<std::vector<float>> covariance_matrix,
                                               std::vector<std::vector<float>> magnetic_field,
                                               std::vector<int> pid_vector){
            particles->Fill( primary_vertex, track_parameters, covariance_matrix, magnetic_field, pid_vector );
            auto n_particles = particles->Find();
//            std::cout << event_id << ": " << n_particles << std::endl;
            return n_particles;
          }, {"evtId", "primary_vtx", "stsTrackParameters", "stsTrackCovMatrix", "stsTrackMagField", "pdg_vector"} )
          .Define("candidate_p", [particles](){
//            std::cout << "candidate_p" << std::endl;
            return  particles->GetCandidateMomenta();
            }, {} )
          ;
  dd.Foreach( [](unsigned int event_id){ if event_id % 100 == 0 std::cout << event_id << std::endl; }, {"evtId" } );

  dd.Snapshot("t", "candidates.root", dd.GetDefinedColumnNames());
  std::cout << "Bruh" << std::endl;
}