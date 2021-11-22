#include "interruption_as_prediction.h" // used the experiments in which schedules get the interruption time as prediction
#include "binary_prediction.h" // used the experiments in which schedules get answers to binary queries

/* parse the file input file with a given name and run the experiment (call the appropriate method with the setting indicated in the input file).
returns false if there is an error parsing the input.
max_T indicates the range of the interruption values in the experiments (its default value is 2^20).
n is the number of binary queries for Robust (its default value is 100).
*/
bool parse_input_and_run_experiments(string input_file_name, int max_T, int n)
{
    ifstream inFile;
    inFile.open(input_file_name.c_str());
    if (!inFile) {
        cerr << "Unable to open file " << input_file_name << endl;
        return false;   // call system to stop
    }

    string next_cmnt;
    //skipping the header of the input file
    int input_head_line_no = 8;
    for (int line = 0; line < input_head_line_no; line++)
        getline(inFile, next_cmnt);
    double r, H, noise_std_dev;
    int n_points; //n_points indicates the number of evenly-spaced points for the interruption time 

    string prediction_type_str, noise_type_str;
    inFile >> next_cmnt >> prediction_type_str >> next_cmnt >> r >> next_cmnt >> H >> next_cmnt >> noise_type_str >> next_cmnt >> noise_std_dev;

    if (prediction_type_str != "binary")
        prediction_type_str = "interruption"; //default
    if (noise_type_str != "uniform")
        noise_type_str = "normal"; //default

    string output_file_name("");
    output_file_name += "(prediction_type_is_" + prediction_type_str + ")_";
    output_file_name += "(r_is_" + double_two_digit_precision(r) + ")_"; // just include two digit precision in the file name 
    output_file_name += "(H_is_" + double_two_digit_precision(H) + ")_";
    output_file_name += "(noise_type_is_" + noise_type_str + ")_";
    if (noise_type_str == "normal")
        output_file_name += "(noise_std_dev_is_" + double_two_digit_precision(noise_std_dev) + ")";;

    inFile >> next_cmnt >> n_points;
    output_file_name += "(n_points_is_" + to_string(n_points) + ")";

    int n_sched = 0;
    double* buffer_ratios = new double[20]; // max number of algorithms to compare is 20
    while (inFile >> next_cmnt)
    {
        inFile >> buffer_ratios[n_sched];
        n_sched++;
    }
    // parsing paramter file ended. next, set a couple of parameters accordingly

    int noise_type = 1; // only used when prediction is interruption time. noise type is normal (1) by defaul 
    if (noise_type_str == "uniform")
        noise_type = 0;

    // the header for the .csv files
    string header_str = "interruption time, baseline schedule 1, baseline schedule 2, baseline schedule 3,";
    for (int i = 0; i < n_sched; i++)
    {
        if (buffer_ratios[i] < 0.01 || buffer_ratios[i] > 0.9)
        {
            cout << "error; buffer_ratio is too small or too large.\n";
            return false;
        }

        header_str += " p = " + double_two_digit_precision(buffer_ratios[i]);
        if (buffer_ratios[i] == H)
            header_str += " (H-aware)";
        if (i < n_sched - 1)
            header_str += ",";
    }
    
    // make sure the input is in the right range
    if (r < 4 || r > 7)
    {
        cout << "error; make sure r is in the range [4,7].\n";
        return false;
    }

    if (H < 0.01 || H > 0.9)
    {
        cout << "error; make sure H is in the range [0.01,0.9].\n";
        return false;
    }

    if (noise_std_dev < 0.01)
    {
        cout << "error; noise_std_dev is too small or negative.\n";
        return false;
    }

    if (n_points < 10 || n_points > 5000)
    {
        cout << "error; n_points is too large.\n";
        return false;
    }

    ofstream ofile_score, ofile_ratio, ofile_adv; //output files for ratio (csv), score (csv), (latex format in txt) of different schedules
    string str_score_file_name = "Results/score_" + output_file_name + ".csv";
    string str_ratio_file_name = "Results/ratio_" + output_file_name + ".csv";
    string str_adv_file_name  = "Results/adv_" + output_file_name + ".txt";

    ofile_score.open(str_score_file_name.c_str());
    ofile_ratio.open(str_ratio_file_name.c_str());
    ofile_adv.open(str_adv_file_name.c_str());


    ofile_ratio << header_str << endl;
    ofile_score << header_str << endl;
    cout << output_file_name << endl << "Please wait; the progress percentage is as follows:" << endl;
    if (prediction_type_str == "binary")
        test_different_deadlines_binary(r, H, n, buffer_ratios, n_sched, max_T, ofile_ratio, ofile_score, ofile_adv, n_points);
    else
        test_prediction_is_interruption(r, H, buffer_ratios, n_sched, max_T, noise_type, noise_std_dev, ofile_ratio, ofile_score, ofile_adv, n_points);
    return true;
}

int main(int argc, char* argv[])
{
    time_t t;
    srand((unsigned)time(&t));     /* Intializes random number generator */

    int max_T = 1 << 20; // 2^20; the upper bound for the interruption time in our experiments
    int n = 100; // number of schedules in the binary setting
    string input_file_name = "input_parameters.txt";
    if (argc > 1)
        input_file_name = argv[1];
    bool res = parse_input_and_run_experiments(input_file_name, max_T, n);
    if (res)
        cout << "\n The process ended successfully; the results can be found in the \"Results\" folder.\n";
    else
        cout << "Error parsing the input; please follow the instructions in \"ReadMe.txt\"";
    return 0;
}
