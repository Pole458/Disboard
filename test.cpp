#include <omp.h>
#include <stdio.h>

int main()
{
    int var1, var2, var3;

    //Serial code executed by master thread

    #pragma omp parallel private(var1, var2) shared(var3) //openMP directive
    {
        // Parallel section executed by all threads
        printf("hello from %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
        // omp_get_thread_num() and omp_get_num_threads() are openMP routines
    }

    // Resume serial code executed by master thread
}