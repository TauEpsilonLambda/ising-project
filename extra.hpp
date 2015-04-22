#include <gsl/gsl_rng.h>
#include <vector>
#include <string>

#ifdef imageSuite
#include "imageProcessing.hpp"
#endif

using std::vector;
using std::string;
using std::size_t;

gsl_rng* rng = gsl_rng_alloc( gsl_rng_default );

//the pow function annoyingly only gives doubles. To avoid having to constantly recast to ints, I made my own function. Requires non-negative exponent.
int power(int base, unsigned int exponent) {
	if (exponent==0){return 1;}
	else {return base*power(base,exponent-1);}
}

//like PHP's explode, since C++ doesn't have such a function built in; I use this to parse the instruction file
	vector<string> explode(string input, string delim){
		vector<string> output;
		size_t pos;
		do{
			pos=input.find(delim);
			output.push_back(input.substr(0,pos));
			input.erase(0,pos+delim.length());
		}while(pos+1!=0);
		return output;
	}
	
	//the inverse of explode. implode didn't seem like a suitable name though...
	string concatenate(vector<string> pieces, string glue){
		string result="";
		for (unsigned int i=0; i<pieces.size(); i++){
			result.append(pieces[i]);
			result.append(glue);
		}
		result.erase(result.length()-glue.length());//remove the extra residue
		return result;
	}
	
	//strips whitespace
	void stripWS(string& text){
		size_t pos=0;
		do{
			if(isspace(text.at(pos))){text.erase(pos,1);}
			else{pos++;}
		}while(pos<text.length());
	}
	