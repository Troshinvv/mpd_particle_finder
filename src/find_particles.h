//
// Created by Misha on 3/15/2023.
//

#ifndef BMN_PARTICLE_FINDER_FIND_PARTICLES_H
#define BMN_PARTICLE_FINDER_FIND_PARTICLES_H

#include <vector>
#include <array>
#include <cmath>

#include <Decay.hpp>
#include <Daughter.hpp>
#include <Mother.hpp>
#include <InputContainer.hpp>
#include <OutputContainer.hpp>
#include <SimpleFinder.hpp>

#include <Math/LorentzVector.h>
#include <Math/PtEtaPhiM4D.h>
#include <Math/Cartesian3D.h>

#include "utils.h"

class FindParticles {
public:
  FindParticles() {
    input_container_ = new InputContainer();
//    input_container_->Clear();
    std::cout << input_container_->GetTracks().size() << std::endl;
    std::cout << "FindParticles::" << __func__ << "()" << std::endl;
  };
  ~FindParticles() { std::cout << "FindParticles::" << __func__ << "()" << std::endl; };

  void SetDecays(const std::vector<Decay> &decays) {
    decays_ = decays;
  }
  void AddDecay( const std::string& name, int mother_pdg, std::vector<int> daughter_pdg ){
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
  }

  void Fill(std::vector<float> primary_vertex,
            std::vector<std::vector<float>> track_parameters,
            std::vector<std::vector<float>> covariance_matrix,
            std::vector<std::vector<float>> magnetic_field,
            std::vector<int> pid_vector){
    std::cout << "FindParticles::" << __func__ << "()" << std::endl;
//    input_container_ = new InputContainer();
//    input_container_->Clear();
    input_container_->SetPV( 0, 0, 0 );
    KFPVertex primVtx_tmp;
    primVtx_tmp.SetXYZ(0, 0, 0);
    primVtx_tmp.SetCovarianceMatrix(0, 0, 0, 0, 0, 0);//NOTE
    primVtx_tmp.SetNContributors(0);
    primVtx_tmp.SetChi2(-100);

//    auto vtx = KFVertex(primVtx_tmp);
    input_container_->SetPV(primVtx_tmp);
//    std::cout << input_container_->GetTracks().size() << std::endl;
    auto n_particles = track_parameters.size();
    std::cout << input_container_->GetTracks().size() << std::endl;
    input_container_->Reserve( n_particles );
    for( int i=0; i<n_particles; ++i ){
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

      auto p = fabsf( 1.0f/qp );
      auto q = qp * p;
      auto pz = sqrtf( p*p / ( tx*tx + ty*ty +1 ) );
      auto px = tx/pz;
      auto py = ty/pz;

      auto J = Utils::CalculateJacobian(trk_parameters);
      auto new_cov = Utils::CalculateCovariance( trk_cov, J );
      std::vector<float> par{ x, y, z, px, py, pz };
      input_container_->AddTrack( par, new_cov, trk_field, static_cast<int>(q), trk_pid, i );
    }
//    Find();
//    std::cout << candidates_.size() << std::endl;
//    return candidates_.size();
  }
  void Find(){
//    std::cout << "FindParticles::" << __func__ << "()" << std::endl;
    std::cout << "FindParticles::" << __func__ << "()" << std::endl;
    std::unique_ptr<SimpleFinder> finder = std::make_unique<SimpleFinder>();
    std::cout << "input_container_.size(): " << input_container_->GetTracks().size() << std::endl;
    finder->SetDecays(decays_);
    finder->Init(*input_container_);
    finder->FindParticles();
    candidates_ = finder->GetCandidates();
  }
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float>>> GetCandidateMomenta(){
    if( candidates_.empty() )
      Find();
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float>>> momenta;
    for( const auto& candidate : candidates_ ){
      auto m = candidate.GetMass();
      auto px = candidate.GetPx();
      auto py = candidate.GetPy();
      auto pz = candidate.GetPz();
      auto pT = sqrtf( px*px + py*py );
      auto phi = atan2f( py, px );
      auto theta = atan2f(pz,pT);
      auto eta = -logf( tanf( theta/2.0f ) );
      momenta.emplace_back(pT, phi, eta, m );
    }
    return momenta;
  }
  std::vector<std::vector<float>> GetCandidateMomentumErr(){
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
    return errors;
  }
  std::vector<int> GetCandidatePDG(){
    if( candidates_.empty() )
      Find();
    std::vector<int> pdg;
    for( const auto& candidate : candidates_ ){
      pdg.push_back( candidate.GetPdg() );
    }
    return pdg;
  }
  std::vector<std::vector<float>> GetCandidateCosines(){
    if( candidates_.empty() )
      Find();
    std::vector<std::vector<float>> cosines;
    for( const auto& candidate : candidates_ ){
      for( int i=0; i<3; ++i )
        cosines.back().emplace_back(candidate.GetCos(i));
    }
    return cosines;
  }
  std::vector<std::vector<float>> GetCandidateCosTopo(){
    if( candidates_.empty() )
      Find();
    std::vector<std::vector<float>> cosines;
    for( const auto& candidate : candidates_ ){
      for( int i=0; i<3; ++i )
        cosines.back().emplace_back(candidate.GetCosineTopo(i));
    }
    return cosines;
  }
  std::vector<double> GetDaughterDCA(){
    if( candidates_.empty() )
      Find();
    std::vector<double> dca;
    for( const auto& candidate : candidates_ ){
      dca.emplace_back( candidate.GetDistance() );
    }
    return dca;
  }
  std::vector<double> GetDaughterDistanceToSV(){
    if( candidates_.empty() )
      Find();
    std::vector<double> distance;
    for( const auto& candidate : candidates_ ){
      distance.emplace_back(candidate.GetDistanceToSV() );
    }
    return distance;
  }
  std::vector<double> GetCandidateL(){
    if( candidates_.empty() )
      Find();
    std::vector<double> distance;
    for( const auto& candidate : candidates_ ){
      distance.emplace_back(candidate.GetL() );
    }
    return distance;
  }
  std::vector<double> GetCandidateLdL(){
    if( candidates_.empty() )
      Find();
    std::vector<double> distance;
    for( const auto& candidate : candidates_ ){
      distance.emplace_back(candidate.GetLdL() );
    }
    return distance;
  }
  std::vector<double> PrimaryToSecondaryVertexDistance(){
    if( candidates_.empty() )
      Find();
    std::vector<double> distance;
    for( const auto& candidate : candidates_ ){
      distance.emplace_back( candidate.GetDistanceToSV() );
    }
    return distance;
  }
  std::vector<std::vector<float>> GetChi2Geo(){
    if( candidates_.empty() )
      Find();
    std::vector<std::vector<float>> chi2;
    for( const auto& candidate : candidates_ ){
      for( int i=0; i<4; ++i )
        chi2.back().emplace_back(candidate.GetChi2Geo(i));
    }
    return chi2;
  }
  std::vector<std::vector<float>> GetChi2Topo(){
    if( candidates_.empty() )
      Find();
    std::vector<std::vector<float>> chi2;
    for( const auto& candidate : candidates_ ){
      for( int i=0; i<4; ++i )
        chi2.back().emplace_back(candidate.GetChi2Topo(i));
    }
    return chi2;
  }
  std::vector<std::vector<float>> GetCosTopo(){
    if( candidates_.empty() )
      Find();
    std::vector<std::vector<float>> cosine;
    for( const auto& candidate : candidates_ ){
      for( int i=0; i<4; ++i )
        cosine.back().emplace_back(candidate.GetCosineTopo(i));
    }
    return cosine;
  }
  void Clear(){
    input_container_->Clear();
    candidates_.clear();
  }

private:
  std::vector<Decay> decays_;
  InputContainer* input_container_{nullptr};
  std::vector<OutputContainer> candidates_;
};


#endif //BMN_PARTICLE_FINDER_FIND_PARTICLES_H
