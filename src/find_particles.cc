//
// Created by Misha on 3/15/2023.
//

#include "find_particles.h"

FindParticles::FindParticles(const std::string& out_file_name_ ) : tree_manager_(out_file_name_) {}

void FindParticles::AddDecay( const std::string& name, int mother_pdg, std::vector<int> daughter_pdg){
  std::vector<Daughter> daughters;
  for( auto pdg : daughter_pdg ){
    daughters.emplace_back( pdg );
//    daughters.back().SetCutChi2Prim(18.42);
  }
  Mother mother(mother_pdg);
//  mother.SetCutChi2Geo(3);
//  mother.SetCutDistance(1);
//  mother.SetCutLdL(5);
  decays_.emplace_back( name, mother, daughters );
}

void FindParticles::Fill(std::vector<float> primary_vertex,
          std::vector<std::vector<float>> track_parameters,
          std::vector<std::vector<float>> covariance_matrix,
          std::vector<std::vector<float>> magnetic_field,
          std::vector<int> pid_vector,
          std::vector<bool> is_good_track_){
  Clear();
  input_container_.SetPV( primary_vertex.at(0), primary_vertex.at(1), primary_vertex.at(2) );
  auto n_particles = track_parameters.size();
  input_container_.Reserve( n_particles );
  for( int i=0; i<n_particles; ++i ){
    if( !is_good_track_.empty() ){
      if( !is_good_track_.at(i) )
        continue;
    }
    auto trk_parameters = track_parameters.at(i);
    auto trk_cov = covariance_matrix.at(i);
    auto trk_field = magnetic_field.at(i);
    auto trk_pid = pid_vector.at(i);

    auto x = trk_parameters.at(0);
    auto y = trk_parameters.at(1);
    auto z = trk_parameters.at(2);
    auto tx = trk_parameters.at(3);
    auto ty = trk_parameters.at(4);
    auto qp = trk_parameters.at(5);

    auto p = fabsf( 1.0f / qp );
    auto q = qp * p;
    auto pz = sqrtf( p*p / ( tx*tx + ty*ty + 1 ) );
    auto px = tx*pz;
    auto py = ty*pz;

    auto J = Utils::CalculateJacobian(trk_parameters);
    auto new_cov = Utils::CalculateCovariance( trk_cov, J );
    std::vector<float> par{ x, y, z, px, py, pz };
    input_container_.AddTrack( par, new_cov, trk_field, static_cast<int>(q), trk_pid, i );
  }
//  out_tree_->Fill();
}

size_t FindParticles::Find(){
  std::unique_ptr<SimpleFinder> finder = std::make_unique<SimpleFinder>();
  finder->SetDecays(decays_);
  finder->Init(input_container_);
  finder->FindParticles();
  candidates_ = finder->GetCandidates();
  return candidates_.size();
}
std::vector<ROOT::Math::PtEtaPhiMVector> FindParticles::GetCandidateMomenta(){
  if( candidates_.empty() )
    Find();
  std::vector<ROOT::Math::PtEtaPhiMVector> momenta;
  for( const auto& candidate : candidates_ ){
    auto m = candidate.GetMass();
    auto px = candidate.GetPx();
    auto py = candidate.GetPy();
    auto pz = candidate.GetPz();
    auto pT = sqrtf( px*px + py*py );
    auto phi = atan2f( py, px );
    auto theta = atan2f( pT, pz);
    auto eta = -logf( tanf( theta/2.0f ) );
    momenta.emplace_back( pT, eta, phi, m );
  }
  tree_manager_.SetCandidateMomenta(momenta);
  return momenta;
}

std::vector<float> FindParticles::GetCandidateMass(){
  if( candidates_.empty() )
    Find();
  std::vector<float> mass;
  for( const auto& candidate : candidates_ ){
    auto m = candidate.GetMass();
    mass.push_back( m );
  }
  tree_manager_.SetCandidateMass(mass);
  return mass;
}

std::vector<std::vector<float>> FindParticles::GetCandidateMomentumErr(){
  if( candidates_.empty() )
    Find();
  std::vector<std::vector<float>> errors;
  for( const auto& candidate : candidates_ ){
    auto pT_err = candidate.GetPtError();
    auto phi_err = candidate.GetPhiError();
    auto eta_err = candidate.GetEtaError();
    auto mass_err = candidate.GetMassError();
    errors.push_back({pT_err, phi_err, eta_err, mass_err});
  }
  tree_manager_.SetCandidateMomentumErrors(errors);
  return errors;
}
std::vector<int> FindParticles::GetCandidatePDG(){
  if( candidates_.empty() )
    Find();
  std::vector<int> pdg;
  for( const auto& candidate : candidates_ ){
    pdg.push_back( candidate.GetPdg() );
  }
  tree_manager_.SetCandidatePid(pdg);
  return pdg;
}

std::vector<std::vector<float>> FindParticles::GetCandidateCosines(){
  if( candidates_.empty() )
    Find();
  std::vector<std::vector<float>> cosines{3, std::vector<float>{}};
  for( const auto& candidate : candidates_ ){
    for( int i=0; i<3; ++i )
      cosines.at(i).push_back(candidate.GetCos(i));
  }
  tree_manager_.SetCandidateCosines(cosines);
  return cosines;
}
std::vector<std::vector<float>> FindParticles::GetCandidateCosTopo(){
  if( candidates_.empty() )
    Find();
  std::vector<std::vector<float>> cosines{3, std::vector<float>{}};
  for( const auto& candidate : candidates_ ){
    for( int i=0; i<3; ++i )
      cosines.at(i).push_back(candidate.GetCosineTopo(i));
  }
  tree_manager_.SetCandidateCosTopo(cosines);
  return cosines;
}
std::vector<double> FindParticles::GetDaughterDCA(){
  if( candidates_.empty() )
    Find();
  std::vector<double> dca;
  for( const auto& candidate : candidates_ ){
    dca.emplace_back( candidate.GetDistance() );
  }
  tree_manager_.SetDaughterDca(dca);
  return dca;
}
std::vector<double> FindParticles::GetDaughterDistanceToSV(){
  if( candidates_.empty() )
    Find();
  std::vector<double> distance;
  for( const auto& candidate : candidates_ ){
    distance.emplace_back(candidate.GetDistanceToSV() );
  }
  tree_manager_.SetDaughterDistanceToSv(distance);
  return distance;
}
std::vector<double> FindParticles::GetCandidateL(){
  if( candidates_.empty() )
    Find();
  std::vector<double> distance;
  for( const auto& candidate : candidates_ ){
    distance.emplace_back(candidate.GetL() );
  }
  tree_manager_.SetCandidateL(distance);
  return distance;
}
std::vector<double> FindParticles::GetCandidateLdL(){
  if( candidates_.empty() )
    Find();
  std::vector<double> distance;
  for( const auto& candidate : candidates_ ){
    distance.emplace_back(candidate.GetLdL() );
  }
  tree_manager_.SetCandidateLdL(distance);
  return distance;
}
std::vector<double> FindParticles::GetPrimaryToSecondaryVertexDistance(){
  if( candidates_.empty() )
    Find();
  std::vector<double> distance;
  for( const auto& candidate : candidates_ ){
    distance.emplace_back( candidate.GetDistanceToSV() );
  }
  tree_manager_.SetPrimaryToSecondaryVertexDistance(distance);
  return distance;
}
std::vector<std::vector<float>> FindParticles::GetChi2Geo(){
  if( candidates_.empty() )
    Find();
  std::vector<std::vector<float>> chi2(4, std::vector<float>{});
  for( const auto& candidate : candidates_ ){
    for( int i=0; i<4; ++i )
      chi2.at(i).push_back(candidate.GetChi2Geo(i));
  }
  tree_manager_.SetChi2Geo(chi2);
  return chi2;
}
std::vector<std::vector<float>> FindParticles::GetChi2Topo(){
  if( candidates_.empty() )
    Find();
  std::vector<std::vector<float>> chi2(4, std::vector<float>{});
  for( const auto& candidate : candidates_ ){
    chi2.emplace_back();
    for( int i=0; i<4; ++i )
      chi2.at(i).push_back(candidate.GetChi2Topo(i));
  }
  tree_manager_.SetChi2Topo(chi2);
  return chi2;
}
std::vector<std::vector<float>> FindParticles::GetCosTopo(){
  if( candidates_.empty() )
    Find();
  std::vector<std::vector<float>> cosine(4, std::vector<float>{});
  for( const auto& candidate : candidates_ ){
    cosine.emplace_back();
    for( int i=0; i<4; ++i )
      cosine.at(i).push_back(candidate.GetCosineTopo(i));
  }
  tree_manager_.SetCosTopo(cosine);
  return cosine;
}
void FindParticles::Clear(){
  input_container_.Clear();
  candidates_.clear();
}

void FindParticles::SetDecays(const std::vector<Decay> &decays) {
  decays_ = decays;
}

FindParticles::~FindParticles() {
  decays_.clear();
  input_container_.Clear();
  candidates_.clear();
}

std::vector<int> FindParticles::GetIsTrue(ROOT::VecOps::RVec<int> mother_ids,
                                          ROOT::VecOps::RVec<int> sim_ids,
                                          ROOT::VecOps::RVec<int> sim_pid) {
  std::vector<int> true_pdg;
  for( const auto& cand : candidates_ ){
    auto daughters_id = cand.GetDaughterIds();

    auto daughter1 = daughters_id.at(0);
    auto daughter2 = daughters_id.at(1);

    if( daughter1 > sim_ids.size() ){
      true_pdg.push_back(-1);
      continue;
    }
    if( daughter1 > sim_ids.size() ){
      true_pdg.push_back(-1);
      continue;
    }

    auto match1 = sim_ids.at( daughter1 );
    auto match2 = sim_ids.at( daughter2 );


    auto mother_id1 = mother_ids.at( match1 );
    auto mother_id2 = mother_ids.at( match2 );

    if( mother_id1 != mother_id2 ){
      true_pdg.push_back(-1);
      continue;
    }
    if (mother_id1 == -1){
      true_pdg.push_back(-1);
      continue;
    }
    if ( mother_id1 > sim_pid.size() ){
      true_pdg.push_back(-1);
      continue;
    }
    true_pdg.push_back( sim_pid.at( mother_id1 ) );
  }
  tree_manager_.SetCandidateTruePid(true_pdg);
  return true_pdg;
}

std::vector<std::vector<float>> FindParticles::GetDaughterChi2Prim() {
  if( candidates_.empty() )
    Find();
  std::vector<std::vector<float>> chi2(3, std::vector<float>{});
  for( const auto& candidate : candidates_ ){
    chi2.emplace_back();
    for( int i=0; i<3; ++i )
      chi2.at(i).push_back(candidate.GetChi2Prim(i));
  }
  tree_manager_.SetDaughterChi2Prim(chi2);
  return chi2;
}

void FindParticles::WriteDaughterInfo(std::vector<std::vector<float>> track_parameters, std::vector<int> pid_vector) {
  std::vector<std::vector<ROOT::Math::PtEtaPhiMVector>> daughter_momenta{
          std::vector<ROOT::Math::PtEtaPhiMVector>{},
          std::vector<ROOT::Math::PtEtaPhiMVector>{}
  };
  std::vector<std::vector<int>> daughter_pid{};
  if( candidates_.empty() )
    Find();
  for( const auto& candidate : candidates_ ){
    daughter_momenta.emplace_back();
    daughter_pid.emplace_back();
    auto daughter_id = candidate.GetDaughterIds();
    for( int i=0; i<2; ++i ) {
      auto pid = pid_vector.at( daughter_id.at(i) );
      auto m = TDatabasePDG::Instance()->GetParticle(pid)->Mass();
      auto tx = track_parameters.at(daughter_id.at(i)).at(3);
      auto ty = track_parameters.at(daughter_id.at(i)).at(4);
      auto qp = track_parameters.at(daughter_id.at(i)).at(5);

      auto p = fabs( 1.0f / qp );
      auto pz = sqrt( p*p / ( tx*tx + ty*ty + 1 ) );
      auto px = tx*pz;
      auto py = ty*pz;
      auto pT = static_cast<double>(sqrt( px*px + py*py ));
      auto phi = static_cast<double>(atan2( py, px ));
      auto theta = atan2(pT,pz);
      auto eta = static_cast<double>(-log( tan( theta/2.0f ) ));
      auto E = static_cast<double>( p*p + m*m );

      daughter_momenta.at(i).emplace_back(pT, eta, phi, m);
      daughter_pid.back().push_back(pid);
    }
  }
  tree_manager_.SetDaughter1Momenta(daughter_momenta.at(0));
  tree_manager_.SetDaughter2Momenta(daughter_momenta.at(1));
  tree_manager_.SetDaughterPid(daughter_pid);
}

