// auxiliarly functions used for contract scheduling experiments and generating output files
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

// given a double value, return a string with only two bit precision, e.g., when x = 12.3456, it returns 12.34
string double_two_digit_precision(double x); 

// given the values for advantage and strong advantage of the schedules with prediction, form the latex table with these values
void form_latex_adv_table(double* adv, double* strong_adv, double* buffer_ratios, int n_schedule, ofstream& ofile_adv);

