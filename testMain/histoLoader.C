#include <iostream>
#include <fstream>
#include <cassert>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <TChain.h>
#include <vector>
#include <TMath.h>
#include <TGraph.h>
#include <TF1.h>

Double_t moliFunc(Double_t);

void histoLoader(char* arg){
	
	//Take the arguments and save them into respective strings
	std::string infileName, outfileName;
	std::string inF, outF;
	std::string inPrefix, outPrefix;
	std::string layers;
	std::istringstream stm(arg);
	
	inPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/analysisRoot/";
	outPrefix = "/home/p180f/Do_Jo_Ol_Ma/Analysis/MainProcedure/testMain/moliRoot/";
	
	outF = "moli_all.root";
	outfileName = outPrefix + outF;

	int i = 0;
	Double_t thetaRMS[5];
	while(true){
		if( std::getline(stm, layers, ' ') ){
			inF        = "errorAnalysis_" + layers + "layers.root";
			infileName = inPrefix + inF;

			TFile *inFile = new TFile(infileName.c_str());
			TF1 *func = (TF1*)inFile->Get("gausFuncCuts");
			Double_t RMS1;
			Double_t RMS2;	
			RMS1 = TMath::Abs(func->GetParameter("Sigma1"));
			RMS2 = TMath::Abs(func->GetParameter("Sigma2"));
			std::cout << "RMS1_" << i << " = " << RMS1 << std::endl;
			std::cout << "RMS2_" << i << " = " << RMS2 << std::endl;
			if( RMS1 > RMS2 ){
				thetaRMS[i] = RMS2;
			} else thetaRMS[i] = RMS1;
			inFile->Close();
		}
		else break;
		i++;
	}
	
	TFile *outFile = new TFile(outfileName.c_str(), "RECREATE");

	Double_t x_data[5] = {0, 2.54 / 4, 2.54 / 2, 2.54*3/4, 2.54};
	Double_t x[20] = {0};
	Double_t y[20] = {0};
	int j=1;
	for (Double_t h=0.15; j<20; j++) {
		x[j] = h;
		y[j] = moliFunc(x[j]);
		h+=0.15;
		std::cout << "x[" << j << "] = " << x[j] << std::endl;
		std::cout << "y[" << j << "] = " << y[j] << std::endl;
	}

	TGraph *moliGraph = new TGraph(20, x, y);
	TGraph *dataGraph = new TGraph(5, x_data, thetaRMS);
	//datGraph->Fit("moliFunc");

	std::cout << "thetaRMS[0] = " << thetaRMS[0] << std::endl;
	std::cout << "thetaRMS[1] = " << thetaRMS[1] << std::endl;
	std::cout << "thetaRMS[2] = " << thetaRMS[2] << std::endl;
	std::cout << "thetaRMS[3] = " << thetaRMS[3] << std::endl;
	std::cout << "thetaRMS[4] = " << thetaRMS[4] << std::endl;
	
	moliGraph->Draw("AP");	
	dataGraph->Draw("*");	

	moliGraph->Write();	

	outFile->Close();	
}

Double_t moliFunc(Double_t x) {
	Double_t const m = 105.6583715;
	Double_t p  = 248.5;
	Double_t Bc = p / TMath::Sqrt((m*m + (p*p)));
	Double_t z  = 1;
	Double_t X0 = 0.56;
	/*
	par[0] = Bc (beta c- velocity of incident particle)
	par[1] = p (momentum of incident particle)
	par[2] = z (charge of incident particle)
	par[3] = X_0 (radiation length of target (cm))
	*/
	Double_t Val = ((13.6) / (Bc * p)) * z * TMath::Sqrt( x / X0) * ( 1 + 0.038 * TMath::Log( x / X0) )*180/3.14159265;
	
	return Val;
	
}
