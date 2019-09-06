/*
 June 2017
 Jason Bono
 
 Notes:
 
The functions t2NominalSpeed and d2NominalSpeed give you the desired distance/time to speed transformations by calling the lower level functions LookupTable and InterpolateVoltage. Call d2NominalSpeed in mm, and it will return in mm/ns; Call t2NominalSpeed in ns, and it will return in mm/ns. Voltage is always called in SI units.
 
 If you would rather get the distance to time or time to distance relationships, you may use d2tLorentz or t2dLorentz, respectivly, calling with phi=bfield=0 to remove the lorentz effect. The other parameters are called in the same way as t2NominalSpeed and d2NominalSpeed.
 

 */



#include <math.h>
#include <iostream>
#include <ctime>
using namespace std;




//function prototypes
double d2tLorentz(double distance, double phi, double voltage, double bfield);
double t2dLorentz(double time, double phi, double voltage, double bfield);
double GammaFactor(double distance, double phi, double voltage, double bfield);
double t2NominalSpeed(double time, double voltage);
double d2NominalSpeed(double distance, double voltage);
double InterpolateVoltage(double distance, double voltage);
double LookupTable(double distance, double voltage);





//for timing
int timecode(){

    double distance = 1.0777;
    double phi = 3.14/2;
//    phi = 0;
    double voltage = 1400;
    double bfield = 1;
//    double time = 45.1;
    
    clock_t beginA = clock();
    double answerA = d2tLorentz(distance, phi, voltage, bfield);
    clock_t endA = clock();
    double elapsed_secsA = double(endA - beginA) / CLOCKS_PER_SEC;
    //cout <<answerA<<endl;
    cout << "d2t takes " << double(endA - beginA) << " cpu ticks and " << elapsed_secsA <<"seconds"<<endl;
    
    clock_t beginB = clock();
    double answerB = t2dLorentz(answerA, phi, voltage, bfield);
    clock_t endB = clock();
    double elapsed_secsB = double(endB - beginB) / CLOCKS_PER_SEC;
    //cout <<answerB<<endl;
    cout << "t2d takes " << double(endB - beginB) << " cpu ticks and " << elapsed_secsB <<"seconds"<<endl;
    
    cout << "end precision = " << distance - answerB <<endl;
    
    return 0;
    
    
}




//Takes in distance and returns corrected time. call in mm, returns in ns
double d2tLorentz(double distance, double phi, double voltage, double bfield){

    
    //get the nominal drift speed in m/s
    double nomS
    = d2NominalSpeed(distance,voltage);
//    cout << " Nominal Drift Speed = " << nomS <<endl;
    
    //calculate the nominal time in seconds
    double nomT = distance/nomS;
//    cout << " Nominal Time = " << nomT <<endl;
    
    //get the gamma factor
    double Gamma = GammaFactor(distance, phi, voltage, bfield);
//    cout << " Gamma Factor = " << Gamma <<endl;
    
    //calculate the lorentz corrected time in ns
    double lorT = nomT*Gamma;
//    cout << " Lorentz Corrected Time = " << lorT <<endl;
    
    return lorT; //in ns
}




//Takes in time and returns corrected distance. call in ns, returns in mm
double t2dLorentz(double time, double phi, double voltage, double bfield){
    
    
    //get the "default" nominal drift speed in mm/ns
    double nomS = t2NominalSpeed(time,voltage);
//    cout << " Nominal Drift Speed = " << nomS <<endl;
    
    
    //calculate the nominal distance in mm
    double nomD = time*nomS;
//    cout << " Nominal Distance = " << nomD <<endl;
    
    //get the gamma factor
    double Gamma = GammaFactor(nomD, phi, voltage, bfield);
//    cout << " Gamma Factor = " << Gamma <<endl;
    
    //calculate the lorentz corrected distance in mm
    double lorD = nomD/Gamma;
//    cout << " Lorentz Corrected Distance = " << lorD <<endl;
    
    //declare a fake time for iterations (in s)
    double faketime;
    
    
    //takes about 4 iterations to converge sufficiently
    for (int i=0; i<3; i++) {
//        cout <<"---------"<<endl;
        
        //calculate fake time in ns
        faketime = time/Gamma;
        //get the nominal drift speed in mm/ns
        nomS = t2NominalSpeed(faketime,voltage);
//        cout << " Nominal Drift Speed 2= " << nomS <<endl;
        
        
        //recalculate the nominal distance in mm
        nomD = time*nomS;

        
        //get the gamma factor
        Gamma = GammaFactor(lorD, phi, voltage, bfield);
//        cout << " Gamma Factor 2= " << Gamma <<endl;
        
        //calculate the lorentz corrected distance in mm
        lorD = nomD/Gamma;
//        cout << " Lorentz Corrected Distance 2= " << lorD <<endl;
    }
    
    return lorD; // in mm
}









double GammaFactor(double distance, double phi, double voltage, double bfield){
    
    //get the nominal drift speed
    double nomS = d2NominalSpeed(distance,voltage);
    //need SI units to calculate mu and zeta properly, so convert
    // mm/ns to m/s
    nomS = nomS*1000000.;
//    cout << "nomS = " <<nomS<<endl;
    //mm to m
    distance = distance/1000.;
//    cout << "distance = " <<distance<<endl;
    
    //define the wire and straw radius in mm
    double a = 12.5/1000.; //12.5 um in mm
    double b = 2.5; //2.5 mm in mm
    
    //calculate the E field
    double E = voltage/(distance*log(b/a));
//    cout << "E = " <<E<<endl;
    
    //calculate the gas mobility (using SI units!)
    double mu = nomS/E;
//    cout << "mu = " <<mu<<endl;
    
    //calculate dimensionless "zeta" as defined in the lorentz document
    //calculated from all SI units!!!
    double zeta = mu*bfield;
//    cout << "calculated zeta = " <<zeta<<endl;
    
    double Gamma = (1 + pow(zeta,2)/3)/(1 + pow(zeta*cos(phi),2)/3);
    
    return Gamma; //dimensionless SI!!!
}




//Takes in time and gives nominal speed in mm/ns. Call in ns and V
double t2NominalSpeed(double time, double voltage){
    double speed = 0;
    double error = 10; //initialize error in mm, set to somthing big
    
    double precision = 0.01; //desired precision in mm, the more precision, the more opererations carried out.
    double distance = precision/2.; //first guess of distance in tenths of a mm
    int count = 0;
    //seek out the correct distance/time
    while (fabs(error) > precision){
        
        //cout <<"--------"<<endl;
        
        //increment the distance in tenths of mm
        distance = distance + precision/2.;
        
        //get the speed in mm/ns
        speed = d2NominalSpeed(distance, voltage);
        
        //compute the error in mm
        error = time*speed - distance;
        //cout <<"time = "<<time<<", distance = "<<distance<<", speed = "<<speed<<endl;
        //cout << "computed distance = " <<time*speed<<", error = "<<error<<", precision = "<<precision << endl;
        //gSystem->Sleep(250);
        //cout <<count<<endl;
        count++;
    }
    

    //cout <<"+++++++++++++finally:++++++++++++++++"<<endl;
    //cout <<"time = "<<time<<", distance = "<<distance<<", speed = "<<speed<<endl;
    //cout << "computed distance = " <<time*speed<<", error = "<<error<<", precision = "<<precision << endl;
    //cout << "abs(error) - precision = " <<abs(error) - precision<<endl;
    
    
    return speed;
}



//take distance and give nominal speed. Call in mm, returns in mm/ns
double d2NominalSpeed(double distance, double voltage){
    
    
    // convert from mm to tenths of mm for rounding to the nearest 0.1 mm
    distance = distance*10.;
    
    
    //round up
    double top = ceil(distance);
    //round down
    double bottom = floor(distance);
    //get the weighting factor for top and bottom
    double wbottom = ceil(distance) - distance;
    double wtop = 1 - wbottom;
    
    
    //convert all three distances back to mm
    top = top*0.1;
    bottom = bottom*0.1;
    distance = distance*0.1;
    
    
    //check that everything looks ok
//    cout <<"top = " << top << ", wtop = "<<wtop<<endl;
//    cout <<"bottom = " << bottom << ", wbottom = "<<wbottom<<endl;
//     cout << "the next two numbers should be equal: " << distance<<" & " <<wtop*double(top) + wbottom*double(bottom)<<endl;
    
    //get speeds for top and bottom
    //double stop = LookupTable(top, voltage);
    double stop = InterpolateVoltage(top, voltage);
    //double sbottom = LookupTable(bottom, voltage);
    double sbottom = InterpolateVoltage(bottom, voltage);
    
    //take the weighted average of the speeds
    double speed = stop*wtop + sbottom*wbottom;
    //check that everything looks right
    //cout <<"sbottom = " << sbottom << ", stop = "<<stop<< ", speed = "<<speed <<endl;
    

    
    return speed;
    
}






//takes in distance and voltage ->
//-> then interpolates based on voltage, and gives speed in mm/ns.
//call in mm and in V
double InterpolateVoltage(double distance, double voltage){
    
    // convert from V to hundreds of V
    double fvoltage = voltage/100.;
    //round up
    int top = ceil(fvoltage);
    //round down
    int bottom = floor(fvoltage);
    //get the weighting factor for top and bottom
    double wbottom = top - fvoltage;
    double wtop = 1 - wbottom;
    
    //convert top and bottom into units of V
    top = top*100.;
    bottom = bottom*100.;
    
    
    //get speeds for top and bottom
    double stop = LookupTable(distance, top);
    double sbottom = LookupTable(distance, bottom);
    //take the weighted average of the speeds
    double speed = stop*wtop + sbottom*wbottom;
    //check that everything looks right
    //cout <<"sbottom = " << sbottom << ", stop = "<<stop<< ", speed = "<<speed <<endl;
    
    
    return speed; //in mm/ns
    
}









//called in mm, returns in mm/ns
double LookupTable(double distance, double voltage){
    
    //convert from tens of mm to tens of mm
    distance = distance*10.;
    
    //this LUT is is informed from the numerical integration routeine in ...MockCode/NomDrift2.C
    //right now, only using results from 1500 V, but can interpolate later
    //case0 corresponds to sitting on the wire; case1, to 0.1mm; case2, to 0.2mm; etc.
    double speed = 0.; //speed in um/ns
    int tabledist = int(distance); //this rounds down, but is accounted for (see )
    
    
    
    
    if (int(voltage) == 1700) {
        switch (tabledist) {
            case 0: speed = 68.5; break;
            case 1: speed = 68.5; break;
            case 2: speed = 68.5; break;
            case 3: speed = 68.5; break;
            case 4: speed = 68.5; break;
            case 5: speed = 68.5002; break;
            case 6: speed = 68.5383; break;
            case 7: speed = 68.4251; break;
            case 8: speed = 68.124; break;
            case 9: speed = 67.6726; break;
            case 10: speed = 67.1548; break;
            case 11: speed = 66.6157; break;
            case 12: speed = 66.1096; break;
            case 13: speed = 65.6479; break;
            case 14: speed = 65.2306; break;
            case 15: speed = 64.8691; break;
            case 16: speed = 64.5417; break;
            case 17: speed = 64.2362; break;
            case 18: speed = 63.9461; break;
            case 19: speed = 63.6602; break;
            case 20: speed = 63.3653; break;
            case 21: speed = 63.0597; break;
            case 22: speed = 62.7381; break;
            case 23: speed = 62.3913; break;
            case 24: speed = 62.0004; break;
            case 25: speed = 61.5687; break;
            default: speed = 61.5687; //anything beyond 2.5 mm keep constant
        }
    }
    else if (int(voltage) == 1600) {
        switch (tabledist) {
            case 0: speed = 68.5; break;
            case 1: speed = 68.5; break;
            case 2: speed = 68.5; break;
            case 3: speed = 68.5; break;
            case 4: speed = 68.5; break;
            case 5: speed = 68.5104; break;
            case 6: speed = 68.5262; break;
            case 7: speed = 68.3161; break;
            case 8: speed = 67.9082; break;
            case 9: speed = 67.388; break;
            case 10: speed = 66.8145; break;
            case 11: speed = 66.2642; break;
            case 12: speed = 65.7595; break;
            case 13: speed = 65.3045; break;
            case 14: speed = 64.9115; break;
            case 15: speed = 64.5613; break;
            case 16: speed = 64.2362; break;
            case 17: speed = 63.9288; break;
            case 18: speed = 63.6236; break;
            case 19: speed = 63.3089; break;
            case 20: speed = 62.9812; break;
            case 21: speed = 62.6326; break;
            case 22: speed = 62.2496; break;
            case 23: speed = 61.8155; break;
            case 24: speed = 61.3396; break;
            case 25: speed = 60.8213; break;
            default: speed = 60.8213; //anything beyond 2.5 mm keep constant
        }
    }
    else if (int(voltage) == 1500) {
        switch (tabledist) {
            case 0: speed = 68.5; break;
            case 1: speed = 68.5; break;
            case 2: speed = 68.5; break;
            case 3: speed = 68.5; break;
            case 4: speed = 68.5; break;
            case 5: speed = 68.531; break;
            case 6: speed = 68.4668; break;
            case 7: speed = 68.15; break;
            case 8: speed = 67.6398; break;
            case 9: speed = 67.0449; break;
            case 10: speed = 66.4434; break;
            case 11: speed = 65.8881; break;
            case 12: speed = 65.3908; break;
            case 13: speed = 64.9605; break;
            case 14: speed = 64.5837; break;
            case 15: speed = 64.2362; break;
            case 16: speed = 63.9087; break;
            case 17: speed = 63.5816; break;
            case 18: speed = 63.2442; break;
            case 19: speed = 62.8913; break;
            case 20: speed = 62.5113; break;
            case 21: speed = 62.082; break;
            case 22: speed = 61.5981; break;
            case 23: speed = 61.068; break;
            case 24: speed = 60.4936; break;
            case 25: speed = 59.8665; break;
            default: speed = 59.8665; //anything beyond 2.5 mm, keep constant
        }
    }
    else if (int(voltage) == 1400) {
        switch (tabledist) {
            case 0: speed = 68.5; break;
            case 1: speed = 68.5; break;
            case 2: speed = 68.5; break;
            case 3: speed = 68.5; break;
            case 4: speed = 68.5; break;
            case 5: speed = 68.5378; break;
            case 6: speed = 68.3566; break;
            case 7: speed = 67.9082; break;
            case 8: speed = 67.3085; break;
            case 9: speed = 66.6536; break;
            case 10: speed = 66.0407; break;
            case 11: speed = 65.4942; break;
            case 12: speed = 65.0178; break;
            case 13: speed = 64.6092; break;
            case 14: speed = 64.2362; break;
            case 15: speed = 63.8858; break;
            case 16: speed = 63.5342; break;
            case 17: speed = 63.1707; break;
            case 18: speed = 62.7853; break;
            case 19: speed = 62.3648; break;
            case 20: speed = 61.8805; break;
            case 21: speed = 61.3396; break;
            case 22: speed = 60.7459; break;
            case 23: speed = 60.0981; break;
            case 24: speed = 59.3834; break;
            case 25: speed = 58.6153; break;
            default: speed = 58.6153; //anything beyond 2.5 mm keep constant
        }
    }
    else if (int(voltage) == 1300) {
        switch (tabledist) {
            case 0: speed = 68.5; break;
            case 1: speed = 68.5; break;
            case 2: speed = 68.5; break;
            case 3: speed = 68.5; break;
            case 4: speed = 68.5063; break;
            case 5: speed = 68.5095; break;
            case 6: speed = 68.1828; break;
            case 7: speed = 67.5962; break;
            case 8: speed = 66.9031; break;
            case 9: speed = 66.2236; break;
            case 10: speed = 65.6141; break;
            case 11: speed = 65.0855; break;
            case 12: speed = 64.6395; break;
            case 13: speed = 64.2362; break;
            case 14: speed = 63.859; break;
            case 15: speed = 63.4786; break;
            case 16: speed = 63.0837; break;
            case 17: speed = 62.6608; break;
            case 18: speed = 62.1867; break;
            case 19: speed = 61.6368; break;
            case 20: speed = 61.025; break;
            case 21: speed = 60.3555; break;
            case 22: speed = 59.6089; break;
            case 23: speed = 58.7943; break;
            case 24: speed = 57.9286; break;
            case 25: speed = 57.0084; break;
            default: speed = 57.0084; //anything beyond 2.5 keep constant
        }
    }
    else {
        cout <<voltage <<" IS A BAD VOLTAGE"<<endl;
        exit(EXIT_FAILURE);
    }
    
    //speed = 60; //set a constant speed for debugging
    
    speed = speed*0.001; //convert from um/ns to mm/ns
    return speed; // in mm/ns
}
