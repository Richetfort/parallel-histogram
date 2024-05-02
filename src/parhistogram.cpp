#include "histogram.hpp"
#include "utilities.hpp"

ParHistogram::ParHistogram(int nbins, double min, double max, int num_threads) : Histogram(nbins,min,max) {

	this->num_threads = num_threads;
	this->list_hist = new SeqHistogram * [num_threads]();

	#pragma omp parallel for
	for(int i = 0; i < num_threads; i++){
		
		this->list_hist[i] = new SeqHistogram(nbins,min,max);

	}

};

void ParHistogram::fill(double val,int idx){

	if(val < this->min){
		this->list_hist[idx]->underflow_bin.push_back(val);
	}else if(val >= this->max){
		this->list_hist[idx]->overflow_bin.push_back(val);
	}else{
		this->list_hist[idx]->bins[(int)((val-this->min)/this->step)].push_back(val);
	}
};

void ParHistogram::fillHist(std::size_t n, int seed){

	omp_set_num_threads(this->num_threads);	

	#pragma omp parallel
	{
		int rk = omp_get_thread_num();
	
		std::default_random_engine generator(seed+rk);
		std::normal_distribution<double> distribution(0.0, 1.0);

		size_t size = n/num_threads + (rk < (int)(n%num_threads));
		for(size_t i = 0; i < size; i++){
			this->fill(distribution(generator),rk);
		}	
	}
};

int ParHistogram::counts(int idx){
	
	omp_set_num_threads(this->num_threads);	

	int * sizes = new int[this->num_threads];
	
	#pragma omp parallel for
	for(int i = 0; i < this->num_threads; i++){
		sizes[i] = this->list_hist[i]->counts(idx);
	}
	int count = 0;
	#pragma omp critical
	{
		for(int i = 0; i < this->num_threads; i++){
			count += sizes[i];
		}	
	}
	
	delete [] sizes;

	return count;
};

int ParHistogram::underflows(){

	omp_set_num_threads(this->num_threads);	

	int * sizes = new int[this->num_threads];
	
	#pragma omp parallel for
	for(int i = 0; i < this->num_threads; i++){
		sizes[i] = this->list_hist[i]->underflows();
	}
	int count = 0;
	#pragma omp critical
	{
		for(int i = 0; i < this->num_threads; i++){
			count += sizes[i];
		}	
	}
	
	delete [] sizes;

	return count;

};

int ParHistogram::overflows(){
	
	omp_set_num_threads(this->num_threads);	

	int * sizes = new int[this->num_threads];
	
	#pragma omp parallel for
	for(int i = 0; i < this->num_threads; i++){
		sizes[i] = this->list_hist[i]->overflows();
	}
	int count = 0;
	#pragma omp critical
	{
		for(int i = 0; i < this->num_threads; i++){
			count += sizes[i];
		}	
	}
	
	delete [] sizes;

	return count;

};

void ParHistogram::print(){

	Histogram::print();

	int total = 0;
	
	std::cout << "Un -> " << this->underflows() << std::endl;
	for(int i = 0; i < this->nbins; i++){

		total += this->counts(i);
		std::cout << i;
		if(i < 10){
			std::cout << "  -> ";
		}
		else if(i < 100){
			std::cout << " -> ";
		}
		else{
			std::cout << "-> ";
		}
		std::cout << this->counts(i) << std::endl;

	}
	std::cout << "Ov -> " << this->overflows()  << std::endl;
	std::cout << "============================================" << std::endl;
	std::cout << "TOTAL      = " << total + this->underflows() + this->overflows() << std::endl;

};

ParHistogram::~ParHistogram(){
	
	omp_set_num_threads(this->num_threads);	

	#pragma omp parallel for
	for(int i = 0; i < this->num_threads; i++){
		delete this->list_hist[i];
	}

	delete [] this->list_hist;

};
