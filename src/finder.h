//
// Created by Misha on 4/14/2023.
//

#ifndef BMN_PARTICLE_FINDER_FINDER_H
#define BMN_PARTICLE_FINDER_FINDER_H


#include <utility>
#include <vector>
#include <InputContainer.hpp>
#include <Decay.hpp>
#include <OutputContainer.hpp>
#include <ROOT/RVec.hxx>

class Finder {
public:
  Finder() = default;
  explicit Finder( std::vector<Decay>  decays ) : decays_(std::move(decays)) {}
  ~Finder() = default;
  void SetDecays(const std::vector<Decay> &decays) {
    decays_ = decays;
  }
  void AddDecay( const std::string& name, int mother_pdg, const std::vector<int>& daughter_pdg ) {
    std::vector<Daughter> daughters;
    for( auto pdg : daughter_pdg ){
      daughters.emplace_back( pdg );
//      daughters.back().SetCutChi2Prim(18.42);
      daughters.back().CancelCutChi2Prim();
      daughters.back().CancelCutCos();
    }
    Mother mother(mother_pdg);
//    mother.SetCutChi2Geo(3);
    mother.CancelCutChi2Geo();
//    mother.SetCutDistance(1);
    mother.CancelCutDistance();
//    mother.SetCutLdL(5);
    mother.CancelCutLdL();
    mother.CancelCutDistanceToSV();
    decays_.emplace_back( name, mother, daughters );
  };
  ROOT::RVec<OutputContainer> operator()(const std::vector<float>& primary_vertex,
                                          const std::vector<std::vector<float>>& track_parameters,
                                          const std::vector<std::vector<float>>& covariance_matrix,
                                          const std::vector<std::vector<float>>& magnetic_field,
                                          const std::vector<int>& pid_vector,
                                          const std::vector<int>& is_good_track_);
private:
  ROOT::RVec<OutputContainer> Find();
  std::vector<Decay> decays_;
  InputContainer input_container_{};

};


#endif //BMN_PARTICLE_FINDER_FINDER_H
