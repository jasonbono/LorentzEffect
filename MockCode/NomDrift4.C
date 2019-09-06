//Jason Bono


void NomDrift4(){

    //gStyle->SetTitleXOffset(1);
    gStyle->SetTitleYOffset(1.3);
    //gStyle->SetTitleSize(0.09);
    gStyle->SetTitleXSize(0.045);
    
    
    const int Nd = 25;
    double d[Nd]={};
    for (int i=0; i<Nd; i++) {
        d[i] = 0.01 + 0.01*i;
    }
    
    const int Nv = 5;
    double Voltage[Nv]={};
    for (int j=0; j<Nv; j++) {
        Voltage[j] = 1500 + 325*j;
    }
    
    double test[Nd];
    double speed[Nv][Nd]; // the order is switched for plotting
    for (int i=0; i<Nd; i++) {
            for (int j=0; j<Nv; j++) {
                //cout << d[i]<<", " << Voltage[j] <<endl;
                speed[j][i] = getspeed(d[i],Voltage[j]);
                //test[i] = getspeed(d[i],1000);
            }
    }
    
//    case 25: speed = 59.9663; break;
    //print out a form to copy and paste into the reconstruction code
    for (int j=0; j<Nv; j++) {
        cout <<", V = " << Voltage[j] <<endl;
        for (int i=0; i<Nd; i++) {
            cout<<"case "<<d[i]*100<<": speed = " << speed[j][i]*10<<"; break;" <<endl;
        }
    }
    
    int iV = 0;
    TGraph* gr[Nv];
    TMultiGraph *mg = new TMultiGraph();
    for (int j=0; j<Nv; j++){
        iV = Voltage[j];
        
        gr[j] = new TGraph(Nd,d,speed[j]);
        gr[j]->SetTitle(Form("%i V",iV));
        gr[j]->SetMarkerStyle(20 + j);
        gr[j]->SetMarkerColor(j+1);
        gr[j]->SetDrawOption("P");
        gr[j]->SetLineColor(j + 1);
        gr[j]->SetLineWidth(2);
        gr[j]->SetFillStyle(0);
        
        
        mg->Add(gr[j]);
        
    }

    //TGraph* g = new TGraph(Nd,d,speed[0]);
    //g->Draw("ALP");
    //g->SetTitle("graph 3");
    
    
    TCanvas *c3 = new TCanvas("c3","c3",600, 400);
    mg->Draw("ALP");
    mg->GetXaxis()->SetTitle("d (cm)");
    mg->GetXaxis()->CenterTitle();mg->GetYaxis()->CenterTitle();
    mg->GetYaxis()->SetTitle("Effective Nominal Drift Velocity (cm/#mu s)");
    mg->SetTitle("Effective Nominal Drift Velocity");
    c3->BuildLegend(.75,.9,.9,.55);
    c3->SetGrid();
    
    for (int j=0; j<Nv; j++) {
        for (int i=0; i<Nd; i++) {
            //cout << Voltage[j] << ", " << d[i]<< ", " << speed[j][i] <<endl;
        }
    }

    for (int j=0; j<Nv; j++) {
        for (int i=0; i<Nd; i++) {
            //cout << speed[j][i]<< ", ";
        }
    }
    

}


void InstantaneousSpeed(){
    
    //gStyle->SetTitleXOffset(1);
    gStyle->SetTitleYOffset(1.3);
    //gStyle->SetTitleSize(0.09);
    gStyle->SetTitleXSize(0.045);
    
    
    const int Ni = 35;
    double E[Ni] = {0.070,0.140,0.210,0.280,0.350,0.420,0.490,0.559,0.629,0.699,
        0.769,0.839,0.909,0.979,1.049,1.119,1.189,1.259,1.329,1.399,1.573,1.748,1.923,2.098,2.448,2.797,3.147,3.497,3.846,4.196,4.545,4.895,5.245,5.735,500}; //I made up the last data point
    double v[Ni] = {0.21,0.46,0.71,0.97,1.26,1.57,1.90,2.26,2.65,3.05,3.47,
        3.84,4.25,4.58,4.91,5.15,5.36,5.56,5.68,5.77,5.92,5.98,6.02,6.02,6.09,6.19,6.35,6.48,6.64,6.72,6.83,6.87,6.89,6.85,6.85}; //I made up the last data point
    
    //convert field from KV/cm to V/cm
    for (int i=0; i<Ni; i++) {
        E[i] = E[i]*1000;
    }

    
    const int Nv = 7;
    double Voltage[Nv]={};
    for (int j=0; j<Nv; j++) {
        Voltage[j] = 1200 + 50*j;
    }
    
    
    
    
    //define r in cm, convert the units, fill a graph
    TGraph* InstSpeed[Nv];
    for (int j=0; j<Nv; j++) {
        InstSpeed[j] = new TGraph();
    }
    double r[Ni]={};
    double speed[Ni] = {}; //will be the same as v but with a scale factor for plotting
    for (int i=0; i<Ni; i++) {
        for (int j=0; j<Nv; j++) {
            r[i] = 10*Voltage[j]/(E[i]*5.983); //convert r to mm
            speed[i] = v[i]*10;//speed to convert to um/ns
            //cout << "i = " << i << " j = " << j << " Voltage = " << Voltage[j] << " E = "<<E[i] <<" r = "<<r[i]<< " speed = " <<speed[i] <<endl;
            InstSpeed[j]->SetPoint(i,r[i],speed[i]);
        }
    }
    
    
    //InstSpeed[1]->Draw("ap");
    
    
    int iV = 0;
    TMultiGraph *mg = new TMultiGraph();
    for (int j=0; j<Nv; j++){
        iV = Voltage[j];
        

        InstSpeed[j]->SetTitle(Form("%i V",iV));
        InstSpeed[j]->SetMarkerStyle(20 + j);
        InstSpeed[j]->SetMarkerColor(j+1);
        InstSpeed[j]->SetDrawOption("P");
        InstSpeed[j]->SetLineColor(j + 1);
        InstSpeed[j]->SetLineWidth(2);
        InstSpeed[j]->SetFillStyle(0);
        mg->Add(InstSpeed[j]);
        
    }


    TCanvas *c3 = new TCanvas("c3","c3",600, 400);
    mg->Draw("ALP");
    mg->GetXaxis()->SetTitle("Distance from wire (mm)");
    mg->GetXaxis()->CenterTitle();mg->GetYaxis()->CenterTitle();
    mg->GetYaxis()->SetTitle("Instantaneous Drift Speed (#mu m/ns)");
    mg->SetTitle("Instantaneous Drift Speed");
    c3->BuildLegend(.75,.9,.9,.55);
    c3->SetGrid();
    
    
    
    
    
}





double getspeed(double d,double Voltage)
{
    //const int Ni = 11; //the number of prexisting data points.
    //velocity in cm/us
    //double v[Ni] = {4.75,5.9,6.0,6.15,6.25,6.4,6.65,6.67,6.75,6.8,6.75};
    //double E[Ni] = {1000,1500,2000,2500,3000,3500,4000,4500,5000,5500,6000};
    //double v[Ni] = {6,6,6,6,6,6,6,6,6,6,6};
    
    
    //double d = .251;
    
    
    
    const int Ni = 35;
    double E[Ni] = {0.070,0.140,0.210,0.280,0.350,0.420,0.490,0.559,0.629,0.699,
        0.769,0.839,0.909,0.979,1.049,1.119,1.189,1.259,1.329,1.399,1.573,1.748,1.923,2.098,2.448,2.797,3.147,3.497,3.846,4.196,4.545,4.895,5.245,5.735,500}; //I made up the last data point
    double v[Ni] = {0.21,0.46,0.71,0.97,1.26,1.57,1.90,2.26,2.65,3.05,3.47,
        3.84,4.25,4.58,4.91,5.15,5.36,5.56,5.68,5.77,5.92,5.98,6.02,6.02,6.09,6.19,6.35,6.48,6.64,6.72,6.83,6.87,6.89,6.85,6.85}; //I made up the last data point
    
    //convert field from KV/cm to V/cm
    for (int i=0; i<Ni; i++) {
        E[i] = E[i]*1000;
    }
    
    //define r in cm
    //double Voltage = 1400;
    double r[Ni]={};
    for (int i=0; i<Ni; i++) {
        //r[i] = 2.5/(E[i]/1000.0);
        r[i] = Voltage/(E[i]*5.983);
        //r[i] = 1.0/(E[i]/1056.0);
        //cout << "r = "<<r[i] <<endl;
    }
    
    //double r[Ni] = {100,50,25,12.5,6.25,3.125,1.5,0.75,0.375,187};
    
    //make the interpolation points
    const int Nj = 500; // granularity: number of fake points
    double fNj = Nj; //need a double for calculations
    double Ir[Ni - 1][Nj]={}; //dont interpolate the last point
    double Iv[Ni - 1][Nj]={};
    for (int i=0; i<Ni - 1; i++) {
        for (int j=0; j<Nj; j++) {
            Ir[i][j] = ((fNj - j)*r[i] + (1.0 + j)*r[i+1])/(Nj + 1);
            Iv[i][j] = ((fNj - j)*v[i] + (1.0 + j)*v[i+1])/(Nj + 1);
            //cout << "i,j = " <<i<<","<<j<<", Ir = " << Ir[i][j] <<endl;
        }
    }
    
    // combine interpolation and prexisting data
    double R[Ni - 1][Nj + 1]={}; //dont do the last i, and add an extra j per i
    double V[Ni - 1][Nj + 1]={};
    for (int i=0; i<Ni - 1; i++) {
        for (int j=0; j<Nj + 1; j++) {
            if (j==0){
                R[i][j] = r[i];
                V[i][j] = v[i];
            }
            else {
                R[i][j] = Ir[i][j-1];
                V[i][j] = Iv[i][j-1];
            }
            //cout << "i,j = " <<i<<","<<j<<", R = " << R[i][j] <<endl;
        }
    }
    
 //   double aa=1;
    double length[Ni-1][Nj+1]={};
    double totallength = 0;
    double time[Ni-1][Nj+1]={};
    double totaltime = 0;
    //calculate the length of each segment and the time to traverse that length
    for (int i=0; i<Ni - 1; i++) {
        for (int j=0; j<Nj + 1; j++) {
            if (j==Nj) {
                length[i][j] = R[i][j] - R[i+1][0];
            }
            else {
                length[i][j] = R[i][j] - R[i][j+1];
            }
            
            time[i][j] = length[i][j]/V[i][j];
        
            //cout << "i,j = " <<i<<","<<j<<", time = " << time[i][j] <<", length = " << length[i][j] <<endl;
            
            //compute time it takes a particle starting from d, to hit the sense wire
            if (R[i][j] <= d) {
                totaltime += time[i][j];
                totallength += length[i][j]; //just as a check
            }

        }
    }
    
    
    
    //cout << "total time = " << totaltime <<", total length = " << totallength <<", avg speed = " <<totallength/totaltime <<endl;
    
    return totallength/totaltime;
    
    
    
}

