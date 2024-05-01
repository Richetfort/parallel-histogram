#include <iostream>
#include <vector>
#include <random>
#include <omp.h>

class Histogram {
	public : 
		// Initialize a histogram with "nbins" uniform bins
		// Between "min" and "max
		Histogram(int nBins, double min, double max);

		//Print the histogram
		void print();

		//Increment the counter in the bin that "val" falls into
		//If val < min, count it separately as an "underflow"
		//If val > max, count it separately as an "overflow"
		void fill(double val);
		void fillHist(std::size_t n, int seed = time(NULL));
		void PAR_fillHist(std::size_t n);

		int counts(int binIndex);
		int underflows(); // Return the number of values < min
		int overflows();  // Return the number of values > max

		int nbins;
		double min, max, step;

		std::vector<double> * bins;
		std::vector<double> underflow_bin;
		std::vector<double> overflow_bin;

		//std::normal_distribution<double> law(0.0,1.0);
};
