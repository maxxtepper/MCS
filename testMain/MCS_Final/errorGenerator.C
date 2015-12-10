//Error Analysis script

#include <iostream>
#include <fstream>
#include <cassert>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <cmath>
#include <TMath.h>

void errorAnalysis_Mall(char* arg){

	//Take the arguments and save them into respective strings
	std::string infileName, outfileName;
	std::string inF, outF;
	std::string inPrefix, outPrefix;
	std::string layers;
	std::string runCount;
	std::istringstream stm(arg);

	inPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/angleRoot/";
	outPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/errorRoot/";

	//Hard code of the uncertianties in x and y
	double const Pi = 3.14159265359;
	double convert = 180 / Pi;
	
	while (true) {	
		if (std::getline(stm, layers, ' ')) {

			//load the input root files
			TChain *chain = new TChain("angleCutsTree");
			
			inF = "angle_" + layers + "layers.root";
			infileName = inPrefix + inF;
			chain->Add(infileName.c_str());
			cout << "Got " << inF << std::endl;

			std::cout << inF << " attained\n";

			//create the output root file
			outF = "error_" + layers + "layers.root";
			outfileName = outPrefix + outF;
			TFile *outFile = new TFile(outfileName.c_str(), "RECREATE");
			std::cout << outF << " created\n";

			//define frame arrays
			int frame[480][640]={0};
			
			const int width = 480;
			const int height = 640;

			//define picture numbers for each tree
			// (in case they are needed)
			int pNum = 0;

			//define analysis variables
			double ang[3] = {0};
			double x[4] = {0};
			double y[4] = {0};
			double x_int = 0;
			double x_diff= 0;
			double y_int = 0;
			double m0 = 0;
			double m1 = 0;
			double b0 = 0;
			double b1 = 0;
			
			//branch time
			chain->SetBranchAddress("ang",&ang);
			chain->SetBranchAddress("x", &x);
			chain->SetBranchAddress("y", &y);
			chain->SetBranchAddress("x_int", &x_int);
			chain->SetBranchAddress("x_diff", &x_int);
			chain->SetBranchAddress("y_int", &y_int);
			chain->SetBranchAddress("m0", &m0);
			chain->SetBranchAddress("m1", &m1);
			chain->SetBranchAddress("b0", &b0);
			chain->SetBranchAddress("b1", &b1);

			// new Trees???
			TTree *errorT= new TTree("errorTree","Holds all of the error analysis values (uncert. for each value)");
			
			TTree *aeT = new TTree("aeTree", "Angle and Error Tree Combined");

			//define analysis variables
			double ang_sig[3] = {0};
			double x_sig[4] = {0.5, 0.5, 0.5, 0.5};
			double y_sig[4] = {3.6, 3.6, 3.6, 3.6};
			double x_int_sig = 0;
			double y_int_sig = 0;
			double m0_sig = 0;
			double m1_sig = 0;
			double b0_sig = 0;
			double b1_sig = 0;
			double const Pi = 3.14159265359;
			double convert = 180 / Pi;
			
			//get branch info from these guyz
			errorT->Branch("ang_sig",&ang_sig, "ang_sig[3]/D");
			errorT->Branch("x_sig", &x_sig, "x_sig[4]/D");
			errorT->Branch("y_sig", &y_sig, "y_sig[4]/D");
			errorT->Branch("x_int_sig", &x_int_sig, "x_int_sig/D");
			errorT->Branch("y_int_sig", &y_int_sig, "y_int_sig/D");
			errorT->Branch("m0_sig", &m0_sig, "m0_sig/D");
			errorT->Branch("m1_sig", &m1_sig, "m1_sig/D");
			errorT->Branch("b0_sig", &b0_sig, "b0_sig/D");
			errorT->Branch("b1_sig", &b1_sig, "b1_sig/D");
			
			aeT->Branch("ang",&ang, "ang[3]/D");
			aeT->Branch("x", &x, "x[4]/D");
			aeT->Branch("y", &y, "y[4]/D");
			aeT->Branch("x_int", &x_int, "x_int/D");
			aeT->Branch("x_diff", &x_diff, "x_diff/D");
			aeT->Branch("y_int", &y_int, "y_int/D");
			aeT->Branch("m0", &m0, "m0/D");
			aeT->Branch("m1", &m1, "m1/D");
			aeT->Branch("b0", &b0, "b0/D");
			aeT->Branch("b1", &b1, "b1/D");
			aeT->Branch("ang_sig",&ang_sig, "ang_sig[3]/D");
			aeT->Branch("x_sig", &x_sig, "x_sig[4]/D");
			aeT->Branch("y_sig", &y_sig, "y_sig[4]/D");
			aeT->Branch("x_int_sig", &x_int_sig, "x_int_sig/D");
			aeT->Branch("y_int_sig", &y_int_sig, "y_int_sig/D");
			aeT->Branch("m0_sig", &m0_sig, "m0_sig/D");
			aeT->Branch("m1_sig", &m1_sig, "m1_sig/D");
			aeT->Branch("b0_sig", &b0_sig, "b0_sig/D");
			aeT->Branch("b1_sig", &b1_sig, "b1_sig/D");

			Int_t nentries = chain->GetEntries();

			
			for (Int_t i=0; i<nentries; i++) {
				chain->GetEntry(i);
				//This is where all of the math will occur for calculating the uncertianty
				//in each value. Error for all values must be obtained.
				m0_sig = m0*TMath::Sqrt((2*TMath::Power(((y_sig[0])/(y[0]-y[1])), 2)+2*TMath::Power(((x_sig[0])/(x[0]-x[1])), 2)));
				m1_sig = m1*TMath::Sqrt((2*TMath::Power(((y_sig[0])/(y[3]-y[2])), 2)+2*TMath::Power(((x_sig[0])/(x[3]-x[2])), 2)));
				b0_sig = TMath::Sqrt(TMath::Power(y_sig[0], 2)+TMath::Power(m0_sig*x[0], 2)+TMath::Power(x_sig[0]*m0, 2));
				b1_sig = TMath::Sqrt(TMath::Power(y_sig[0], 2)+TMath::Power(m1_sig*x[2], 2)+TMath::Power(x_sig[0]*m1, 2));
				x_int_sig = x_int*TMath::Sqrt(TMath::Power(((b0_sig+b1_sig)/(b1-b0)), 2)+TMath::Power(((m0_sig+m1_sig)/(m1-m0)), 2));
				y_int_sig = TMath::Sqrt(TMath::Power(m0_sig*x_int, 2)+TMath::Power(x_int_sig*m0, 2)+TMath::Power(b0_sig, 2));
				ang_sig[0] = m0_sig*TMath::Sqrt(1/(1+m0*m0));
				ang_sig[1] = m1_sig*TMath::Sqrt(1/(1+m1*m1));
				ang_sig[2] = TMath::Sqrt(ang_sig[0]*ang_sig[0]+ang_sig[1]*ang_sig[1]);
			
				errorT->Fill();
				aeT->Fill();
/*	
				if (ang[2]>-20 && ang[2]<20) {
					std::cout << "From entry: " << i << " --> ang[2] = " << ang[2] << " +/- " << ang_sig[2] << std::endl;
					cin.ignore();
				}*/
			}
			
			std::cout << "Processing complete\n";	
			// things we need:
			//	-algorithm to alg the rithm's
			// 	-algorithm to do the sick ass trig to find the angles between the paths (get them scattering angles)
			//	-maybe code to determine the errors in the angles as well

			//	-Probably need a new tree with branches determined by the previous comments
			//Write the trees into the root file
			errorT->Write();
			aeT->Write();
			outFile->Close();
		} else break;
	} 
	std::cout << "Complete!\n";
}
