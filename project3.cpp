/**
 * CS 475 Project 3
 * Author: Jesse Thoren
 * Date: April 24, 2017
 * Description: A project to compare performance of different approaches
 *   to mitigate the negative performance impact of false sharing in
 *   a multi-threaded application.
 * References: web.engr.oregonstate.edu/~mjb/cs575/Projects/proj03.html
 *    stackoverflow.com/questions/4287531
 * **/

#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

//Run by default with 4 threads unless script overrides NUMT
#ifndef NUMT
#define NUMT    4
#endif

//The maximum number of NUMT tested in the test script
#ifndef MAXNUMT
#define MAXNUMT 8
#endif

//Run by default with NUMPAD 15 unless script overrides NUMPAD
#ifndef NUMPAD
#define NUMPAD  15
#endif

//Run by default with SomeBigNumber = 1000000 unless script overrides
#ifndef SomeBigNumber
#define SomeBigNumber 1000000
#endif

//Loop 10 times by default to compute an average run unless script overrides
#ifndef NUMTRIES
#define NUMTRIES    10
#endif

int main()
{
    struct s
    {
        float value;
        int pad[NUMPAD];
    } Array[MAXNUMT];

    //Initialize array values to 1
    //(Avoids patterns with -inf and inf)
    for(int i = 0; i < MAXNUMT; i++){
        Array[i].value = 1.;
    }

    //Generate SomeBigNumber random numbers
    //(This sidesteps the slowdown from rand in pragma omp parallel)
    srand(time(NULL));
    int randNums[SomeBigNumber];
    for(int i = 0; i< SomeBigNumber; i++)
        randNums[i] = (float)rand();

    //Confirm OpenMP is supported by current machine
    #ifndef _OPENMP
        fprintf(stderr, "OpenMP is not supported on this machine. \n");
        return 1;
    #endif

    //Inform user if NUMT exceeds omp_get_max_threads();
    if(NUMT > omp_get_max_threads())
    {
        fprintf(stderr, "Caution: NUMT specified exceeds omp_get_max_threads()\n");
        fprintf(stderr, "   NUMT: %d\nmaxNUMT: %d\n", NUMT, omp_get_max_threads());
        return 1;
    }   

    //Set Number of threads to calculate with
    omp_set_num_threads(NUMT);

    double maxMegaAdds = 0.;
    double sumMegaAdds = 0.;
    
    //Loop NUMTRIES times to form an average performance
    for(int times = 0; times < NUMTRIES; times++)
    {
        double time0 = omp_get_wtime();
        #pragma omp parallel for
        for( int i = 0; i < NUMT; i++ )
        {
            float tmp = Array[i].value;
            for(unsigned int j = 0; j < SomeBigNumber; j++)
            {
                //Array[i].value = Array[i].value + randNums[j];
                tmp = tmp + randNums[j];
            }
        }
        double time1 = omp_get_wtime();
        double megaAdds = (double)SomeBigNumber*NUMT/(time1-time0)/1000000.;
        if(maxMegaAdds < megaAdds)
            maxMegaAdds = megaAdds;
        sumMegaAdds += megaAdds;
    }
    double avgMegaAdds = sumMegaAdds/(double)NUMTRIES;
    fprintf(stderr, "Threads = %2d, NUMPAD = %2d\n", NUMT, NUMPAD);
    fprintf(stderr, "Max Performance: %8.2lf MegaAdds/sec\n",maxMegaAdds);
    fprintf(stderr, "Avg Performance: %8.2lf MegaAdds/sec\n\n",avgMegaAdds);

    return 0;
}
