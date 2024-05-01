#include <iostream>
#include "utilities.hpp"
#include "histogram.hpp"

int main(int argc, char **argv){

	int nbins = 12, n_random = 10000000;
	double min = -4.0, max = 4.0;

	Histogram * hist = new Histogram(nbins,min,max);

	double start = Tick();

	hist->fillHist(n_random);

	double seq_elapsed_time = ElapsedTime(start);
	
	hist->print();	
	
	std::cout << "Sequential filling elapsed time = " << seq_elapsed_time << "\n" << std::endl;
	
	Histogram * hist_par = new Histogram(nbins,min,max);

	start = Tick();

	hist_par->PAR_fillHist(n_random);
	
	double par_elapsed_time = ElapsedTime(start);

	hist_par->print();

	std::cout << "Parallel filling elapsed time   = " << par_elapsed_time << "\n" << std::endl;

	std::cout << "-> Speed-up = " << seq_elapsed_time/par_elapsed_time << "\n" << std::endl;

	return 0;
};

