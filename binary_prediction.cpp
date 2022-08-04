#include "binary_prediction.h"

// set the parameter d in Robust; see Theorem 4 in the paper for details
double get_d(double r, double p, int n)
{
    double br = (r + sqrt(r * r - 4 * r)) / 2;
    double K = (double) n /(2 * p * n + 1) ; // (2 * p * n + 1) / n; // fixed July 2022
    double res = -1;
    if (r <= (1 + K) * (1 + K) / K)
        res = br;
    else
        res = 1 + K;
    return res;
}

// set all lengths for the i'th schedule and store them in X
void set_contracts(int i, double d, int n, double* X)
{
    for (int j = 0; j < MAX_BID_NO; j++)
    {
        double expo = j + (double)(i) / n;
        X[j] = pow(d, expo);
    }
}

// the schedule with the best score is the one that the correct prediction points to (the index of such prediction is returned).
int get_schedule_with_best_score(double** X, int n, double actual_deadline)
{
    double max_score = -1;
    int res = -1;
    for (int i = 0; i < n; i++)
    {
        double my_score = get_score_from_contracts(X[i], actual_deadline);
        if (my_score > max_score)
        {
            max_score = my_score;
            res = i;
        }
    }
    return res;
}

/* given the best schedule (in unary in terms of NNNNNNYYYYYYYYYY), flip a fraction eta of bits, and output the number of No's
best_schedule is the index of the best schedule among the n schedules */
int get_noisy_prediction(int n, int best_schedule, double error_lower_bound, double error_upper_bound)
{
    int* answers = new int[n];
    for (int i = 0; i < best_schedule; i++)
        answers[i] = 0;
    for (int i = best_schedule; i < n; i++)
        answers[i] = 1;

    int* already_flipped = new int[n]; // used to avoid double-flipping
    for (int i = 0; i < n; i++)
        already_flipped[i] = 0;

    double eta = (double)(rand()) / RAND_MAX; // between 0 and 1
    eta *= (error_upper_bound - error_lower_bound); // between 0 and ub-lb
    eta += error_lower_bound;

    int no_flipped_bits = (int)(eta * n);

    for (int j = 0; j < no_flipped_bits; j++)
    {
        int indx = rand() % n; // the index to flip
        while (already_flipped[indx] == 1)
            indx = rand() % n;

        answers[indx] = 1 - answers[indx]; // flip
        already_flipped[indx] = 1;
    }
    
    int no_No_s = 0; // number of No's 
    for (int i = 0; i < n; i++)
        no_No_s += (1 - answers[i]);

    delete[] answers;
    return no_No_s;
}

// p acts as buffer (the larger p, the more conservative the algorithm), r is the intended robustness, n is the number of schedules, eta is the error
double get_robust_expected_score(double actual_deadline, double p, double r, int n, double error_lower_bound, double error_upper_bound)
{
    double d = get_d(r, p, n);

    double** X = new double* [n];
    for (int i = 0; i < n; i++)
        X[i] = new double[MAX_BID_NO];

    for (int i = 0; i < n; i++)
        set_contracts(i, d, n, X[i]);

    int best_schedule = get_schedule_with_best_score(X, n, actual_deadline);

    int n_tests = 1000; // expectation is taken over 1000 tests
    double sum_scores = 0;
    for (int t = 0; t < n_tests; t++)
    {
        int N = get_noisy_prediction(n, best_schedule, error_lower_bound, error_upper_bound); // predicted number of No's
        int selected_schedule = (int)(N - 1 - p * n);
        if (selected_schedule < 0)
            selected_schedule += n;
        double selected_schedule_score = get_score_from_contracts(X[selected_schedule], actual_deadline);
        sum_scores += selected_schedule_score;
    }

    double avg_score = sum_scores / n_tests;
    for (int i = 0; i < n; i++)
        delete[] X[i];
    delete[] X;
    return avg_score;
}

void test_different_deadlines_binary(double r, double noise_upper_bound, int n, double* buffer_ratio, int n_schedules, int max_T, ofstream& ofile_ratio, ofstream& ofile_score, ofstream& ofile_adv, int n_points)
{
    double br = (r + sqrt(r * r - 4 * r)) / 2;
    double cr = (r - sqrt(r * r - 4 * r)) / 2;
    double bcr = 2; // r / 2;

    double* adv_schedule = new double[n_schedules]; // the advantage over baseline
    double* strong_adv_schedule = new double[n_schedules]; // the 20% plus advantage over baseline
    int* score_alg = new int[n_schedules]; // score at each test
    for (int j = 0; j < n_schedules; j++)
        adv_schedule[j] = strong_adv_schedule[j] = score_alg[j] = 0;

    double* contracts = new double[MAX_BID_NO];
    int exp_no = 0;
    double step = (max_T - 1) / n_points;
    for (int interruption_time = 2; interruption_time < max_T; interruption_time = (int)(interruption_time + step)) // u = (int)(real_u * inc_ratio)) // += 1000) // *= 1.02)// += 100000)
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
            score_alg[j] = (int)(get_robust_expected_score(interruption_time, buffer_ratio[j], r, n, 0, noise_upper_bound));
            if (score_alg[j] > score_base_1)
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


