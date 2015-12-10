//This script takes the processed data (in a ROOT file), and shows the frames one at a time
//it also shows x-axis chain for the individual chambers.

#include <iostream>
#include <fstream>
#include <cassert>
#include <TFile.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <cmath>
#include <TChain.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TTree.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>

#include <iostream>

void frameViewer(char* arg) {
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

			const int width=480;	//width of the raw image
			const int height=640;	//height of the raw image


			int x=-10;	//x from file
			int y=-10;	//y from file
			int intensity=-10;	//pixle intensity from file
			int pNum=0;//a counter of the order in which the frame was processed

			//the 2d array which will store each frame of image data.
			int frame[480][640]={0};

			//variables
			int UNIXtime=0;
			float tdc[2]={-10,-10};


			//TTree *T = new TTree("T","TTree of muplus data");
			//add the 'branches' to the tree we will now read in
			chain->SetBranchAddress("pNum",&pNum);	//branch for the frame number
			chain->SetBranchAddress("frame",&frame);	//branch for frame data

			TH2I *frameHisto = new TH2I("Single 4 Spark Event","",width,0,width,height,0,height);	//histogram for the stacked images

			TH1I *chamber1 = new TH1I("chamber1","Chamber 1",width,0,width);//histogram for chamber 1 (the top one)
			TH1I *chamber2 = new TH1I("chamber2","Chamber 2",width,0,width);//histogram for chamber 2
			TH1I *chamber3 = new TH1I("chamber3","Chamber 3",width,0,width);//histogram for chamber 3
			TH1I *chamber4 = new TH1I("chamber4","Chamber 4",width,0,width);//histogram for chamber 4 (the bottom one)
			
			TH1I *chamber1y = new TH1I("chamber1y","Chamber 1",height,0,height);//histogram for chamber 1 (the top one)
			TH1I *chamber2y = new TH1I("chamber2y","Chamber 2",height,0,height);//histogram for chamber 2
			TH1I *chamber3y = new TH1I("chamber3y","Chamber 3",height,0,height);//histogram for chamber 3
			TH1I *chamber4y = new TH1I("chamber4y","Chamber 4",height,0,height);//histogram for chamber 4 (the bottom one)

			//output the plot of the stacked images
		
			TCanvas *fH2 = new TCanvas("fH2", "Single 4 Spark Event", 0, 0, 800, 800);
			fH2->cd();
			frameHisto->Draw();
			frameHisto->GetXaxis()->SetTitle("X position (px)");
			frameHisto->GetXaxis()->CenterTitle();

			frameHisto->GetYaxis()->SetTitle("Intensity");
			frameHisto->GetYaxis()->SetTitleOffset(1.4);
			frameHisto->GetYaxis()->CenterTitle();

			TCanvas *pc2 = new TCanvas("pc2","Frame",0,0,800,800);
			
			pc2->Divide(2,2);
			
			pc2->cd(1);
			chamber1->Draw();
			chamber1->GetXaxis()->SetTitle("X position (px)");
			chamber1->GetXaxis()->CenterTitle();

			chamber1->GetYaxis()->SetTitle("Intensity");
			chamber1->GetYaxis()->SetTitleOffset(1.4);
			chamber1->GetYaxis()->CenterTitle();

			pc2->cd(2);
			chamber2->Draw();
			chamber2->GetXaxis()->SetTitle("X position (px)");
			chamber2->GetXaxis()->CenterTitle();

			chamber2->GetYaxis()->SetTitle("Intensity");
			chamber2->GetYaxis()->SetTitleOffset(1.4);
			chamber2->GetYaxis()->CenterTitle();

			pc2->cd(3);
			chamber3->Draw();
			chamber3->GetXaxis()->SetTitle("X position (px)");
			chamber3->GetXaxis()->CenterTitle();

			chamber3->GetYaxis()->SetTitle("Intensity");
			chamber3->GetYaxis()->SetTitleOffset(1.4);
			chamber3->GetYaxis()->CenterTitle();

			pc2->cd(4);
			chamber4->Draw();
			chamber4->GetXaxis()->SetTitle("X position (px)");
			chamber4->GetXaxis()->CenterTitle();

			chamber4->GetYaxis()->SetTitle("Intensity");
			chamber4->GetYaxis()->SetTitleOffset(1.4);
			chamber4->GetYaxis()->CenterTitle();

			//TFile myF("trackTree.root","RECREATE");
			

			//loop over all data in chain
			Int_t nevent = chain->GetEntries();	//get the number of entries in the TChain
			for (Int_t i=0;i<nevent;i++) {
				chain->GetEntry(i);

				//put the frame data into the histogram for this event
				for(int x=0;x<width;x++){
					for(int y=0;y<height;y++){
						if(frame[x][y]>0){
							frameHisto->Fill(x,y,frame[x][y]);
							if(y>580 && y<610){
								chamber1->Fill(x,frame[x][y]);
								chamber1y->Fill(y,frame[x][y]);
							}
							else if(y>400 && y<440){
								chamber2->Fill(x,frame[x][y]);
								chamber2y->Fill(y,frame[x][y]);
							}
							else if(y>240 && y<280){
								chamber3->Fill(x,frame[x][y]);
								chamber3y->Fill(y,frame[x][y]);
							}
							else if(y>50 && y<100){
								chamber4->Fill(x,frame[x][y]);
								chamber4y->Fill(y,frame[x][y]);
							}
						}	
					}
				}

				double x12[2];
				double y12[2];
				double x34[2];
				double y34[2];

				x12[0] = chamber1->GetMean();
				y12[0] = chamber1y->GetMean();			//593.3;
				x12[1] = chamber2->GetMean();
				y12[1] = chamber2y->GetMean();			//424.7;
				x34[0] = chamber3->GetMean();
				y34[0] = chamber3y->GetMean();			//262.5;
				x34[1] = chamber4->GetMean();
				y34[1] = chamber4y->GetMean();			//69.33;

				cout << "Chamber1x: " << chamber1->GetMean() << endl;
				cout << "Chamber1y: " << chamber1y->GetMean() << endl;
				cout << "Chamber2x: " << chamber2->GetMean() << endl;
				cout << "Chamber2y: " << chamber2y->GetMean() << endl;
				cout << "Chamber3x: " << chamber3->GetMean() << endl;
				cout << "Chamber3y: " << chamber3y->GetMean() << endl;
				cout << "Chamber4x: " << chamber4->GetMean() << endl;
				cout << "Chamber4y: " << chamber4y->GetMean() << endl;

				pc2->cd(1);
				chamber1->Draw();
				gPad->Update();
				
				pc2->cd(2);
				chamber2->Draw();
				gPad->Update();
				
				pc2->cd(3);
				chamber3->Draw();
				gPad->Update();
				
				pc2->cd(4);
				chamber4->Draw();
				gPad->Update();
				
				fH2->cd();
				frameHisto->Draw();
				gPad->Update();

				//wait for user input to advance to next event
				cout << "Frame Number=" << pNum<<endl;
				cout << "Press enter to advance to the next frame" << endl;
				cin.ignore();
				
				//clear the old frame from the histogram
				frameHisto->Reset();
				chamber1->Reset();
				chamber2->Reset();
				chamber3->Reset();
				chamber4->Reset();
			}
		} else break;
	}
}
