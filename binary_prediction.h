// the header file for experiments about binary queries
#include "baseline.h"

// set the parameter d in Robust; see Theorem 4 in the paper for details
double get_d(double r, double p, int n);

// set all lengths for the i'th schedule and store them in X
void set_contracts(int i, double d, int n, double* X);

// the schedule with the best score is the one that the correct prediction points to (the index of such prediction is returned).
int get_schedule_with_best_score(double** X, int n, double actual_deadline);

/* given the best schedule (in unary in terms of NNNNNNYYYYYYYYYY), flip a fraction eta of bits, and output the number of No's
best_schedule is the index of the best schedule among the n schedules */
int get_noisy_prediction(int n, int best_schedule, double error_lower_bound, double error_upper_bound);

// p acts as buffer (the larger p, the more conservative the algorithm), r is the intended robustness, n is the number of schedules, eta is the error
double get_robust_expected_score(double actual_deadline, double p, double r, int n, double error_lower_bound, double error_upper_bound);
void test_different_deadlines_binary(double r, double noise_upper_bound, int n, double* buffer_ratio, int n_schedules, int max_T, ofstream& ofile_ratio, ofstream& ofile_score, ofstream& ofile_adv, int n_points);
