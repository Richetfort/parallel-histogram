#include <iostream>
#include <vector>
#include <random>
#include <omp.h>

class Histogram {
	public : 
		// Initialize a histogram with "nbins" uniform bins
		// Between "min" and "max
		Histogram(int nbins, double min, double max);

		//Print the histogram
		void print();

		//Increment the counter in the bin that "val" falls into
		//If val < min, count it separately as an "underflow"
		//If val > max, count it separately as an "overflow"
		void fill(double val){};
		void fillHist(std::size_t n, int seed = time(NULL)){};

		int counts(int binIndex) const;
		int underflows() const; // Return the number of values < min
		int overflows() const;  // Return the number of values > max

		int nbins;
		double min, max, step;

};

class SeqHistogram : public Histogram {

	public :

		//Contigous histogram data structure. Can be filled both sequentially or in parallel
		SeqHistogram(int nbins, double min, double max);

		void print();
		void fill(double val);
		void reserveBinsSize(std::size_t n);		
		void fillHist(std::size_t n, int seed = time(NULL));

		void PAR_reserveBinsSize(std::size_t n, int num_threads = 4);		
		void PAR_fillHist(std::size_t n, int num_threads = 4, int seed = time(NULL));
		
		int counts(int binIndex);
		int underflows(); // Return the number of values < min
		int overflows();  // Return the number of values > max
	
		~SeqHistogram();

		std::vector<double> * bins; //Bins represented by contigous std::vector<double>
		std::vector<double> underflow_bin;
		std::vector<double> overflow_bin;

};

class ParHistogram : Histogram {

	public : 
		
		//Non-contiguous histogram data structure. Filled in parallel

		ParHistogram(int nBins, double min, double max, int num_threads = 4);
		
		void print();
		
		void fill(double val,int idx);
		void fillHist(std::size_t n, int seed = time(NULL));
		
		int counts(int binIndex);
		int underflows(); // Return the number of values < min
		int overflows();  // Return the number of values > max

		~ParHistogram();

		int num_threads;
		SeqHistogram ** list_hist = (SeqHistogram **)NULL; //Non contigous instance containing the local histograms

};
