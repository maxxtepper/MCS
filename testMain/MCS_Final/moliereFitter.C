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
#include <TLegend.h>
#include <TStyle.h>
#include <TGraphErrors.h>

Double_t moliFunc(Double_t, Double_t);
Double_t moliFunc_Fit(Double_t*, Double_t*);

void histoLoader_new(char* arg){
	
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

	Double_t p0 = 248.5;
	Double_t p1 = 222;
	Double_t p2 = 275;

	int i = 0;
	Double_t thetaRMS[4];
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

	Double_t x_data[4] = {2.54 / 4, 2.54 / 2, 2.54*3/4, 2.54};
	Double_t xerr[4]   = {0.005, 0.005, 0.005, 0.005};
	Double_t yerr[4]   = {0.197, 0.272, 0.351, 0.136};

	Double_t x0[20] = {0};
	Double_t y0[20] = {0};
	Double_t x1[20] = {0};
	Double_t y1[20] = {0};
	Double_t x2[20] = {0};
	Double_t y2[20] = {0};
	int j=1;
	for (Double_t h=0.15; j<20; j++) {
		x0[j] = h;
		y0[j] = moliFunc(x0[j], p0);
		h+=0.15;
		std::cout << "x0[" << j << "] = " << x0[j] << std::endl;
		std::cout << "y0[" << j << "] = " << y0[j] << std::endl;
	}
	int k=1;
	for (Double_t h=0.15; k<20; k++) {
		x1[k] = h;
		y1[k] = moliFunc(x1[k], p1);
		h+=0.15;
		std::cout << "x1[" << k << "] = " << x1[k] << std::endl;
		std::cout << "y1[" << k << "] = " << y1[k] << std::endl;
	}
	int l=1;
	for (Double_t h=0.15; l<20; l++) {
		x2[l] = h;
		y2[l] = moliFunc(x2[l], p2);
		h+=0.15;
		std::cout << "x2[" << l << "] = " << x2[l] << std::endl;
		std::cout << "y2[" << l << "] = " << y2[l] << std::endl;
	}
	
	TGraph *moliGraph0 = new TGraph(20, x0, y0);
	TGraph *moliGraph1 = new TGraph(20, x1, y1);
	TGraph *moliGraph2 = new TGraph(20, x2, y2);
	TGraphErrors *dataGraph = new TGraphErrors(4, x_data, thetaRMS, xerr, yerr);

	TF1 *moliFit = new TF1("moliFit", moliFunc_Fit, 0, 10, 1);
	moliFit->SetLineColor(6);
	moliFit->SetLineWidth(2);
	moliFit->SetParameters(0, 13.6);
	moliFit->SetParNames("Constant");
	moliFit->SetParLimits(0, 0, 30);
//	moliFit->SetParLimits(1, 0, 0.25);
	dataGraph->Fit("moliFit");

	std::cout << "thetaRMS[0] = " << thetaRMS[0] << std::endl;
	std::cout << "thetaRMS[1] = " << thetaRMS[1] << std::endl;
	std::cout << "thetaRMS[2] = " << thetaRMS[2] << std::endl;
	std::cout << "thetaRMS[3] = " << thetaRMS[3] << std::endl;
	//std::cout << "thetaRMS[4] = " << thetaRMS[4] << std::endl;

	moliGraph0->SetLineColor(2);
	moliGraph0->SetLineWidth(1.5);
	moliGraph1->SetLineColor(8);
	moliGraph1->SetLineWidth(1.5);
	moliGraph2->SetLineColor(9);
	moliGraph2->SetLineWidth(1.5);
	dataGraph ->SetMarkerStyle(22);		

	moliGraph0->Draw("AC");	
	moliGraph1->Draw("PC");	
	moliGraph2->Draw("PC");	
	dataGraph->Draw("P");
	//moliFit->Draw("*");	

	TLegend *leg = new TLegend(0.1, 0.7, 0.48, 0.9);
	leg->SetHeader("Legend");
//	gStyle->SetLegendFillColor(0);
	leg->AddEntry(moliGraph1, "Moliere Distribution for p = 222 MeV/c", "LP");
	leg->AddEntry(moliGraph0, "Moliere Distribution for p = 248.5 MeV/c", "LP");
	leg->AddEntry(moliGraph2, "Moliere Distribution for p = 275 MeV/c", "LP");
	leg->AddEntry(dataGraph, "The angular RMS for varying Pb thicknesses", "LP");
	leg->AddEntry(moliFit, "The angular RMS data fit to the Moliere Distribution", "LP");
	leg->Draw();

	moliGraph0->Write();	
	moliGraph1->Write();	
	moliGraph2->Write();	
	moliFit->Write();
	dataGraph->Write();
	
	outFile->Close();	
}

Double_t moliFunc(Double_t x, Double_t p) {
	Double_t const m = 105.6583715;
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
Double_t moliFunc_Fit(Double_t *x, Double_t *par) {
	Double_t const m = 105.6583715;
	Double_t p  = 248.5;
	Double_t Bc = p / TMath::Sqrt((m*m + (p*p)));
	Double_t z  = 1;
	Double_t X0 = 0.56;
	Double_t fitVal = ((par[0]) / (Bc * p)) * z * TMath::Sqrt( x[0] / X0) * ( 1 + 0.038 * TMath::Log( x[0] / X0) )*180/3.14159265;
	
	return fitVal;
	
}

