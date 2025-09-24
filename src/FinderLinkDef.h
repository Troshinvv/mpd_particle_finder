//
// Created by Misha on 3/15/2023.
//

#if defined(__ROOTCLING__) || defined(__MAKECINT__)

#include <vector>

#pragma link off all class;
#pragma link off all function;
#pragma link off all global;
#pragma link off all typedef;

#pragma link C++ class FindParticles+;
#pragma link C++ class Finder+;
#pragma link C++ class Utils+;

#pragma link C++ class std::vector<std::vector<int>>+;
#pragma link C++ class std::vector<std::vector<float>>+;
#pragma link C++ class ROOT::VecOps::RVec<ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag>>+;
#pragma link C++ class ROOT::VecOps::RVec<ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<double> >>+;
#pragma link C++ class ROOT::VecOps::RVec<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >>+;
#pragma link C++ class ROOT::VecOps::RVec<vector<float>>+;
#pragma link C++ class vector<ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > >+;
#pragma link C++ class ROOT::VecOps::RVec<vector<double> >+;
#pragma link C++ class map<int,ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag> >+;

#endif
