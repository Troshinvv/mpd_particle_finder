void rec_k_short_sim_qa(std::string str_efficiency_file="/lustre/home/user/v/vtroshin/mpd_hyperons/rec30_mpd_eff_map_lamb.root"){
std::unique_ptr<TFile> efficiency_file{TFile::Open( str_efficiency_file.c_str(), "READ" )};
  TH2D* efficiency_histo{nullptr};
  efficiency_file->GetObject( "h2_pT_y_selected_signal", efficiency_histo );
  if( !efficiency_histo ){ std::cout << "Efficiency histogram cannot be retrieved from file" << std::endl; }
	ROOT::RDataFrame d("t", "candidates.root");
	auto dd = d
                        .Define( "tru_pT", "std::vector<float> pT; for( auto mom : simMom ){ pT.push_back( mom.Pt() ); } return pT;")
      .Define( "tru_y", "std::vector<float> y; for( auto mom : simMom ){ y.push_back( mom.Eta() ); } return y;")
      .Define( "is_lambda", "simPdg ==310")
      .Define( "is_fd_lambda", "simPdg == 310 && simMotherId != -1")
      .Define( "is_prim_lambda", "simPdg == 310 && simMotherId == -1")
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
               if(is_part.at(i)!=1 || pt.at(i)<0.5 || pt.at(i)>3){
                 in_range.push_back(0);}
               else{
                 in_range.push_back(1);}
             }
             return in_range;
         }, {"is_lambda","tru_pT","tru_y"})
         .Define("is_fd_range",[](ROOT::VecOps::RVec<int> is_part,std::vector<float> pt, std::vector<float> eta){
             std::vector<int> in_range;
             for(int i=0;i<pt.size();i++){
               if(is_part.at(i)!=1 || pt.at(i)<0.5 || pt.at(i)>3){
                 in_range.push_back(0);}
               else{
                 in_range.push_back(1);}
             }
             return in_range;
         }, {"is_fd_lambda","tru_pT","tru_y"})
         .Define("is_prim_range",[](ROOT::VecOps::RVec<int> is_part,std::vector<float> pt, std::vector<float> eta){
             std::vector<int> in_range;
             for(int i=0;i<pt.size();i++){
               if(is_part.at(i)!=1 || pt.at(i)<0.5 || pt.at(i)>3){
                 in_range.push_back(0);}
               else{
                 in_range.push_back(1);}
             }
             return in_range;
         }, {"is_prim_lambda","tru_pT","tru_y"})

      .Define("candidate_p", "std::vector<float> p; for( auto mom : candidate_momenta ){ p.push_back( mom.P() ); } return p;")
			.Define( "candidate_pT", "std::vector<float> pT; for( auto mom : candidate_momenta ){ pT.push_back( mom.Pt() ); } return pT;")
			.Define("candidate_phi", "std::vector<float> phi; for( auto mom : candidate_momenta ){ phi.push_back( mom.Phi() ); } return phi;")
			.Define("candidate_rapidity", "std::vector<float> rapidity; for( auto mom : candidate_momenta ){ rapidity.push_back( mom.Eta() ); } return rapidity;")
			.Define("pT_err", "std::vector<float> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(0) ); } return err;")
			.Define("phi_err", "std::vector<float> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(1) ); } return err;")
			.Define("eta_err", "std::vector<float> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(2) ); } return err;")
			.Define("m_err", "std::vector<float> err; for( auto mom : candidate_momentum_errors ){ err.push_back( mom.at(3) ); } return err;")
			.Define("daughter1_cos", "std::vector<float> cosine; for( int i=0; i<daughter_cosines.at(0).size(); ++i ){ cosine.push_back( daughter_cosines.at(0).at(i) ); } return cosine;")
			.Define("daughter2_cos", "std::vector<float> cosine; for( int i=0; i<daughter_cosines.at(1).size(); ++i ){ cosine.push_back( daughter_cosines.at(1).at(i) ); } return cosine;")
			.Define("daughter1_chi2_prim", "std::vector<float> chi2; for( int i=0; i<daughter_chi2_prim.at(0).size(); ++i ){ chi2.push_back( daughter_chi2_prim.at(0).at(i) ); } return chi2;")
			.Define("daughter2_chi2_prim", "std::vector<float> chi2; for( int i=0; i<daughter_chi2_prim.at(1).size(); ++i ){ chi2.push_back( daughter_chi2_prim.at(1).at(i) ); } return chi2;")                        
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
                        .Define("signal", "candidate_true_pid == 310 && candidate_true_id == -1")
                        .Define("background", "candidate_true_pid != 310")
                         .Define("feed_down", "candidate_true_pid == 310 && candidate_true_id != -1")
          .Define("under_the_peak",
                        "std::vector<double> under;"
                                        "for(int i=0; i<daughter_chi2_prim.at(0).size(); ++i){"
                                                "if( candidate_mass.at(i) >0.52 || candidate_mass.at(i) <0.48){"
                                                           "under.push_back(0);"
                                                        "continue;"
                                                           "}"
                                                        "under.push_back(1 );"
                                        "} return under;"
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
                                                "if( daughter1_chi2_prim.at(i) < 100 ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( daughter2_chi2_prim.at(i) < 100 ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( candidate_L.at(i) < 1 ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( candidate_LdL.at(i) < 15 ){"
                                                        "basic_var.push_back(0);"
                                                        "continue;"
                                                "}"
                                                "if( candidate_cos_topo.at(i) < 0.995){"
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
                                                "if( candidate_rapidity.at(i) > 1.2 || candidate_rapidity.at(i) < -1.2 || candidate_pT.at(i)<0.5 || candidate_pT.at(i)>3 ){"
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
                                "selected_signal.push_back( 1 );"
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
                        " return selected_background;")

.Define("under_signal",
                        "std::vector<int> under_signal;"
                        " for(int i=0; i<daughter_chi2_prim.at(0).size(); ++i )"
                        "{ "
                                "if( signal.at(i) == 0 ){"
                                        "under_signal.push_back(0);"
                                        "continue;"
                                "}"
                                "if( under_the_peak.at(i) == 0 ){"
                                        "under_signal.push_back(0);"
                                        "continue;"
                                "}"
                                "if( candidate_rapidity.at(i) > 1.2 || candidate_rapidity.at(i) < -1.2 || candidate_pT.at(i)<0.5 || candidate_pT.at(i)>3 ){"
                                                        "under_signal.push_back(0);"
                                                        "continue;"
                                                "}"
                                "under_signal.push_back( 1 );"
                        "}"
                        " return under_signal;")
                        .Define("under_background",
                        "std::vector<int> under_background;"
                        " for(int i=0; i<daughter_chi2_prim.at(0).size(); ++i )"
                        "{ "
                                "if( background.at(i) == 0 ){"
                                        "under_background.push_back(0);"
                                        "continue;"
                                "}"
                                "if( under_the_peak.at(i) == 0 ){"
                                        "under_background.push_back(0);"
                                        "continue;"
                                "}"
                                "if( candidate_true_pid.at(i) == -993 ){"
                                        "under_background.push_back(0);"
                                        "continue;"
                                "}"
                                "if( candidate_rapidity.at(i) > 1.2 || candidate_rapidity.at(i) < -1.2 || candidate_pT.at(i)<0.5 || candidate_pT.at(i)>3 ){"
                                                        "under_background.push_back(0);"
                                                        "continue;"
                                                "}"
                                "under_background.push_back( 1 );"
                        "}"
                        " return under_background;")

           .Define("background_2",
                       "std::vector<int> selected_background2;"
                        " for(int i=0; i<basic_var.size(); ++i )"
                        "{ "
                                "if( background.at(i) == 0 ){"
                                        "selected_background2.push_back(0);"
                                        "continue;"
                                "}"
                                "if( basic_var.at(i) == 0 ){"
                                        "selected_background2.push_back(0);"
                                        "continue;"
                                "}"
                                "if( candidate_origin_flag.at(i) !=-2  ){"
                                                        "selected_background2.push_back(0);"
                                                        "continue;"
                                                "}"
                                "selected_background2.push_back( 1 );"
                        "}"
                        " return selected_background2;")
.Define("background_3",
                        "std::vector<int> selected_background3;"
                        " for(int i=0; i<basic_var.size(); ++i )"
                        "{ "
                                "if( background.at(i) == 0 ){"
                                        "selected_background3.push_back(0);"
                                        "continue;"
                                "}"
                                "if( basic_var.at(i) == 0 ){"
                                        "selected_background3.push_back(0);"
                                        "continue;"
                                "}"
                                "if( candidate_origin_flag.at(i) !=-3  ){"
                                                        "selected_background3.push_back(0);"
                                                        "continue;"
                                                "}"
                                "selected_background3.push_back( 1 );"
                        "}"
                        " return selected_background3;")
.Define("background_4",
                        "std::vector<int> selected_background4;"
                        " for(int i=0; i<basic_var.size(); ++i )"
                        "{ "
                                "if( background.at(i) == 0 ){"
                                        "selected_background4.push_back(0);"
                                        "continue;"
                                "}"
                                "if( basic_var.at(i) == 0 ){"
                                        "selected_background4.push_back(0);"
                                        "continue;"
                                "}"
                                "if( candidate_origin_flag.at(i) !=-4  ){"
                                                        "selected_background4.push_back(0);"
                                                        "continue;"
                                                "}"
                                "selected_background4.push_back( 1 );"
                        "}"
                        " return selected_background4;")
.Define("background_5",
                        "std::vector<int> selected_background5;"
                        " for(int i=0; i<basic_var.size(); ++i )"
                        "{ "
                                "if( background.at(i) == 0 ){"
                                        "selected_background5.push_back(0);"
                                        "continue;"
                                "}"
                                "if( basic_var.at(i) == 0 ){"
                                        "selected_background5.push_back(0);"
                                        "continue;"
                                "}"
                                "if( candidate_origin_flag.at(i) !=-5  ){"
                                                        "selected_background5.push_back(0);"
                                                        "continue;"
                                                "}"
                                "selected_background5.push_back( 1 );"
                        "}"
                        " return selected_background5;")
                        .Define("background_6",
                        "std::vector<int> selected_background6;"
                        " for(int i=0; i<basic_var.size(); ++i )"
                        "{ "
                                "if( background.at(i) == 0 ){"
                                        "selected_background6.push_back(0);"
                                        "continue;"
                                "}"
                                "if( basic_var.at(i) == 0 ){"
                                        "selected_background6.push_back(0);"
                                        "continue;"
                                "}"
                                "if( candidate_origin_flag.at(i) !=-6  ){"
                                                        "selected_background6.push_back(0);"
                                                        "continue;"
                                                "}"
                                "if( candidate_mass.at(i) <1.105 || candidate_mass.at(i) >1.13 ){"
                                        "selected_background6.push_back(0);"
                                        "continue;"
                                "}"
                                "selected_background6.push_back( 1 );"
                        "}"
                        " return selected_background6;")
                        .Define("background_other",
                        "std::vector<int> selected_backgroundother;"
                        " for(int i=0; i<basic_var.size(); ++i )"
                        "{ "
                                "if( background.at(i) == 0 ){"
                                        "selected_backgroundother.push_back(0);"
                                        "continue;"
                                "}"
                                "if( basic_var.at(i) == 0 ){"
                                        "selected_backgroundother.push_back(0);"
                                        "continue;"
                                "}"
                                "if( candidate_origin_flag.at(i) ==-6 || candidate_origin_flag.at(i) ==-5 || candidate_origin_flag.at(i) ==-4 || candidate_origin_flag.at(i) ==-3 || candidate_origin_flag.at(i) ==-2  ){"
                                                        "selected_backgroundother.push_back(0);"
                                                        "continue;"
                                                "}"
                                "selected_backgroundother.push_back( 1 );"
                        "}"
                        " return selected_backgroundother;")
.Define("selected_feed_down",
                        "std::vector<int> selected_feed_down;"
                        " for(int i=0; i<basic_var.size(); ++i )"
                        "{ "
                                "if( feed_down.at(i) == 0 ){"
                                        "selected_feed_down.push_back(0);"
                                        "continue;"
                                "}"
                                "if( basic_var.at(i) == 0 ){"
                                        "selected_feed_down.push_back(0);"
                                        "continue;"
                                "}"
                                "selected_feed_down.push_back( 1 );"
                        "}"
                        " return selected_feed_down;")
	;

	std::vector<ROOT::RDF::RResultPtr<::TH1D>> hist1d;
	std::vector<ROOT::RDF::RResultPtr<::TH2D>> hist2d;
        std::vector<ROOT::RDF::RResultPtr<::TH3D>> hist3d;
        double gaps[21];
        for(int i=0;i<21;i++){
gaps[i]=(0.6-0.4)/20*i +0.4;
}
//double gappt[7]={0,0.25,0.5,0.75,1.0,1.5,2.0};
//double gapy[13]={-0.2,-0.1,0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};
double gappt[9]={0,0.2,0.5,0.8,1.0,1.2,1.5,2.0,3.0};
double gapy[7]={-1.2,-0.8,-0.4,0,0.4,0.8,1.2};
        std::vector<std::string> cuts{ "signal", "background", "basic_var", "selected_signal", "selected_background","under_signal","under_background"/*,"feed_down","selected_feed_down","background_2","background_3","background_4","background_5","background_6", "background_other"*/ };


	hist1d.push_back( dd.Histo1D( { "h1_mass", ";m (GeV/c^2); counts", 100, 0.4, 0.6 }, "candidate_mass" ) );
	hist1d.push_back( dd.Histo1D( { "h1_pT", ";p_{T} (GeV/c); counts", 100, -1.00, 5.0 }, "candidate_pT" ) );
	hist1d.push_back( dd.Histo1D( { "h1_phi", ";#varphi (rad); counts", 100, -3.50, 3.5 }, "candidate_phi" ) );
	hist1d.push_back( dd.Histo1D( { "h1_rapdity", ";y_{lab}; counts", 100, -3, 3 }, "candidate_rapidity" ) );
	hist1d.push_back( dd.Histo1D( { "h1_pT_err", ";#Deltap_{T}; counts", 100, 0.0, 0.1 }, "pT_err" ) );
	hist1d.push_back( dd.Histo1D( { "h1_phi_err", ";#Delta#varphi; counts", 100, 0.0, 0.1 }, "phi_err" ) );
	hist1d.push_back( dd.Histo1D( { "h1_eta_err", ";#Delta#eta; counts", 100, 0.0, 0.1 }, "eta_err" ) );
	hist1d.push_back( dd.Histo1D( { "h1_mass_err", ";#Deltam; counts", 100, 0.0, 0.05 }, "m_err" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter1_cos", ";cos(#varphi_{1}); counts", 100, 0.8, 1.1 }, "daughter1_cos" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter2_cos", ";cos(#varphi_{2}); counts", 100, 0.98, 1.1 }, "daughter2_cos" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter1_chi2_prim", ";#chi^{2}_{prim}^{1}; counts", 500, -10.0, 1000.0 }, "daughter1_chi2_prim" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter2_chi2_prim", ";#chi^{2}_{prim}^{2}; counts", 150, -10.0, 300.0 }, "daughter2_chi2_prim" ) );
	hist1d.push_back( dd.Histo1D( { "h1_candidate_chi2_geo", ";#chi^{2}_{geo}; counts", 500, -10.0, 100.0 }, "candidate_chi2_geo" ) );
	hist1d.push_back( dd.Histo1D( { "h1_candidate_chi2_topo", ";#chi^{2}_{topo}; counts", 150, -10.0, 150.0 }, "candidate_chi2_topo" ) );
	hist1d.push_back( dd.Histo1D( { "h1_candidate_cos_topo", ";r_{#lambda}p_{#lambda}; counts", 100, 0.98, 1.1 }, "candidate_cos_topo" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter_dca", ";DCA; counts", 200, -1.0, 1 }, "daughter_dca" ) );
	hist1d.push_back( dd.Histo1D( { "h1_daughter_distance_to_sv", ";DCA_{prim}; counts", 100, -10.0, 10.0 }, "distance_to_sv" ) );
	hist1d.push_back( dd.Histo1D( { "h1_candidate_L", ";L; counts", 200, -20.0, 20.0 }, "candidate_L" ) );
	hist1d.push_back( dd.Histo1D( { "h1_candidate_LdL", ";L/dL; counts", 200, -20.0, 20.0 }, "candidate_LdL" ) );
	hist2d.push_back( dd.Histo2D( { "h2_pT_y", ";y;p_{T} (GeV/c)", 120, -3.0, 3.0, 60, 0.0, 3  }, "candidate_rapidity", "candidate_pT" ) );
        hist2d.push_back( dd.Histo2D( { "h2_tru_pT_tru_y", ";y;p_{T} (GeV/c)",120, -3.0, 3.0, 60, 0.0, 3 }, "tru_y", "tru_pT", "is_lambda" ) );
        hist2d.push_back( dd.Histo2D( { "h2_fd_tru_pT_tru_y", ";y;p_{T} (GeV/c)", 120, -3.0, 3.0, 60, 0.0, 3 }, "tru_y", "tru_pT", "is_fd_lambda" ) );

hist1d.push_back( dd.Histo1D( { "h1_psi_rp", ";psi_rp counts", 500, -8, 8 }, "mcRP" ) );

       
 hist2d.push_back( dd.Histo2D( { "h2_m_err_m_inv", ";#Deltam (GeV/c^2);m_inv (GeV/c^2)", 100, 0.0, 0.01, 100, 0.4, 0.6  }, "m_err", "candidate_mass" ) );
 hist2d.push_back( dd.Histo2D( { "h2_pi_chi2_prim_m_inv", ";#chi^{2}_{prim}^{1};m_inv (GeV/c^2)", 100, 0.0, 300, 100, 0.4, 0.6  }, "daughter1_chi2_prim", "candidate_mass" ) );       
 hist2d.push_back( dd.Histo2D( { "h2_chi2_topo_m_inv", ";#chi^{2}_{topo};m_inv (GeV/c^2)", 100, 0.0, 100, 100, 0.4, 0.6  }, "candidate_chi2_topo", "candidate_mass" ) );
hist2d.push_back( dd.Histo2D( { "h2_p_chi2_prim_m_inv", ";#chi^{2}_{prim}^{2};m_inv (GeV/c^2)", 100, 0.0, 100, 100, 0.4, 0.6  }, "daughter2_chi2_prim", "candidate_mass" ) );
hist2d.push_back( dd.Histo2D( { "h2_cos_p_lam_m_inv", ";cos(#varphi_{2});m_inv (GeV/c^2)", 100, 0.98, 1, 100, 0.4, 0.6  }, "daughter2_cos", "candidate_mass" ) );
hist2d.push_back( dd.Histo2D( { "h2_dca_m_inv", ";DCA;m_inv (GeV/c^2)", 200, 0.0, 2, 100, 0.4, 0.6  }, "daughter_dca", "candidate_mass" ) );
hist2d.push_back( dd.Histo2D( { "h2_chi2_geo_m_inv", ";#chi^{2}_{geo};m_inv (GeV/c^2)", 50, 0.0, 100, 100, 0.4, 0.6  }, "candidate_chi2_geo", "candidate_mass" ) );
hist2d.push_back( dd.Histo2D( { "h2_L_m_inv", ";L;m_inv (GeV/c^2)", 160, -20.0, 20, 100, 0.4, 0.6  }, "candidate_L", "candidate_mass" ) );
hist2d.push_back( dd.Histo2D( { "h2_LdL_m_inv", ";L/dL;m_inv (GeV/c^2)", 320, -40.0, 40, 100, 0.4, 0.6  }, "candidate_LdL", "candidate_mass" ) );
hist2d.push_back( dd.Histo2D( { "h2_cos_topo_m_inv", ";r_{#lambda}p_{#lambda};m_inv (GeV/c^2)", 100, 0.98, 1, 100, 0.4, 0.6  }, "candidate_cos_topo", "candidate_mass" ) );

hist1d.push_back( dd.Profile1D( { "prof1d_tru_v1_vs_y", ";y_{cm};v_{1}", 20,-2,2 },"tru_y", "tru_v1", "is_range" ) );
hist1d.push_back( dd.Profile1D( { "prof1d_tru_v2_vs_pT", ";p_{T} GeV/c;v_{2}", 20,0,3 },"tru_pT", "tru_v2", "is_v2_range" ) );
hist1d.push_back( dd.Profile1D( { "prof1d_tru_fd_v1_vs_y", ";y_{cm};v_{1}", 20,-2,2 },"tru_y", "tru_v1", "is_fd_range" ) );
hist1d.push_back( dd.Profile1D( { "prof1d_tru_prim_v1_vs_y", ";y_{cm};v_{1}", 20,-2,2 },"tru_y", "tru_v1", "is_prim_range" ) );

	for( auto cut : cuts ){
		hist1d.push_back( dd.Histo1D( { std::data("h1_mass_"+cut), ";m (GeV/c^2); counts", 300, 0.4, 0.6 }, "candidate_mass", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_pT_"+cut), ";p_{T} (GeV/c); counts", 100, 0.00, 5.0 }, "candidate_pT", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_phi_"+cut), ";#varphi (rad); counts", 100, -3.50, 3.5 }, "candidate_phi", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_rapdity_"+cut), ";y_{lab}; counts", 100, -3, 3 }, "candidate_rapidity", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_pT_err_"+cut), ";#Deltap_{T}; counts", 100, 0.0, 0.1 }, "pT_err", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_phi_err_"+cut), ";#Delta#varphi; counts", 100, 0.0, 0.1 }, "phi_err", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_eta_err_"+cut), ";#Delta#eta; counts", 100, 0.0, 0.1 }, "eta_err", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_mass_err_"+cut), ";#Deltam; counts", 100, 0.0, 0.05 }, "m_err", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter1_cos_"+cut), ";cos(#varphi_{1}); counts", 1000, -1, 1.0 }, "daughter1_cos", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter2_cos_"+cut), ";cos(#varphi_{2}); counts", 1000, -1, 1.0 }, "daughter2_cos", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter1_chi2_prim_"+cut), ";#chi^{2}_{prim}^{1}; counts", 500, 0.0, 1000.0 }, "daughter1_chi2_prim", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter2_chi2_prim_"+cut), ";#chi^{2}_{prim}^{2}; counts", 150, 0.0, 300.0 }, "daughter2_chi2_prim", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_chi2_geo_"+cut), ";#chi^{2}_{geo}; counts", 100, 0.0, 100.0 }, "candidate_chi2_geo", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_chi2_topo_"+cut), ";#chi^{2}_{topo}; counts", 150, 0.0, 150.0 }, "candidate_chi2_topo", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_cos_topo_"+cut), ";r_{#lambda}p_{#lambda}; counts", 200, 0.95, 1.0 }, "candidate_cos_topo", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter_dca_"+cut), ";DCA; counts", 100, 0.0, 5 }, "daughter_dca", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_daughter_distance_to_sv_"+cut), ";DCA_{prim}; counts", 100, 0.0, 10.0 }, "distance_to_sv", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_L_"+cut), ";L; counts", 100, 0.0, 50.0 }, "candidate_L", cut ) );
			hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_LdL_"+cut), ";L/dL; counts", 100, 0.0, 50.0 }, "candidate_LdL", cut ) );
hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_true_pid_"+cut), ";pid; counts", 20000, -10000, 10000 }, "candidate_true_pid", cut ) );
hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_true_id_"+cut), ";id; counts", 20000, -10000, 10000 }, "candidate_true_id", cut ) );
hist1d.push_back( dd.Histo1D( { std::data("h1_candidate_origin_flag_"+cut), ";origin flag; counts", 20, -10, 10 }, "candidate_origin_flag", cut ) );
			hist2d.push_back( dd.Histo2D( { std::data("h2_pT_y_"+cut), ";y;p_{T} (GeV/c)", 120, -3, 3.0, 60, 0.0, 3  }, "candidate_rapidity", "candidate_pT", cut ) );


hist2d.push_back( dd.Histo2D( { std::data("h2_m_err_m_inv"+cut), ";#Deltam (GeV/c^2);m_inv (GeV/c^2)", 100, 0.0, 0.01, 100, 0.4, 0.6  }, "m_err", "candidate_mass",cut ) );
 hist2d.push_back( dd.Histo2D( { std::data("h2_pi_chi2_prim_m_inv"+cut), ";#chi^{2}_{prim}^{1};m_inv (GeV/c^2)", 10000, 0, 10000, 100, 0.4, 0.6  }, "daughter1_chi2_prim", "candidate_mass",cut ) );       
 hist2d.push_back( dd.Histo2D( { std::data("h2_chi2_topo_m_inv"+cut), ";#chi^{2}_{topo};m_inv (GeV/c^2)", 200, 0.0, 200, 100, 0.4, 0.6  }, "candidate_chi2_topo", "candidate_mass",cut ) );
hist2d.push_back( dd.Histo2D( { std::data("h2_p_chi2_prim_m_inv"+cut), ";#chi^{2}_{prim}^{2};m_inv (GeV/c^2)", 1000, 0, 1000, 100, 0.4, 0.6  }, "daughter2_chi2_prim", "candidate_mass",cut ) );
hist2d.push_back( dd.Histo2D( { std::data("h2_cos_p_lam_m_inv"+cut), ";cos(#varphi_{2});m_inv (GeV/c^2)", 1000, -1, 1, 100, 0.4, 0.6  }, "daughter2_cos", "candidate_mass",cut ) );
hist2d.push_back( dd.Histo2D( { std::data("h2_dca_m_inv"+cut), ";DCA;m_inv (GeV/c^2)", 200, 0.0, 2, 100, 0.4, 0.6  }, "daughter_dca", "candidate_mass",cut ) );
hist2d.push_back( dd.Histo2D( { std::data("h2_chi2_geo_m_inv"+cut), ";#chi^{2}_{geo};m_inv (GeV/c^2)", 100, 0.0, 100, 100, 0.4, 0.6  }, "candidate_chi2_geo", "candidate_mass",cut ) );
hist2d.push_back( dd.Histo2D( { std::data("h2_L_m_inv"+cut), ";L;m_inv (GeV/c^2)", 200, -50.0, 50, 100, 0.4, 0.6  }, "candidate_L", "candidate_mass",cut ) );
hist2d.push_back( dd.Histo2D( { std::data("h2_LdL_m_inv"+cut), ";L/dL;m_inv (GeV/c^2)", 400, -100.0, 100, 100, 0.4, 0.6  }, "candidate_LdL", "candidate_mass",cut ) );
hist2d.push_back( dd.Histo2D( { std::data("h2_cos_topo_m_inv"+cut), ";r_{#lambda}p_{#lambda};m_inv (GeV/c^2)", 1000, 0.99, 1, 100, 0.4, 0.6  }, "candidate_cos_topo", "candidate_mass",cut ) );

hist3d.push_back( dd.Histo3D( { std::data("h1_mass_pt_eta_"+cut),";m (GeV/c^2); p_{T} (GeV/c);#eta",20,gaps,8,gappt,6,gapy}, "candidate_mass", "candidate_pT","candidate_rapidity", cut ) );
	}
	auto file_out = TFile::Open("rec_lambda_sim_qa.root", "recreate");
	for( auto& h1 : hist1d )
		h1->Write();
	for( auto& h2 : hist2d )
		h2->Write();
        for( auto& h3 : hist3d )
                h3->Write();
	file_out->Close();
     //   auto ddd=dd.Filter("basic_var==1");
     //   ddd.Snapshot("t", "selected_lamb_candidates.root");
}
