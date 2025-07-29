//
// Created by Misha on 4/14/2023.
//

#ifndef BMN_PARTICLE_FINDER_GETTERS_H
#define BMN_PARTICLE_FINDER_GETTERS_H

#include <Math/LorentzVector.h>
#include <Math/PtEtaPhiM4D.h>
#include <Math/Cartesian3D.h>
#include <ROOT/RVec.hxx>
#include <Math/GenVector/PtEtaPhiE4D.h>
#include <Math/Vector4D.h>

#include <OutputContainer.hpp>

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
      true_pdg.push_back(-1);
      continue;
    }
    if( daughter2 >= sim_ids.size() ){
      true_pdg.push_back(-1);
      continue;
    }

    const auto match1 = sim_ids.at(daughter1);
    const auto match2 = sim_ids.at(daughter2);

    if(match1 < 0){
      true_pdg.push_back(-1);
      continue;
    }
    if(match2 < 0){
      true_pdg.push_back(-1);
      continue;
    }

    const auto mother_id1 = mother_ids.at(match1);
    const auto mother_id2 = mother_ids.at(match2);

    if( mother_id1 != mother_id2 ){
      true_pdg.push_back(-1);
      continue;
    }
    if (mother_id1 == -1){
      true_pdg.push_back(-1);
      continue;
    }
    if ( mother_id1 >= sim_pid.size() ){
      true_pdg.push_back(-1);
      continue;
    }
    true_pdg.push_back(sim_pid.at(mother_id1));
  }
  return true_pdg;
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
