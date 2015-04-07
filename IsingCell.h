#include <vector>
#include <cmath>
#include <gsl/gsl_rng.h>

using std::vector;

class IsingCell
{
private:
	short int Spin;
	
public:
	//a vector containing each cell's neighbours
	//it needs to be public so IsingMatrix can access it for calculation of flipping energies
	vector<int> Neighbours;

	IsingCell(short int s)
	{
		Spin = s; //s SHOULD only be +1 or -1 for the basic model. It won't BREAK the model if not, but it WILL skew energy calculations. BUT for ferrimagnetism, we would WANT to use other values of s.
	}
	
	short int GetSpin() const { return Spin; }
	
	void addNeighbour(int id)
	{
		for (unsigned int i=0; i<Neighbours.size(); i++){
			if (Neighbours[i]==id){return;} //don't add a neighbour that already IS one!
		}
		Neighbours.push_back(id);
	}
	
	void removeNeighbour(int id) //shouldn't need this but mistakes happen...
	{
		for (unsigned int i=0; i<Neighbours.size(); i++){
			if (Neighbours[i]==id){Neighbours.erase(Neighbours.begin()+i);}
		}
	}
	
	void flip(double deltaE, double Temp)
	{
		gsl_rng* rng = gsl_rng_alloc( gsl_rng_default );
		//RHS will be > 1 anyway if deltaE < 0, so we don't actually need to compare deltaE; remember in our units, kB=1
		if (gsl_rng_uniform(rng) < exp(-deltaE/(Temp))){
			Spin *= -1;
		}
	}
};