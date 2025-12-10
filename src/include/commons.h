#ifndef __COMMONS_H_
#define __COMMONS_H_

typedef double T; 
#define PI 3.14159265358979323846

T single_multiv_gaussian_pdf(T* x, int dim, T* means, T** cov_matrix);


#endif