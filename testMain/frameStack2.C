//This script takes the processed data, and shows the frames stacked on top of eachother
#include <TChain.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>

#include <iostream>

using namespace std;

int frameStack2(){
	TChain *hits = new TChain("fourChamTree");	//define the TChain (using the tree 'T' in the file)
	hits->Add("run0_0layers.root");	//add the first data file to the TChain
//	hits->Add("run0_2layers.root");
//	hits->Add("run0_0layers.root");

	const int width=480;	//width of the raw image
	const int height=640;	//height of the raw image


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
	hits->SetBranchAddress("pNum",&pNum);	//branch for the frame number
	hits->SetBranchAddress("frame",&frame);	//branch for frame data


	TH2I *frameHisto = new TH2I("frameHisto","Stacked Frames",width/4,0,width,height/4,0,height);	//histogram for the stacked images
	TH1I *chamber1 = new TH1I("chamber1","Chamber 1",width/4,0,width);//histogram for chamber 1 (the top one)
	TH1I *chamber2 = new TH1I("chamber2","Chamber 2",width/4,0,width);//histogram for chamber 2
	TH1I *chamber3 = new TH1I("chamber3","Chamber 3",width/4,0,width);//histogram for chamber 3
	TH1I *chamber4 = new TH1I("chamber4","Chamber 4",width/4,0,width);//histogram for chamber 4 (the bottom one)

	//loop over all data in chain
	Int_t nevent = hits->GetEntries();	//get the number of entries in the TChain
	for (Int_t i=0;i<nevent;i++) {
		hits->GetEntry(i);
		for(int x=0;x<width;x++){
			for(int y=0;y<height;y++){
				if(frame[x][y]>0){
					frameHisto->Fill(x,y,frame[x][y]);
					if(y>580 && y<610){
						chamber1->Fill(x,frame[x][y]);
					}
					else if(y>400 && y<440){
						chamber2->Fill(x,frame[x][y]);
					}
					else if(y>240 && y<280){
						chamber3->Fill(x,frame[x][y]);
					}
					else if(y>50 && y<100){
						chamber4->Fill(x,frame[x][y]);
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
	pc2->Print("chamberStack.png");//output to a graphics file 

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
	
	projC->Print("projC");
	sliceX->Print("sliceX.png");
	return 0;  
}
