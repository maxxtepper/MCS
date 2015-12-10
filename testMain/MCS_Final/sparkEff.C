//	mcs_sparkEff.C
//	checks the efficiency of each spark chamber within a frame

#include <TChain.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
void mcs_sparkEff_Mall(char* arg) {

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
			outF = "sparkEff_" + layers + "layers.root";
			outfileName = outPrefix + outF;
			TFile *outFile = new TFile(outfileName.c_str(), "RECREATE");
			std::cout << outF << " created\n";
			
			//load the input root files
			TChain *chain = new TChain("rawTree");

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
	
			//	create new tree
			TTree* sparkCheckT = new TTree("sparkCheckT", "checks for legitimate sparks");

			int count0 = 0;
			int count1 = 0;
			int count2 = 0;
			int count3 = 0;
			int count0_max = 0;
			int count1_max = 0;
			int count2_max = 0;
			int count3_max = 0;
			int pNum = 0;

			//	BRANCHES, yo
			sparkCheckT->Branch("pNum", &pNum, "pNum/I");	
			sparkCheckT->Branch("count0", &count0, "count0/I");	
			sparkCheckT->Branch("count1", &count1, "count1/I");	
			sparkCheckT->Branch("count2", &count2, "count2/I");	
			sparkCheckT->Branch("count3", &count3, "count3/I");	

			//	set variables
			int frame[480][640];
			int width = 480;
			int height = 640;

			//
			chain->SetBranchAddress("pNum", &pNum);
			chain->SetBranchAddress("frame", &frame);

			//
			Int_t nentries = chain->GetEntries();

			//
			for (Int_t i=0; i<nentries; i++) {
				chain->GetEntry(i);
				for (int x = 0; x<width; x++){
					for (int y = 0; y<height; y++){
						if(frame[x][y]>0){
							if(y>580 && y<610){
								count0++;
							}
							else if(y>400 && y<440){
								count1++;
							}
							else if(y>240 && y<280){
								count2++;
							}
							else if(y>50 && y<100){
								count3++;
							}
						}	

					}
				}
				//	end of image, fill branches
				sparkCheckT->Fill();
				cout << "Processing frame number: " << pNum << "\r";
				pNum++;
				//	check for the max
				if (count0_max < count0) {
					count0_max = count0;
				}
				if (count1_max < count1) {
					count1_max = count1;
				}
				if (count2_max < count2) {
					count2_max = count2;
				}
				if (count3_max < count3) {
					count3_max = count3;
				}
				//	reset for next image
				count0 = 0;
				count1 = 0;
				count2 = 0;
				count3 = 0;
			}
			cout << endl;
			cout << "count0_max = " << count0_max << endl;
			cout << "count1_max = " << count1_max << endl;
			cout << "count2_max = " << count2_max << endl;
			cout << "count3_max = " << count3_max << endl;
			//	make histos for chambers
			TH1I *chamber1 = new TH1I("chamber1","Chamber 1",count0_max,1,count0_max);//histogram for chamber 1 (the top one)
			TH1I *chamber2 = new TH1I("chamber2","Chamber 2",count1_max,1,count1_max);//histogram for chamber 2 (the top one)
			TH1I *chamber3 = new TH1I("chamber3","Chamber 3",count2_max,1,count2_max);//histogram for chamber 3 (the top one)
			TH1I *chamber4 = new TH1I("chamber4","Chamber 4",count3_max,1,count3_max);//histogram for chamber 4 (the top one)

			Int_t nevents = sparkCheckT->GetEntries();

			//	fill the histos
			for (Int_t i=0; i<nevents; i++) {
				sparkCheckT->GetEntry(i);
				chamber1->Fill(count0);
				chamber2->Fill(count1);
				chamber3->Fill(count2);
				chamber4->Fill(count3);
			}

			//	make and fill canvas
			TCanvas *sparkEff1 = new TCanvas("sparkEff1", "", 0, 0, 800, 600);
			sparkEff1->cd();
			chamber1->Draw();
			chamber1->GetXaxis()->SetTitle("Pixel Count");
			chamber1->GetXaxis()->CenterTitle();
			chamber1->GetYaxis()->SetTitle("Image count");
			chamber1->GetYaxis()->CenterTitle();

			TCanvas *sparkEff2 = new TCanvas("sparkEff2", "", 0, 0, 800, 600);
			sparkEff2->cd();
			chamber2->Draw();
			chamber2->GetXaxis()->SetTitle("Pixel Count");
			chamber2->GetXaxis()->CenterTitle();
			chamber2->GetYaxis()->SetTitle("Image count");
			chamber2->GetYaxis()->CenterTitle();

			TCanvas *sparkEff3 = new TCanvas("sparkEff3", "", 0, 0, 800, 600);
			sparkEff3->cd();
			chamber3->Draw();
			chamber3->GetXaxis()->SetTitle("Pixel Count");
			chamber3->GetXaxis()->CenterTitle();
			chamber3->GetYaxis()->SetTitle("Image count");
			chamber3->GetYaxis()->CenterTitle();

			TCanvas *sparkEff4 = new TCanvas("sparkEff4", "", 0, 0, 800, 600);
			sparkEff4->cd();
			chamber4->Draw();
			chamber4->GetXaxis()->SetTitle("Pixel Count");
			chamber4->GetXaxis()->CenterTitle();
			chamber4->GetYaxis()->SetTitle("Image count");
			chamber4->GetYaxis()->CenterTitle();	

			gPad->Update();

			sparkEff1->Print("sparkEff1.png");
			sparkEff2->Print("sparkEff2.png");
			sparkEff3->Print("sparkEff3.png");
			sparkEff4->Print("sparkEff4.png");
			sparkCheckT->Write();
			sparkEff1->Write();
			sparkEff2->Write();
			sparkEff3->Write();
			sparkEff4->Write();
		} else break;
	}
}
