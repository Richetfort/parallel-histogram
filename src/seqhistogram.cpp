#include "histogram.hpp"
#include "utilities.hpp"

SeqHistogram::SeqHistogram(int nbins, double min, double max) : Histogram(nbins,min,max) {

	this->bins = new std::vector<double>[nbins]();
};

void SeqHistogram::fill(double val){

	if(val < this->min){
		this->underflow_bin.push_back(val);
	}else if(val >= this->max){
		this->overflow_bin.push_back(val);
	}else{
		this->bins[(int)((val-this->min)/this->step)].push_back(val);
	}
};

void SeqHistogram::fillHist(std::size_t n, int seed){

	std::default_random_engine generator(seed);
	std::normal_distribution<double> distribution(0.0, 1.0);
	
	for(size_t i = 0; i < n; i++){
		this->fill(distribution(generator));
	}	

};

void SeqHistogram::PAR_fillHist(std::size_t n, int num_threads, int seed){

	omp_set_num_threads(num_threads);

	SeqHistogram ** local_hist = new SeqHistogram * [num_threads]();

	#pragma omp parallel
	{
		int rk = omp_get_thread_num();
		
		if(rk){
			local_hist[rk] = new SeqHistogram(this->nbins, this->min, this->max);
		}else{
			local_hist[0]  = this;
		}
		
		local_hist[rk]->fillHist(n/num_threads + (rk < (int)(n%num_threads)),seed+rk);
	}

	#pragma omp parallel
	{
		int size;
		
		#pragma omp for schedule(static)
		for(int i = 0; i < this->nbins+2; i++){
			if(i == this->nbins){
				size = this->underflows();
				for(int j = 1; j < num_threads; j++){
					size += local_hist[j]->underflows();
				}
				this->underflow_bin.reserve(size);
			}
			else if(i == this->nbins+1){
				size = this->overflows();
				for(int j = 1; j < num_threads; j++){
					size += local_hist[j]->overflows();
				}
				this->overflow_bin.reserve(size);
			}else{
				size = this->counts(i);
				for(int j = 1; j < num_threads; j++){
					size += local_hist[j]->counts(i);
				}
				this->bins[i].reserve(size);
			}
		}
		
		#pragma omp for schedule(dynamic)
		for(int i = 0; i < this->nbins+2; i++){
			if(i == this->nbins){
				for(int j = 1; j < num_threads; j++){
					this->underflow_bin.insert(this->underflow_bin.end(),local_hist[j]->underflow_bin.begin(),local_hist[j]->underflow_bin.end());
				}
			}
			else if(i == this->nbins+1){
				for(int j = 1; j < num_threads; j++){
					this->overflow_bin.insert(this->overflow_bin.end(),local_hist[j]->overflow_bin.begin(),local_hist[j]->overflow_bin.end());
				}
			}else{
				for(int j = 1; j < num_threads; j++){
					this->bins[i].insert(this->bins[i].end(),local_hist[j]->bins[i].begin(),local_hist[j]->bins[i].end());
				}
			}
		}
	}
	
	#pragma omp parallel for
	for(int j = 1; j < num_threads; j++){

		delete local_hist[j];
	
	};

	delete [] local_hist;
};

int SeqHistogram::counts(int idx){

	if(idx < 0){
		return this->underflows();
	}else if(idx > this->nbins){
		return this->overflows();
	}else{
		return this->bins[idx].size();
	}
};

int SeqHistogram::underflows(){

	return this->underflow_bin.size();

};

int SeqHistogram::overflows(){

	return this->overflow_bin.size();

};

void SeqHistogram::print(){

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

SeqHistogram::~SeqHistogram(){

	delete [] this->bins;

};
