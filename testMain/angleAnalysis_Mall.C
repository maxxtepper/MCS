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

void angleAnalysis_Mall(char* arg){

	//Take the arguments and save them into respective strings
	std::string infileName, outfileName;
	std::string inF, outF;
	std::string inPrefix, outPrefix;
	std::string layers;
	std::istringstream strm(arg);

	inPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/angleRoot/";
	outPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/analysisRoot/";
	
	while (true) {
		if (std::getline(strm, layers, ' ')) {
			
			inF = "angle_" + layers + "layers.root";
			outF = "analysis_" + layers + "layers.root";

			infileName = inPrefix + inF;
			outfileName = outPrefix + outF;

			// input file
			TFile *inFile = new TFile(infileName.c_str());
			TFile *outFile = new TFile(outfileName.c_str(),"RECREATE");

			//probably don't need info from all of these trees but they're here in case we need them
			TTree *angleT = (TTree*)inFile->Get("angleTree");
			TTree *angleCutsT = (TTree*)inFile->Get("angleCutsTree");
			
			//Define constants	
			const int width = 480;
			const int height = 640;

			//Define branch references
			double x_int   = 0;
			double y_int   = 0;
			double ang[3]  = {0};
			double ang_min = 0;
			double ang_max = 0;	
			double xCuts_int   = 0;
			double yCuts_int   = 0;
			double angCuts[3]  = {0};
			double angCuts_min = 0;
			double angCuts_max = 0;	

			//get branch info from these guyz
			angleT->SetBranchAddress("x_int", &x_int);
			angleT->SetBranchAddress("y_int", &y_int);
			angleT->SetBranchAddress("ang", &ang);	

			angleCutsT->SetBranchAddress("x_int", &xCuts_int);
			angleCutsT->SetBranchAddress("y_int", &yCuts_int);
			angleCutsT->SetBranchAddress("ang", &angCuts);	

			//get ang_min and ang_max
			Int_t nentries = angleT->GetEntries();
			for (Int_t i=0; i<nentries; i++) {
				angleT->GetEntry(i);
				if(ang[2] > ang_max)
					ang_max = ang[2];
				else if(ang[2] < ang_min)
					ang_min = ang[2];
			}

			Int_t nCutsEntries = angleCutsT->GetEntries();
			for (Int_t i=0; i<nCutsEntries; i++) {
				angleCutsT->GetEntry(i);
				if(angCuts[2] > angCuts_max)
					angCuts_max = angCuts[2];
				else if(angCuts[2] < angCuts_min)
					angCuts_min = angCuts[2];
			}

			//making the intersection histos
			TH1D *x_intProj = new TH1D("x_intProj", "Projection of x intersection", width/4, 0, width);
			TH1D *y_intProj = new TH1D("y_intProj", "Projection of y intersection", height/4, 0, height);
			
			TH1D *x_intCutsProj = new TH1D("x_intCutsProj", "Projection of x intersection", width/4, 0, width);
			TH1D *y_intCutsProj = new TH1D("y_intCutsProj", "Projection of y intersection", height/4, 0, height);
			
			//making the angle histo
			TH1D *angle_histo = new TH1D("angle_histo", "Distribution of scattering angles", 80, -40, 40);
			
			TH1D *angleCuts_histo = new TH1D("angleCuts_histo", "Distribution of scattering angles", 80, -40, 40);
			
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
			std::cout << "The number of events in the interval = " << count << std::endl;
			std::cout << "The number of events in total        = " << totCount << std::endl;

			//Make the function to be fit
			TF1 *func = new TF1("gausFunc", gausFunc, -40, 40, 6);

			//Get and set parameters for fitting
			Double_t mag[2] = {0};
			Double_t mean[2] = {0};
			Double_t RMS[2] = {0};
			Double_t scale[2][3] = {{0.2, 1, 2.0}, {0.8, 2.0, 2.0}};

			for (int i=0; i<2; i++) {
				mag[i] = angle_histo->GetMaximum()*scale[i][0];
				mean[i] = angle_histo->GetMean()*scale[i][1];
				RMS[i] = angle_histo->GetRMS()*scale[i][2];
			}

			std::cout << "Sigma 1 before = " << RMS[0] << std::endl;
			std::cout << "Sigma 2 before = " << RMS[1] << std::endl;			

			func->SetParameters(mag[0], mean[0], RMS[0], mag[1], mean[1], RMS[1]);	
			func->SetParLimits(2, -5, 5);
			func->SetParLimits(5, -20, 20);
			func->SetParNames("Constant1", "Mean_value1", "Sigma1", "Constant2", "Mean_value2", "Sigma2");
			
			//Take the histo and fit it
			angle_histo->Fit("gausFunc");

			std::cout << "Sigma 1 after = " << func->GetParameter("Sigma1") << std::endl;
			std::cout << "Sigma 2 after = " << func->GetParameter("Sigma2") << std::endl;
			
			//Write the histograms to root file
			x_intProj->Write();
			y_intProj->Write();
			angle_histo->Write();
			func->Write();
			
			//Fill the histograms with data from angleCutsT
			count = 0;
			totCount = 0;
			std::cout << "The number of entries to be checked  = "<< nentries << std::endl;
			for (Int_t i=0; i<nCutsEntries; i++) {
				angleCutsT->GetEntry(i);
				x_intCutsProj->Fill(xCuts_int);
				y_intCutsProj->Fill(yCuts_int);
				angleCuts_histo->Fill(angCuts[2]);
				if (yCuts_int<347 && yCuts_int>333) {
					count++;
				}
				totCount++;
			}
			cout << "The number of events in the interval = " << count << endl;
			cout << "The number of events in total        = " << totCount << endl;

			//Make the function to be fit
			TF1 *funcCuts = new TF1("gausFuncCuts", gausFunc, -40, 40, 6);

			//Get and set parameters for fitting
			Double_t magCuts[2] = {0};
			Double_t meanCuts[2] = {0};
			Double_t RMSCuts[2] = {0};
			Double_t scaleCuts[2][3] = {{0.2, 0.2, 0.2}, {0.8, 2.0, 2.0}};

			for (int i=0; i<2; i++) {
				magCuts[i] = angleCuts_histo->GetMaximum()*scaleCuts[i][0];
				meanCuts[i] = angleCuts_histo->GetMean()*scaleCuts[i][1];
				RMSCuts[i] = angleCuts_histo->GetRMS()*scaleCuts[i][2];
			}

			std::cout << "Sigma 1 before = " << RMSCuts[0] << std::endl;
			std::cout << "Sigma 2 before = " << RMSCuts[1] << std::endl;
	
			funcCuts->SetParameters(magCuts[0], meanCuts[0], RMSCuts[0], magCuts[1], meanCuts[1], RMSCuts[1]);	
			funcCuts->SetParLimits(2, -5, 5);
			funcCuts->SetParLimits(5, -20, 20);
			funcCuts->SetParNames("Constant1", "Mean_value1", "Sigma1", "Constant2", "Mean_value2", "Sigma2");
			
			//Take the histo and fit it
			angleCuts_histo->Fit("gausFuncCuts");

			std::cout << "Sigma 1 after = " << funcCuts->GetParameter("Sigma1") << std::endl;
			std::cout << "Sigma 2 after = " << funcCuts->GetParameter("Sigma2") << std::endl;
			
			//Write the histograms to root file
			x_intCutsProj->Write();
			y_intCutsProj->Write();
			angleCuts_histo->Write();
			funcCuts->Write();
			
			//Close the files
			inFile->Close();
			outFile->Close();	
		} else break;
	}
	std::cout << "Complete!\n";
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
