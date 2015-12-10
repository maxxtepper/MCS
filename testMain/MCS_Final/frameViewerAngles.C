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
#include <TH2D.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <cmath>
#include <TMath.h>
#include <TLine.h>
#include <TGraph.h>
#include <TPad.h>
#include <TCanvas.h>

void frameViewerAngles_Mall(char* arg){

	//Take the arguments and save them into respective strings
	std::string infileName;
	std::string inF;
	std::string inPrefix;
	std::string runs, layers;
	std::string runCount;
	std::istringstream stm(arg);

	inPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/rawRoot/";

	while (true) {	
		if (std::getline(stm, layers, ' ')) {

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

			//define analysis variables
			Double_t ang[3] = {0};
			Double_t angD = 0;
			Double_t x[4] = {0};
			Double_t y[4] = {0};
			Double_t x_int[1];
			Double_t x_diff= 0;
			Double_t y_int[1];
			Double_t m0 = 0;
			Double_t m1 = 0;
			Double_t b0 = 0;
			Double_t b1 = 0;
			Double_t const Pi = 3.14159265359;
			Double_t convert = 180 / Pi;
			
			TH2D *frameHisto = new TH2D("frameHisto","Muon Tracking by Line Fitting",width,0,width,height,0,height);	//histogram for the stacked images
			TH1D *chamber1x = new TH1D("chamber1x", "Chamber 1x", width, 0, width);
			TH1D *chamber1y = new TH1D("chamber1y", "Chamber 1y", height, 0, height);
			TH1D *chamber2x = new TH1D("chamber2x", "Chamber 2x", width, 0, width);
			TH1D *chamber2y = new TH1D("chamber2y", "Chamber 2y", height, 0, height);
			TH1D *chamber3x = new TH1D("chamber3x", "Chamber 3x", width, 0, width);
			TH1D *chamber3y = new TH1D("chamber3y", "Chamber 3y", height, 0, height);
			TH1D *chamber4x = new TH1D("chamber4x", "Chamber 4x", width, 0, width);
			TH1D *chamber4y = new TH1D("chamber4y", "Chamber 4y", height, 0, height);

			TCanvas *pc2 = new TCanvas("pc2","Frame",0,0,800,800);

			pc2->cd();
			frameHisto->SetStats(false);
			frameHisto->GetXaxis()->SetTitle("X position (px)");
			frameHisto->GetXaxis()->CenterTitle();
			frameHisto->GetYaxis()->SetTitle("Y position (px)");
			frameHisto->GetYaxis()->CenterTitle();
		
			Int_t nentries = chain->GetEntries();
			int count = 0;
			for (Int_t i=0; i<nentries; i++) {
				chain->GetEntry(i);
				for (int x=0; x<width; x++) {
					for (int y=0; y<height; y++) {
						frameHisto->Fill(x, y, frame[x][y]);
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
				
				//Calculate slopes and intercepts
				m0 = ((y[1]-y[0])/(x[1]-x[0]));
				m1 = ((y[3]-y[2])/(x[3]-x[2]));
				b0 = (y[0]-(m0*x[0]));
				b1 = (y[2]-(m1*x[2]));
				
				//Calculate the intersection coordinates
				x_int[0] = ((b1-b0)/(m0-m1));
				y_int[0] = ((m0*x_int[0])+b0);
				
				//Calculate the angle
				ang[0] = atan(m0)*convert;
				ang[1] = atan(m1)*convert;

				//Calculate the rough difference in x in medium
				x_diff = ((340-b0)/m0) - ((340-b1)/m1);

//				std::cout << "ang[0], frame" << i << " = " << ang[0]<< std::endl;
//				std::cout << "ang[1], frame" << i << " = " << ang[1]<< std::endl;
//				cin.ignore();

				//Make array of xdiff RMS
				Double_t x_diffRMS[5] = {41.06, 45.03, 43.29, 40.71, 34.25};		
				Double_t xRMS[4] = {0};
		
				xRMS[0] = chamber1x->GetRMS();
				xRMS[1] = chamber2x->GetRMS();
				xRMS[2] = chamber3x->GetRMS();
				xRMS[3] = chamber4x->GetRMS();
 
				if(ang[0] < 0)
					ang[0] += 180;
				if(ang[1] < 0)
					ang[1] += 180;
				ang[2] = (ang[1]-ang[0]);
			
				//OTHER WAY TO FIND THE ANGLES
				Double_t x_comp[2] = {(x[3]-x[2]), (x[1]-x[0])};
				Double_t y_comp[2] = {(y[3]-y[2]), (y[1]-y[0])};
					
				Double_t vec_mag[2];
				vec_mag[0] = TMath::Sqrt(x_comp[0]*x_comp[0]+y_comp[0]*y_comp[0]);	
				vec_mag[1] = TMath::Sqrt(x_comp[1]*x_comp[1]+y_comp[1]*y_comp[1]);
	
				Double_t cosArg = ((x_comp[0]*x_comp[1]+y_comp[0]*y_comp[1])/(vec_mag[0]*vec_mag[1]));
				angD = acos(cosArg)*convert;	

				if (x_comp[1]<x_comp[0]) {
					angD*=-1;
				}	

				Double_t x_RMS[4];
				x_RMS[0] = chamber1x->GetRMS();
				x_RMS[1] = chamber2x->GetRMS();
				x_RMS[2] = chamber3x->GetRMS();
				x_RMS[3] = chamber4x->GetRMS();

				std::cout << "Processing frame number: " << pNum << "\r";
	
				if (x_RMS[0]>5 && x_RMS[1]>5 && x_RMS[2]>5 && x_RMS[3]>5) {

					//find new x points from given ymin, ymax
					
					double x_min[2];
					double x_max[2];
					double y_min[2] = {0,0};
					double y_max[2] = {640,640};				

					x_min[0] = (y_min[0] - b0) / m0;
					x_max[0] = (y_max[0] - b0) / m0;
					x_min[1] = (y_min[1] - b1) / m1;
					x_max[1] = (y_max[1] - b1) / m1;

					TLine *line0 = new TLine(x_min[0], y_min[0], x_max[0], y_max[0]);
					TLine *line1 = new TLine(x_min[1], y_min[1], x_max[1], y_max[1]);
					TGraph *intersect = new TGraph(1, x_int, y_int); 
					
					pc2->cd();
					frameHisto->Draw("COLZ");
					line0->Draw();
					line1->Draw();
					intersect->Draw("F1*");
					gPad->Update();				

					//wait for user input to advance to next event
					std::cout << "Frame Number=" << pNum << std::endl;
					std::cout << "Press enter to advance to the next frame" << std::endl;
					std::cout << "x[0] RMS = " << x_RMS[0] << std::endl;
					std::cout << "x[1] RMS = " << x_RMS[1] << std::endl;
					std::cout << "x[2] RMS = " << x_RMS[2] << std::endl;
					std::cout << "x[3] RMS = " << x_RMS[3] << std::endl;
					std::cin.ignore();
					
					//clear the old frame from the histogram
					frameHisto->Reset();
					chamber1x->Reset();
					chamber1y->Reset();
					chamber2x->Reset();
					chamber2y->Reset();
					chamber3x->Reset();
					chamber3y->Reset();
					chamber4x->Reset();
					chamber4y->Reset();
				}
			}
		} else {
			break;
		}
	} 
	std::cout << "Complete!\n";
}
