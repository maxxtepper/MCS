//Analysis program for MCS
//preliminary script for analyzing data
//honestly, probably more of an exercise for me to reinforce some root stuff
//-Joe

#include <iostream>
#include <fstream>
#include <cassert>
#include <TFile.h>
#include <TTree.h>
#include <stdio.h>
#include <string>
#include <sstream>


void analysisFile(char *arg){

	//Take the arguments and save them into respective strings
	std::string infileName, outfileName;
	std::istringstream stm(arg);
	if (std::getline(stm, infileName, ' ') && std::getline(stm, outfileName, ' '));
	std::cout<< "infile " << infileName<< " outfile " << outfileName << std::endl;


	// input file
	TFile *inFile = new TFile(infileName.c_str());
	TFile *outFile = new TFile(outfileName.c_str(),"RECREATE");

	// takin in the trees, yeah
	//probably don't need info from all of these trees but they're here in case we need them
	TTree *fourChamT = (TTree*)inFile->Get("fourChamTree");
	TTree *threeChamT = (TTree*)inFile->Get("threeChamTree");

	//define frame arrays
	int frame[480][640]={0};

	//define picture numbers for each tree
	// (in case they are needed)
	int pNum = 0;

	//get branch info from these guyz
	fourChamT->SetBranchAddress("pNum", &pNum);
	fourChamT->SetBranchAddress("frame", &frame_four);
	threeChamT->SetBranchAddress("pNum", &pNum_three);
	threeChamT->SetBranchAddress("frame", &frame_three);

	// new Trees???
	TTree *angleT = new TTree("angleTree","holds info for getting angles");

	//define analysis variables
	double ang[3] = {0};
	double x[4] = {0};
	double y[4] = {0};
	double x_int = 0;
	double y_int = 0;
	double m0 = 0;
	double m1 = 0;
	double b0 = 0;
	double b1 = 0;

	//branch time
	angleT->Branch("angles",&ang,"ang[3]/I");
	angleT->Branch("x", &x, "x[4]/I");
	angleT->Branch("y", &y, "y[4]/I");
	angleT->Branch("x_int", &x_int, "x_int/I");
	angleT->Branch("y_int", &y_int, "y_int/I");
	angleT->Branch("m0", &m0, "m0/I");
	angleT->Branch("m1", &m1, "m1/I");
	angleT->Branch("b0", &b0, "b0/I");
	angleT->Branch("b1", &b1, "b1/I");

	// things we need:
	// 	-algorithm to find the position (x_j,y_j)_i of a spark in the jth chamber during the ith event
	//	-algorithm to determine the slope of the line connecting the first and second chamber sparks and the slope for the
	//	 line connecting the third and fourth chambers
	//	-algorithm to alg the rithm's
	// 	-algorithm to do the sick ass trig to find the angles between the paths (get them scattering angles)
	//	-maybe code to determine the errors in the angles as well

	//	-Probably need a new tree with branches determined by the previous comments


}
