//Generates the line fits for trajectories, along with angles.
//Creates stacks of spark data (pixels) and intensity for analysis.
//
//
//

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

void angleDataGenerator(char* arg){

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

			// new Trees
			TTree *angleT = new TTree("angleTree","holds info for getting angles");
			TTree *angleCutsT = new TTree("angleCutsTree","holds info for getting angles with cuts");

			//define analysis variables
			double ang[3] = {0};
			double angD = 0;
			double x[4] = {0};
			double y[4] = {0};
			double x_int = 0;
			double x_diff= 0;
			double y_int = 0;
			double m0 = 0;
			double m1 = 0;
			double b0 = 0;
			double b1 = 0;
			double x_RMS[4] = {0};
			double const Pi = 3.14159265359;
			double convert = 180 / Pi;

			//branch time
			angleT->Branch("ang",&ang,"ang[3]/D");
			angleT->Branch("angD",&angD,"angD/D");
			angleT->Branch("x", &x, "x[4]/D");
			angleT->Branch("y", &y, "y[4]/D");
			angleT->Branch("x_int", &x_int, "x_int/D");
			angleT->Branch("x_diff", &x_diff, "x_diff/D");
			angleT->Branch("y_int", &y_int, "y_int/D");
			angleT->Branch("m0", &m0, "m0/D");
			angleT->Branch("m1", &m1, "m1/D");
			angleT->Branch("b0", &b0, "b0/D");
			angleT->Branch("b1", &b1, "b1/D");
			angleT->Branch("x_RMS", &x_RMS, "x_RMS[4]/D");
			
			angleCutsT->Branch("ang",&ang,"ang[3]/D");
			angleCutsT->Branch("angD",&angD,"angD/D");
			angleCutsT->Branch("x", &x, "x[4]/D");
			angleCutsT->Branch("y", &y, "y[4]/D");
			angleCutsT->Branch("x_int", &x_int, "x_int/D");
			angleCutsT->Branch("x_diff", &x_diff, "x_diff/D");
			angleCutsT->Branch("y_int", &y_int, "y_int/D");
			angleCutsT->Branch("m0", &m0, "m0/D");
			angleCutsT->Branch("m1", &m1, "m1/D");
			angleCutsT->Branch("b0", &b0, "b0/D");
			angleCutsT->Branch("b1", &b1, "b1/D");
			angleCutsT->Branch("x_RMS", &x_RMS, "x_RMS[4]/D");

			TH1D *chamber1x = new TH1D("chamber1x", "Chamber 1x", width, 0, width);
			TH1D *chamber1y = new TH1D("chamber1y", "Chamber 1y", height, 0, height);
			TH1D *chamber2x = new TH1D("chamber2x", "Chamber 2x", width, 0, width);
			TH1D *chamber2y = new TH1D("chamber2y", "Chamber 2y", height, 0, height);
			TH1D *chamber3x = new TH1D("chamber3x", "Chamber 3x", width, 0, width);
			TH1D *chamber3y = new TH1D("chamber3y", "Chamber 3y", height, 0, height);
			TH1D *chamber4x = new TH1D("chamber4x", "Chamber 4x", width, 0, width);
			TH1D *chamber4y = new TH1D("chamber4y", "Chamber 4y", height, 0, height);

			TH1D *chamberStackCutsX = new TH1D("chamberStackCutsX", "Sparks stacked and centered at their RMS", 2*width, -width/2, width/2);
			TH1D *chamberStackCutsY = new TH1D("chamberStackCutsY", "Sparks stacked and centered at their RMS", height/2, -height/2, height/2);
			TH1D *chamberStackCutsX1 = new TH1D("chamberStackCutsX1", "Sparks stacked and centered at their RMS", 2*width, -width/2, width/2);
			TH1D *chamberStackCutsY1 = new TH1D("chamberStackCutsY1", "Sparks stacked and centered at their RMS", height/2, -height/2, height/2);
			TH1D *chamberStackCutsX2 = new TH1D("chamberStackCutsX2", "Sparks stacked and centered at their RMS", 2*width, -width/2, width/2);
			TH1D *chamberStackCutsY2 = new TH1D("chamberStackCutsY2", "Sparks stacked and centered at their RMS", height/2, -height/2, height/2);
			TH1D *chamberStackCutsX3 = new TH1D("chamberStackCutsX3", "Sparks stacked and centered at their RMS", 2*width, -width/2, width/2);
			TH1D *chamberStackCutsY3 = new TH1D("chamberStackCutsY3", "Sparks stacked and centered at their RMS", height/2, -height/2, height/2);
			TH1D *chamberStackCutsX4 = new TH1D("chamberStackCutsX4", "Sparks stacked and centered at their RMS", 2*width, -width/2, width/2);
			TH1D *chamberStackCutsY4 = new TH1D("chamberStackCutsY4", "Sparks stacked and centered at their RMS", height/2, -height/2, height/2);
			TH1D *chamberStackX = new TH1D("chamberStackX", "Sparks stacked and centered at their RMS", 2*width, -width/2, width/2);
			TH1D *chamberStackY = new TH1D("chamberStackY", "Sparks stacked and centered at their RMS", height/2, -height/2, height/2);
			TH1D *chamberStackX1 = new TH1D("chamberStackX1", "Sparks stacked and centered at their RMS", 2*width, -width/2, width/2);
			TH1D *chamberStackY1 = new TH1D("chamberStackY1", "Sparks stacked and centered at their RMS", height/2, -height/2, height/2);
			TH1D *chamberStackX2 = new TH1D("chamberStackX2", "Sparks stacked and centered at their RMS", 2*width, -width/2, width/2);
			TH1D *chamberStackY2 = new TH1D("chamberStackY2", "Sparks stacked and centered at their RMS", height/2, -height/2, height/2);
			TH1D *chamberStackX3 = new TH1D("chamberStackX3", "Sparks stacked and centered at their RMS", 2*width, -width/2, width/2);
			TH1D *chamberStackY3 = new TH1D("chamberStackY3", "Sparks stacked and centered at their RMS", height/2, -height/2, height/2);
			TH1D *chamberStackX4 = new TH1D("chamberStackX4", "Sparks stacked and centered at their RMS", 2*width, -width/2, width/2);
			TH1D *chamberStackY4 = new TH1D("chamberStackY4", "Sparks stacked and centered at their RMS", height/2, -height/2, height/2);
			TH1D* chamber1x_RMS = new TH1D("chamber1x_RMS", "Chamber 1x RMS", 100, 0, 100);
			TH1D* chamber2x_RMS = new TH1D("chamber2x_RMS", "Chamber 2x RMS", 100, 0, 100);
			TH1D* chamber3x_RMS = new TH1D("chamber3x_RMS", "Chamber 3x RMS", 100, 0, 100);
			TH1D* chamber4x_RMS = new TH1D("chamber4x_RMS", "Chamber 4x RMS", 100, 0, 100);

			chamberStackX->GetXaxis()->SetTitle("X Position (px)");
			chamberStackY->GetYaxis()->SetTitle("Number of Pixels");
			
			Int_t nentries = chain->GetEntries();
			
			int countRaw = 0;
			int countCuts = 0;
			
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
		
				x_RMS[0] = chamber1x->GetRMS();
				x_RMS[1] = chamber2x->GetRMS();
				x_RMS[2] = chamber3x->GetRMS();
				x_RMS[3] = chamber4x->GetRMS();
				
				chamber1x_RMS->Fill(x_RMS[0]);
				chamber2x_RMS->Fill(x_RMS[1]);
				chamber3x_RMS->Fill(x_RMS[2]);
				chamber4x_RMS->Fill(x_RMS[3]);
	
				Double_t x_pos = 0;
				Double_t y_pos = 0;
				
				for (int i=0; i<width; i++) {
					for (int j=0; j<height; j++) {
						if (frame[i][j]>0) {
							if (j>580 && j<610) {
								chamberStackX->Fill(x_pos-x[0], frame[i][j]);
								chamberStackY->Fill(y_pos-y[0], frame[i][j]);
								chamberStackX1->Fill(x_pos-x[0], frame[i][j]);
								chamberStackY1->Fill(y_pos-y[0], frame[i][j]);
							}
							else if (j>400 && j<440) {
								chamberStackX->Fill(x_pos-x[1], frame[i][j]);
								chamberStackY->Fill(y_pos-y[1], frame[i][j]);
								chamberStackX2->Fill(x_pos-x[1], frame[i][j]);
								chamberStackY2->Fill(y_pos-y[1], frame[i][j]);
							}
							else if (j>240 && j<280) {
								chamberStackX->Fill(x_pos-x[2], frame[i][j]);
								chamberStackY->Fill(y_pos-y[2], frame[i][j]);
								chamberStackX3->Fill(x_pos-x[2], frame[i][j]);
								chamberStackY3->Fill(y_pos-y[2], frame[i][j]);
							}
							else if (j>50 && j<100)	{
								chamberStackX->Fill(x_pos-x[3], frame[i][j]);
								chamberStackY->Fill(y_pos-y[3], frame[i][j]);
								chamberStackX4->Fill(x_pos-x[3], frame[i][j]);
								chamberStackY4->Fill(y_pos-y[3], frame[i][j]);
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

				//Make array of xdiff RMS
				double x_diffRMS[5] = {41.06, 45.03, 43.29, 40.71, 34.25};		
 
				if(ang[0] < 0)
					ang[0] += 180;
				if(ang[1] < 0)
					ang[1] += 180;
				ang[2] = (ang[1]-ang[0]);
			
				//OTHER WAY TO FIND THE ANGLES
				double x_comp[2] = {(x[3]-x[2]), (x[1]-x[0])};
				double y_comp[2] = {(y[3]-y[2]), (y[1]-y[0])};
					
				double vec_mag[2];
				vec_mag[0] = TMath::Sqrt(x_comp[0]*x_comp[0]+y_comp[0]*y_comp[0]);	
				vec_mag[1] = TMath::Sqrt(x_comp[1]*x_comp[1]+y_comp[1]*y_comp[1]);
	
				double cosArg = ((x_comp[0]*x_comp[1]+y_comp[0]*y_comp[1])/(vec_mag[0]*vec_mag[1]));
				angD = acos(cosArg)*convert;	

				if (x_comp[1]<x_comp[0]) {
					angD*=-1;
				}	
				//Fill the angle tree
				angleT->Fill();
				countRaw++;

				Double_t sparkCut = 5;
				int temp = atoi(layers.c_str());	
				if ((x_diff < x_diffRMS[temp] && x_diff > -1 * x_diffRMS[temp])) {
					for (int i=0; i<width; i++) {
						for (int j=0; j<height; j++) {
							if (frame[i][j]>0) {
								if (j>580 && j<610) {
									chamberStackCutsX->Fill(x_pos-x[0], frame[i][j]);
									chamberStackCutsY->Fill(y_pos-y[0], frame[i][j]);
									chamberStackCutsX1->Fill(x_pos-x[0], frame[i][j]);
									chamberStackCutsY1->Fill(y_pos-y[0], frame[i][j]);
								}
								else if (j>400 && j<440) {
									chamberStackCutsX->Fill(x_pos-x[1], frame[i][j]);
									chamberStackCutsY->Fill(y_pos-y[1], frame[i][j]);
									chamberStackCutsX2->Fill(x_pos-x[1], frame[i][j]);
									chamberStackCutsY2->Fill(y_pos-y[1], frame[i][j]);
								}
								else if (j>240 && j<280) {
									chamberStackCutsX->Fill(x_pos-x[2], frame[i][j]);
									chamberStackCutsY->Fill(y_pos-y[2], frame[i][j]);
									chamberStackCutsX3->Fill(x_pos-x[2], frame[i][j]);
									chamberStackCutsY3->Fill(y_pos-y[2], frame[i][j]);
								}
								else if (j>50 && j<100)	{
									chamberStackCutsX->Fill(x_pos-x[3], frame[i][j]);
									chamberStackCutsY->Fill(y_pos-y[3], frame[i][j]);
									chamberStackCutsX4->Fill(x_pos-x[3], frame[i][j]);
									chamberStackCutsY4->Fill(y_pos-y[3], frame[i][j]);
								}
							}
							y_pos++;
						}
						x_pos++;
					}
					
					if (x_RMS[0]<sparkCut && x_RMS[1]<sparkCut && x_RMS[2]<sparkCut && x_RMS[3]<sparkCut) { 
						angleCutsT->Fill();
						countCuts++;
						
					}
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
			std::cout << "Number in raw  = " << countRaw << std::endl;
			std::cout << "Number in cuts = " << countCuts << std::endl;
			angleT->Write();
			angleCutsT->Write();
			chamberStackX->Write();
			chamberStackY->Write();
			chamberStackX1->Write();
			chamberStackY1->Write();
			chamberStackX2->Write();
			chamberStackY2->Write();
			chamberStackX3->Write();
			chamberStackY3->Write();
			chamberStackX4->Write();
			chamberStackY4->Write();
			chamberStackCutsX->Write();
			chamberStackCutsY->Write();
			chamberStackCutsX1->Write();
			chamberStackCutsY1->Write();
			chamberStackCutsX2->Write();
			chamberStackCutsY2->Write();
			chamberStackCutsX3->Write();
			chamberStackCutsY3->Write();
			chamberStackCutsX4->Write();
			chamberStackCutsY4->Write();
			chamber1x_RMS->Write();
			chamber2x_RMS->Write();
			chamber3x_RMS->Write();
			chamber4x_RMS->Write();
			outFile->Close();
		} else {
			break;
		}
	} 
	std::cout << "Complete!\n";
}
