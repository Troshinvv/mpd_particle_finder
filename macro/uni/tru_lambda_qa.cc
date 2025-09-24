void tru_lambda_qa(std::string list){
  TFileCollection collection( "collection", "", list.c_str() );
  auto* chain = new TChain( "t" );
  chain->AddFileInfoList( collection.GetList() );
  ROOT::RDataFrame d( *chain );
	auto dd = d
          .Define( "pT", "std::vector<float> pT; for( auto mom : simMom ){ pT.push_back( mom.Pt() ); } return pT;")
          .Define( "y", "std::vector<float> y; for( auto mom : simMom ){ y.push_back( mom.Rapidity()-0.986446 ); } return y;")
          .Define( "is_lambda", "simPdg == 3122")
		.Define("is_uni_lamb",
                        "std::vector<double> under;"
                                        "for(int i=0; i<simMom.size(); ++i){"
						"if(simPdg.at(i)!=3122){"
							"under.push_back(0);"
							"continue;"
								"}"
                                                "if(fMCGenIDMap[i]!=-9 && fMCGenIDMap[i]!=-15 && fMCGenIDMap[i]!=-3){"
                                                           "under.push_back(0);"
                                                        "continue;"
                                                           "}"
                                                        "under.push_back(1 );"
                                        "} return under;"
                        )
		.Define("is_geant_lamb",
                        "std::vector<double> under;"
                                        "for(int i=0; i<simMom.size(); ++i){"
                                                "if(simPdg.at(i)!=3122){"
                                                        "under.push_back(0);"
                                                        "continue;"
                                                                "}"
                                                "if(fMCGenIDMap[i]==-9 || fMCGenIDMap[i]==-15 || fMCGenIDMap[i]==-3){"
                                                           "under.push_back(0);"
                                                        "continue;"
                                                           "}"
                                                        "under.push_back(1 );"
                                        "} return under;"
                        )
	
          ;
	std::vector<ROOT::RDF::RResultPtr<::TH1D>> hist1d;
	std::vector<ROOT::RDF::RResultPtr<::TH2D>> hist2d;

  hist2d.push_back( dd.Histo2D( { "h2_pT_y_uni", ";y;p_{T} (GeV/c)", 40, -2.0, 2.0, 25, 0.0, 2.5 }, "y", "pT", "is_uni_lamb" ) );
  hist2d.push_back( dd.Histo2D( { "h2_pT_y", ";y;p_{T} (GeV/c)", 40, -2.0, 2.0, 25, 0.0, 2.5 }, "y", "pT", "is_lambda" ) );
  hist2d.push_back( dd.Histo2D( { "h2_pT_y_geant", ";y;p_{T} (GeV/c)", 40, -2.0, 2.0, 25, 0.0, 2.5 }, "y", "pT", "is_geant_lamb" ) );

	auto file_out = TFile::Open("tru_lambda_qa.root", "recreate");
	for( auto& h1 : hist2d )
		h1->Write();
	file_out->Close();
}
