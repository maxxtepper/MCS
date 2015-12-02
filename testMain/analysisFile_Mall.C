//Analysis program for MCS
//preliminary script for analyzing data
//honestly, probably more of an exercise for me to reinforce some root stuff
//-Joe
//This is a test
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

void analysisFile_Mall(char* arg){

	//Take the arguments and save them into respective strings
	std::string infileName, outfileName;
	std::string inF, outF;
	std::string inPrefix, outPrefix;
	std::string runs, layers;
	std::string runCount;
	std::istringstream stm(arg);

	inPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/rawRoot/";
	outPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/angleRoot/";

	while (true) {	
		if (std::getline(stm, layers, ' ')) {

			//create the output root file
			outF = "angle_" + layers + "layers.root";
			outfileName = outPrefix + outF;
			TFile *outFile = new TFile(outfileName.c_str(), "RECREATE");
			std::cout << outF << " created\n";
			
			//load the input root files
			TChain *chain = new TChain("fourChamTree");

			for (int i=0; ; i++) {
				runCount = std::to_string(i);
				inF = "run" + runCount + "_" + layers + "layers.root";
				infileName = inPrefix + inF;
				ifstream fin;
				fin.open(infileName.c_str());
				if (!fin.fail()) {
					fin.close();
					chain->Add(infileName.c_str());
					std::cout << "Got " << inF << std::endl;
				} else break;
			}

			//define frame arrays
			int frame[480][640]={0};
			
			const int width = 480;
			const int height = 640;

			//define picture numbers for each tree
			// (in case they are needed)
			int pNum = 0;

			//get branch info from these guyz
			chain->SetBranchAddress("pNum", &pNum);
			chain->SetBranchAddress("frame", &frame);
		//	threeChamT->SetBranchAddress("pNum", &pNum_three);
		//	threeChamT->SetBranchAddress("frame", &frame_three);

			// new Trees???
			TTree *angleT = new TTree("angleTree","holds info for getting angles");
			TTree *angleCutsT = new TTree("angleCutsTree","holds info for getting angles with cuts");

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
			double const Pi = 3.14159265359;
			double convert = 180 / Pi;

			//branch time
			angleT->Branch("ang",&ang,"ang[3]/D");
			angleT->Branch("x", &x, "x[4]/D");
			angleT->Branch("y", &y, "y[4]/D");
			angleT->Branch("x_int", &x_int, "x_int/D");
			angleT->Branch("x_diff", &x_diff, "x_diff/D");
			angleT->Branch("y_int", &y_int, "y_int/D");
			angleT->Branch("m0", &m0, "m0/D");
			angleT->Branch("m1", &m1, "m1/D");
			angleT->Branch("b0", &b0, "b0/D");
			angleT->Branch("b1", &b1, "b1/D");
			
			angleCutsT->Branch("ang",&ang,"ang[3]/D");
			angleCutsT->Branch("x", &x, "x[4]/D");
			angleCutsT->Branch("y", &y, "y[4]/D");
			angleCutsT->Branch("x_int", &x_int, "x_int/D");
			angleCutsT->Branch("x_diff", &x_diff, "x_diff/D");
			angleCutsT->Branch("y_int", &y_int, "y_int/D");
			angleCutsT->Branch("m0", &m0, "m0/D");
			angleCutsT->Branch("m1", &m1, "m1/D");
			angleCutsT->Branch("b0", &b0, "b0/D");
			angleCutsT->Branch("b1", &b1, "b1/D");

			TH1D *chamber1x = new TH1D("chamber1x", "Chamber 1x", width, 0, width);
			TH1D *chamber1y = new TH1D("chamber1y", "Chamber 1y", height, 0, height);
			TH1D *chamber2x = new TH1D("chamber2x", "Chamber 2x", width, 0, width);
			TH1D *chamber2y = new TH1D("chamber2y", "Chamber 2y", height, 0, height);
			TH1D *chamber3x = new TH1D("chamber3x", "Chamber 3x", width, 0, width);
			TH1D *chamber3y = new TH1D("chamber3y", "Chamber 3y", height, 0, height);
			TH1D *chamber4x = new TH1D("chamber4x", "Chamber 4x", width, 0, width);
			TH1D *chamber4y = new TH1D("chamber4y", "Chamber 4y", height, 0, height);

			TH1D *chamberStackX = new TH1D("chamberStackX", "Sparks stacked and centered at their RMS", width/2, -width/2, width/2);
			TH1D *chamberStackY = new TH1D("chamberStackY", "Sparks stacked and centered at their RMS", height/2, -height/2, height/2);
			
			Int_t nentries = chain->GetEntries();

			for (Int_t i=0; i<nentries; i++) {
				chain->GetEntry(i);
				for (int x=0; x<width; x++) {
					for (int y=0; y<height; y++) {
						if (frame[x][y]>0) {
							if (y>580 && y<610) {
								chamber1x->Fill(x, frame[x][y]);
								chamber1y->Fill(y, frame[x][y]);
							}
							else if (y>400 && y<440) {
								chamber2x->Fill(x, frame[x][y]);
								chamber2y->Fill(y, frame[x][y]);
							}
							else if (y>240 && y<280) {
								chamber3x->Fill(x, frame[x][y]);
								chamber3y->Fill(y, frame[x][y]);
							}
							else if (y>50 && y<100)	{
								chamber4x->Fill(x, frame[x][y]);
								chamber4y->Fill(y, frame[x][y]);
							}
						}
					}
				}

				//Get the means and fill the x, y coordinates
				x[0] = chamber1x->GetMean();
				y[0] = chamber1y->GetMean();
				x[1] = chamber2x->GetMean();
				y[1] = chamber2y->GetMean();
				x[2] = chamber3x->GetMean();
				y[2] = chamber3y->GetMean();
				x[3] = chamber4x->GetMean();
				y[3] = chamber4y->GetMean();
			
				Double_t x_pos = 0;
				Double_t y_pos = 0;
				
				for (int i=0; i<width; i++) {
					for (int j=0; j<height; j++) {
						if (frame[i][j]>0) {
							if (j>580 && j<610) {
								chamberStackX->Fill(x_pos-x[0], frame[i][j]);
								chamberStackY->Fill(y_pos-y[0], frame[i][j]);
							}
							else if (j>400 && j<440) {
								chamberStackX->Fill(x_pos-x[1], frame[i][j]);
								chamberStackY->Fill(y_pos-y[1], frame[i][j]);
							}
							else if (j>240 && j<280) {
								chamberStackX->Fill(x_pos-x[2], frame[i][j]);
								chamberStackY->Fill(y_pos-y[2], frame[i][j]);
							}
							else if (j>50 && j<100)	{
								chamberStackX->Fill(x_pos-x[3], frame[i][j]);
								chamberStackY->Fill(y_pos-y[3], frame[i][j]);
							}
						}
						y_pos++;
					}
					x_pos++;
				}

				//Calculate slopes and intercepts
				m0 = ((y[1]-y[0])/(x[1]-x[0]));
				m1 = ((y[3]-y[2])/(x[3]-x[2]));
				b0 = (y[0]-(m0*x[0]));
				b1 = (y[2]-(m1*x[2]));
				
				//Calculate the intersection coordinates
				x_int = ((b1-b0)/(m0-m1));
				y_int = ((m0*x_int)+b0);
				
				//Calculate the angle
				ang[0] = atan(m0)*convert;
				ang[1] = atan(m1)*convert;

				//Calculate the rough difference in x in medium
				x_diff = ((340-b0)/m0) - ((340-b1)/m1);

//				std::cout << "ang[0], frame" << i << " = " << ang[0]<< std::endl;
//				std::cout << "ang[1], frame" << i << " = " << ang[1]<< std::endl;
//				cin.ignore();
				 
				if(ang[0] < 0)
					ang[0] += 180;
				if(ang[1] < 0)
					ang[1] += 180;
				ang[2] = (ang[1]-ang[0]);
				
				//Fill the angle tree
				angleT->Fill();
				
				if (x_diff<34 && x_diff>-34) {
					angleCutsT->Fill();
				}
			
				//reset the histos for the next entry
				chamber1x->Reset();
				chamber1y->Reset();
				chamber2x->Reset();
				chamber2y->Reset();
				chamber3x->Reset();
				chamber3y->Reset();
				chamber4x->Reset();
				chamber4y->Reset();
			}
			std::cout << "Processing complete\n";	
			// things we need:
			//	-algorithm to alg the rithm's
			// 	-algorithm to do the sick ass trig to find the angles between the paths (get them scattering angles)
			//	-maybe code to determine the errors in the angles as well

			//	-Probably need a new tree with branches determined by the previous comments
			//Write the trees into the root file
			angleT->Write();
			angleCutsT->Write();
			chamberStackX->Write();
			chamberStackY->Write();
			outFile->Close();
		} else {
			break;
		}
	} 
	std::cout << "Complete!\n";
}
