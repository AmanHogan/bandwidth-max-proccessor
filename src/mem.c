#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <immintrin.h>
#include <string.h>

/**
 * Author: Aman Hogan-Bailey
 * MavId: 1001830469
 * Summary: Program tries to achieve maximum bandwidth on bell clusters
 * allocated by UTA.Performs writes and reads using various unroll loop sizes,
 * thread counts, and techniques to increase bandwidth
 */

void write_ones(double *vector, long len);
void write_ones_non_temporal(double *vector, long len);
double read_sum(double *vector, long len, int stride);
void write_zeros(double *vector, long len);
void write_zeros_non_temporal(double *vector, long len);

int main(int argc, char *argv[]) 
{
    // size of vector
    unsigned long long N = 1000ULL * 1000 * 1000; 
    double *vector = aligned_alloc(32, N * sizeof(double));
    double t1, t2, bandwidth_write;
    
    // numbers of threads for testing
    int num_threads = omp_get_max_threads();
    
    // unroll loop sizes for testing
    int unroll_loops[] = {1, 2, 4, 8, 16}; 
    
    // number of tests to perform (reads)
    int U = sizeof(unroll_loops) / sizeof(unroll_loops[0]);

    // write file pointer
    FILE *wfp = fopen("../output/write_results.csv", "a+");
    
    // read file pointer
    FILE *rfp = fopen("../output/read_results.csv", "a+"); 

    if (vector == NULL) 
    {  
        printf("Memory allocation failed\n");
        return 1;
    }
    
    if (wfp == NULL || rfp == NULL) 
    {
        printf("Failed to open file for writing\n");
        free(vector);
        return 1;
    }

    // Parse command-line arguments
    int verbose = 0;
    for (int i = 1; i < argc; i++) 
    {
        if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) 
        {
            verbose = atoi(argv[i + 1]);
            i++;  // Skip the next argument since it's the verbosity level
        }
    }

    if (verbose > 0) { printf("Running bandwidth test."); }

    /**
     * Performs no optimization on vector. 
     * Performs writes using various threads and logs bandwidth.
     */
    
    //for (int t = 0; t < T; t++) 
    fprintf(wfp, "No Optimization,%d", num_threads);
    omp_set_num_threads(num_threads);
    
    t1 = omp_get_wtime();
    write_ones(vector, N);
    t2 = omp_get_wtime();
    
    bandwidth_write = N * sizeof(double) / 1e6 / (t2 - t1);
    fprintf(wfp, ",%f", bandwidth_write);
    if (verbose > 0) { printf("No Optimization, Threads: %d, Bandwidth: %f MB/s\n", num_threads, bandwidth_write); }
    
    fprintf(wfp, "\n");

    /**
     * Sets vector to zero before timing.
     * Performs writes using various threads and logs bandwidth.
     */
    fprintf(wfp, "Set Mem to Zero Before Timing,%d", num_threads);
    //for (int t = 0; t < T; t++) 
    
    write_zeros(vector, N);
    omp_set_num_threads(num_threads);
    t1 = omp_get_wtime();
    write_ones(vector, N);
    t2 = omp_get_wtime();
    
    bandwidth_write = N * sizeof(double) / 1e6 / (t2 - t1);
    fprintf(wfp, ",%f", bandwidth_write);
    if (verbose > 0) { printf("Set Mem to Zero Before Timing, Threads: %d, Bandwidth: %f MB/s\n", num_threads, bandwidth_write); }

    fprintf(wfp, "\n");

    /**
     * Sets vector to zero before timing.
     * Performs non-temporal writes using various threads and logs bandwidth.
     */
    fprintf(wfp, "Non-Temporal Writes + Set Mem to Set Mem to Zero Before Timing,%d", num_threads);
    
    write_zeros_non_temporal(vector, N);
    omp_set_num_threads(num_threads);
    t1 = omp_get_wtime();
    write_ones_non_temporal(vector, N);
    t2 = omp_get_wtime();
    
    bandwidth_write = N * sizeof(double) / 1e6 / (t2 - t1);
    fprintf(wfp, ",%f", bandwidth_write);
    if (verbose > 0) { printf("Non-Temp + Mem Set, Threads: %d, Bandwidth: %f MB/s\n", num_threads, bandwidth_write); }

    fprintf(wfp, "\n");
    
    /**
     * Reads from vector.
     * Reads using various loop sizes and thread counts.
     * Logs bandwidth.
     */
    for (int u = 0; u < U; u++) 
    {
        fprintf(rfp, "%d", unroll_loops[u]);
        //for (int t = 0; t < T; t++) 
        {
            omp_set_num_threads(num_threads);
            t1 = omp_get_wtime();
            double sum = read_sum(vector, N, unroll_loops[u]);
            t2 = omp_get_wtime();
            
            double bandwidth_read = N * sizeof(double) / 1e6 / (t2 - t1);
            fprintf(rfp, ",%f", bandwidth_read);
            if (verbose > 0) { printf("Unroll Size %d, Threads: %d, Bandwidth: %f MB/s\n",unroll_loops[u], num_threads, bandwidth_read); }
        }
        fprintf(rfp, "\n");
    }

    fclose(wfp);
    fclose(rfp);
    free(vector);
    return 0;
}

/**
 * Writes ones to a vector using parallelism
 * @param vector a vector of type double
 * @param len length of vector
 */
void write_ones(double *vector, long len) 
{
    #pragma omp parallel for
    for (long i = 0; i < len; i++)
    {
        vector[i] = 1.0;
    }
}

/**
 * Writes ones to a vector non-temporally using parallelism.
 * Uses SSE and ACCX instruction to do the write. Handles remaining elements
 * @param vector a vector of type double
 * @param len length of vector
 */
void write_ones_non_temporal(double *vector, long len) 
{
    #pragma omp parallel for
    for (long i = 0; i < len - len % 4; i += 4)
    {
        _mm256_stream_pd(&vector[i], _mm256_set1_pd(1.0));
    }

    for (long i = len - len % 4; i < len; i++)
    {
        vector[i] = 1.0;
    }
    _mm_mfence();
}

/**
 * Writes zeros to a vector using parallelism
 * @param vector a vector of type double
 * @param len length of vector
 */
void write_zeros(double *vector, long len) 
{
    #pragma omp parallel for
    for (long i = 0; i < len; i++) 
    {
        vector[i] = 0.0;
    }
}

/**
 * Writes zeros to a vector non-temporally using parallelism.
 * Uses SSE and ACCX instruction to do the write. Handles remaining elements
 * @param vector a vector of type double
 * @param len length of vector
 */
void write_zeros_non_temporal(double *vector, long len) 
{
    #pragma omp parallel for
    for (long i = 0; i < len - len % 4; i += 4) 
    {
        _mm256_stream_pd(&vector[i], _mm256_set1_pd(0.0));
    }

    for (long i = len - len % 4; i < len; i++)
    {
        vector[i] = 0.0;
    }
    _mm_mfence();
}

/**
 * Reads vector using loop unrolling depending on loop unrolling factor.
 * Handles remaining elements.
 * @param vector a vector of type double
 * @param len length of vector
 * @param stride stride size to be used in loop unrolling
 */
double read_sum(double *vector, long len, int stride) 
{
    double sum = 0.0;
    #pragma omp parallel reduction(+:sum)
    {
        #pragma omp for
        for (long i = 0; i < len - len % stride; i += stride) 
        {
            for (int j = 0; j < stride; j++) 
            {
                sum += vector[i + j];
            }
        }

        for (long i = len - len % stride; i < len; i++) 
        {
            sum += vector[i];
        }
    }
    return sum;
}
