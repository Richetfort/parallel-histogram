#include <omp.h>

inline double Tick(){
	
	return (double)omp_get_wtime();
};

inline double ElapsedTime(double start){

	return (double)omp_get_wtime() - start;
};


