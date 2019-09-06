

void PlotIsochrone()
{

    Float_t C = 0.1892; //in 1/mm (using, mm and us convention at 1400 V)
    Float_t v = 60; //in mm/us
    Float_t d[4] = {3.0,2.5,2.0,1.5};
    Float_t t[4] = {0.02,0.03,0.04,0.05}; 
    
    
    TCanvas * CPol = new TCanvas("CPol","",800,800);
    Double_t rmin=0;
    Double_t rmax=TMath::Pi()*2;
    Double_t r[1000];
    Double_t theta[1000];
    
    
    //because root sucks, must set the range by plotting an invisible function
    TF1 * frange = new TF1("frange","x/3.2 + 0.5",rmin,rmax);
    for (Int_t ipt = 0; ipt < 1000; ipt++) {
        r[ipt] = ipt*(rmax-rmin)/1000+rmin;
        theta[ipt] = frange->Eval(r[ipt]);
    }
    TGraphPolar * grange;
    grange = new TGraphPolar(1000,r,theta);
    grange->SetTitle("");
    grange->SetLineColor(0);
    grange->SetLineWidth(10);
    
    grange->Draw();
    
    //make plots without lorentz effect
    TGraphPolar *gcir[4];
    for (int i=0; i<4; i++) {
        TF1 *fcir = new TF1(Form("fcir%i",i),Form("%f*%f",v,t[i]),rmin,rmax);
        for (Int_t ipt = 0; ipt < 1000; ipt++) {
            r[ipt] = ipt*(rmax-rmin)/1000+rmin;
            theta[ipt] = fcir->Eval(r[ipt]);
        }
        
        gcir[i] = new TGraphPolar(1000,r,theta);
        gcir[i]->SetTitle("");
        gcir[i]->SetLineColor(2);
        gcir[i]->SetLineWidth(2);
        //gcir[i]->SetLineStyle(2);
        gcir[i]->Draw("same");
    }

    
    //make plots with lorentz effect
    TGraphPolar *gdist[4];
    for (int i=0; i<4; i++) {
        TF1 *fdist = new TF1(Form("fdist%i",i),Form("(%f*%f)*(1 + pow(%f*cos(x)*%f*%f/3.0,2))/(1 + pow(%f*%f*%f/3.0,2) )",v,t[i],C,v,t[i],C,v,t[i]),rmin,rmax);
        for (Int_t ipt = 0; ipt < 1000; ipt++) {
            r[ipt] = ipt*(rmax-rmin)/1000+rmin;
            theta[ipt] = fdist->Eval(r[ipt]);
        }
        gdist[i] = new TGraphPolar(1000,r,theta);
        gdist[i]->SetTitle("");
        gdist[i]->SetLineColor(8);
        gdist[i]->SetLineWidth(2);
        //gdist[i]->SetLineStyle(9);
        gdist[i]->Draw("same");
    }

   
    
    //Now, since root sucks even more, have to draw functions that are
    //out of range in order to make a legend on a polar graph.
    
    TF1 *t1 = new TF1("t1","x",100,200);
    t1->SetLineWidth(2);
    t1->SetLineColor(2);
    t1->Draw("same");
    TF1 *t2 = new TF1("t2","x",100,200);
    t2->SetLineWidth(2);
    t2->SetLineColor(8);
    t2->Draw("same");
    
    leg = new TLegend(0.03,0.87,0.3,.97);
    leg->AddEntry("t1","No Lorentz Effect","l");
    leg->AddEntry("t2","Third Approximation","l");
    leg->Draw();
    

}



