double GetIntegral(TH1D *const& hist, double a, double b)
{
  double result = 0.;
  double abin = hist->GetXaxis()->FindBin(a);
  double bbin = hist->GetXaxis()->FindBin(b);
  for (int i=abin; i<bbin; ++i){
    result += hist->GetBinContent(i);
  }
  return result;
}

void get_pics(std::string iName = "candidates.root", std::string oName = "cand_qa.root")
{

  TStopwatch timer;
  timer.Start();
  
  ROOT::RDataFrame d("t", iName.c_str());
  TFile *fo = new TFile(oName.c_str(), "recreate");

  std::vector<ROOT::RDF::RResultPtr<::TH1D >> hists1d;
  std::vector<ROOT::RDF::RResultPtr<::TH2D >> hists2d;
  
  auto dd = d
    .Filter("mcB<20.")
    .Define("lambda_mass_w_cuts_sg", [](ROOT::VecOps::RVec<float> _mass,
                                     ROOT::VecOps::RVec<float> _chi2_topo,
                                     ROOT::VecOps::RVec<float> _chi2_geo,
                                     ROOT::VecOps::RVec<double> _L,
                                     ROOT::VecOps::RVec<double> _LdL){
      std::vector<float> result;
      for (int i=0; i<_mass.size(); ++i){
        auto mass = _mass.at(i);
        auto chi2topo = _chi2_topo.at(i);
        auto chi2geo = _chi2_geo.at(i);
        auto L = _L.at(i);
        auto LdL = _LdL.at(i);
        if (chi2topo < 50 && chi2geo < 50 && L > 3 && LdL > 5)
          result.push_back(mass);
        else
          result.push_back(-999.);
      }
      return result;
    }, {"lambda_candidate_mass",
        "lambda_candidate_chi2_topo",
        "lambda_candidate_chi2_geo",
        "lambda_candidate_L",
        "lambda_candidate_LdL"})
    .Define("lambda_mass_w_cuts_bg", [](ROOT::VecOps::RVec<float> _mass,
                                     ROOT::VecOps::RVec<float> _chi2_topo,
                                     ROOT::VecOps::RVec<float> _chi2_geo,
                                     ROOT::VecOps::RVec<double> _L,
                                     ROOT::VecOps::RVec<double> _LdL){
      std::vector<float> result;
      for (int i=0; i<_mass.size(); ++i){
        auto mass = _mass.at(i);
        auto chi2topo = _chi2_topo.at(i);
        auto chi2geo = _chi2_geo.at(i);
        auto L = _L.at(i);
        auto LdL = _LdL.at(i);
        if (chi2topo < 50 && chi2geo < 50 && L > 3 && LdL > 5)
          result.push_back(mass);
        else
          result.push_back(-999.);
      }
      return result;
    }, {"lambda_bg_candidate_mass",
        "lambda_bg_candidate_chi2_topo",
        "lambda_bg_candidate_chi2_geo",
        "lambda_bg_candidate_L",
        "lambda_bg_candidate_LdL"})
    .Define("kshort_mass_w_cuts_sg", [](ROOT::VecOps::RVec<float> _mass,
                                     ROOT::VecOps::RVec<float> _chi2_topo,
                                     ROOT::VecOps::RVec<float> _chi2_geo,
                                     ROOT::VecOps::RVec<double> _L,
                                     ROOT::VecOps::RVec<double> _LdL){
      std::vector<float> result;
      for (int i=0; i<_mass.size(); ++i){
        auto mass = _mass.at(i);
        auto chi2topo = _chi2_topo.at(i);
        auto chi2geo = _chi2_geo.at(i);
        auto L = _L.at(i);
        auto LdL = _LdL.at(i);
        if (chi2topo < 50 && chi2geo < 50 && L > 3 && LdL > 5)
          result.push_back(mass);
        else
          result.push_back(-999.);
      }
      return result;
    }, {"kshort_candidate_mass",
        "kshort_candidate_chi2_topo",
        "kshort_candidate_chi2_geo",
        "kshort_candidate_L",
        "kshort_candidate_LdL"})
    .Define("kshort_mass_w_cuts_bg", [](ROOT::VecOps::RVec<float> _mass,
                                     ROOT::VecOps::RVec<float> _chi2_topo,
                                     ROOT::VecOps::RVec<float> _chi2_geo,
                                     ROOT::VecOps::RVec<double> _L,
                                     ROOT::VecOps::RVec<double> _LdL){
      std::vector<float> result;
      for (int i=0; i<_mass.size(); ++i){
        auto mass = _mass.at(i);
        auto chi2topo = _chi2_topo.at(i);
        auto chi2geo = _chi2_geo.at(i);
        auto L = _L.at(i);
        auto LdL = _LdL.at(i);
        if (chi2topo < 50 && chi2geo < 50 && L > 3 && LdL > 5)
          result.push_back(mass);
        else
          result.push_back(-999.);
      }
      return result;
    }, {"kshort_bg_candidate_mass",
        "kshort_bg_candidate_chi2_topo",
        "kshort_bg_candidate_chi2_geo",
        "kshort_bg_candidate_L",
        "kshort_bg_candidate_LdL"})
    ;

  dd.Foreach([](ULong64_t evtId){if (evtId % 1000 == 0) cout << "\r" << evtId;}, {"rdfentry_"});

  hists1d.push_back(dd.Histo1D({"h1_lambda_sg_mass","m_{inv} signal;m_{inv}, GeV/c^{2}", 200, 1., 1.2}, "lambda_mass_w_cuts_sg"));
  hists1d.push_back(dd.Histo1D({"h1_lambda_bg_mass","m_{inv} background;m_{inv}, GeV/c^{2}", 200, 1., 1.2}, "lambda_mass_w_cuts_bg"));
  hists1d.push_back(dd.Histo1D({"h1_kshort_sg_mass","m_{inv} signal;m_{inv}, GeV/c^{2}", 300, 0.35, 0.65}, "kshort_mass_w_cuts_sg"));
  hists1d.push_back(dd.Histo1D({"h1_kshort_bg_mass","m_{inv} background;m_{inv}, GeV/c^{2}", 300, 0.35, 0.65}, "kshort_mass_w_cuts_bg"));

  hists2d.push_back(dd.Histo2D({"h2_lambda_sg_mass_chi2topo","m_{inv} vs #chi^{2}_{topo};m_{inv}, GeV/c^{2};#chi^{2}_{topo}", 200, 1., 1.2, 500, 0., 250.}, "lambda_candidate_mass", "lambda_candidate_chi2_topo"));
  hists2d.push_back(dd.Histo2D({"h2_lambda_sg_mass_chi2geo","m_{inv} vs #chi^{2}_{geo};m_{inv}, GeV/c^{2};#chi^{2}_{geo}", 200, 1., 1.2, 500, 0., 250.}, "lambda_candidate_mass", "lambda_candidate_chi2_geo"));
  hists2d.push_back(dd.Histo2D({"h2_lambda_sg_mass_L","m_{inv} vs L;m_{inv}, GeV/c^{2};L, cm", 200, 1., 1.2, 500, 0., 10.}, "lambda_candidate_mass", "lambda_candidate_L"));
  hists2d.push_back(dd.Histo2D({"h2_lambda_sg_mass_LdL","m_{inv} vs L/dL;m_{inv}, GeV/c^{2};L/dL, cm", 200, 1., 1.2, 500, 0., 10.}, "lambda_candidate_mass", "lambda_candidate_LdL"));
  hists2d.push_back(dd.Histo2D({"h2_lambda_bg_mass_chi2topo","m_{inv} vs #chi^{2}_{topo};m_{inv}, GeV/c^{2};#chi^{2}_{topo}", 200, 1., 1.2, 500, 0., 250.}, "lambda_bg_candidate_mass", "lambda_bg_candidate_chi2_topo"));
  hists2d.push_back(dd.Histo2D({"h2_lambda_bg_mass_chi2geo","m_{inv} vs #chi^{2}_{geo};m_{inv}, GeV/c^{2};#chi^{2}_{geo}", 200, 1., 1.2, 500, 0., 250.}, "lambda_bg_candidate_mass", "lambda_bg_candidate_chi2_geo"));
  hists2d.push_back(dd.Histo2D({"h2_lambda_bg_mass_L","m_{inv} vs L;m_{inv}, GeV/c^{2};L, cm", 200, 1., 1.2, 500, 0., 10.}, "lambda_bg_candidate_mass", "lambda_bg_candidate_L"));
  hists2d.push_back(dd.Histo2D({"h2_lambda_bg_mass_LdL","m_{inv} vs L/dL;m_{inv}, GeV/c^{2};L/dL, cm", 200, 1., 1.2, 500, 0., 10.}, "lambda_bg_candidate_mass", "lambda_bg_candidate_LdL"));
  hists2d.push_back(dd.Histo2D({"h2_kshort_sg_mass_chi2topo","m_{inv} vs #chi^{2}_{topo};m_{inv}, GeV/c^{2};#chi^{2}_{topo}", 300, 0.35, 0.65, 500, 0., 250.}, "kshort_candidate_mass", "kshort_candidate_chi2_topo"));
  hists2d.push_back(dd.Histo2D({"h2_kshort_sg_mass_chi2geo","m_{inv} vs #chi^{2}_{geo};m_{inv}, GeV/c^{2};#chi^{2}_{geo}", 300, 0.35, 0.65, 500, 0., 250.}, "kshort_candidate_mass", "kshort_candidate_chi2_geo"));
  hists2d.push_back(dd.Histo2D({"h2_kshort_sg_mass_L","m_{inv} vs L;m_{inv}, GeV/c^{2};L, cm", 300, 0.35, 0.65, 500, 0., 10.}, "kshort_candidate_mass", "kshort_candidate_L"));
  hists2d.push_back(dd.Histo2D({"h2_kshort_sg_mass_LdL","m_{inv} vs L/dL;m_{inv}, GeV/c^{2};L/dL, cm", 300, 0.35, 0.65, 500, 0., 10.}, "kshort_candidate_mass", "kshort_candidate_LdL"));
  hists2d.push_back(dd.Histo2D({"h2_kshort_bg_mass_chi2topo","m_{inv} vs #chi^{2}_{topo};m_{inv}, GeV/c^{2};#chi^{2}_{topo}", 300, 0.35, 0.65, 500, 0., 250.}, "kshort_bg_candidate_mass", "kshort_bg_candidate_chi2_topo"));
  hists2d.push_back(dd.Histo2D({"h2_kshort_bg_mass_chi2geo","m_{inv} vs #chi^{2}_{geo};m_{inv}, GeV/c^{2};#chi^{2}_{geo}", 300, 0.35, 0.65, 500, 0., 250.}, "kshort_bg_candidate_mass", "kshort_bg_candidate_chi2_geo"));
  hists2d.push_back(dd.Histo2D({"h2_kshort_bg_mass_L","m_{inv} vs L;m_{inv}, GeV/c^{2};L, cm", 300, 0.35, 0.65, 500, 0., 10.}, "kshort_bg_candidate_mass", "kshort_bg_candidate_L"));
  hists2d.push_back(dd.Histo2D({"h2_kshort_bg_mass_LdL","m_{inv} vs L/dL;m_{inv}, GeV/c^{2};L/dL, cm", 300, 0.35, 0.65, 500, 0., 10.}, "kshort_bg_candidate_mass", "kshort_bg_candidate_LdL"));

  fo->cd();
  for(auto& hist:hists1d)
    hist->Write();
  for(auto& hist:hists2d)
    hist->Write();
  fo->Close();

  timer.Stop();
  timer.Print();
}
