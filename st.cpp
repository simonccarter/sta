#include <iostream>
#include <vector>
#include <climits>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include <map>
using namespace std;

static int n_flag;
static int min_flag;
static int max_flag;
static int mean_flag;
static int sum_flag;
static int sd_flag;
static int population_flag;
static int sample_flag;
static int comp_flag;
static int var_flag;

//running stats needed
void compute_line_stats(vector<long double> * stats){
	long double stat = (*stats).at(0);

	//sum
	(*stats).at(1) += stat;

	//N
	(*stats).at(4)+=1;

	//min
	if ((*stats).at(2) > stat){
		(*stats).at(2) = stat;
	}

	//max
	if ((*stats).at(3) < stat){
		(*stats).at(3) = stat; 
	}
}

//global descriptive statistics
void compute_global_stats(vector<long double> * stats, vector<long double> * points, map<string, long double> * global_stats, map<string,int> * opts){
	//mean
	long double mean = (*stats).at(1) / (*stats).at(4); 
	long double n = (*stats).at(4);

	//sum of squared deviations
	long double sum = 0;
	//for compensated variant
	long double sum3 = 0;
	for (long index=0; index<(long)(*points).size(); ++index){
		long double diff = (*points).at(index) - mean;
		sum += (diff * diff);
		sum3 += diff;
	}	


	//for compensated variant biased
	long double comp_var_s = (sum-((sum3*sum3)/n))/n;	

	//for compensated variant unbiased when n is not equal to |population|
	long double comp_var_p = (sum-((sum3*sum3)/n))/n-1;	

	//sample sd comp
	long double samp_sd_comp = sqrt(comp_var_s);

	//pop sd comp
	long double pop_sd_comp = sqrt(comp_var_p);

	//sample variance
	long double var_s = sum/n;

	//population variance
	long double var_p = sum/(n-1);

	//sample sd	
	long double samp_sd = sqrt(var_s);

	//pop sd	
	long double pop_sd = sqrt(var_p);

	//init global_stats

	(*global_stats)["n"] = n;
	(*global_stats)["mean"] = mean;

	(*global_stats)["sampl_sd"] = samp_sd;
	(*global_stats)["pop_sd"] = pop_sd;

	(*global_stats)["pop_sd_comp"] = pop_sd_comp;
	(*global_stats)["samp_sd_comp"] = samp_sd_comp;

	(*global_stats)["sum"] = (*stats).at(1);
	(*global_stats)["min"] = (*stats).at(2);
	(*global_stats)["max"] = (*stats).at(3);
	
	
	if( (*opts).find("population") != (*opts).end() && (*opts).find("compensative") != (*opts).end()){
		(*global_stats)["sd"] = pop_sd_comp;
		(*global_stats)["var"] = comp_var_p;
	}
	if( (*opts).find("population") != (*opts).end() && (*opts).find("compensative") == (*opts).end() ) {
		(*global_stats)["sd"] = pop_sd;
		(*global_stats)["var"] = var_p;
	}
	if( (*opts).find("population") == (*opts).end() && (*opts).find("compensative") != (*opts).end()){
		(*global_stats)["sd"] = samp_sd_comp;
		(*global_stats)["var"] = comp_var_s;
	}
	if( (*opts).find("population") == (*opts).end() && (*opts).find("compensative") == (*opts).end() ) {
		(*global_stats)["sd"] = samp_sd;
		(*global_stats)["var"] = var_s;
	}

}

void print_stats(map<string, long double> * stats, map<string,int> * opts){

	vector<string> opts_ordered; 
	opts_ordered.push_back("N");
	opts_ordered.push_back("max");
	opts_ordered.push_back("min");
	opts_ordered.push_back("sum");
	opts_ordered.push_back("mean");
	opts_ordered.push_back("sd");
	opts_ordered.push_back("var");

	string output = " "; 

	for(vector<string>::iterator ii=opts_ordered.begin(); ii!=opts_ordered.end();++ii){
		if ( (*opts).find(*ii)  ==  (*opts).end())
			continue;
		cout << *ii << "\t";

	}	
	cout<<endl;
	for(vector<string>::iterator ii=opts_ordered.begin(); ii!=opts_ordered.end();++ii){
		if ( (*opts).find(*ii)  ==  (*opts).end())
			continue;
		cout << (*stats)[*ii] << "\t";
	}	
	cout<<endl;
}

void set_flags(int argc, char **argv, map<string,int>  * opts){
	int c;
	while (1){
	   static struct option long_options[] = {
	       /* These options set a flag. */
	       {"sum", no_argument,       &sum_flag, 1},
	       {"min",   no_argument,       &min_flag, 1},
	       {"max",   no_argument,       &max_flag, 1},
	       {"mean",   no_argument,       &mean_flag, 1},
	       {"sd",   no_argument,       &sd_flag, 1},
	       {"var",   no_argument,       &var_flag, 1},
	       {"n",   no_argument,       &n_flag, 1},
	       {"population",   no_argument,       &population_flag, 1},
	       {"sample",   no_argument,       &sample_flag, 1},
	       {"compensated",   no_argument,       &comp_flag, 1},
	     };
	   /* getopt_long stores the option index here. */
	   int option_index = 0;

	   c = getopt_long (argc, argv, "abc:d:f:",
			    long_options, &option_index);

	   /* Detect the end of the options. */
	   if (c == -1)
	     break;

	   switch (c)
	     {
	     case 0:
	       /* If this option set a flag, do nothing else now. */
	       if (long_options[option_index].flag != 0)
		 break;
	       printf ("option %s", long_options[option_index].name);
	       if (optarg)
		 printf (" with arg %s", optarg);
	       printf ("\n");
	       break;

	     case '?':
	       /* getopt_long already printed an error message. */
	       break;

	     default:
	       abort ();
	     }
	 }

	if (sum_flag)
		(*opts)["sum"] = 1;
	if (n_flag)
		(*opts)["N"] = 1;
	if (min_flag)
		(*opts)["min"] = 1;
	if (max_flag)
		(*opts)["max"] = 1;
	if (sd_flag)
		(*opts)["sd"] = 1;
	if (comp_flag)
		(*opts)["compensative"] = 1;
	if (sample_flag)
		(*opts)["sample"] = 1;
	if (population_flag)
		(*opts)["population"] = 1;
	if (mean_flag)
		(*opts)["mean"] = 1;
	if (var_flag)
		(*opts)["var"] = 1;


       /* Print any remaining command line arguments (not options). */
	if (optind < argc){
           printf ("non-option ARGV-elements: ");
           while (optind < argc)
             printf ("%s ", argv[optind++]);
           putchar ('\n');
	}	

	/**
	If opts is empty, print out all info, and assume sample statistics
	**/
	if((*opts).size()==0){
		(*opts)["sum"] = 1;
		(*opts)["N"] = 1;
		(*opts)["min"] = 1;
		(*opts)["max"] = 1;
		(*opts)["mean"] = 1;
		(*opts)["sd"] = 1;
	}

}

int main (int argc, char **argv){
	map<string,int> * opts = new map<string, int>();	
	set_flags(argc, argv,opts);
	long double sum = 0;
	long double n = 0;
	long double min = INT_MAX;
	long double max = INT_MIN;
	long double mean = 0;
	long double sd = 0;

	vector<long double> * stats = new vector<long double>();
	(*stats).push_back(sum);
	(*stats).push_back(sum);
	(*stats).push_back(min);
	(*stats).push_back(max); 
	(*stats).push_back(n); 
	(*stats).push_back(mean); 
	(*stats).push_back(sd); 
        vector<long double> * points = new vector<long double>();
	map<string, long double> * global_stats = new map<string, long double>();

	while(cin){
		string input_line;
		getline(cin,input_line);
		if(input_line=="") continue;
		long double stat = (long double) ::atof(input_line.c_str());
		(*stats).at(0) = stat;
		compute_line_stats(stats);
		(*points).push_back(stat);
	}
	compute_global_stats(stats, points, global_stats, opts);
	print_stats(global_stats,opts);
	
	delete points;
	delete stats;
	delete global_stats;
	delete opts;
	return 0;
}


