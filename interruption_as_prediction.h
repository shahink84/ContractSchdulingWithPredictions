// the header file for experiments about schedules that receive interruption time as prediction.
#include "baseline.h"


/* given a sequence of contracts, scale them such that a contract ends exactly at the predicted interruption (tau) */
void shrink_contracts(double* contracts, double predicted_interruption);

/* Given a value of r >=4 for robustness, a "buffer_ratio" p \in (0,1), and a predicted value for the interruption time,
form the schedule of Lemma 1 (via scaling of exponential schedule with base br), and return the length of the largest
contract of this schedule when the interruption is "actual_interruption"
("contracts" is not a part of input and is just used for speedup) */
double get_scaling_alg_score(double r, double buffer_ratio, double predicted_interruption, double actual_interruption, double* contracts);


/* Returns the expectde cost of the algorithm that receives deadline as prediction.
r >= 4 is the robustness parameter. The actual interruption is give in "actual_interruption".
A random noise, whose distribution indicated in "noise_distribution" (0 is uniform, 1 is normal), has standard_deviation "noise_std_dev" (if normal)
and is upper bounded by "noise_upper_bound" (H), is repeatedly applied (1000 times) to actual_interruption to create
the predicted interruptions.  The algorithm has "buffer_ratio" p (in the range (0,1)).
contracts is not a part of input and is used only for speedup*/
double get_expected_cost_alg_prediction_is_interruption(double r, double actual_interruption, int noise_type, double noise_upper_bound, double noise_std_dev, double buffer_ratio, double* contracts);

/* run the algorithm that receives interruption time as prediction for different values of interruption time in the range (2, max_T)
For each interruption time, find the expected score of the algorithm and contrast it with the score of baseline algorithms.
The function reports the advantage and strong advantage of "n_schedules" with different buffer ratios stored in buffer_ration.
It also reports either the expected acceleration ratios (when "report_type" is 0) or the expected scores (when "report_type" is 1)
and for all schedules for each given actual_interruoptn_time.
n_points indicates the number of evenly-spaced points for the interruption time
*/
void test_prediction_is_interruption(double r, double noise_upper_bound, double* buffer_ratio,
    int n_schedules, int max_T, int noise_type, double noise_std_dev, ofstream& ofile_ratio, ofstream& ofile_score, ofstream& ofile_adv, int n_points);


