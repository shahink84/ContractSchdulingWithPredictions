#include "interruption_as_prediction.h"

/* given a sequence of contracts, scale them such that a contract ends exactly at the predicted interruption (tau) */
void shrink_contracts(double* contracts, double predicted_interruption)
{
    double sum = 0;
    int i = 0;
    while (sum < predicted_interruption)
    {
        sum += contracts[i];
        i++;
    }
    double ratio = predicted_interruption / sum;
    for (i = 0; i < MAX_BID_NO; i++)
        contracts[i] *= ratio;
}

/* Given a value of r >=4 for robustness, a "buffer_ratio" p \in (0,1), and a predicted value for the interruption time,
form the schedule of Lemma 1 (via scaling of exponential schedule with base br), and return the length of the largest
contract of this schedule when the interruption is "actual_interruption"
("contracts" is not a part of input and is just used for speedup) */
double get_scaling_alg_score(double r, double buffer_ratio, double predicted_interruption, double actual_interruption, double* contracts)
{
    double br = (r + sqrt(r * r - 4 * r)) / 2;
    get_exp_schedule(br, contracts);
    double buffered_predicted_interruption = (round)((1 - buffer_ratio) * predicted_interruption);
    shrink_contracts(contracts, buffered_predicted_interruption);
    double score = get_score_from_contracts(contracts, actual_interruption);
    return score;
}

/* Returns the expectde cost of the algorithm that receives deadline as prediction.
r >= 4 is the robustness parameter. The actual interruption is give in "actual_interruption".
A random noise, whose distribution indicated in "noise_distribution" (0 is uniform, 1 is normal), has standard_deviation "noise_std_dev" (if normal)
and is upper bounded by "noise_upper_bound" (H), is repeatedly applied (1000 times) to actual_interruption to create
the predicted interruptions.  The algorithm has "buffer_ratio" p (in the range (0,1)).
contracts is not a part of input and is used only for speedup*/
double get_expected_cost_alg_prediction_is_interruption(double r, double actual_interruption, int noise_type, double noise_upper_bound, double noise_std_dev, double buffer_ratio, double* contracts)
{
    static default_random_engine generator; // used for generating random numbers with normal distribution

    normal_distribution<double> n_distribution(0, noise_std_dev); //only used if noise_type indicates a normal distribution

    int n_test = 1000;
    int predicted_u = -1;
    int buffered_predicted_u = -1;
    int score = -1;
    unsigned long int sum = 0;
    for (int test = 0; test < n_test; test++)
    {
        double noise_fct = -1;
        if (noise_type == 0) // uniform
            while (noise_fct < 1 - noise_upper_bound || noise_fct > 1 + noise_upper_bound)
                noise_fct = 2 * ((double)(rand()) / (RAND_MAX));
        else
            while (noise_fct < 1 - noise_upper_bound || noise_fct > 1 + noise_upper_bound)
                noise_fct = n_distribution(generator) + 1;

        predicted_u = (round)(actual_interruption / noise_fct);
        buffered_predicted_u = (round)((1 - buffer_ratio) * predicted_u);
        score = (int)(get_scaling_alg_score(r, buffer_ratio, predicted_u, actual_interruption, contracts));
        sum += score;
    }
    return sum / n_test;
}

/* run the algorithm that receives interruption time as prediction for different values of interruption time in the range (2, max_T)
For each interruption time, find the expected score of the algorithm and contrast it with the score of baseline algorithms.
The function reports the advantage and strong advantage of "n_schedules" with different buffer ratios stored in buffer_ration.
It also reports either the expected acceleration ratios (when "report_type" is 0) or the expected scores (when "report_type" is 1)
and for all schedules for each given actual_interruoptn_time.
n_points indicates the number of evenly-spaced points for the interruption time
*/
void test_prediction_is_interruption(double r, double noise_upper_bound, double* buffer_ratio,
    int n_schedules, int max_T, int noise_type, double noise_std_dev, ofstream& ofile_ratio, ofstream& ofile_score, ofstream& ofile_adv, int n_points)
{
    // setting baseline algorithms' parameters
    double br = (r + sqrt(r * r - 4 * r)) / 2;
    double bcr = 2;
    double cr = (r - sqrt(r * r - 4 * r)) / 2;

    double* adv_schedule = new double[n_schedules]; // the advantage over baseline
    double* strong_adv_schedule = new double[n_schedules]; // the 20% plus advantage over baseline
    int* score_alg = new int[n_schedules]; // score at each test
    for (int j = 0; j < n_schedules; j++)
        adv_schedule[j] = strong_adv_schedule[j] = score_alg[j] = 0;

    int exp_no = 0;
    double* contracts = new double[MAX_BID_NO];
    double step = (max_T - 1) / n_points;
    for (int interruption_time = 2; interruption_time < max_T; interruption_time = (int)(interruption_time + step))
    {
        cout << (double)(interruption_time) * 100 / max_T << "\r"; // just to keep 
        exp_no++;

        int score_base_1 = (int)(get_expo_schedule_score(br, interruption_time, contracts));
        int score_base_2 = (int)(get_expo_schedule_score(bcr, interruption_time, contracts));
        int score_base_3 = (int)(get_expo_schedule_score(cr, interruption_time, contracts));
        ofile_ratio << (int)interruption_time << "," << (double)(interruption_time) / score_base_1 << ","
            << (double)(interruption_time) / score_base_2 << "," << (double)(interruption_time) / score_base_3 << ",";
        ofile_score << (int)interruption_time << "," << score_base_1 << "," << score_base_2 << "," << score_base_3 << ",";

        for (int j = 0; j < n_schedules; j++)
        {
            score_alg[j] = (int)(get_expected_cost_alg_prediction_is_interruption(r, interruption_time, noise_type, noise_upper_bound, noise_std_dev, buffer_ratio[j], contracts));
            if (score_alg[j] > score_base_1) // we compare against the baseline that often works better
                adv_schedule[j] ++;
            if (score_alg[j] > 1.2 * score_base_1)
                strong_adv_schedule[j] ++;
            ofile_ratio << (double)(interruption_time) / score_alg[j] << ",";
            ofile_score << score_alg[j] << ",";
        }
        ofile_score << endl;
        ofile_ratio << endl;
    }


    for (int j = 0; j < n_schedules; j++)
    {
        adv_schedule[j] /= exp_no;
        strong_adv_schedule[j] /= exp_no;
    }
    form_latex_adv_table(adv_schedule, strong_adv_schedule, buffer_ratio, n_schedules, ofile_adv);

    delete[] adv_schedule;
    delete[] strong_adv_schedule;
    delete[] contracts;
    delete[] score_alg;
}


