#include "baseline.h"

/* given a sequence of contracts, find the length of the longest contract that ends before the actual_interruption */
double get_score_from_contracts(double* contracts, double actual_interruption)
{
    double sum = 0;
    int i = 0;
    while (sum <= actual_interruption)
    {
        sum += contracts[i];
        i++;
    }
    // contracts[i-1] causes sum exceed actual_prediction, and hence completes after deadline
    return contracts[i - 2];
}

/* given a base value x, create an exponential schedule <1, x, x^2, ...> and store it in "contracts". */
void get_exp_schedule(double base, double* contracts)
{
    double current_length = 1.0;

    for (int i = 0; i < MAX_BID_NO; i++)
    {
        contracts[i] = current_length;
        current_length *= base;
    }
}

/* given a base value for an exponential schedule and an actual interruption time, find the length of the largest contract
completed by the schedule before the interruption time ("conracts" is not a part of input and is just used for speedup) */
double get_expo_schedule_score(double base, double actual_interruption, double* contracts)
{
    get_exp_schedule(base, contracts);
    double score = get_score_from_contracts(contracts, actual_interruption);
    return score;
}

