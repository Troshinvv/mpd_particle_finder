//
// Created by Misha on 4/14/2023.
//

#ifndef BMN_PARTICLE_FINDER_GETTERS_H
#define BMN_PARTICLE_FINDER_GETTERS_H

#include <Math/LorentzVector.h>
#include <Math/PtEtaPhiM4D.h>
#include <Math/PtEtaPhiE4D.h>
#include <Math/Cartesian3D.h>
#include <ROOT/RVec.hxx>
#include <Math/GenVector/PtEtaPhiE4D.h>
#include <Math/Vector4D.h>
#include <TLorentzVector.h>
#include <Math/Boost.h>

#include <OutputContainer.hpp>
//using namespace ROOT;
//using namespace ROOT::Math;
//using namespace ROOT::RDF;
namespace Getters{

inline auto GetMomenta( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<ROOT::Math::PtEtaPhiMVector> momenta{};
  for( const auto& candidate : candidates ){
    const auto m = candidate.GetMass();
    const auto px = candidate.GetPx();
    const auto py = candidate.GetPy();
    const auto pz = candidate.GetPz();
    const auto pT = sqrtf( px*px + py*py );
    const auto phi = atan2f( py, px );
    const auto theta = atan2f( pT, pz);
    const auto eta = -logf( tanf( theta/2.0f ) );
    momenta.emplace_back( pT, eta, phi, m );
  }
  return momenta;
}


inline auto GetPhiProt(const ROOT::RVec<OutputContainer>& candidates, std::vector<ROOT::Math::PtEtaPhiMVector> candidate_momenta,ROOT::VecOps::RVec<ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<double> > > global_mom, ROOT::VecOps::RVec<ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<double> > > sim_mom,ROOT::VecOps::RVec<int> sim_ids ){
  std::vector<double> phiprot{};
        for(int i=0;i<candidates.size();i++){
                const auto candidate = candidates.at(i);
                const auto mom = candidate_momenta.at(i);
                auto daughters_id = candidate.GetDaughterIds();
                const auto daughter1 = daughters_id.at(1);
		ROOT::Math::PxPyPzEVector cand_cartezian(mom.Px(),mom.Py(),mom.Pz(),mom.E());
		double beta_x = (-1)*cand_cartezian.Px()/cand_cartezian.E();
		double beta_y = (-1)*cand_cartezian.Py()/cand_cartezian.E();
		double beta_z = (-1)*cand_cartezian.Pz()/cand_cartezian.E();	
		ROOT::Math::Boost boost_to_rest(beta_x,beta_y,beta_z);
		double mass_prot = 0.938272;
		double pt_prot = global_mom.at(daughter1).Pt();
		double eta_prot = global_mom.at(daughter1).Eta();
		double en_daughter = TMath::Sqrt(mass_prot*mass_prot+pt_prot*pt_prot*std::cosh(eta_prot)*std::cosh(eta_prot));
                ROOT::Math::PxPyPzEVector boosted = boost_to_rest(ROOT::Math::PxPyPzEVector(global_mom.at(daughter1).Px(),global_mom.at(daughter1).Py(),global_mom.at(daughter1).Pz(),en_daughter));
                phiprot.push_back(boosted.Phi());
  }
  return phiprot;
}

inline auto GetMass( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<float> mass{};
  for( const auto& candidate : candidates ){
    const auto m = candidate.GetMass();
    mass.push_back( m );
  }
  return mass;
}

inline auto GetMomentumErr( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<std::vector<float>> errors{};
  for( const auto& candidate : candidates ){
    const auto pT_err = candidate.GetPtError();
    const auto phi_err = candidate.GetPhiError();
    const auto eta_err = candidate.GetEtaError();
    const auto mass_err = candidate.GetMassError();
    errors.push_back({pT_err, phi_err, eta_err, mass_err});
  }
  return errors;
}

inline auto GetPDG( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<int> pdg{};
  for( const auto& candidate : candidates ){
    pdg.push_back( candidate.GetPdg() );
  }
  return pdg;
}

inline auto GetDaughterCosines( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<std::vector<float>> cosines{2, std::vector<float>{}};
  for( const auto& candidate : candidates ){
    for( int i=0; i<2; ++i )
      cosines.at(i).push_back(candidate.GetCos(i));
  }
  return cosines;
}
inline auto GetDaughterChi2PV( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<std::vector<float>> chi2(2, std::vector<float>{});
  for( const auto& candidate : candidates ){
    chi2.emplace_back();
    for( int i=0; i<2; ++i )
      chi2.at(i).push_back(candidate.GetChi2Prim(i));
  }
  return chi2;
}

inline auto GetCosineTopo( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<float> cosines{};
  for( const auto& candidate : candidates ){
      cosines.push_back(candidate.GetCosineTopo(0));
  }
  return cosines;
}

inline auto GetDaughterDCA( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<double> dca{};
  for( const auto& candidate : candidates ){
    dca.emplace_back( candidate.GetDistance() );
  }
  return dca;
}

inline auto GetDistanceToSV( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<double> distance{};
  for( const auto& candidate : candidates ){
    distance.emplace_back(candidate.GetDistanceToSV() );
  }
  return distance;
}

inline auto GetL( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<double> distance{};
  for( const auto& candidate : candidates ){
    distance.emplace_back(candidate.GetL() );
  }
  return distance;
}

inline auto GetLdL( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<double> distance{};
  for( const auto& candidate : candidates ){
    distance.emplace_back(candidate.GetLdL() );
  }
  return distance;
}

inline auto GetChi2Geo( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<float> chi2{};
  for( const auto& candidate : candidates ){
    chi2.push_back(candidate.GetChi2Geo(0));
  }
  return chi2;
}

inline auto GetChi2Topo( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<float> chi2{};
  for( const auto& candidate : candidates ){
    chi2.push_back(candidate.GetChi2Topo(0));
  }
  return chi2;
}

inline auto GetTruePDG( const ROOT::RVec<OutputContainer>& candidates,
                  ROOT::VecOps::RVec<int> mother_ids,
                  ROOT::VecOps::RVec<int> sim_ids,
                  ROOT::VecOps::RVec<int> sim_pid) try {
  std::vector<int> true_pdg;
  for( const auto& cand : candidates ){
    auto daughters_id = cand.GetDaughterIds();

    const auto daughter1 = daughters_id.at(0);
    const auto daughter2 = daughters_id.at(1);

    if( daughter1 >= sim_ids.size() ){
      true_pdg.push_back(-999);
      continue;
    }
    if( daughter2 >= sim_ids.size() ){
      true_pdg.push_back(-998);
      continue;
    }

    const auto match1 = sim_ids.at(daughter1);
    const auto match2 = sim_ids.at(daughter2);

    if(match1 < 0){
      true_pdg.push_back(-997);
      continue;
    }
    if(match2 < 0){
      true_pdg.push_back(-996);
      continue;
    }

    const auto mother_id1 = mother_ids.at(match1);
    const auto mother_id2 = mother_ids.at(match2);

    if( mother_id1 != mother_id2 ){
      true_pdg.push_back(-995);
      continue;
    }
    if (mother_id1 == -1){
      true_pdg.push_back(-994);
      continue;
    }
    if ( mother_id1 >= sim_pid.size() ){
      true_pdg.push_back(-993);
      continue;
    }
    true_pdg.push_back(sim_pid.at(mother_id1));
  }
  return true_pdg;
} catch ( const std::out_of_range &e ){
  std::cerr << __func__ << "(): Exception was issued" << "\n";
  throw e;
}

inline auto GetTruepT( const ROOT::RVec<OutputContainer>& candidates,
                  ROOT::VecOps::RVec<int> mother_ids,
                  ROOT::VecOps::RVec<int> sim_ids,
                  std::vector<float> sim_pt) try {
  std::vector<float> true_pdg;
  for( const auto& cand : candidates ){
    auto daughters_id = cand.GetDaughterIds();

    const auto daughter1 = daughters_id.at(0);
    const auto daughter2 = daughters_id.at(1);

    if( daughter1 >= sim_ids.size() ){
      true_pdg.push_back(-999);
      continue;
    }
    if( daughter2 >= sim_ids.size() ){
      true_pdg.push_back(-998);
      continue;
    }

    const auto match1 = sim_ids.at(daughter1);
    const auto match2 = sim_ids.at(daughter2);

    if(match1 < 0){
      true_pdg.push_back(-997);
      continue;
    }
    if(match2 < 0){
      true_pdg.push_back(-996);
      continue;
    }

    const auto mother_id1 = mother_ids.at(match1);
    const auto mother_id2 = mother_ids.at(match2);

    if( mother_id1 != mother_id2 ){
      true_pdg.push_back(-995);
      continue;
    }
    if (mother_id1 == -1){
      true_pdg.push_back(-994);
      continue;
    }
    if ( mother_id1 >= sim_pt.size() ){
      true_pdg.push_back(-993);
      continue;
    }
    true_pdg.push_back(sim_pt.at(mother_id1));
  }
  return true_pdg;
} catch ( const std::out_of_range &e ){
  std::cerr << __func__ << "(): Exception was issued" << "\n";
  throw e;
}
inline auto GetTrueId( const ROOT::RVec<OutputContainer>& candidates,
                  ROOT::VecOps::RVec<int> mother_ids,
                  ROOT::VecOps::RVec<int> sim_ids,
                  ROOT::VecOps::RVec<int> sim_pid) try {
  std::vector<int> true_id;
  for( const auto& cand : candidates ){
    auto daughters_id = cand.GetDaughterIds();

    const auto daughter1 = daughters_id.at(0);
    const auto daughter2 = daughters_id.at(1);

    if( daughter1 >= sim_ids.size() ){
      true_id.push_back(-999);
      continue;
    }
    if( daughter2 >= sim_ids.size() ){
      true_id.push_back(-998);
      continue;
    }

    const auto match1 = sim_ids.at(daughter1);
    const auto match2 = sim_ids.at(daughter2);

    if(match1 < 0){
      true_id.push_back(-997);
      continue;
    }
    if(match2 < 0){
      true_id.push_back(-996);
      continue;
    }

    const auto mother_id1 = mother_ids.at(match1);
    const auto mother_id2 = mother_ids.at(match2);

   if( mother_id1 != mother_id2 ){
      true_id.push_back(-995);
      continue;
    }
    if (mother_id1 == -1){
      true_id.push_back(-994);
      continue;
    }
    if ( mother_id1 >= sim_pid.size() ){
      true_id.push_back(-993);
      continue;
    }
    true_id.push_back(mother_ids.at(mother_id1));
  }
  return true_id;
} catch ( const std::out_of_range &e ){
  std::cerr << __func__ << "(): Exception was issued" << "\n";
  throw e;
}

inline auto GetUniGenFlag( const ROOT::RVec<OutputContainer>& candidates,
                  ROOT::VecOps::RVec<int> mother_ids,
                  ROOT::VecOps::RVec<int> sim_ids,
                  ROOT::VecOps::RVec<int> sim_pid,
		  ROOT::VecOps::RVec<int> sim_uniflag) try {
  std::vector<int> true_id;
  for( const auto& cand : candidates ){
    auto daughters_id = cand.GetDaughterIds();

    const auto daughter1 = daughters_id.at(0);
    const auto daughter2 = daughters_id.at(1);

    if( daughter1 >= sim_ids.size() ){
      true_id.push_back(-999);
      continue;
    }
    if( daughter2 >= sim_ids.size() ){
      true_id.push_back(-998);
      continue;
    }

    const auto match1 = sim_ids.at(daughter1);
    const auto match2 = sim_ids.at(daughter2);

    if(match1 < 0){
      true_id.push_back(-997);
      continue;
    }
    if(match2 < 0){
      true_id.push_back(-996);
      continue;
    }

    const auto mother_id1 = mother_ids.at(match1);
    const auto mother_id2 = mother_ids.at(match2);
	
   if (mother_id1 != -1 && mother_id2!=-1 && mother_id1==mother_id2){
	if(mother_id1>=sim_uniflag.size()) {
	true_id.push_back(-20);//size error( fix it)
	continue;}
	if(sim_uniflag.at(mother_id1)==-3){
		true_id.push_back(-3);//original
		continue;}
	if(sim_uniflag[mother_id1]==-9){
                true_id.push_back(-9);//enhanced
		continue;}
	if(sim_uniflag[mother_id1]==-15){ 
                true_id.push_back(-15);//first in the events where is no origin
		continue;}
	if(sim_uniflag[mother_id1]==0){
		true_id.push_back(-25);//access empty map element
		continue;} 
	if(sim_uniflag[mother_id1]==-900){
                true_id.push_back(-900);//see converter -900 flag
                continue;}
	true_id.push_back(-30);//what the freak
	continue;
    }

if ( mother_id1 >= sim_pid.size() ){
      true_id.push_back(-993);
      continue;
    }
    true_id.push_back(mother_ids.at(mother_id1));
  }
  return true_id;
} catch ( const std::out_of_range &e ){
  std::cerr << __func__ << "(): Exception was issued" << "\n";
  throw e;
}
inline auto GetFlag( const ROOT::RVec<OutputContainer>& candidates,
                  ROOT::VecOps::RVec<int> mother_ids,
                  ROOT::VecOps::RVec<int> sim_ids,
                  ROOT::VecOps::RVec<int> sim_pid) try {
  std::vector<int> true_id;
  for( const auto& cand : candidates ){
    auto daughters_id = cand.GetDaughterIds();

    const auto daughter1 = daughters_id.at(0);
    const auto daughter2 = daughters_id.at(1);

    if( daughter1 >= sim_ids.size() ){
      true_id.push_back(-999);
      continue;
    }
    if( daughter2 >= sim_ids.size() ){
      true_id.push_back(-998);
      continue;
    }

    const auto match1 = sim_ids.at(daughter1);
    const auto match2 = sim_ids.at(daughter2);

    if(match1 < 0){
      true_id.push_back(-997);
      continue;
    }
    if(match2 < 0){
      true_id.push_back(-996);
      continue;
    }

    const auto mother_id1 = mother_ids.at(match1);
    const auto mother_id2 = mother_ids.at(match2);

    
   if( mother_id1==-1 && mother_id2!=-1 ){
      
      true_id.push_back(-2);
      continue;
    }
    if (mother_id1 != -1 && mother_id2==-1){
      true_id.push_back(-3);
      continue;
    }
    if (mother_id1 == -1 && mother_id2==-1){
      true_id.push_back(-4);
      continue;
    }
    if (mother_id1 != -1 && mother_id2!=-1 && mother_id1!=mother_id2){
      true_id.push_back(-5);
      continue;
    }
   if (mother_id1 != -1 && mother_id2!=-1 && mother_id1==mother_id2){
      true_id.push_back(-6);
      continue;
    }
    
if ( mother_id1 >= sim_pid.size() ){
      true_id.push_back(-993);
      continue;
    }
    true_id.push_back(mother_ids.at(mother_id1));
  }
  return true_id;
} catch ( const std::out_of_range &e ){
  std::cerr << __func__ << "(): Exception was issued" << "\n";
  throw e;
}
inline auto GetDaughterId( const ROOT::RVec<OutputContainer>& candidates ){
  std::vector<std::vector<int>> indices{ 2, std::vector<int>{} };

  for( const auto& candidate : candidates ){
    auto daughters_id = candidate.GetDaughterIds();

    const auto daughter1 = daughters_id.at(0);
    const auto daughter2 = daughters_id.at(1);

    indices.at(0).emplace_back( daughter1 );
    indices.at(1).emplace_back( daughter2 );
  }
  return indices;
}



}

#endif //BMN_PARTICLE_FINDER_GETTERS_H
