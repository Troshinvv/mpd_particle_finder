void tru_lambda_qa(std::string list){
  TFileCollection collection( "collection", "", list.c_str() );
  auto* chain = new TChain( "t" );
  chain->AddFileInfoList( collection.GetList() );
  ROOT::RDataFrame d( *chain );
	auto dd = d
          .Define( "pT", "std::vector<float> pT; for( auto mom : simMom ){ pT.push_back( mom.Pt() ); } return pT;")
          .Define( "y", "std::vector<float> y; for( auto mom : simMom ){ y.push_back( mom.Rapidity() ); } return y;")
          .Define( "is_lambda", "simPdg == 3122")
          ;
	std::vector<ROOT::RDF::RResultPtr<::TH1D>> hist1d;
	std::vector<ROOT::RDF::RResultPtr<::TH2D>> hist2d;

  hist2d.push_back( dd.Histo2D( { "h2_pT_y", ";y;p_{T} (GeV/c)", 30, 0.0, 3.0, 25, 0.0, 2.5 }, "y", "pT", "is_lambda" ) );

	auto file_out = TFile::Open("tru_lambda_qa.root", "recreate");
	for( auto& h1 : hist2d )
		h1->Write();
	file_out->Close();
}
