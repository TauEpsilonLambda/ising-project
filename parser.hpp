#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "simulator.hpp"
using std::vector;
using std::string;
using std::size_t;


	
	vector<string> readfile(string filename){
		std::ifstream f(filename.c_str());
		vector<string> data;
		string stuff;
		while(getline(f,stuff,'\n')){
			data.push_back(stuff);
		};
		f.close();
		return data;
	}
	
	vector<int> layoutParse(vector<string> data){
		if(data[0].substr(0,7)=="LAYOUT:"){
			vector<string> params = explode(data[0].substr(7),":");
			int N = atoi(params[0].c_str());
			int D = atoi(params[1].c_str());
			data.erase(data.begin());//we don't need the header any more
			string layout=concatenate(data,"");//lets us strip whitespace all in one fell swoop
			stripWS(layout);
			vector<int> spins;
			for (unsigned int i=0; i<layout.length(); i++){
				char c = layout.at(i);
				if (c=='1'||c=='+'||c=='u'){spins.push_back(1);}
				else if (c=='0'||c=='-'||c=='d'){spins.push_back(-1);}
				//other characters are ignored!
			}
			spins.push_back(N);
			spins.push_back(D);//they'll be popped back off; it's just a pain to return a vector AND two ints.
			return spins;
		}
		else {
			std::cout<<"Invalid layout file passed to parser.\nPlease consult readme file for proper syntax.\nPress any key to exit program.";
			while(!std::cin.get()){}
			exit(EXIT_FAILURE);
		}
	}
	
	void instrxnParse(vector<string> data){
		if(data[0].substr(0,7)=="INSTRXN"){
			stripWS(data[1]);
			vector<string> instrxn = explode(data[1],";");
			double J = atof(instrxn[0].c_str());
			vector<string> temp; //temp for temporary, not temp for temperature
			short int Htype;
			switch((int) instrxn[1].at(0)){//switch requires ints for its cases; casting to an int gives the ascii value
				case 83:	//s
				case 115: //I may be a sensitive guy, but I'm not case sensitive!
					Htype=1;
					instrxn[1].erase(0,2);//trims off identifier once we've got it
					break;
				case 80:	//p
				case 112:
					Htype=2;
					instrxn[1].erase(0,2);
					break;
				case 69:	//e
				case 101:
					Htype=3;
					instrxn[1].erase(0,2);
					break;
				default:
					Htype=0;
			}
			temp = explode(instrxn[1],":");
			vector<double> H;
			for (unsigned int i=0; i<temp.size(); i++){
				H.push_back(atof(temp[i].c_str()));
			}
			short int Ttype;
			switch((int) instrxn[2].at(0)){
				case 83:	//s
				case 115:
					Ttype=1;
					instrxn[2].erase(0,2);//trims off identifier once we've got it
					break;
				case 80:	//p
				case 112:
					Ttype=2;
					instrxn[2].erase(0,2);
					break;
				case 69:	//e
				case 101:
					Ttype=3;
					instrxn[2].erase(0,2);
					break;
				default:
					Ttype=0;
			}
			temp = explode(instrxn[2],":");
			vector<double> T;
			for (unsigned int i=0; i<temp.size(); i++){
				T.push_back(atof(temp[i].c_str()));
			}
			vector<int> layout;
			short int layout_type;
			double M=0;
			switch((int) instrxn[3].at(0)){
				#ifdef imageSuite
				case 73:	//i
				case 105:
					instrxn[3].erase(0,2);
					temp = explode(instrxn[3],":");
					double threshold;
					if (temp.size()==1){threshold=0.5;}
					else {threshold=atof(temp[1].c_str());}
					layout = readImage(temp[0],threshold);//Can change 0.5 if you like depending on the white balance of your image or personal taste. After all, one man's black and blue is another's white and gold.
					layout_type=1;
					break;
				#endif
				case 84:	//t
				case 116:
					instrxn[3].erase(0,2);
					layout = layoutParse(readfile(instrxn[3]));
					layout_type=1;
					break;
				default:
					layout_type=0;
					temp = explode(instrxn[3],":");
					for (unsigned int i=0; i<2; i++){
						layout.push_back(atoi(temp[i].c_str()));
					}
					M=atof(temp[2].c_str());
			}
			int timesteps=atoi(instrxn[4].c_str());
			//Now we've extracted all the parameters, converted them to their respective types...
			//Let's get dangerous and run the simulation!
			runSimulation(J,Htype,H,Ttype,T,layout_type,layout,M,timesteps); //remember N and D are contained in layout.
			
			data.erase(data.begin()+1);//removes the line we just parsed
			if(data.size()>1){         //if there IS a next line (remember to keep the header)...
				instrxnParse(data);    //... then we process that line! et cetera ad infinitum... or ad nauseam, which is probably more likely, or ad BSOD, which is yet more likely still
			}
			else{return;}
		}
		else {
			std::cout<<"Invalid instruction file passed to parser.\nPlease consult readme file for proper syntax.\nPress any key to exit program.";
			while(!std::cin.get()){}
			exit(EXIT_FAILURE);
		}
	}