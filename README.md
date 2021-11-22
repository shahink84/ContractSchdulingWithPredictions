# ContractSchdulingWithPredictions

This repo contains the code for the project “Contract SchedulingWith Predictions”.


**Files description:**
	aux_methods.h/cpp: auxiliarly functions used for contract scheduling experiments and generating output files
	baseline.h/cpp: computation of the exponential schedules (without prediction) that act as baseline schedules 
	interruption_as_prediction.h/cpp: experiments when the prediction is the interruption time
	binary_prediction.h/cpp: experiments when the prediction is answers to binary queries
	main.cpp: setting input/output files and the main function.


**Compile: **
	To compile the code, simply run the following:
		g++ -std=c++11 -o contract_scheduling *.cpp
	The outcome will be an executable named “contract_scheduling”


**Setting parameters:**
	In an input file, set the parameters using the template provided in the sample “input_parameters.txt”. Please follow the instructions in that file.
	You might copy “input_parameters.txt” and change the parameters (e.g., r or H) in a new file. 
	Do not remove/rename “input_parameters.txt” as it acts as default input when no input-file is provided


**Running the code:**
	Simply run the following:
		contract_scheduling “input_file_name”

        Where “input_file_name” is the name of the file. For example: 	
		contract_scheduling input.txt

	If the file name is not provided, the default file “input_parameters.txt” will be used as the input.


**Output:**
	The output will be generated inside the folder “Results”. 
	There will be three outputs: 
		1) two .csv files which indicate the score/acceleration ratio of different schedules for different interruption times. Each column is associated with a schedule and each row is associated with an interruption time. 
		2) a .txt file which includes a table (in LaTeX format) of advantage/strong-advantage of scheduling algorithms with prediction over the baseline algorithm.
	The names of the files include all parameters as indicated in the input file.

The results reported in the paper are included in the "Results" folder.




