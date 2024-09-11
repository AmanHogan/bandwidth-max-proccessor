#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/**
 * Author: Charles Garrett
 * Summary: Boilerplate code provided to get started for testing the maximum bandwtdh
 * on the UTA clusters
 */

void write_one(double *v, long len);
double read_sum(double *v, long len);

int main()
{
    long N = 100 * 1000 * 1000;
    double *v = malloc(N * sizeof(double));
    double t1 = omp_get_wtime();
    
    printf("Write\n");
    write_one(v, N);
    double t2 = omp_get_wtime();
    
    printf("Read\n");
    double sum = read_sum(v, N);
    double t3 = omp_get_wtime();
    
    printf("Sum = %f\n", sum);
    printf("Bandwidth write = %f MB/s\n", N * sizeof(double) / 1e6 / (t2 - t1));
    printf("Bandwidth read = %f MB/s\n", N * sizeof(double) / 1e6 / (t3 - t2));
    free(v);
    
    return 0;
}

void write_one(double *v, long len)
{
    for (long i = 0; i < len; i++) 
    {
        v[i] = 1.0;
    }
}

double read_sum(double *v, long len)
{
    double sum = 0.0;
    for (long i = 0; i < len; i++) 
    {
        sum += v[i];
    }
    return sum;
}
