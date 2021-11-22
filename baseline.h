/* the header file for computation of the exponential schedules (without prediction) 
that act as baseline schedules in our experiments */

#include "aux_methods.h"

#ifndef __max_bid_no_
#define __max_bid_no_
/* contract lengths increase exponentially, and the interruption occurs before 60 contracts. */
const int MAX_BID_NO = 60;
#endif

/* given a sequence of contracts, find the length of the longest contract that ends before the actual_interruption */
double get_score_from_contracts(double* contracts, double actual_interruption);

/* given a base value x, create an exponential schedule <1, x, x^2, ...> and store it in "contracts". */
void get_exp_schedule(double base, double* contracts);


/* given a base value for an exponential schedule and an actual interruption time, find the length of the largest contract
completed by the schedule before the interruption time ("conracts" is not a part of input and is just used for speedup) */
double get_expo_schedule_score(double base, double actual_interruption, double* contracts);
