void rec_lambda_sim_qa_whitening(std::string str_efficiency_file="/scratch2/troshin/govorun_backup/UrQMD_UniGen_eff_map_lamb.root"){
std::unique_ptr<TFile> efficiency_file{TFile::Open( str_efficiency_file.c_str(), "READ" )};
  TH2D* efficiency_histo{nullptr};
  efficiency_file->GetObject( "h2_pT_y_selected_signal", efficiency_histo );
  if( !efficiency_histo ){ std::cout << "Efficiency histogram cannot be retrieved from file" << std::endl; }
	ROOT::RDataFrame d("t", "candidates.root");
	auto dd = d
                        .Define( "tru_pT", "std::vector<float> pT; for( auto mom : simMom ){ pT.push_back( mom.Pt() ); } return pT;")
      .Define( "tru_y", "std::vector<float> y; for( auto mom : simMom ){ y.push_back( mom.Rapidity()-0.986446 ); } return y;")
      .Define( "is_lambda", "simPdg ==3122")
      .Define( "is_fd_lambda", "simPdg == 3122 && simMotherId != -1")
      .Define( "is_prim_lambda", "simPdg == 3122 && simMotherId == -1")
          .Define( "tru_phi", "std::vector<float> phi_part; for( auto mom : simMom ){ phi_part.push_back( mom.Phi() ); } return phi_part;")
          .Define("tru_v1", []( std::vector<float> phi, double psi){
            std::vector<float> v1_true;
            for(int i=0;i<phi.size();i++){
              v1_true.push_back(TMath::Cos(phi.at(i)-psi));
            }
            return v1_true;
           }, {"tru_phi","mcRP" })
           .Define("tru_v2", []( std::vector<float> phi, double psi){
            std::vector<float> v2_true;
            for(int i=0;i<phi.size();i++){
              v2_true.push_back(TMath::Cos(2*(phi.at(i)-psi)));
            }
            return v2_true;
           }, {"tru_phi","mcRP" })
           .Define("is_v2_range",[](ROOT::VecOps::RVec<int> is_part,std::vector<float> pt, std::vector<float> eta){
             std::vector<int> in_range;
             for(int i=0;i<eta.size();i++){
               if(is_part.at(i)!=1 || eta.at(i)<-1.2 || eta.at(i)>1.2){
                 in_range.push_back(0);}
               else{
                 in_range.push_back(1);}
             }
             return in_range;
         }, {"is_lambda","tru_pT","tru_y"})
          .Define("is_range",[](ROOT::VecOps::RVec<int> is_part,std::vector<float> pt, std::vector<float> eta){
             std::vector<int> in_range;
             for(int i=0;i<pt.size();i++){
               if(is_part.at(i)!=1 || pt.at(i)<0.5 || pt.at(i)>2){
                 in_range.push_back(0);}
               else{
                 in_range.push_back(1);}
             }
             return in_range;
         }, {"is_lambda","tru_pT","tru_y"})
         .Define("is_fd_range",[](ROOT::VecOps::RVec<int> is_part,std::vector<float> pt, std::vector<float> eta){
             std::vector<int> in_range;
             for(int i=0;i<pt.size();i++){
               if(is_part.at(i)!=1 || pt.at(i)<0.5 || pt.at(i)>2){
                 in_range.push_back(0);}
               else{
                 in_range.push_back(1);}
             }
             return in_range;
         }, {"is_fd_lambda","tru_pT","tru_y"})
         .Define("is_prim_range",[](ROOT::VecOps::RVec<int> is_part,std::vector<float> pt, std::vector<float> eta){
             std::vector<int> in_range;
             for(int i=0;i<pt.size();i++){
               if(is_part.at(i)!=1 || pt.at(i)<0.5 || pt.at(i)>2){
                 in_range.push_back(0);}
               else{
                 in_range.push_back(1);}
             }
             return in_range;
         }, {"is_prim_lambda","tru_pT","tru_y"})
	.Define("FHCalX", "std::vector<float> pos_x; for( auto pos : fhcalModPos ){ pos_x.push_back( pos.X() ); } return pos_x;")
	.Define("FHCalY", "std::vector<float> pos_y; for( auto pos : fhcalModPos ){ pos_y.push_back( pos.Y() ); } return pos_y;")
	.Define("FHCalZ", "std::vector<float> pos_z; for( auto pos : fhcalModPos ){ pos_z.push_back( pos.Z() ); } return pos_z;")
      .Define("candidate_p", "std::vector<float> p; for( auto mom : candidate_momenta ){ p.push_back( mom.P() ); } return p;")
			.Define( "candidate_pT", "std::vector<float> pT; for( auto mom : candidate_momenta ){ pT.push_back( mom.Pt() ); } return pT;")
			.Define("candidate_phi", "std::vector<float> phi; for( auto mom : candidate_momenta ){ phi.push_back( mom.Phi() ); } return phi;")
			.Define("candidate_rapidity", "std::vector<float> rapidity; for( auto mom : candidate_momenta ){ rapidity.push_back( mom.Rapidity()-0.986446 ); } return rapidity;")
			.Define("pT_err", "std::vector<float> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(0) ); } return err;")
			.Define("phi_err", "std::vector<float> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(1) ); } return err;")
			.Define("eta_err", "std::vector<float> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(2) ); } return err;")
			.Define("m_err", "std::vector<double> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(3) ); } return err;")
			.Define("daughter1_cos", "std::vector<double> cosine; for( int i=0; i<daughter_cosines.at(0).size(); ++i ){ cosine.push_back( daughter_cosines.at(0).at(i) ); } return cosine;")
			.Define("daughter2_cos", "std::vector<double> cosine; for( int i=0; i<daughter_cosines.at(1).size(); ++i ){ cosine.push_back( daughter_cosines.at(1).at(i) ); } return cosine;")
			.Define("daughter1_chi2_prim", "std::vector<double> chi2; for( int i=0; i<daughter_chi2_prim.at(0).size(); ++i ){ chi2.push_back( daughter_chi2_prim.at(0).at(i) ); } return chi2;")
			.Define("daughter2_chi2_prim", "std::vector<double> chi2; for( int i=0; i<daughter_chi2_prim.at(1).size(); ++i ){ chi2.push_back( daughter_chi2_prim.at(1).at(i) ); } return chi2;")
			.Redefine("candidate_cos_topo",[](const std::vector<float>& old_var){return std::vector<double>(old_var.begin(),old_var.end());},{"candidate_cos_topo"})
			.Redefine("candidate_chi2_topo",[](const std::vector<float>& old_var){return std::vector<double>(old_var.begin(),old_var.end());},{"candidate_chi2_topo"})
			.Redefine("candidate_chi2_geo",[](const std::vector<float>& old_var){return std::vector<double>(old_var.begin(),old_var.end());},{"candidate_chi2_geo"})

                        .Define("candidate_diffphi", "std::vector<float> diff; for( auto phi : candidate_phiprot ){ diff.push_back(phi-mcRP); } return diff;")
			.Define("candidate_sin_diffphi", "std::vector<float> diff; for( auto phi : candidate_phiprot ){ diff.push_back(TMath::Sin(phi-mcRP)); } return diff;")
			.Define("candidate_A0_corr", "std::vector<float> A0; for( auto theta : candidate_thetaprot ){ A0.push_back(TMath::Sin(theta)); } return A0;")
			.Define("candidate_diffphi_phiprot", "std::vector<float> diff; for( int i=0;i<candidate_phi.size();i++ ){ float phi=candidate_phi.at(i)-candidate_phiprot.at(i);if(phi<0) phi+=2*TMath::Pi(); diff.push_back( phi); } return diff;")
			.Define("candidate_mc_sin_diffphi", "std::vector<float> diff; for( auto phi : candidate_mc_phiprot ){ diff.push_back(TMath::Sin(phi-mcRP)); } return diff;")
			.Define("candidate_mc_A0_corr", "std::vector<float> A0; for( auto theta : candidate_mc_thetaprot ){ A0.push_back(TMath::Sin(theta)); } return A0;")
                        .Define("candidate_mc_diffphi_phiprot", "std::vector<float> diff; for( int i=0;i<candidate_phi.size();i++ ){ float phi=candidate_phi.at(i)-candidate_mc_phiprot.at(i);if(phi<0) phi+=2*TMath::Pi(); diff.push_back( phi); } return diff;")
      .Define( "candidate_weight", [efficiency_histo](std::vector<float> vec_y, std::vector<float> vec_pT){
                  if( !efficiency_histo ){
                      return std::vector<float>(vec_y.size(), 1);
                    }
                  std::vector<float> vec_weight{};
                  vec_weight.reserve(vec_y.size()); 
                  for( int i=0; i<vec_y.size(); ++i ){
                    auto pT = vec_pT.at(i);
                    auto y = vec_y.at(i);
                    auto y_bin = efficiency_histo->GetXaxis()->FindBin( y );
                    auto pT_bin = efficiency_histo->GetYaxis()->FindBin( pT );
                    auto efficiency = efficiency_histo->GetBinContent( y_bin, pT_bin );
                    auto weight = efficiency > 1e-2 ? 1.0 / efficiency : 0.0;
                    vec_weight.push_back(weight );
                  }
                  return vec_weight;
            }, {"candidate_rapidity", "candidate_pT"} )
                        .Define("signal", "candidate_true_pid == 3122 && candidate_true_id == -1 && candidate_mc_phiprot!=-999 && candidate_mc_thetaprot!=-999")
			.Define("signal_orig", "candidate_true_pid == 3122 && candidate_true_id == -1 && candidate_UniGen_flag==-3")
			.Define("signal_enhanced", "candidate_true_pid == 3122 && candidate_true_id == -1 && (candidate_UniGen_flag==-9 || candidate_UniGen_flag==-15)")
                        .Define("background", "candidate_true_pid != 3122")
                         .Define("feed_down", "candidate_true_pid == 3122 && candidate_true_id != -1")
          .Define("under_the_peak",
                        "std::vector<double> under;"
                                        "for(int i=0; i<daughter_chi2_prim.at(0).size(); ++i){"
                                                "if( candidate_mass.at(i) >1.13 || candidate_mass.at(i) <1.1){"
                                                           "under.push_back(0);"
                                                        "continue;"
                                                           "}"
                                                        "under.push_back(1 );"
                                        "} return under;"
                        )
		.Define("basic",
					"std::vector<double> basic;"
                                        "for(int i=0; i<daughter_chi2_prim.at(0).size(); ++i){"
                                                "if( candidate_UniGen_flag.at(i) == -900 && candidate_true_id.at(i) != -1 ){"
                                                        "basic.push_back(0);"
                                                        "continue;"
                                                "}"
						"if( candidate_L.at(i) < 0 ){"
                                                        "basic.push_back(0);"
                                                        "continue;"
                                                "}"
						"if( candidate_pT.at(i)<0.5 || candidate_pT.at(i)>2 ){"
                                                        "basic.push_back(0);"
                                                        "continue;"
                                                "}"
						"if( candidate_rapidity.at(i)<-1.2 || candidate_rapidity.at(i)>1.2 ){"
                                                        "basic.push_back(0);"
                                                        "continue;"
                                                "}"
						"if( candidate_mass.at(i) >1.25 || candidate_mass.at(i) <1.05){"
                                                       "basic.push_back(0);"
                                                       "continue;"
                                                 "}"
						"basic.push_back(1 );"
                                        "} return basic;"
                        )			
          .Define("basic_var",
                                        "std::vector<double> basic_var;"
                                        "for(int i=0; i<daughter_chi2_prim.at(0).size(); ++i){"
                                             /*   "if( candidate_mass.at(i) >1.122 || candidate_mass.at(i) <1.11){"
                                                       "basic_var.push_back(0);"
                                                       "continue;"
                                                 "}"*/
                                            /* "if( candidate_mass.at(i) >1.137 || candidate_mass.at(i) <1.095){"
                                                       "basic_var.push_back(0);"
                                                       "continue;"
                                                 "}"*/
						"if( candidate_UniGen_flag.at(i) == -900 && candidate_true_id.at(i) != -1 ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
						"if( daughter1_chi2_prim.at(i) < 100 ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( daughter2_chi2_prim.at(i) < 50 ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( candidate_L.at(i) < 2 ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( candidate_LdL.at(i) < 10 ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( candidate_cos_topo.at(i) < 0.99){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( candidate_chi2_topo.at(i) > 100 ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( candidate_chi2_geo.at(i) >100  ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( daughter_dca.at(i) > 1  ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
						"if( daughter2_cos.at(i) > 0.9995){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
						"if( m_err.at(i) > 0.003 || m_err.at(i) < 0.0008  ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( candidate_pT.at(i)<0.5 || candidate_pT.at(i)>2 ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "basic_var.push_back(1 );"
                                        "} return basic_var;"
                        )
			.Define("selected_signal",
                        "std::vector<int> selected_signal;"
                        " for(int i=0; i<basic_var.size(); ++i )"
                        "{ "
                                "if( signal.at(i) == 0 ){"
                                        "selected_signal.push_back(0);"
                                        "continue;"
                                "}"
                                "if( basic_var.at(i) == 0 ){"
                                        "selected_signal.push_back(0);"
                                        "continue;"
                                "}"
                                "selected_signal.push_back( basic_var.at(i) );"
                        "}"
                        " return selected_signal;")
                        .Define("selected_background",
                        "std::vector<int> selected_background;"
                        " for(int i=0; i<basic_var.size(); ++i )"
                        "{ "
                                "if( background.at(i) == 0 ){"
                                        "selected_background.push_back(0);"
                                        "continue;"
                                "}"
                                "if( basic_var.at(i) == 0 ){"
                                        "selected_background.push_back(0);"
                                        "continue;"
                                "}"
                                "selected_background.push_back( 1 );"
                        "}"
                        " return selected_background;");

	std::vector<ROOT::RDF::RResultPtr<::TH1D>> hist1d;
	std::vector<ROOT::RDF::RResultPtr<::TH2D>> hist2d;
        std::vector<ROOT::RDF::RResultPtr<::TH3D>> hist3d;
	std::vector<ROOT::RDF::RResultPtr<::THnD>> hist4d;

	std::vector<std::string> varnames = {"daughter1_chi2_prim","daughter2_chi2_prim","candidate_L","candidate_LdL","candidate_cos_topo","candidate_chi2_topo","candidate_chi2_geo","daughter_dca","daughter1_cos","daughter2_cos","m_err"};

        std::vector<std::string> cuts{ "basic" };

	hist2d.push_back( dd.Histo2D( { "h2_pT_y", ";y;p_{T} (GeV/c)", 120, -3.0, 3.0, 60, 0.0, 3  }, "candidate_rapidity", "candidate_pT" ) );
        hist2d.push_back( dd.Histo2D( { "h2_tru_pT_tru_y", ";y;p_{T} (GeV/c)",120, -3.0, 3.0, 60, 0.0, 3 }, "tru_y", "tru_pT", "is_lambda" ) );
        hist2d.push_back( dd.Histo2D( { "h2_fd_tru_pT_tru_y", ";y;p_{T} (GeV/c)", 120, -3.0, 3.0, 60, 0.0, 3 }, "tru_y", "tru_pT", "is_fd_lambda" ) );

hist1d.push_back( dd.Histo1D( { "h1_psi_rp", ";psi_rp counts", 500, -8, 8 }, "mcRP" ) );

       
hist1d.push_back( dd.Profile1D( { "prof1d_tru_v1_vs_y", ";y_{cm};v_{1}", 6,-1.2,1.2 },"tru_y", "tru_v1", "is_range" ) );
hist1d.push_back( dd.Profile1D( { "prof1d_tru_v2_vs_pT", ";p_{T} GeV/c;v_{2}", 6,0,3 },"tru_pT", "tru_v2", "is_v2_range" ) );
hist1d.push_back( dd.Profile1D( { "prof1d_tru_fd_v1_vs_y", ";y_{cm};v_{1}", 6,-1.2,1.2 },"tru_y", "tru_v1", "is_fd_range" ) );
hist1d.push_back( dd.Profile1D( { "prof1d_tru_prim_v1_vs_y", ";y_{cm};v_{1}", 6,-1.2,1.2 },"tru_y", "tru_v1", "is_prim_range" ) );

	for( auto cut : cuts ){
		for(size_t iter = 0;iter<varnames.size();iter++){
			for(size_t jter = 0;jter<varnames.size();jter++){
				std::string outname = "p_"+varnames[iter]+"_"+varnames[jter];
				 dd = dd.Define(outname,[](std::vector<double> v1,std::vector<double> v2){std::vector<double> result; for(int k=0;k<v1.size();k++){result.push_back(v1[k]*v2[k]);}return result;}, {varnames[iter],varnames[jter]});
				hist1d.push_back(dd.Profile1D({outname.c_str(),outname.c_str(),6,-1.2,1.2},"candidate_rapidity",outname,cut));	
			}
		}
		hist1d.push_back(dd.Profile1D({"prof1d_daughter1_chi2_prim","prof1d_daughter1_chi2_prim",6,-1.2,1.2},"candidate_rapidity","daughter1_chi2_prim",cut));	
		hist1d.push_back(dd.Profile1D({"prof1d_daughter2_chi2_prim","prof1d_daughter2_chi2_prim",6,-1.2,1.2},"candidate_rapidity","daughter2_chi2_prim",cut));
		hist1d.push_back(dd.Profile1D({"prof1d_candidate_L","prof1d_candidate_L",6,-1.2,1.2},"candidate_rapidity","candidate_L",cut));
		hist1d.push_back(dd.Profile1D({"prof1d_candidate_LdL","prof1d_candidate_LdL",6,-1.2,1.2},"candidate_rapidity","candidate_LdL",cut));
		hist1d.push_back(dd.Profile1D({"prof1d_candidate_cos_topo","prof1d_candidate_cos_topo",6,-1.2,1.2},"candidate_rapidity","candidate_cos_topo",cut));
		hist1d.push_back(dd.Profile1D({"prof1d_candidate_chi2_topo","prof1d_candidate_chi2_topo",6,-1.2,1.2},"candidate_rapidity","candidate_chi2_topo",cut));
		hist1d.push_back(dd.Profile1D({"prof1d_candidate_chi2_geo","prof1d_candidate_chi2_geo",6,-1.2,1.2},"candidate_rapidity","candidate_chi2_geo",cut));
		hist1d.push_back(dd.Profile1D({"prof1d_daughter_dca","prof1d_daughter_dca",6,-1.2,1.2},"candidate_rapidity","daughter_dca",cut));
		hist1d.push_back(dd.Profile1D({"prof1d_daughter1_cos","prof1d_daughter1_cos",6,-1.2,1.2},"candidate_rapidity","daughter1_cos",cut));
		hist1d.push_back(dd.Profile1D({"prof1d_daughter2_cos","prof1d_daughter2_cos",6,-1.2,1.2},"candidate_rapidity","daughter2_cos",cut));
		hist1d.push_back(dd.Profile1D({"prof1d_m_err","prof1d_m_err",6,-1.2,1.2},"candidate_rapidity","m_err",cut));

		hist1d.push_back( dd.Histo1D( { std::data("h1_mass_"+cut), ";m (GeV/c^2); counts", 300, 1.05, 1.25 }, "candidate_mass", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_pT_"+cut), ";p_{T} (GeV/c); counts", 100, -5.00, 5.0 }, "candidate_pT", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_phi_"+cut), ";#varphi (rad); counts", 100, -3.50, 3.5 }, "candidate_phi", cut ) );

                        hist1d.push_back( dd.Histo1D( { std::data("h1_rapdity_"+cut), ";y_{lab}; counts", 100, -3, 3 }, "candidate_rapidity", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_pT_err_"+cut), ";#Deltap_{T}; counts", 1000, -1, 1 }, "pT_err", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_phi_err_"+cut), ";#Delta#varphi; counts", 1000, -1, 1 }, "phi_err", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_eta_err_"+cut), ";#Delta#eta; counts", 1000, -1, 1 }, "eta_err", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_mass_err_"+cut), ";#Deltam; counts", 1000, -0.5, 0.5 }, "m_err", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_daughter1_cos_"+cut), ";cos(#varphi_{1}); counts", 1000, -1, 1.0 }, "daughter1_cos", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_daughter2_cos_"+cut), ";cos(#varphi_{2}); counts", 1000, -1, 1.0 }, "daughter2_cos", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_daughter1_chi2_prim_"+cut), ";#chi^{2}_{prim}^{1}; counts", 1000, -1000, 1000.0 }, "daughter1_chi2_prim", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_daughter2_chi2_prim_"+cut), ";#chi^{2}_{prim}^{2}; counts", 300, -300, 300.0 }, "daughter2_chi2_prim", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_chi2_geo_"+cut), ";#chi^{2}_{geo}; counts", 200, -200, 100.0 }, "candidate_chi2_geo", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_chi2_topo_"+cut), ";#chi^{2}_{topo}; counts", 300, -150, 150.0 }, "candidate_chi2_topo", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_cos_topo_"+cut), ";r_{#lambda}p_{#lambda}; counts", 1000, -1, 1.0 }, "candidate_cos_topo", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_daughter_dca_"+cut), ";DCA; counts", 200, -5, 5 }, "daughter_dca", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_L_"+cut), ";L; counts", 200, -50.0, 50.0 }, "candidate_L", cut ) );
                        hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_LdL_"+cut), ";L/dL; counts", 200, -50, 50.0 }, "candidate_LdL", cut ) );

		hist1d.push_back( dd.Histo1D( { std::data("h1_mass_"+cut), ";m (GeV/c^2); counts", 300, 1.05, 1.25 }, "candidate_mass", cut ) );
			hist2d.push_back( dd.Histo2D( { std::data("h2_pT_y_"+cut), ";y;p_{T} (GeV/c)", 120, -3, 3.0, 60, 0.0, 3  }, "candidate_rapidity", "candidate_pT", cut ) );
hist2d.push_back( dd.Histo2D( { std::data("h2__candidate_pt_simpt_"+cut), ";simpT;pT", 20, 0.0, 2, 20, 0, 2  }, "candidate_true_pt", "candidate_pT",cut ) );
	hist2d.push_back( dd.Histo2D( { std::data("h2__candidate_pt_simpt_"+cut), ";simpT;pT", 20, 0.0, 2, 20, 0, 2  }, "candidate_true_pt", "candidate_pT",cut ) );

	}

	auto file_out = TFile::Open("rec_lambda_sim_qa.root", "recreate");
	for( auto& h1 : hist1d )
		h1->Write();
	for( auto& h2 : hist2d )
		h2->Write();
        for( auto& h3 : hist3d )
                h3->Write();
	for( auto& h4 : hist4d )
                h4->Write();
	file_out->Close();
}
