#include <iostream>
#include "utilities.hpp"
#include "histogram.hpp"

int main(int argc, char **argv){

	int num_threads = 4;
	
	int nbins = 10, n_random = 100000;
	double min = -4.0, max = 4.0;

	SeqHistogram * hist = new SeqHistogram(nbins,min,max);

	double start = Tick();

	hist->fillHist(n_random);

	double seq_elapsed_time = ElapsedTime(start);

	hist->print();	
	
	delete hist;
	
	std::cout << "Sequential filling elapsed time = " << seq_elapsed_time << "\n" << std::endl;
	
	SeqHistogram * hist_seq_par = new SeqHistogram(nbins,min,max);

	start = Tick();

	hist_seq_par->PAR_fillHist(n_random,num_threads);
	
	double seq_par_elapsed_time = ElapsedTime(start);
	
	hist_seq_par->print();
	
	delete hist_seq_par;

	std::cout << "Parallel filling in Contiguous bins elapsed time    = " << seq_par_elapsed_time << "\n" << std::endl;

	std::cout << "-> Speed-up = " << seq_elapsed_time/seq_par_elapsed_time << "\n" << std::endl;
	
	ParHistogram * hist_par = new ParHistogram(nbins,min,max,num_threads);

	start = Tick();

	hist_par->fillHist(n_random);
	
	double par_elapsed_time = ElapsedTime(start);

	hist_par->print();
	
	delete hist_par;

	std::cout << "Parallel filling in Splited bins elapsed time   = " << par_elapsed_time << "\n" << std::endl;

	std::cout << "-> Speed-up = " << seq_elapsed_time/par_elapsed_time << "\n" << std::endl;

	return 0;
};

