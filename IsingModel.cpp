#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include "parser.hpp"

int main(){
	std::ofstream f2("IsingSummary.dat");
	f2 << "t Jsign H T M(init) M(final) E \n";
	f2.close();
	instrxnParse(readfile("instructions.dat"));
}