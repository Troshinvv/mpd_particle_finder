//
// Created by mikhail on 2/28/21.
//

void upper_edge(){
  gROOT->Macro( "/mnt/c/Users/Misha/CLionProjects/RISOVALKA/example/bm@n_paper/style.cc" );
  auto leg1 = new TLegend( 0.6, 0.75, 0.9, 0.94 );
  auto leg2 = new TLegend( 0.2, 0.8, 0.5, 0.7 );
  MultiCorrelation stack_ref;

//  std::string file_rec = "/home/mikhail/flow_calculator/cmake-build-debug/fhcal_efficiency_2022_07_14.root";
  std::string file_rec = "/home/mikhail/lambda_DCMQGSM_SMM_3AGeV_2023_05_05.root";
  MultiCorrelation stack_rec;
  std::vector<std::string> histo_names{
          "h1_mass",
          "h1_pT",
          "h1_phi",
          "h1_rapdity",
          "h1_pT_err",
          "h1_phi_err",
          "h1_eta_err",
          "h1_mass_err",
          "h1_daughter1_cos",
          "h1_daughter2_cos",
          "h1_daughter1_chi2_prim",
          "h1_daughter2_chi2_prim",
          "h1_candidate_chi2_geo",
          "h1_candidate_chi2_topo",
          "h1_candidate_cos_topo",
          "h1_daughter_dca",
          "h1_candidate_L",
          "h1_candidate_LdL",

  };

  auto pic_layout = new HeapPicture("ag123", {2000, 1100});
  Book book;
  book.SetLayout( pic_layout );
  for( const auto& name : histo_names ){
    auto h1_sig = new Histogram1D(file_rec, std::vector<std::string>{ name+"_signal" }, "signal" );
    auto n_bins = h1_sig->GetHistogram()->GetXaxis()->GetNbins();
    auto norm_sig = h1_sig->GetHistogram()->Integral(0, n_bins+1);
    h1_sig->GetHistogram()->Scale( 1.0 / norm_sig );

    auto h1_bg = new Histogram1D(file_rec, std::vector<std::string>{ name+"_background" }, "background" );
    auto norm_bg = h1_bg->GetHistogram()->Integral(0, n_bins+1);
    h1_bg->GetHistogram()->Scale( 1.0 / norm_bg );

    auto g1_sig_acc = new TGraphErrors(n_bins);
    auto g1_bj_rej = new TGraphErrors(n_bins);
    auto g1_distance = new TGraphErrors(n_bins);
    float min_distance = 999.0;
    float min_x = -1.f;
    float optimal_acceptance = -1.f;
    float optimal_rejection = -1.f;
    std::vector<float> vec_x{};
    for( int i=1; i<=n_bins; i++ ){
      auto x = h1_bg->GetHistogram()->GetXaxis()->GetBinCenter(i);
      vec_x.push_back(x);
      auto acceptance = h1_sig->GetHistogram()->Integral(0, i);
      g1_sig_acc->SetPoint( i-1, x, acceptance );
      auto rejection = h1_bg->GetHistogram()->Integral(i, n_bins+1);
      g1_bj_rej->SetPoint( i-1, x, rejection );
      auto distance = sqrt((acceptance-1)*(acceptance-1) + (rejection-1)*(rejection-1));
      g1_distance->SetPoint( i-1, x, distance );
      if(distance < min_distance) {
        min_distance = distance;
        min_x = x;
        optimal_acceptance = acceptance;
        optimal_rejection = rejection;
      }
    }
    auto graph_acceptance_rate = new Graph();
    graph_acceptance_rate->SetPoints(g1_sig_acc);
    graph_acceptance_rate->SetTitle("signal");
    graph_acceptance_rate->SetStyle( kBlue, -1 );

    auto graph_rejection_rate = new Graph();
    graph_rejection_rate->SetPoints(g1_bj_rej);
    graph_rejection_rate->SetTitle("background");
    graph_rejection_rate->SetStyle( kRed, -1 );

    auto graph_distance = new Graph();
    graph_distance->SetPoints(g1_distance);
    graph_distance->SetTitle("distance");
    graph_distance->SetStyle( kBlack, -1 );

    h1_bg->SetStyle( kRed, -1 );

    auto pic = new HeapPicture( name, {1500, 1100} );
    pic->AddText({0.55, 0.4, std::data("Parameter <= "+std::to_string(min_x)) }, 0.03);
    pic->AddText({0.55, 0.37, std::data("BG rejection: "+std::to_string(optimal_rejection))}, 0.03);
    pic->AddText({0.55, 0.34, std::data("Sig. acceptance: "+std::to_string(optimal_acceptance))}, 0.03);
    pic->AddLine( {min_x, 0.0, min_x, 1.0}, kBlack, 2 );

    pic->SetXRange({vec_x.front(), vec_x.back()});
    pic->SetAxisTitles({name, "counts"});
    pic->AddDrawable( graph_acceptance_rate );
    pic->AddDrawable( graph_rejection_rate );
    pic->AddDrawable( graph_distance );
    pic->SetYRange({0.0, 1.2} );
    pic->Draw();
    pic->Save("/mnt/c/Users/Misha/CLionProjects/RISOVALKA/example/pfsimple/pictures/lambda_upper_edge/"+name, "png");
    book.AddPage(pic);
  }

  std::string save_name = "/mnt/c/Users/Misha/CLionProjects/RISOVALKA/example/pfsimple/pictures/lambda_upper_edge.pdf";
  book.SavePDF(save_name);
  book.SaveRoot("~/upper_edge.root");
}