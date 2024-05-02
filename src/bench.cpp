#include <iostream>
#include "utilities.hpp"
#include "histogram.hpp"

int main(int argc, char **argv){

	int nbins = 10, n_random = 10000000;
	double min = -4.0, max = 4.0;

	SeqHistogram * seq_hist = (SeqHistogram *)NULL; //
	ParHistogram * par_hist = (ParHistogram *)NULL;

	double start, ref_elapsed_time;

	seq_hist = new SeqHistogram(nbins,min,max);

	start = Tick();

	seq_hist->fillHist(n_random);
	
	ref_elapsed_time = ElapsedTime(start);
	
	std::cout << "Sequential filling elapsed time = " << ref_elapsed_time << "\n" << std::endl;

	delete seq_hist;

	int list_num_threads[4] = {2,4,8,16};
	double seq_elapsed_time[4];
	double par_elapsed_time[4];
	int num_runs = 10;

	for(int i = 0; i < 4; i++){

		for(int j = 0; j < num_runs; j++){

			seq_hist = new SeqHistogram(nbins,min,max);

			start = Tick();	
			seq_hist->PAR_fillHist(n_random,list_num_threads[i]);
			seq_elapsed_time[i] += ElapsedTime(start);

			delete seq_hist;
		}
		
		seq_elapsed_time[i] /= num_runs;

		for(int j = 0; j < num_runs; j++){

			par_hist = new ParHistogram(nbins,min,max,list_num_threads[i]);

			start = Tick();	
			par_hist->fillHist(n_random);
			par_elapsed_time[i] += ElapsedTime(start);

			delete par_hist;
		}

		par_elapsed_time[i] /= num_runs;

	}

	std::cout << "Sequential hist par-filling vs. Parallel hist filling" << std::endl;

	for(int i = 0; i < 4; i++){

		std::cout << "Speed-up with " << list_num_threads[i];
		if(list_num_threads[i] < 10){
			std::cout << " ";
		}
	       	std::cout << " threads = x" << ref_elapsed_time/seq_elapsed_time[i] << " vs. x" << ref_elapsed_time/par_elapsed_time[i] << std::endl;

	}

	return 0;
};

