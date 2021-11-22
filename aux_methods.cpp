#include "aux_methods.h"

// given a double value, return a string with only two bit precision, e.g., when x = 12.3456, it returns 12.34
string double_two_digit_precision(double x)
{
    int floor = (int)(x); // int component
    double real_component = (x - floor) * 100; //real_component
    string str = to_string(floor);
    if (real_component < 10)
        str += ".0" + to_string(real_component).substr(0, 1);
    else
        str += "." + to_string(real_component).substr(0, 2);
    return str;
}

// given the values for advantage and strong advantage of the schedules with prediction, form the latex table with these values
void form_latex_adv_table(double* adv, double* strong_adv, double* buffer_ratios, int n_schedule, ofstream& ofile_adv)
{

    ofile_adv << "\\begin{table} [!h] \n\t\\centering \n\t\\scalebox{0.85}{\n\t\\begin{tabular}{ | p{ 0.3\\columnwidth } | p{ 0.17\\columnwidth } | p{ 0.17\\columnwidth } | p{ 0.15\\columnwidth } | p{ 0.15\\columnwidth } | }\n\t\t\\hline \n\t\t&";
    for (int i = 0; i < n_schedule; i++)
    {
        ofile_adv << " $p =" << buffer_ratios[i] << "$ ";
        if (i < n_schedule - 1)
            ofile_adv << "&";
        else
            ofile_adv << "\\\\%";
    }
    ofile_adv << "\n\t\t\\hline \\hline";

    ofile_adv << "\n\t\timprovement & ";
    for (int i = 0; i < n_schedule; i++)
    {
        ofile_adv << " " << double_two_digit_precision(adv[i] * 100) << "\\%";
        if (i < n_schedule - 1)
            ofile_adv << "&";
        else
            ofile_adv << "\\\\%";
    }
    ofile_adv << "\n\t\t\\hline";

    ofile_adv << "\n\t\tstrong improv. & ";
    for (int i = 0; i < n_schedule; i++)
    {
        ofile_adv << double_two_digit_precision(strong_adv[i] * 100) << "\\%"; // write the percentages with two digit precision
        if (i < n_schedule - 1)
            ofile_adv << " & ";
        else
            ofile_adv << "\\\\%";
    }
    ofile_adv << "\n\t\t\\hline";

    ofile_adv << "\n\t\\end{tabular} \n\t} \n\t\\caption{Percentage of interruptions in $[2,2 ^ {20}]$ for which the algorithm with prediction outperforms the baseline schedule.}\n\\end{table}";
}

