#include <vector>
#include <list>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include "IsingMatrix.h"
#include "cavlib/constants.cc"

using std::vector;
using std::list;
using std::string;
using std::size_t;

	static int simnum=0; //used to keep count, for file enumeration
	const double pi = C::pi;
	//used to evaluate the values of time-varying fields/temperature at a particular time
	double evalFunc (short int func_type, vector<double> params, int t){
		double result=0;
		switch(func_type){
			case 1: //sinusoid; parameters: amplitude, period, phase
				result = params[0]*cos(2*pi*((t/params[1])+params[2]));
			break;
			case 2: //polynomial; params[n] is coefficient of t^n
				for (unsigned int i=0; i<params.size(); i++){
					result+= params[i]*pow(t,i);
				}
			break;
			case 3: //exponential; parameters: initial value, asymptotic value, time constant
				result = params[1]+ (params[0]-params[1])*(exp(params[2]*t));
			break;
			default:
				result = params[0];
				//if func_type=0, we'll just pass in a one-element vector
		}
		return result;
	}
	
	void runSimulation(double J, short int Htype, vector<double> H, short int Ttype, vector<double> T, short int layout_type, vector<int> layout, double M, int timesteps){
		static int D = layout.back(); layout.pop_back();
		static int N = layout.back(); layout.pop_back(); //popping N and D back off so we can initialise our lattice later
		static bool Jsign;
		if(J>=0){Jsign=true;}
		else {Jsign=false; J *=-1;}
		
		vector<double> Hrange;
		vector<double> Trange;
		if(Htype==0){
			double H0=H[0];
			if(H.size()==1){Hrange.push_back(H0);}
			else while(H0<H[1]){
				Hrange.push_back(H0);
				H0+=H[2];
			}
		}
		else {Hrange.push_back(0);}//in truth it doesn't matter what we stick here if Htype!=0, since we're only reading it if Htype==0; we just need AN element in here for the for loops to work
		if(Ttype==0){
			double T0=T[0];
			if(T.size()==1){Trange.push_back(T0);}
			else while(T0<T[1]){
				Trange.push_back(T0);
				T0+=H[2];
			}
		}
		else {Trange.push_back(0);}//second verse same as the first
		
		
		for(unsigned int i=0; i<Hrange.size(); i++){		//since if we have a range of H & T values...
			for (unsigned int j=0; j<Trange.size(); j++){	//we should run the simulation for EACH value!
				IsingMatrix lattice;
				if(layout_type==0){lattice = IsingMatrix(N,D,M);}
				else {lattice = IsingMatrix(N,D,layout);}
				//remember layout_type tells us if we already have spins predefined, or if we're just randomly assigning on creation
				
				
				if(Htype==0){H.clear(); H.push_back(Hrange[i]);}
				if(Ttype==0){T.clear(); T.push_back(Trange[i]);}
				//we gleaned the data from these vectors already so we can clear them; pushing Hrange[i] into H then allows us to call evalFunc even if Htype==0 (it'll just return the double from H, since H is constant!)
				char Filename1[24];
				sprintf(Filename1, "IsingData%d.dat", simnum);
				#ifdef imageSuite
					char Filename2[24];
					sprintf(Filename2, "IsingAnim%d.gif", simnum);
					char Filename3[24];
					sprintf(Filename3, "IsingInit%d.gif", simnum);
					char Filename4[24];
					sprintf(Filename4, "IsingLast%d.gif", simnum);
				#endif
				std::ofstream f(Filename1);

				double thisH;
				double thisT;
				f<< "t H T M E ; J>=0 == "<<Jsign<<"\n";//more for my benefit, so I don't mix up columns.
				#ifdef imageSuite
					list<Image> frames;
				#endif
				for (int k=0; k<timesteps; k++){
					thisH=evalFunc(Htype,H,k);
					thisT=evalFunc(Ttype,T,k);
					f<< k << " " << thisH/J << " " << thisT/J << " " << lattice.GetMagnetisation() << " " << lattice.GetEnergy(Jsign,thisH/J) << "\n";
					#ifdef imageSuite
						if(D==2){frames.push_back(createImage(N,lattice.GetSpins()));}
					#endif
					lattice.Timestep(Jsign,thisH/J,thisT/J,false,false);//feel free to change the latter two bools, but I feel this choice works best.
				}
				f<< timesteps << " " << thisH/J << " " << thisT/J << " " << lattice.GetMagnetisation() << " " << lattice.GetEnergy(Jsign,thisH/J) << "\n";
				f.close();
				#ifdef imageSuite
					if(D==2){
						frames.push_back(createImage(N,lattice.GetSpins()));
						printImageAnim(frames,Filename2);
						printImageStatic(frames.front(),Filename3);
						printImageStatic(frames.back(),Filename4);
					}
				#endif
				simnum++;
			}
		}
		
	}