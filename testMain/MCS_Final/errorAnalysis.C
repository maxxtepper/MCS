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

void errorAngle_Mall(char* arg){

	//Take the arguments and save them into respective strings
	std::string infileName, outfileName;
	std::string inF, outF;
	std::string inPrefix, outPrefix;
	std::string layers;
	std::istringstream strm(arg);

	inPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/errorRoot/";
	outPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/analysisRoot/";

	TCanvas *aeAll = new TCanvas("aeAll", "Angular Distribution with Respect to Scattering Medium", 0, 0, 800, 800);
	aeAll->Divide(2,2);

	while (true) {
		if (std::getline(strm, layers, ' ')) {
			inF = "error_" + layers + "layers.root";
			outF = "errorAnalysis_" + layers + "layers.root";

			infileName = inPrefix + inF;
			outfileName = outPrefix + outF;

			// input file
			TFile *inFile = new TFile(infileName.c_str());
			TFile *outFile = new TFile(outfileName.c_str(),"RECREATE");

			//probably don't need info from all of these trees but they're here in case we need them
//			TTree *aeT = (TTree*)inFile->Get("aeTree");
			TTree *aeCutsT = (TTree*)inFile->Get("aeTree");

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
//			aeT->SetBranchAddress("x_int", &x_int);
//			aeT->SetBranchAddress("y_int", &y_int);
//			aeT->SetBranchAddress("ang", &ang);	

			aeCutsT->SetBranchAddress("x_int", &xCuts_int);
			aeCutsT->SetBranchAddress("y_int", &yCuts_int);
			aeCutsT->SetBranchAddress("ang", &angCuts);	

			//get ang_min and ang_max
//			Int_t nentries = aeT->GetEntries();
//			for (Int_t i=0; i<nentries; i++) {
//				aeT->GetEntry(i);
//				if(ang[2] > ang_max)
//					ang_max = ang[2];
//				else if(ang[2] < ang_min)
//					ang_min = ang[2];
//			}

			Int_t nCutsEntries = aeCutsT->GetEntries();
			for (Int_t i=0; i<nCutsEntries; i++) {
				aeCutsT->GetEntry(i);
				if(angCuts[2] > angCuts_max)
					angCuts_max = angCuts[2];
				else if(angCuts[2] < angCuts_min)
					angCuts_min = angCuts[2];
			}

			int binning[5] = {30, 40, 30, 28, 37};
			int LAY = atoi(layers.c_str());

			//making the intersection histos
//			TH1D *x_intProj = new TH1D("x_intProj", "Projection of x intersection", width/4, 0, width);
//			TH1D *y_intProj = new TH1D("y_intProj", "Projection of y intersection", height/4, 0, height);
			
			TH1D *x_intCutsProj = new TH1D("x_intCutsProj", "Projection of x intersection", width/4, 0, width);
			TH1D *y_intCutsProj = new TH1D("y_intCutsProj", "Projection of y intersection", height/4, 0, height);
			
			//making the angle histo
//			TH1D *angle_histo = new TH1D("angle_histo", "Distribution of scattering angles", 80, -40, 40);
			
			TH1D *aeCuts_histo = new TH1D("aeCuts_histo", "Distribution of scattering angles", binning[LAY], -40, 40);
			
			//Fill the histograms with data from aeT
/*			cout << "The number of entries to be checked  = "<< nentries << endl;
			for (Int_t i=0; i<nentries; i++) {
				aeT->GetEntry(i);
				x_intProj->Fill(x_int);
				y_intProj->Fill(y_int);
				angle_histo->Fill(ang[2]);
			}

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
*/			
			//Fill the histograms with data from aeCutsT
			std::cout << "The number of entries to be checked  = "<< nCutsEntries << std::endl;
			for (Int_t i=0; i<nCutsEntries; i++) {
				aeCutsT->GetEntry(i);
				x_intCutsProj->Fill(xCuts_int);
				y_intCutsProj->Fill(yCuts_int);
				aeCuts_histo->Fill(angCuts[2]);
			}

			TF1 *normalGaus = new TF1("normalGaus", "gaus", -40, 40);

			Double_t magG = aeCuts_histo->GetMaximum();
			Double_t meanG = aeCuts_histo->GetMean();
			Double_t RMSG = aeCuts_histo->GetRMS();
			
			normalGaus->SetParameters(magG, meanG, RMSG);
			normalGaus->SetParNames("magG", "meanG", "RMSG");
			aeCuts_histo->Fit("normalGaus");

			//Make the function to be fit
			TF1 *funcCuts = new TF1("gausFuncCuts", gausFunc, -40, 40, 5);

			//Get and set parameters for fitting
			Double_t magCuts[2] = {0};
			Double_t meanCuts = 0;
			Double_t RMSCuts[2] = {0};
			Double_t scale[2][3] = {{1, 1, 1}, {0.5, 1, 4}};
			for (int i=0; i<2; i++) {
				magCuts[i] = normalGaus->GetParameter("magG")*scale[i][0];
				RMSCuts[i] = normalGaus->GetParameter("RMSG")*scale[i][2];
			}

			funcCuts->SetParameters(magCuts[0], meanCuts, RMSCuts[0], magCuts[1], RMSCuts[1]);	
//			funcCuts->SetParLimits(2, 0, 8);
//			funcCuts->SetParLimits(4, 0, 20);
			funcCuts->SetParNames("Constant1", "Mean_value1", "Sigma1", "Constant2", "Sigma2");
			
			//Take the histo and fit it
			aeCuts_histo->Fit("gausFuncCuts");

			std::cout << "Sigma 1 after = " << funcCuts->GetParameter("Sigma1") << std::endl;
			std::cout << "Sigma 2 after = " << funcCuts->GetParameter("Sigma2") << std::endl;

			if (atoi(layers.c_str()) != 0 ) {
				aeAll->cd(atoi(layers.c_str()));
				aeCuts_histo->Draw();
				aeCuts_histo->GetXaxis()->SetTitle("Angle (Degrees)");
				aeCuts_histo->GetXaxis()->CenterTitle();
				aeCuts_histo->GetYaxis()->SetTitle("Number of Events");
				aeCuts_histo->GetYaxis()->CenterTitle();
				gPad->Update();
			} else {
				aeCuts_histo->Draw();
				aeCuts_histo->GetXaxis()->SetTitle("Angle (Degrees)");
				aeCuts_histo->GetXaxis()->CenterTitle();
				aeCuts_histo->GetYaxis()->SetTitle("Number of Events");
				aeCuts_histo->GetYaxis()->CenterTitle();
			}	

			//Write the histograms to root file
			x_intCutsProj->Write();
			y_intCutsProj->Write();
			aeCuts_histo->Write();
			funcCuts->Write();

			//Close the files
			inFile->Close();
			outFile->Close();	
		} else break; 
	}
	aeAll->Draw();
	gPad->Update();
	std::cout << "Complete!\n";
}

Double_t gausFunc(Double_t *x, Double_t *par) {
	Double_t arg[2] = {0};
	if (par[2]!=0 && par[4]!=0) {
		arg[0] = ((x[0]-par[1])/par[2]);
		arg[1] = ((x[0]-par[1])/par[4]);
	}
	Double_t fitVal = par[0]*TMath::Exp(-0.5*arg[0]*arg[0]) + par[3]*TMath::Exp(-0.5*arg[1]*arg[1]);
	return fitVal;
}	
