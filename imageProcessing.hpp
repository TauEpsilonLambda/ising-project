#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <Magick++.h>

using namespace Magick;

using std::vector;
using std::list;
using std::string;

	//reads a static gif file, converts each pixel to a spin based on its lightness compared to a threshold, and outputs a vector of spins along with the lattice size and dimension (i.e. 2)
	vector<int> readImage(string filename, double threshold){
		Image input = Image(filename);
		int w = input.columns();
		int h = input.rows();
		int n = std::max(w,h);
		int d = 2;
		vector<int> spins;
		PixelPacket *pixels = input.getPixels(0, 0, w, h);
		for (int i=0; i<n; i++){
			for(int j=0; j<n; j++){
				if (i>=w||j>=h){spins.push_back(-1);}
				else{
					Color pixel = pixels[i*h+j];
					int R=pixel.redQuantum();
					int G=pixel.greenQuantum();
					int B=pixel.blueQuantum();
					//std::cout<<i<<" "<<j<<"\n";
					//std::cout<<R<<" "<<G<<" "<<B<<" "<<MaxRGB<<"\n";
					double Lightness = (std::max(R,std::max(G,B))+std::min(R,std::min(G,B)))*0.5/MaxRGB;
					//std::cout<<Lightness;
					//while(!std::cin.get()){}
					if (Lightness<threshold){spins.push_back(1);}
					else {spins.push_back(-1);}
				}
			}
		}
		spins.push_back(n);
		spins.push_back(d);
		return spins;
	}

	//generates a black and white image based on the spin states in a 2D lattice, which can then be printed out to a .gif file
	Image createImage (int n, vector<int> spins){
		Image canvas = Image(Geometry(n,n), "white");//white is down spin, black is up (as in readImage())
		for (int i=0; i<n; i++){
			for(int j=0; j<n; j++){
				if (spins[i+j*n]==1){canvas.pixelColor(i,j,Color("black"));}
			}
		}
		canvas.animationDelay(10);
		return canvas;
	}

	void printImageStatic (Image image, string filename){
		image.write(filename.c_str());
	}

	void printImageAnim(list<Image> frames, string filename){
		writeImages(frames.begin(),frames.end(),filename.c_str());
	}