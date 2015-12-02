//This script takes the processed data (in a ROOT file), and shows the frames one at a time
//it also shows x-axis hits for the individual chambers.

#include <TChain.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TTree.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TLine.h>

#include <iostream>

using namespace std;

void frameViewerAngles() {
	TChain *hits = new TChain("rawTree");	//define the TChain (using the tree 'T' in the file)
	hits->Add("run0_0layers.root");	//add the first data file to the TChain
	//hits.Add("file2.root");
	//hits.Add("file3.root");

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
	hits->SetBranchAddress("pNum",&pNum);	//branch for the frame number
	hits->SetBranchAddress("frame",&frame);	//branch for frame data


	TH2D *frameHisto = new TH2D("frameHisto","Frame",width,0,width,height,0,height);	//histogram for the stacked images

	TH1D *chamber1 = new TH1D("chamber1","Chamber 1",width,0,width);//histogram for chamber 1 (the top one)
	TH1D *chamber2 = new TH1D("chamber2","Chamber 2",width,0,width);//histogram for chamber 2
	TH1D *chamber3 = new TH1D("chamber3","Chamber 3",width,0,width);//histogram for chamber 3
	TH1D *chamber4 = new TH1D("chamber4","Chamber 4",width,0,width);//histogram for chamber 4 (the bottom one)
	
	TH1D *chamber1y = new TH1D("chamber1y","Chamber 1",height,0,height);//histogram for chamber 1 (the top one)
	TH1D *chamber2y = new TH1D("chamber2y","Chamber 2",height,0,height);//histogram for chamber 2
	TH1D *chamber3y = new TH1D("chamber3y","Chamber 3",height,0,height);//histogram for chamber 3
	TH1D *chamber4y = new TH1D("chamber4y","Chamber 4",height,0,height);//histogram for chamber 4 (the bottom one)

	//output the plot of the stacked images
	TCanvas *pc2 = new TCanvas("pc2","Frame",0,0,800,800);
	//pc2->SetGrayscale(); //if you want grayscale only

	pc2->cd();
	frameHisto->SetStats(false);
	frameHisto->GetXaxis()->SetTitle("X position (px)");
	//frameHisto->GetXaxis()->SetTitleSize(0.055);
	//frameHisto->GetXaxis()->SetTitleOffset(1.0);
	//frameHisto->GetXaxis()->SetLabelSize(0.055);
	frameHisto->GetXaxis()->CenterTitle();

	frameHisto->GetYaxis()->SetTitle("Y position (px)");
	//frameHisto->GetYaxis()->SetTitleSize(0.055);
	//frameHisto->GetYaxis()->SetTitleOffset(1.4);
	//frameHisto->GetYaxis()->SetLabelSize(0.055);
	frameHisto->GetYaxis()->CenterTitle();

	//loop over all data in chain
	Int_t nevent = hits->GetEntries();	//get the number of entries in the TChain
	for (Int_t i=0;i<nevent;i++) {
		hits->GetEntry(i);

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
		double m[2];
		double b[2];
		Double_t x_int[1];
		Double_t y_int[1];
		double ymin[2]={0,0};
		double xmin[2];
		double ymax[2]={640,640};
		double xmax[2];

		x12[0] = chamber1->GetMean();
		y12[0] = chamber1y->GetMean();			//593.3;
		x12[1] = chamber2->GetMean();
		y12[1] = chamber2y->GetMean();			//424.7;
		x34[0] = chamber3->GetMean();
		y34[0] = chamber3y->GetMean();			//262.5;
		x34[1] = chamber4->GetMean();
		y34[1] = chamber4y->GetMean();			//69.33;

		m[0] = (y12[1] - y12[0])/(x12[1] - x12[0]);
		m[1] = (y34[1] - y34[0])/(x34[1] - x34[0]);
		b[0] = y12[0] - (m[0] * x12[0]);
		b[1] = y34[0] - (m[1] * x34[0]);

		x_int[0] = ((b[1] - b[0]) / (m[0] - m[1]));
		y_int[0] = ((m[0] * x_int[0]) + b[0]);
	
		//find new x points from given ymin, ymax
		for(size_t i = 0; i < 2; ++i){
			xmin[i] = (ymin[i] - b[i]) / m[i];		
			xmax[i] = (ymax[i] - b[i]) / m[i];		
		}
	
		//cout << "Chamber1x: " << chamber1->GetMean() << endl;
		//cout << "Chamber1y: " << chamber1y->GetMean() << endl;
		//cout << "Chamber2x: " << chamber2->GetMean() << endl;
		//cout << "Chamber2y: " << chamber2y->GetMean() << endl;
		//cout << "Chamber3x: " << chamber3->GetMean() << endl;
		//cout << "Chamber3y: " << chamber3y->GetMean() << endl;
		//cout << "Chamber4x: " << chamber4->GetMean() << endl;
		//cout << "Chamber4y: " << chamber4y->GetMean() << endl;

		TLine *line0      = new TLine(xmin[0], ymin[0], xmax[0], ymax[0]);
		TLine *line1      = new TLine(xmin[1], ymin[1], xmax[1], ymax[1]);
		TGraph *intersect = new TGraph(1, x_int, y_int); 
		
		pc2->cd();
		frameHisto->Draw("COLZ");
		line0->Draw();
		line1->Draw();
		intersect->Draw("F1*");
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
		chamber1y->Reset();
		chamber2y->Reset();
		chamber3y->Reset();
		chamber4y->Reset();
	}
}
