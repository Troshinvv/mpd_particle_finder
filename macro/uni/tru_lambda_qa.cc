using namespace ROOT;
using namespace ROOT::Math;
using namespace ROOT::RDF;
void tru_lambda_qa(std::string list){
  TFileCollection collection( "collection", "", list.c_str() );
  auto* chain = new TChain( "decays" );
  chain->AddFileInfoList( collection.GetList() );
  ROOT::RDataFrame d( *chain );
	auto dd = d
	  .Define("centrality", [](Double_t B){
            float centrality{-1.f};
/*                if(B<3.44)
                centrality=5;
        else if(B<4.88)
                centrality=15;
        else if(B<5.84)
                centrality=25;
        else if(B<6.64)
                centrality=35;
        else if(B<7.44)
                centrality=45;
        else if(B<8.08)
                centrality=55;
        else if(B<8.72)
                centrality=65;
        else if(B<9.36)
                centrality=75;
        else if(B<9.84)
                centrality=85;
        else
                centrality=95;*/
		if(B<2.87406)
			centrality=2.5;
		else if(B<4.05374)
                        centrality=7.5;
		else if(B<5.0333)
                        centrality=12.5;
		else if(B<5.86304)
                        centrality=17.5;
		else if(B<6.58249)
                        centrality=22.5;
		else if(B<7.22197)
                        centrality=27.5;
		else if(B<7.80409)
                        centrality=32.5;
                else if(B<8.34523)
                        centrality=37.5;
                else if(B<8.8571)
                        centrality=42.5;
                else if(B<9.34824)
                        centrality=47.5;
                else if(B<9.8255)
                        centrality=52.5;
                else if(B<10.2956)
                        centrality=57.5;
		else if(B<10.7666)
                        centrality=62.5;
                else if(B<11.2494)
                        centrality=67.5;
                else if(B<11.7595)
                        centrality=72.5;
                else if(B<12.3179)
                        centrality=77.5;
                else if(B<12.9533)
                        centrality=82.5;
                else if(B<13.7034)
                        centrality=87.5;
		else 
                        centrality=95;
            return centrality;
            }, {"fB" })
//	.Filter("fB>4.05 && fB<7.8")
		.Define("Poly",
			"std::vector<double> y_comp;"
			"for(int i=0;i<LambdaPolarization.size();i++){"
				"if(LambdaPolarization.at(i).Y()!=0)"
				"y_comp.push_back(LambdaPolarization.at(i).Y()*100);"
			"}"
			"return y_comp;")	
          ;
	std::vector<ROOT::RDF::RResultPtr<::TH1D>> hist1d;

  hist1d.push_back( dd.Profile1D( { "p_poly_cent",";centrality, %;poly",10,0,100},"centrality","Poly") );


	auto file_out = TFile::Open("tru_lambda_qa.root", "recreate");
	for( auto& h1 : hist1d )
                h1->Write();
	file_out->Close();
}
