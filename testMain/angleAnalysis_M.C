//Angle analysis program for MCS
//preliminary script for analyzing data

//This is a test
#include <iostream>
#include <fstream>
#include <cassert>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TMath.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TPad.h>
#include <stdio.h>
#include <string>
#include <sstream>

Double_t gausFunc(Double_t*, Double_t*);

void angleAnalysis_M(char* arg){

	//Take the arguments and save them into respective strings
	std::string infileName, outfileName;
	std::string layers;
	std::istringstream strm(arg);
	if (std::getline(strm, layers, ' ')) {};

	infileName = "angle_" + layers + "layers.root";
	outfileName = "analysis_" + layers + "layers.root";

	// input file
	TFile *inFile = new TFile(infileName.c_str());
	TFile *outFile = new TFile(outfileName.c_str(),"RECREATE");

	//probably don't need info from all of these trees but they're here in case we need them
	TTree *angleT = (TTree*)inFile->Get("angleTree");
	
	//Define constants	
	const int width = 480;
	const int height = 640;

	//Define branch references
	double x_int   = 0;
	double y_int   = 0;
	double ang[3]  = {0};
	double ang_min = 0;
	double ang_max = 0;	

	//get branch info from these guyz
	angleT->SetBranchAddress("x_int", &x_int);
	angleT->SetBranchAddress("y_int", &y_int);
	angleT->SetBranchAddress("ang", &ang);	

	//get ang_min and ang_max
	Int_t nentries = angleT->GetEntries();
	for (Int_t i=0; i<nentries; i++) {
		angleT->GetEntry(i);
		if(ang[2] > ang_max)
			ang_max = ang[2];
		else if(ang[2] < ang_min)
			ang_min = ang[2];
	}

	//making the intersection histos
	TH1D *x_intProj = new TH1D("x_intProj", "Projection of x intersection", width/4, 0, width);
	TH1D *y_intProj = new TH1D("y_intProj", "Projection of y intersection", height/4, 0, height);
	
	//making the angle histo
	TH1D *angle_histo = new TH1D("angle_histo", "Distribution of scattering angles", 60, -40, 40);
	
	//Fill the histograms with data from angleT
	int count = 0;
	int totCount = 0;
	cout << "The number of entries to be checked  = "<< nentries << endl;
	for (Int_t i=0; i<nentries; i++) {
		angleT->GetEntry(i);
		x_intProj->Fill(x_int);
		y_intProj->Fill(y_int);
		angle_histo->Fill(ang[2]);
		if (y_int<347 && y_int>333) {
			count++;
		}
		totCount++;
	}
	cout << "The number of events in the interval = " << count << endl;
	cout << "The number of events in total        = " << totCount << endl;

	//Make the function to be fit
	TF1 *func = new TF1("gausFunc", gausFunc, -40, 40, 6);

	//Get and set parameters for fitting
	Double_t mag[2] = {0};
	Double_t mean[2] = {0};
	Double_t RMS[2] = {0};
	Double_t scale[2] = {0.2, 0.8};

	for (int i=0; i<2; i++) {
		mag[i] = 1*scale[i];
		mean[i] = angle_histo->GetMean()*scale[i];
		RMS[i] = angle_histo->GetRMS()*scale[i];
	}

	func->SetParameters(mag[0], mean[0], RMS[0], mag[1], mean[1], RMS[1]);	
	func->SetParNames("Constant1", "Mean_value1", "Sigma1", "Constant2", "Mean_value2", "Sigma2");
	
	//Take the histo and fit it
	angle_histo->Fit("gausFunc");

	//Write the histograms to root file
	x_intProj->Write();
	y_intProj->Write();
	angle_histo->Write();
	
	//Close the files
	inFile->Close();
	outFile->Close();	
}

Double_t gausFunc(Double_t *x, Double_t *par) {
	Double_t arg[2] = {0};
	if (par[2]!=0 && par[5]!=0) {
		arg[0] = ((x[0]-par[1])/par[2]);
		arg[1] = ((x[0]-par[4])/par[5]);
	}
	Double_t fitVal = par[0]*TMath::Exp(-0.5*arg[0]*arg[0]) + par[3]*TMath::Exp(-0.5*arg[1]*arg[1]);
	return fitVal;
}	
