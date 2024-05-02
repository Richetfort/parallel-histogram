#include "histogram.hpp"
#include "utilities.hpp"

Histogram::Histogram(int nbins, double min, double max){

	this->nbins = nbins;
	this->min  = min;
	this->max  = max;
	this->step = (max-min)/nbins;
};

void Histogram::print(){

	std::cout << "=== Histogram of " << this->nbins << " bins in range (" << this->min << "," << this->max << ") ===" << std::endl; 

};
