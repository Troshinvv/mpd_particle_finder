//
// Created by Misha on 3/9/2023.
//

#include <TROOT.h>
#include "find_particles.h"


int main(int argc, char** argv) {
  if(argc < 2)
    throw std::runtime_error( "Too few arguments are provided. At least 1 is expected" );
  std::string macro{argv[1]};
  std::string list{argv[2]};
  std::string macro_full = macro+"(\""+list+"\")";
  gROOT->Macro(macro_full.c_str());
  return 0;
}