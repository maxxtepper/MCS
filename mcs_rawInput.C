//This program takes the raw ASCII data recorded by the LabView computer in the MCS experiment
//and stores it in a ROOT tree for further processing.

#include <iostream>
#include <fstream>
#include <cassert>
#include <TFile.h>
#include <TTree.h>
#include <stdio.h>
#include <string>
#include <sstream>

void mcs_rawInput(char* arg){

	//Take the arguments and save them into respective strings
	std::string infileName, outfileName;
	std::istringstream stm(arg);
	if (std::getline(stm, infileName, ' ') && std::getline(stm, outfileName, ' '));
	else {};
	std::cout<< "infile " << infileName<< " outfile " << outfileName << std::endl;

	//open the raw data text file (if it is formatted correctly)
	ifstream fin;
	fin.open(infileName.c_str()); 

	//if the file can not be opened, fail and tell standard error
	assert(!fin.fail());

	//check that the output data file name ends in '.root'
	TFile *outfile;
	outfile = TFile::Open( outfileName.c_str(), "RECREATE" );	//make the file

	//the width and height of the image frames (in pixels)
	int width=480;
	int height=640;

	//data cuts (based on raw data)
	int width_cuts_upper=430;
	int width_cuts_lower=50;
	int pixel_cuts = 5;

	//the 2d array which will store each frame of image data.
	int frame[480][640]={0};
	int frame_cuts[480][640]={0};	
	int frame_four[480][640]={0};
	int frame_three[480][640]={0};
		
	//a counter of the order in which the frame was processed
	int pNum=0;
	
	//A counter for the number of pixels/chamber/frame
	int count[4] = {0};

	//the format of the input is:
	// y x intensity
	//with 0 0 255 the code for the start of an event
	int x=-10;
	int y=-10;
	int intensity=-10;

	//Create ROOT Trees
	TTree *rawT = new TTree("rawTree", "The raw data");
	TTree *cutsT = new TTree("cutsTree", "Data after pixel and edge cuts");
	TTree *fourChamT = new TTree("fourChamTree", "Frames with four chambers sparking (post-cuts)");
	TTree *threeChamT = new TTree("threeChamTree", "Frames with three chambers sparking (post-cuts)");

	//Add Branches to the Trees
	rawT->Branch("pNum", &pNum, "pNum/I");
	rawT->Branch("frame", &frame, "frame[480][640]/I");
	cutsT->Branch("pNum", &pNum, "pNum/I");
	cutsT->Branch("frame", &frame_cuts, "frame[480][640]/I"); 
	fourChamT->Branch("pNum", &pNum, "pNum/I");
	fourChamT->Branch("frame", &frame_four, "frame[480][640]/I"); 
	threeChamT->Branch("pNum", &pNum, "pNum/I");
	threeChamT->Branch("frame", &frame_three, "frame[480][640]/I"); 

	while (fin >> y >> x >> intensity){ //swap the axes (there is another transform below)
		//is this the start of an event?
		if (x==0 && y==0 && intensity==255){
			//if this is the beginning of the second event or after, store to the tree
			if (pNum>0){
				std::cout << "Processing frame number " << pNum << "\r";//this overwrites the line every time
				
				//Fill the raw data tree
				rawT->Fill();

				//Check each chamber, and make the pixel cuts
				if (count[0]<=pixel_cuts) {
					for (int y=580; y<610; y++) {
						for (int x=0; x<width; x++) {
							frame_cuts[x][y] = 0;
						}
					}
				}				
				if (count[1]<=pixel_cuts) {
					for (int y=400; y<440; y++) {
						for (int x=0; x<width; x++) {
							frame_cuts[x][y] = 0;
						}
					}
				}				
				if (count[2]<=pixel_cuts) {
					for (int y=240; y<280; y++) {
						for (int x=0; x<width; x++) {
							frame_cuts[x][y] = 0;
						}
					}
				}				
				if (count[3]<=pixel_cuts) {
					for (int y=50; y<100; y++) {
						for (int x=0; x<width; x++) {
							frame_cuts[x][y] = 0;
						}
					}
				}				

				//Fill the cuts data tree
				cutsT->Fill();

				//Find the number of chambers with legitamite sparks 
				int chamCount = 0;
				for (int i=0; i<4; i++) {
					chamCount += (count[i]>pixel_cuts);
				}

				//Fill the four and three spark chambers appropriately
				if (chamCount == 4) {
					fourChamT->Fill();
				}
				if (chamCount == 3) {
					threeChamT->Fill();
				}
			} 
			//now incriment the event counter
			pNum++;

			//finally, clear the frame arrays for the next entry in the branch
			for (int i=0; i<width;i++){
				for (int j=0;j<height;j++){
					frame[i][j] = 0;
					frame_cuts[i][j] = 0;
					frame_four[i][j] = 0;
					frame_three[i][j] = 0;
				}
			}
			//and clear the counter for the pixels
			for (int i=0; i<4; i++) {
				count[i] = 0;
			}
		}
		//is the data in range? if so, put the image data in the array
		else if (x>=0 && x <=480 && y>=0 && y<=640 && intensity >=0 && intensity <=256 && (x!=0 && y!=0 && intensity!=255)){
			//change from 'image' coordinates to plot coordinates
			x = 480-x;
			y = 640-y;
			frame[x][y]=intensity;
			if (x>=width_cuts_lower && x <=width_cuts_upper && y>=0 && y<=640 && intensity >=0 && intensity <=256 && (x!=0 && y!=0 && intensity!=255)){
				//Load the intensities into the frame arrays
				frame_cuts[x][y] = intensity;
				frame_four[x][y] = intensity;
				frame_three[x][y] = intensity;

				//Append the pixel count per chamber
				if (y>580 && y<610) count[0]++;
				else if (y>400 && y<440) count[1]++;
				else if (y>240 && y< 280) count[2]++;
				else if (y>50 && y<100) count[3]++;
			}
		}
		else {
			std::cout << "Unkonwn data line or data out of bounds" << std::endl;
		}
	}
	std::cout << "Processing complete" << std::endl;
	
	//Close the input file
	fin.close();
	
	//Write the trees to the ROOT file
	rawT->Write();
	cutsT->Write();
	fourChamT->Write();
	threeChamT->Write();
	
	//Close the ROOT file
	outfile->Close();

}
