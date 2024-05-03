#include <iostream>
#include "utilities.hpp"
#include "histogram.hpp"

int main(int argc, char **argv){

	//Benchmark of the two data structures implemented : Contiguous bins with parallel filling vs Non-Contigous bins with parallel filling
	
	int nbins = 10, n_random = 1000000;
	double min = -4.0, max = 4.0;

	SeqHistogram * seq_hist = (SeqHistogram *)NULL; //
	ParHistogram * par_hist = (ParHistogram *)NULL;

	double start, ref_elapsed_time;

	seq_hist = new SeqHistogram(nbins,min,max);

	start = Tick();

	seq_hist->fillHist(n_random);
	
	ref_elapsed_time = ElapsedTime(start); //Time reference
	
	std::cout << "Contigous bins with sequential filling elapsed time = " << ref_elapsed_time << "\n" << std::endl;

	delete seq_hist;

	int list_num_threads[5] = {2,4,8,16,28}; //Number of threads used in the benchmark

	double seq_elapsed_time[5] = {0};
	double par_elapsed_time[5] = {0};

	int num_runs = 10; //The speed-up measurement is averaged on 10 runs

	for(int i = 0; i < 5; i++){

		for(int j = 0; j < num_runs; j++){

			seq_hist = new SeqHistogram(nbins,min,max);

			start = Tick();	
			seq_hist->PAR_fillHist(n_random,list_num_threads[i]);
			seq_elapsed_time[i] += ElapsedTime(start);

			delete seq_hist;
		}
		
		seq_elapsed_time[i] /= num_runs; //Benchmark of the Contiguous bins with parallel filling

		for(int j = 0; j < num_runs; j++){

			par_hist = new ParHistogram(nbins,min,max,list_num_threads[i]);

			start = Tick();	
			par_hist->fillHist(n_random);
			par_elapsed_time[i] += ElapsedTime(start);

			delete par_hist;
		}

		par_elapsed_time[i] /= num_runs; //Benchmark of the Non-Contiguous bins with parallel filling

	}

	std::cout << "Contigious hist par-filling vs. Non-Contiguous hist filling" << std::endl;

	for(int i = 0; i < 5; i++){

		std::cout << "Speed-up with " << list_num_threads[i];
		if(list_num_threads[i] < 10){
			std::cout << " ";
		}
	       	std::cout << " threads = x" << ref_elapsed_time/seq_elapsed_time[i] << " vs. x" << ref_elapsed_time/par_elapsed_time[i] << std::endl;

	}

	return 0;
};

