#include "histogram.hpp"
#include "utilities.hpp"

SeqHistogram::SeqHistogram(int nbins, double min, double max) : Histogram(nbins,min,max) {

	this->bins = new std::vector<double>[nbins]();
};

void SeqHistogram::reserveBinsSize(std::size_t n){
	//Reserve the size with an overapproximation of the normal distribution function by linear curves
	
	if(this->min < 0 && this->max > 0){

		for(int i = 0; i < (int)(this->nbins/2); i++){

			this->bins[i].reserve((int)(1.05*n*this->step*((this->min+(i+0.5)*this->step)/(-this->min*2.5)+0.4)));
			//std::cout << (int)(n*this->step*((this->min+(i+0.5)*this->step)/(-this->min*2.5)+0.4)) << std::endl;
		}
		
		for(int i = (int)(this->nbins/2); i < this->nbins; i++){

			this->bins[i].reserve((int)(1.05*n*this->step*((this->min+(i+0.5)*this->step)/(-this->max*2.5)+0.4)));
			//std::cout << (int)(n*this->step*((this->min+(i+0.5)*this->step)/(-this->max*2.5)+0.4)) << std::endl;
		}

	}else{
	
		for(int i = 0; i < this->nbins; i++){

			this->bins[i].reserve((int)n*0.4*this->step);
		}

	}

};

void SeqHistogram::PAR_reserveBinsSize(std::size_t n, int num_threads){
	//OMP version of the above method
	omp_set_num_threads(num_threads);

	if(this->min < 0 && this->max > 0){

		#pragma omp parallel for
		for(int i = 0; i < (int)(this->nbins/2); i++){

			this->bins[i].reserve((int)(1.05*n*this->step*((this->min+(i+0.5)*this->step)/(-this->min*2.5)+0.4)));
		}

		#pragma omp parallel for
		for(int i = (int)(this->nbins/2); i < this->nbins; i++){

			this->bins[i].reserve((int)(1.05*n*this->step*((this->min+(i+0.5)*this->step)/(-this->max*2.5)+0.4)));
		}

	}else{
	
		#pragma omp parallel for
		for(int i = 0; i < this->nbins; i++){

			this->bins[i].reserve((int)n*0.4);
		}

	}
};

void SeqHistogram::fill(double val){
	//Single value filling
	//Check underflow and overflow condition, else classify val in the proper bin
	if(val < this->min){
		this->underflow_bin.push_back(val);
	}else if(val >= this->max){
		this->overflow_bin.push_back(val);
	}else{
		this->bins[(int)((val-this->min)/this->step)].push_back(val);
	}
};

void SeqHistogram::fillHist(std::size_t n, int seed){

	//Sequential filling

	std::default_random_engine generator(seed);
	std::normal_distribution<double> distribution(0.0, 1.0);

	this->reserveBinsSize(n); //Bins space reservation

	for(size_t i = 0; i < n; i++){
		this->fill(distribution(generator));
	}	

};

void SeqHistogram::PAR_fillHist(std::size_t n, int num_threads, int seed){

	//Parallel filling in contiguous data structure
	omp_set_num_threads(num_threads);

	this->PAR_reserveBinsSize(n,num_threads); //OMP Bins space reservation

	SeqHistogram ** local_hist = new SeqHistogram * [num_threads]();
	
	//Parallel filling of independant distribution
	#pragma omp parallel
	{
		int rk = omp_get_thread_num();
		
		if(rk){
			local_hist[rk] = new SeqHistogram(this->nbins, this->min, this->max);
		}else{
			local_hist[0]  = this;
		}
				
		local_hist[rk]->fillHist(n/num_threads + (rk < (int)(n%num_threads)),seed+rk); //randomness must be unique to each thread
	}

	//Concatenation of bins : this is the bottleneck
	//The scheduling must be dynamic to improve the load balance between threads
	//Static can be used for uniform distribution only, here size of data copies are not equals
	#pragma omp parallel
	{
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
