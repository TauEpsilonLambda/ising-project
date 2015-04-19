#include <vector>
#include <iostream>
#include "IsingCell.hpp"

using std::vector;

class IsingMatrix
{
private:
	int N;
	int D;
	vector<IsingCell> Cells; //contains all the cells within the lattice
	
public:
	/* These first two constructors provide the least control to the user over initial layout;
	 * basically, specify a magnetisation, and the constructor will randomly generate spins accordingly
	 *
	 * n is side length, d is dimension
	 *   this generates a linear/square/cubic/tesseractic/hypercubic lattice, but you can tweak the
	 *   assignNeighbours() function to give other geometries; creating a graphene-like honeycomb in 2D, 
	 *   for example, is surprisingly simple even in this paradigm - just omit every other "horizontal"
	 *   adjacency, staggering the omissions between rows; note that the Ising model does NOT involve
	 *   length scales of any kind, so only the topology matters.
	 *
	 * T is temperature (Ising model typically only considers uniform temperature)
	 *   it can easily be adapted to be a vector if need be
	 *
	 * M is initial magnetisation, having factored out volume and moments of cells
	 *   i.e. it ranges from -1 to 1
	 *   Ising Cells are constructed with random spins so as to give net magnetisation equal to M (to 
	 *   within random fluctuations)
	 *   set |M|>=1 to have a wholly uniform initial layout (|M|>1 is just treated as |M|=1, 
	 *   preserving sign, since the RNG only generates values from 0 to 1)
	 */
	
	IsingMatrix()
	{}//empty constructor
	
	IsingMatrix(int n, int d) //if M unspecified, we assume M=0
	{
		N=n;
		D=d;
		for(int i=0; i<power(n,d); i++){
			short int s;
			if(gsl_rng_uniform(rng)*2 < 1){s=1;}
			else{s=-1;}
			IsingCell newCell = IsingCell(s);
			Cells.push_back(newCell);
		}
		assignNeighbours();
	}
	
	IsingMatrix(int n, int d, double M)
	{
		N=n;
		D=d;
		for(int i=0; i<power(n,d); i++){
			short int s;
			if(gsl_rng_uniform(rng)*2 < M+1){s=1;}
			else{s=-1;}
			IsingCell newCell = IsingCell(s);
			Cells.push_back(newCell);
		}
		assignNeighbours();
	}
	
	/* I provide this third constructor to allow the user to specify initial layout via their choice of:
	 *   1) vector (if too short, down spins will be appended)
	 *   2) text file (1, + or u representing up spin; 0, - or d representing down spin; other
	 *      characters ignored; again, if too short, down spins will be appended).
	 *   3) .gif image file (2D will be assumed, in this case; the Lightness component of each pixel (as
	 *      in HSL) will determine the spin, with the threshold being a parameter; down spins are
	 *      appended if image is non-square)
	 * 
	 * Vectors are generated from the latter two via parsers, so all three methods rely on just the one
	 * constructor.
	 *
	 * I provide the latter option since my program can also output .gif files (both animated, and 
	 * static), so if an interesting pattern shows up in my printouts, it can be easily re-used as a
	 * new initial layout.
	 * In addition, it can be easier, more intuitive, and less prone to error for the 2D case to simply
	 * draw your desired layout in a graphics program than to populate the required vector or text file.
	 * Also, because haven't you ever wondered how your face would ferromagnetically evolve?
	 */
	
	//this constructor can actually generate spins other than +1 or -1; this means it could be used to investigate ferrimagnetism
	IsingMatrix(int n, int d, vector<int> spins)
	{
		N=n;
		D=d;
		for(int i=0; i<power(n,d); i++){
			short int s;
			if ((unsigned int)i<spins.size()){s = spins[i];}
			else {s=-1;}
			IsingCell newCell = IsingCell(s);
			Cells.push_back(newCell);
		}
		assignNeighbours();
	}
	
	IsingMatrix& operator=( const IsingMatrix& m )
	{
		N = m.N;
		D = m.D;
		for(int i=0; i<power(N,D); i++){
			short int s = m.Cells[i].GetSpin();
			IsingCell newCell = IsingCell(s);
			Cells.push_back(newCell);
		}
		assignNeighbours();
		return *this;
	}
	
	//end of constructors
	
	
	void assignNeighbours()
	{
		for(unsigned int i=0; i<Cells.size(); i++){
			for(int j=0; j<D; j++){
				int coord = (i%power(N,j+1))/power(N,j); //determines the cell's position along the j-th dimension
				int l=i-power(N,j); //sets "left" and "right" nearest neighbours
				int r=i+power(N,j);
				if (coord==0){l+=power(N,j+1);} //BCs; in this case, we've programmed periodic BCs
				if (coord==N-1){r-=power(N,j+1);}
				Matchmake(i,l);
				Matchmake(i,r);
			}
		}
	}
	
	void Matchmake(int Romeo, int Juliet) //but soft, what light through yonder window breaks?
	{
		Cells[Romeo].addNeighbour(Juliet);
		Cells[Juliet].addNeighbour(Romeo);
	}
	
	/* I provide two methods of selecting spins to flip: systematically trying to flip each spin once 
	 * per timestep, and randomly trying to flip N^D spins.
	 * They sometimes produce starkly different results:
	 * for example, with a "checkerboard" type layout, systematic selection and simultaneous flipping
	 * just flips each spin every timestep in absence of H, whereas random selection actually evolves
	 * the system since some spins won't be selected and thus won't flip, thus destroying the
	 * checkerboard layout and introducing heterogeneity, the condiment of existence.
	 *
	 * I also provide two methods of flipping the spins: either all simultaneously, or each sequentially.
	 *   sel==TRUE if systematic selection, sel==FALSE if random selection
	 *   flip==TRUE if simultaneous flipping, flip==FALSE if sequential flipping
	 *
	 * I don't personally recommend sel==TRUE, flip==FALSE
	 * sel==FALSE, flip==TRUE is also problematic if a spin is selected more than once (which is more
	 * certain than death AND taxes)
	 * sel==FALSE, flip==FALSE probably is the most physical, but sel==TRUE, flip==TRUE isn't awful
	 */
	 
	void Timestep(bool Jsign, double H, double T, bool sel, bool flip)
	{ //I incorporate H and T as parameters here to allow for time-varying fields/temperatures, for example; again, we can make it a vector if we want spatial variation too. Note our choice of units set |J|, kB, and mu_elec all equal to 1 - so energy is measured in |J|, temperature in |J|/kB, field in |J|/mu_elec.
	//The bool Jsign governs whether or not to model antiferromagnetism (which requires J<0). TRUE corresponds to ferromagnetism (J>0).
		vector<double> deltaEnergies;
		short int J=1;
		if (!Jsign) {J*=-1;}
		for(unsigned int i=0; i<Cells.size(); i++){
			int n;
			if (sel){n=i;} else {n=gsl_rng_uniform_int(rng,Cells.size());}
			short int s=Cells[n].GetSpin();
			double deltaE = H * 2*s;
			for(unsigned int j=0; j<Cells[n].Neighbours.size(); j++){
				deltaE += J*Cells[Cells[n].Neighbours[j]].GetSpin() * 2*s;
			}
			if (flip){
				deltaEnergies.push_back(n);
				deltaEnergies.push_back(deltaE); 
			}
			else{
				Cells[n].flip(deltaE,T);
			}
		}
		if (flip){
			for(unsigned int i=0; i<Cells.size(); i++){
				Cells[deltaEnergies[2*i]].flip(deltaEnergies[2*i+1],T);
			}
		}
	}
	
	vector<int> GetSpins() const
	{
		vector<int> spins;
		for(unsigned int i=0; i<Cells.size(); i++){
			spins.push_back(Cells[i].GetSpin());
		}
		return spins;
	}
	
	double GetMagnetisation() const
	{
		double sigmaS = 0;
		for(unsigned int i=0; i<Cells.size(); i++){
			sigmaS += Cells[i].GetSpin();
		}
		return sigmaS/Cells.size();
	}
	
	double GetEnergy(bool Jsign, double H) const
	{
		short int J=1;
		if (!Jsign) {J*=-1;}
		double sigmaE = 0;
		for(unsigned int i=0; i<Cells.size(); i++){
			sigmaE += - H * Cells[i].GetSpin();
			for(unsigned int j=0; j<Cells[i].Neighbours.size(); j++){
				sigmaE += - .5 * J * Cells[i].GetSpin() * Cells[Cells[i].Neighbours[j]].GetSpin();
				//the 1/2 accounting for double counting, of course
			}
		}
		return sigmaE/Cells.size();
	}
};