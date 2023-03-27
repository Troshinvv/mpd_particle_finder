void lambda_qa(){
	ROOT::RDataFrame d("t", "candidates.root");
	auto dd = d.Define( "candidate_pT", "std::vector<float> pT; for( auto mom : candidate_momenta ){ pT.push_back( mom.Pt() ); } return pT;")
			.Define("candidate_phi", "std::vector<float> phi; for( auto mom : candidate_momenta ){ phi.push_back( mom.Phi() ); } return phi;")
			.Define("candidate_rapidity", "std::vector<float> rapidity; for( auto mom : candidate_momenta ){ rapidity.push_back( mom.Rapidity() ); } return rapidity;")
			.Define("pT_err", "std::vector<float> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(0) ); } return err;")
			.Define("phi_err", "std::vector<float> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(1) ); } return err;")
			.Define("eta_err", "std::vector<float> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(2) ); } return err;")
			.Define("m_err", "std::vector<float> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(3) ); } return err;")
			.Define("daughter1_cos", "std::vector<float> cosine; for( int i=0; i<candidate_cosines.at(0).size(); ++i ){ cosine.push_back( candidate_cosines.at(0).at(i) ); } return cosine;")
			.Define("daughter2_cos", "std::vector<float> cosine; for( int i=0; i<candidate_cosines.at(1).size(); ++i ){ cosine.push_back( candidate_cosines.at(1).at(i) ); } return cosine;")
			.Define("daughter1_chi2_prim", "std::vector<float> chi2; for( int i=0; i<daughter_chi2_prim.at(0).size(); ++i ){ chi2.push_back( daughter_chi2_prim.at(0).at(i) ); } return chi2;")
			.Define("daughter2_chi2_prim", "std::vector<float> chi2; for( int i=0; i<daughter_chi2_prim.at(1).size(); ++i ){ chi2.push_back( daughter_chi2_prim.at(1).at(i) ); } return chi2;")
			.Define("candidate_chi2_geo", "std::vector<float> chi2; for( int i=0; i<chi2_geo.at(0).size(); ++i ){ chi2.push_back( chi2_geo.at(0).at(i) ); } return chi2;")
			.Define("candidate_chi2_topo", "std::vector<float> chi2; for( int i=0; i<chi2_topo.at(0).size(); ++i ){ chi2.push_back( chi2_topo.at(0).at(i) ); } return chi2;")
			.Define("candidate_cosine_topo", "std::vector<float> cosine; for( int i=0; i<cos_topo_.at(0).size(); ++i ){ cosine.push_back( cos_topo_.at(0).at(i) ); } return cosine;")
			.Define("signal", "std::vector<int> signal; for( auto pid : candidate_true_pid ){ signal.push_back( pid == 3122 ); } return signal;")
			.Define("background", "std::vector<int> background; for( auto pid : candidate_true_pid ){ background.push_back( pid != 3122 ); } return background;")
			.Define("good_candidate", 
					"std::vector<int> good_candidate;"
					"for(int i=0; i<candidate_cosines.at(0).size(); ++i){"
						"if( candidate_cosines.at(0).at(i) < 0.96 ){"
							"good_candidate.push_back(0);"
							"continue;"
						"}"
						"if( candidate_cosines.at(1).at(i) < 0.995 ){"
							"good_candidate.push_back(0);"
							"continue;"
						"}"
						"if( candidate_L.at(i) < 1.5 ){"
							"good_candidate.push_back(0);"
							"continue;"
						"}"
						"if( candidate_cos_topo.at(0).at(i) < 0.99 ){"
							"good_candidate.push_back(0);"
							"continue;"
						"}"
						"if( chi2_topo.at(0).at(i) < 0.0 ){"
							"good_candidate.push_back(0);"
							"continue;"
						"}"
						"if( chi2_topo.at(0).at(i) > 15.0 ){"
							"good_candidate.push_back(0);"
							"continue;"
						"}"
						"if( m_err.at(i) > 0.0035 ){"
							"good_candidate.push_back(0);"
							"continue;"
						"}"													
						"if( daughter_dca.at(i) > 0.1 ){"
							"good_candidate.push_back(0);"
							"continue;"
						"}"													
						
						"good_candidate.push_back( 1 );" 
					"} return good_candidate;"
			)
			.Define("selected_signal", 
			"std::vector<int> selected_signal;"
			" for(int i=0; i<good_candidate.size(); ++i )"
			"{ "
				"if( signal.at(i) == 0 ){"
					"selected_signal.push_back(0);"
					"continue;"
				"}"
				"if( good_candidate.at(i) == 0 ){"
					"selected_signal.push_back(0);"
					"continue;"
				"}"
				"selected_signal.push_back( 1 );"
			"}"
			" return selected_signal;")
			.Define("selected_background", 
			"std::vector<int> selected_background;"
			" for(int i=0; i<good_candidate.size(); ++i )"
			"{ "
				"if( signal.at(i) == 1 ){"
					"selected_background.push_back(0);"
					"continue;"
				"}"
				"if( good_candidate.at(i) == 0 ){"
					"selected_background.push_back(0);"
					"continue;"
				"}"
				"selected_background.push_back( 1 );"
			"}"
			" return selected_background;")
			
	;

	std::vector<ROOT::RDF::RResultPtr<::TH1D>> hist1d;
	std::vector<ROOT::RDF::RResultPtr<::TH2D>> hist2d;
	std::vector<std::string> cuts{ "signal", "background", "good_candidate", "selected_signal", "selected_background" };

	hist1d.push_back( dd.Histo1D( { "h1_mass", ";m (GeV/c^2); counts", 100, 1.05, 1.25 }, "candidate_mass" ) );
	hist1d.push_back( dd.Histo1D( { "h1_pT", ";p_{T} (GeV/c); counts", 100, 0.00, 2.0 }, "candidate_pT" ) );
	hist1d.push_back( dd.Histo1D( { "h1_phi", ";#varphi (rad); counts", 100, -3.50, 3.5 }, "candidate_phi" ) );
	hist1d.push_back( dd.Histo1D( { "h1_rapdity", ";y_{lab}; counts", 100, 0.0, 3.5 }, "candidate_rapidity" ) );
	hist1d.push_back( dd.Histo1D( { "h1_pT_err", ";#Deltap_{T}; counts", 100, 0.0, 0.1 }, "pT_err" ) );
	hist1d.push_back( dd.Histo1D( { "h1_phi_err", ";#Delta#varphi; counts", 100, 0.0, 0.1 }, "phi_err" ) );
	hist1d.push_back( dd.Histo1D( { "h1_eta_err", ";#Delta#eta; counts", 100, 0.0, 0.1 }, "eta_err" ) );
	hist1d.push_back( dd.Histo1D( { "h1_mass_err", ";#Deltam; counts", 100, 0.0, 0.05 }, "m_err" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter1_cos", ";cos(#varphi_{1}); counts", 100, 0.8, 1.0 }, "daughter1_cos" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter2_cos", ";cos(#varphi_{2}); counts", 100, 0.8, 1.0 }, "daughter2_cos" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter1_chi2_prim", ";#chi^{2}_{prim}^{1}; counts", 150, 0.0, 150.0 }, "daughter1_chi2_prim" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter2_chi2_prim", ";#chi^{2}_{prim}^{2}; counts", 150, 0.0, 150.0 }, "daughter2_chi2_prim" ) );
	hist1d.push_back( dd.Histo1D( { "h1_candidate_chi2_geo", ";#chi^{2}_{geo}; counts", 150, 0.0, 5.0 }, "candidate_chi2_geo" ) );
	hist1d.push_back( dd.Histo1D( { "h1_candidate_chi2_topo", ";#chi^{2}_{topo}; counts", 150, 0.0, 150.0 }, "candidate_chi2_topo" ) );
	hist1d.push_back( dd.Histo1D( { "h1_candidate_cos_topo", ";r_{#lambda}p_{#lambda}; counts", 100, 0.8, 1.0 }, "candidate_cosine_topo" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter_dca", ";DCA; counts", 100, 0.0, 0.5 }, "daughter_dca" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter_distance_to_sv", ";DCA_{prim}; counts", 100, 0.0, 10.0 }, "daughter_distance_to_sv" ) );
	hist1d.push_back( dd.Histo1D( { "h1_candidate_L", ";L; counts", 100, 0.0, 10.0 }, "candidate_L" ) );
	hist1d.push_back( dd.Histo1D( { "h1_candidate_LdL", ";L/dL; counts", 100, 0.0, 6.0 }, "candidate_L" ) );
	hist1d.push_back( dd.Histo1D( { "h1_candidate_LdL", ";L/dL; counts", 100, 0.0, 6.0 }, "candidate_LdL" ) );
	
	for( auto cut : cuts ){
		hist1d.push_back( dd.Histo1D( { std::data("h1_mass_"+cut), ";m (GeV/c^2); counts", 100, 1.05, 1.25 }, "candidate_mass", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_pT_"+cut), ";p_{T} (GeV/c); counts", 100, 0.00, 2.0 }, "candidate_pT", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_phi_"+cut), ";#varphi (rad); counts", 100, -3.50, 3.5 }, "candidate_phi", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_rapdity_"+cut), ";y_{lab}; counts", 100, 0.0, 3.5 }, "candidate_rapidity", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_pT_err_"+cut), ";#Deltap_{T}; counts", 100, 0.0, 0.1 }, "pT_err", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_phi_err_"+cut), ";#Delta#varphi; counts", 100, 0.0, 0.1 }, "phi_err", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_eta_err_"+cut), ";#Delta#eta; counts", 100, 0.0, 0.1 }, "eta_err", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_mass_err_"+cut), ";#Deltam; counts", 100, 0.0, 0.05 }, "m_err", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter1_cos_"+cut), ";cos(#varphi_{1}); counts", 100, 0.8, 1.0 }, "daughter1_cos", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter2_cos_"+cut), ";cos(#varphi_{2}); counts", 100, 0.8, 1.0 }, "daughter2_cos", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter1_chi2_prim_"+cut), ";#chi^{2}_{prim}^{1}; counts", 150, 0.0, 150.0 }, "daughter1_chi2_prim", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter2_chi2_prim_"+cut), ";#chi^{2}_{prim}^{2}; counts", 150, 0.0, 150.0 }, "daughter2_chi2_prim", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_chi2_geo_"+cut), ";#chi^{2}_{geo}; counts", 150, 0.0, 5.0 }, "candidate_chi2_geo", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_chi2_topo_"+cut), ";#chi^{2}_{topo}; counts", 150, 0.0, 150.0 }, "candidate_chi2_topo", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_cos_topo_"+cut), ";r_{#lambda}p_{#lambda}; counts", 100, 0.8, 1.0 }, "candidate_cosine_topo", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter_dca_"+cut), ";DCA; counts", 100, 0.0, 0.5 }, "daughter_dca", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter_distance_to_sv_"+cut), ";DCA_{prim}; counts", 100, 0.0, 10.0 }, "daughter_distance_to_sv", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_L_"+cut), ";L; counts", 100, 0.0, 10.0 }, "candidate_L", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_LdL_"+cut), ";L/dL; counts", 100, 0.0, 6.0 }, "candidate_L", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_LdL_"+cut), ";L/dL; counts", 100, 0.0, 6.0 }, "candidate_LdL", cut ) );
	}
	auto file_out = TFile::Open("lambda_candidates_qa.root", "recreate");
	for( auto& h1 : hist1d )
		h1->Write();
	file_out->Close();
}
