//
// Created by Misha on 3/9/2023.
//

#include <TROOT.h>
#include "find_particles.h"


int main(int argc, char** argv) {
//  std::shared_ptr<FindParticles> particles = std::make_shared<FindParticles>();
  FindParticles particles;
  particles.AddDecay( "lambda", 3122, {-211, 2212} );

//  particles.Clear();
  particles.Fill( std::vector<float>(3, 0.f),
                   {std::vector<float>(6, 0.f)},
                   {std::vector<float>(15, 0.f)},
                   {std::vector<float>(10, 0.f)},
                   std::vector<int>{211} );
  particles.Find();
//  if(argc < 2)
//    throw std::runtime_error( "Too few arguments are provided. At least 1 is expected" );
//  gROOT->Macro(argv[1]);
  return 0;
}