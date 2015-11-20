
//Angle analysis program for MCS
//preliminary script for analyzing data

//This is a test
#include <iostream>
#include <fstream>
#include <cassert>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <stdio.h>
#include <string>
#include <sstream>

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
	int count;
	int totCount;
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
	
	TF1 *gausFit = new TF1("gausFit", gausFunc, -40, 40);

	//Make and fill the canvas
/*	TCanvas *proj = new TCanvas("proj","Projections of x_int and y_int",0,0,800,800);

	TPad *xPad = new TPad("xPad", "Vertical slice of canvas", 0,0, 0.0, 0.5, 1.0, 21);
	TPad *yPad = new TPad("yPad", "Vertical slice of canvas", 0.5, 0.0, 1.0, 1.0, 21);

	xPad->cd();
//	x_intProj->SetStats(false);
	x_intProj->GetXaxis()->SetTitle("X intersection position (px)");
	x_intProj->GetXaxis()->CenterTitle();
	x_intProj->GetYaxis()->SetTitle("Number of frames");
	x_intProj->GetYaxis()->CenterTitle();
	x_intProj->Draw();

	yPad->cd();
//	y_intProj->SetStats(false);
	y_intProj->GetXaxis()->SetTitle("Y intersection position (px)");
	y_intProj->GetXaxis()->CenterTitle();
	y_intProj->GetYaxis()->SetTitle("Number of frames");
	y_intProj->GetYaxis()->CenterTitle();
	y_intProj->Draw();
*/	
	//Write the histograms to root file
	x_intProj->Write();
	y_intProj->Write();
	angle_histo->Write();
	
	//Close the files
	inFile->Close();
	outFile->Close();	
}	
