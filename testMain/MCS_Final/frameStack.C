//This script takes the processed data, and shows the frames stacked on top of eachother
#include <TChain.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <iostream>

int frameStack2_Mall(char* arg){
	//Take the arguments and save them into respective strings
	std::string infileName, outfileName0, outAllfileName0, outfileName1, outAllfileName1;
	std::string inF, outF0, outF1, outAll0, outAll1;
	std::string inPrefix, outPrefix;
	std::string runs, layers;
	std::string runCount;
	std::istringstream stm(arg);

	inPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/rawRoot/";
	outPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/images/";

	outAll0 = "sliceXCuts_allLayers.png";
	outAllfileName0 = outPrefix + outAll0;
	std::cout << outAll0 << " created\n";
	outAll1 = "projYCuts_allLayers.png";
	outAllfileName1 = outPrefix + outAll1;
	std::cout << outAll1 << " created\n";

	const int width=480;	//width of the raw image
	const int height=640;	//height of the raw image
	
	TH2I *frameHistoAll = new TH2I("frameHistoAll","Stacked Frames After Edge Cuts",width/4,0,width,height/4,0,height);	//histogram for the stacked images
	TH1I *chamber1All = new TH1I("chamber1All","Chamber 1 After Edge Cuts",width/4,0,width);//histogram for chamber 1 (the top one)
	TH1I *chamber2All = new TH1I("chamber2All","Chamber 2 After Edge Cuts",width/4,0,width);//histogram for chamber 2
	TH1I *chamber3All = new TH1I("chamber3All","Chamber 3 After Edge Cuts",width/4,0,width);//histogram for chamber 3
	TH1I *chamber4All = new TH1I("chamber4All","Chamber 4 After Edge Cuts",width/4,0,width);//histogram for chamber 4 (the bottom one)

	TCanvas *projCAll = new TCanvas("projCAll","",0,0,800,600);
	TCanvas *pc2All = new TCanvas("pc2All", "Stack of 4 Layer Runs", 0, 0, 800, 600);
	
	while (true) {	
		if (std::getline(stm, layers, ' ')) {

			//create the output root file
			outF0 = "sliceXCuts_" + layers + "layers.png";
			outfileName0 = outPrefix + outF0;
			std::cout << outF0 << " created\n";
			outF1 = "projYCuts_" + layers + "layers.png";
			outfileName1 = outPrefix + outF1;
			std::cout << outF1 << " created\n";
			
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


			int x=-10;	//x from file
			int y=-10;	//y from file
			int intensity=-10;	//pixle intensity from file
			int pNum=0;//the order in which the frame was processed

			//the 2d array which will store each frame of image data.
			int frame[480][640]={0};

			//variables
			int UNIXtime=0;
			float tdc[2]={-10,-10};

			//TTree *T = new TTree("T","TTree of muplus data");
			//add the 'branches' to the tree we will now read in
			chain->SetBranchAddress("pNum",&pNum);	//branch for the frame number
			chain->SetBranchAddress("frame",&frame);	//branch for frame data


			TH2I *frameHisto = new TH2I("frameHisto","Stacked Frames After Edge Cuts",width/4,0,width,height/4,0,height);	//histogram for the stacked images
			TH1I *chamber1 = new TH1I("chamber1","Chamber 1 After Edge Cuts",width/4,0,width);//histogram for chamber 1 (the top one)
			TH1I *chamber2 = new TH1I("chamber2","Chamber 2 After Edge Cuts",width/4,0,width);//histogram for chamber 2
			TH1I *chamber3 = new TH1I("chamber3","Chamber 3 After Edge Cuts",width/4,0,width);//histogram for chamber 3
			TH1I *chamber4 = new TH1I("chamber4","Chamber 4 After Edge Cuts",width/4,0,width);//histogram for chamber 4 (the bottom one)

			//loop over all data in chain
			Int_t nevent = chain->GetEntries();	//get the number of entries in the TChain
			for (Int_t i=0;i<nevent;i++) {
				chain->GetEntry(i);
				for(int x=0;x<width;x++){
					for(int y=0;y<height;y++){
						if(frame[x][y]>0){
							frameHisto->Fill(x,y,frame[x][y]);
							frameHistoAll->Fill(x,y,frame[x][y]);
							if(y>580 && y<610){
								chamber1->Fill(x,frame[x][y]);
								chamber1All->Fill(x,frame[x][y]);
							}
							else if(y>400 && y<440){
								chamber2->Fill(x,frame[x][y]);
								chamber2All->Fill(x,frame[x][y]);
							}
							else if(y>240 && y<280){
								chamber3->Fill(x,frame[x][y]);
								chamber3All->Fill(x,frame[x][y]);
							}
							else if(y>50 && y<100){
								chamber4->Fill(x,frame[x][y]);
								chamber4All->Fill(x,frame[x][y]);
							}

						}

					}
				}
				cout << "Stacking frame number " << pNum << "\r";//this overwrites the line every time
			}
			cout << endl;

			//output the plot of the stacked images
			TCanvas *pc2 = new TCanvas("pc2","Stacked Frames",0,0,600,800);
			pc2->cd();
			frameHisto->SetStats(false);
			frameHisto->Draw("colz");
			frameHisto->GetXaxis()->SetTitle("X position (px)");
			//frameHisto->GetXaxis()->SetTitleSize(0.055);
			//frameHisto->GetXaxis()->SetTitleOffset(1.0);
			//frameHisto->GetXaxis()->SetLabelSize(0.055);
			frameHisto->GetXaxis()->CenterTitle();

			frameHisto->GetYaxis()->SetTitle("Y position (px)");
			//frameHisto->GetYaxis()->SetTitleSize(0.055);
			//frameHisto->GetYaxis()->SetTitleOffset(0.9);
			//frameHisto->GetYaxis()->SetLabelSize(0.055);
			frameHisto->GetYaxis()->CenterTitle();
			gPad->Update();
//			pc2->Print("chamberStack.png");//output to a graphics file 

			//plot the projection onto the Y axis (so we can find our cuts in Y to select each chamber)
			TCanvas *projC = new TCanvas("projC","",0,0,800,600);
			projC->cd();
			TH1D *ydist = frameHisto->ProjectionY("ydist");
			ydist->Draw();
			ydist->GetYaxis()->SetTitle("Entries");
			ydist->GetYaxis()->CenterTitle();

			TCanvas *sliceX = new TCanvas("sliceX","",0,0,800,600);

			sliceX->Divide(2,2);
			
			sliceX->cd(1);
			chamber1->Draw();
			chamber1->GetXaxis()->SetTitle("X position (px)");
			chamber1->GetXaxis()->CenterTitle();
			chamber1->GetYaxis()->SetTitle("Y position (px)");
			chamber1->GetYaxis()->CenterTitle();
		//	chamber1->GetYaxis()->SetMaxDigits(2);
			
			sliceX->cd(2);
			chamber2->Draw();
			chamber2->GetXaxis()->SetTitle("X position (px)");
			chamber2->GetXaxis()->CenterTitle();
			chamber2->GetYaxis()->SetTitle("Y position (px)");
			chamber2->GetYaxis()->CenterTitle();
		//	chamber2->GetYaxis()->SetMaxDigits(2);
			
			sliceX->cd(3);
			chamber3->Draw();
			chamber3->GetXaxis()->SetTitle("X position (px)");
			chamber3->GetXaxis()->CenterTitle();
			chamber3->GetYaxis()->SetTitle("Y position (px)");
			chamber3->GetYaxis()->CenterTitle();
		//	chamber3->GetYaxis()->SetMaxDigits(2);
			
			sliceX->cd(4);
			chamber4->Draw();
			chamber4->GetXaxis()->SetTitle("X position (px)");
			chamber4->GetXaxis()->CenterTitle();
			chamber4->GetYaxis()->SetTitle("Y position (px)");
			chamber4->GetYaxis()->CenterTitle();
		//	chamber4->GetYaxis()->SetMaxDigits(2);

			gPad->Update();
			
			projC->Print(outfileName1.c_str());
			sliceX->Print(outfileName0.c_str());

			frameHisto->Reset();	
			chamber1->Reset();
			chamber2->Reset();
			chamber3->Reset();
			chamber4->Reset();
		} else break;
	}
	
	projCAll->cd();
	TH1D *ydistAll = frameHistoAll->ProjectionY("ydist");
	ydistAll->Draw();
	ydistAll->GetYaxis()->SetTitle("Entries");
	ydistAll->GetYaxis()->CenterTitle();
	
	TCanvas *sliceXAll = new TCanvas("sliceXAll","",0,0,800,600);

	sliceXAll->Divide(2,2);
	
	sliceXAll->cd(1);
	chamber1All->Draw();
	chamber1All->GetXaxis()->SetTitle("X position (px)");
	chamber1All->GetXaxis()->CenterTitle();
	chamber1All->GetYaxis()->SetTitle("Y position (px)");
	chamber1All->GetYaxis()->CenterTitle();
//	chamber1->GetYaxis()->SetMaxDigits(2);
	
	sliceXAll->cd(2);
	chamber2All->Draw();
	chamber2All->GetXaxis()->SetTitle("X position (px)");
	chamber2All->GetXaxis()->CenterTitle();
	chamber2All->GetYaxis()->SetTitle("Y position (px)");
	chamber2All->GetYaxis()->CenterTitle();
//	chamber2->GetYaxis()->SetMaxDigits(2);
	
	sliceXAll->cd(3);
	chamber3All->Draw();
	chamber3All->GetXaxis()->SetTitle("X position (px)");
	chamber3All->GetXaxis()->CenterTitle();
	chamber3All->GetYaxis()->SetTitle("Y position (px)");
	chamber3All->GetYaxis()->CenterTitle();
//	chamber3->GetYaxis()->SetMaxDigits(2);
	
	sliceXAll->cd(4);
	chamber4All->Draw();
	chamber4All->GetXaxis()->SetTitle("X position (px)");
	chamber4All->GetXaxis()->CenterTitle();
	chamber4All->GetYaxis()->SetTitle("Y position (px)");
	chamber4All->GetYaxis()->CenterTitle();
//	chamber4->GetYaxis()->SetMaxDigits(2);

	gPad->Update();
	
	projCAll->Print(outAllfileName1.c_str());
	sliceXAll->Print(outAllfileName0.c_str());
	
	pc2All->cd();
	frameHistoAll->SetStats(false);
	frameHistoAll->Draw("colz");
	frameHistoAll->GetXaxis()->SetTitle("X position (px)");
	//frameHisto->GetXaxis()->SetTitleSize(0.055);
	//frameHisto->GetXaxis()->SetTitleOffset(1.0);
	//frameHisto->GetXaxis()->SetLabelSize(0.055);
	frameHistoAll->GetXaxis()->CenterTitle();

	frameHistoAll->GetYaxis()->SetTitle("Y position (px)");
	//frameHisto->GetYaxis()->SetTitleSize(0.055);
	//frameHisto->GetYaxis()->SetTitleOffset(0.9);
	//frameHisto->GetYaxis()->SetLabelSize(0.055);
	frameHistoAll->GetYaxis()->CenterTitle();
	gPad->Update();

	return 0;  
}
