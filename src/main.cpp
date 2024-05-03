#include <iostream>
#include "utilities.hpp"
#include "histogram.hpp"

int main(int argc, char **argv){

	int num_threads = 4;
	
	int nbins = 10, n_random = 100000;
	double min = -4.0, max = 4.0;

	SeqHistogram * hist = new SeqHistogram(nbins,min,max); //Sequential Histogram as reference

	double start = Tick();

	hist->fillHist(n_random);

	double seq_elapsed_time = ElapsedTime(start);

	hist->print();	//Check that n_random = 100000;
	
	delete hist;
	
	std::cout << "Sequential filling elapsed time = " << seq_elapsed_time << "\n" << std::endl; //Reference time
	
	// ================== Contigous Histogram Approach ====================
	
	SeqHistogram * hist_seq_par = new SeqHistogram(nbins,min,max); //Contiguous Histogram

	start = Tick();

	hist_seq_par->PAR_fillHist(n_random,num_threads); //Parallel Filling of the Contigious Histrogram
	
	double seq_par_elapsed_time = ElapsedTime(start);
	
	hist_seq_par->print(); //Check that n_random = 100000;
	
	delete hist_seq_par;

	std::cout << "Parallel filling in Contiguous bins elapsed time     = " << seq_par_elapsed_time << "\n" << std::endl; //Elapsed Time Contigous Histogram

	std::cout << "-> Speed-up = " << seq_elapsed_time/seq_par_elapsed_time << "\n" << std::endl; //Speed-up
												     
	// ================== Non-Contiguous Histogram Approach ====================
	
	ParHistogram * hist_par = new ParHistogram(nbins,min,max,num_threads);

	start = Tick();

	hist_par->fillHist(n_random); //Parallel Filling implicit to the structure
	
	double par_elapsed_time = ElapsedTime(start);

	hist_par->print();
	
	delete hist_par;

	std::cout << "Parallel filling in Non-Contiguous bins elapsed time = " << par_elapsed_time << "\n" << std::endl; //Elapsed Time Non-Contigous Histogram

	std::cout << "-> Speed-up = " << seq_elapsed_time/par_elapsed_time << "\n" << std::endl; //Speed-up

	return 0;
};

