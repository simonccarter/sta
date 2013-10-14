#include <iostream>
#include <vector>
#include <climits>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include <algorithm> 
#include <map>

using namespace std;

/* FLAGS */
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
static int help_flag;
static int sort_flag; //redundent?
static int quartiles_flag;
static int all_flag;

/* DATA STRUCTURES */
vector<long double> * stats = new vector<long double>();
vector<long double> * points = new vector<long double>();
map<string, long double> * global_stats = new map<string, long double>();
map<string,int> * opts = new map<string, int>();	

/* STATS COMPUTED AT THE LINE */
void compute_line_stats(){
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

void print_help(){
	cerr << " " << endl;	
	cerr << "Usage: cat file.txt | sta [options]" << endl;	
	cerr << " " << endl;	
	cerr << "--help		prints this help " << endl;	
	cerr << " " << endl;	
	cerr << "Desriptive Stastistics: " << endl;	
	cerr << "--sum		sum" << endl;	
	cerr << "--mean		average" << endl;	
	cerr << "--min		min point" << endl;	
	cerr << "--max		max point" << endl;	
	cerr << "--sd		standard deviation" << endl;	
	cerr << "--var		variance" << endl;	
	cerr << "--n		sample size" << endl;	
	cerr << "--q		quartiles" << endl;	
	cerr << " " << endl;	
	cerr << "Options: " << endl;	
	cerr << "--compensated		compenssated variant" << endl;	
	cerr << "--population		unbiased estimator (n-1)" << endl;	
	cerr << " " << endl;	
}

void compute_quartiles(long double &n){
	int const size = (*points).size();
	int const Q1 = size / 4;
	int const Q2 = size / 2;
	int const Q3 = Q1 + Q2;

	/** PARTIAL SORT **/
	nth_element((*points).begin(),          (*points).begin() + Q1, (*points).end());
	nth_element((*points).begin() + Q1 + 1, (*points).begin() + Q2, (*points).end());
	nth_element((*points).begin() + Q2 + 1, (*points).begin() + Q3, (*points).end());

	long double firq = (*points)[Q1];		
	long double lasq = (*points)[Q3];		
	long double median = size%2!=0 ? (*points)[Q2] : ((*points)[Q2]+(*points)[Q2-1])/2;
	
	(*global_stats)["Q1"] = firq;
	(*global_stats)["median"] = median; 
	(*global_stats)["Q3"] = lasq;
}

/* COMPUTE GLOBAL STATS */
void compute_global_stats(){
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

	if(sort_flag){
		compute_quartiles(n);
	}	

	long double sderr = (samp_sd / sqrt(n))

	/* INIT GLOBAL STATS */
	(*global_stats)["N"] = n;
	(*global_stats)["mean"] = mean;

	/** HORRIBLE, referening by index position !?!**/
	(*global_stats)["sum"] = (*stats).at(1);
	(*global_stats)["min"] = (*stats).at(2);
	(*global_stats)["max"] = (*stats).at(3);

	(*global_stats)["sterr"] = stderr;

	if( (*opts).find("population") != (*opts).end() && (*opts).find("compensated") != (*opts).end()){
		(*global_stats)["sd"] = pop_sd_comp;
		(*global_stats)["var"] = comp_var_p;
	}
	if( (*opts).find("population") != (*opts).end() && (*opts).find("compensated") == (*opts).end() ) {
		(*global_stats)["sd"] = pop_sd;
		(*global_stats)["var"] = var_p;
	}
	if( (*opts).find("population") == (*opts).end() && (*opts).find("compensated") != (*opts).end()){
		(*global_stats)["sd"] = samp_sd_comp;
		(*global_stats)["var"] = comp_var_s;
	}
	if( (*opts).find("population") == (*opts).end() && (*opts).find("compensated") == (*opts).end() ) {
		(*global_stats)["sd"] = samp_sd;
		(*global_stats)["var"] = var_s;
	}

}

void print_stats(){
	vector<string> opts_ordered;
	if(!quartiles_flag && !all_flag){ 
		opts_ordered.push_back("N");
		opts_ordered.push_back("min");
		opts_ordered.push_back("max");
		opts_ordered.push_back("sum");
		opts_ordered.push_back("mean");
		opts_ordered.push_back("sd");
		opts_ordered.push_back("sderr");
		opts_ordered.push_back("var");
	}else if(!all_flag){
		opts_ordered.push_back("N");
		opts_ordered.push_back("min");
		opts_ordered.push_back("Q1");
		opts_ordered.push_back("median");
		opts_ordered.push_back("Q3");
		opts_ordered.push_back("max");
	}else{
		opts_ordered.push_back("N");
		opts_ordered.push_back("min");
		opts_ordered.push_back("Q1");
		opts_ordered.push_back("median");
		opts_ordered.push_back("Q3");
		opts_ordered.push_back("max");
		opts_ordered.push_back("sum");
		opts_ordered.push_back("mean");
		opts_ordered.push_back("sd");
		opts_ordered.push_back("sderr");
		opts_ordered.push_back("var");
	}
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
		cout << (*global_stats)[*ii] << "\t";
	}	
	cout<<endl;
}

void read_parameters(int argc, char **argv){
	int c;
	while (1){
	   static struct option long_options[] = {
	       /* These options set a flag. */
	       {"sum", no_argument,       &sum_flag, 1},
	       {"help", no_argument,       &help_flag, 1},
	       {"min",   no_argument,       &min_flag, 1},
	       {"max",   no_argument,       &max_flag, 1},
	       {"mean",   no_argument,       &mean_flag, 1},
	       {"sd",   no_argument,       &sd_flag, 1},
	       {"var",   no_argument,       &var_flag, 1},
	       {"n",   no_argument,       &n_flag, 1},
	       {"population",   no_argument,       &population_flag, 1},
	       {"compensated",   no_argument,       &comp_flag, 1},	
	       {"q",   no_argument,       &quartiles_flag, 1},	
	       {"all",   no_argument,       &all_flag, 1},	
	       {0, 0, 0, 0}
	     };
	   int option_index = 0;
	   c = getopt_long_only (argc, argv, "",
			    long_options, &option_index);
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
	     default:
		abort;
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
		(*opts)["compensated"] = 1;
	if (sample_flag)
		(*opts)["sample"] = 1;
	if (population_flag)
		(*opts)["population"] = 1;
	if (mean_flag)
		(*opts)["mean"] = 1;
	if (var_flag)
		(*opts)["var"] = 1;
	if (all_flag)
		(*opts)["quartiles"] = 1;
		(*opts)["min"] = 1;
		(*opts)["Q1"] = 1;
		(*opts)["median"] = 1;
		(*opts)["Q3"] = 1;
		(*opts)["max"] = 1;
		(*opts)["var"] = 1;
		(*opts)["mean"] = 1;
		(*opts)["sum"] = 1;
		(*opts)["N"] = 1;
		(*opts)["sd"] = 1;
		(*opts)["sderr"] = 1;
		sort_flag = 1;
	if (help_flag){
		print_help();
		return;
	}
	if (quartiles_flag){
		(*opts)["N"] = 1;
		(*opts)["quartiles"] = 1;
		(*opts)["min"] = 1;
		(*opts)["Q1"] = 1;
		(*opts)["median"] = 1;
		(*opts)["Q3"] = 1;
		(*opts)["max"] = 1;
		sort_flag = 1;	
	}

       /* Print any remaining command line arguments (not options). */
	if (optind < argc){
           printf ("non-option ARGV-elements: ");
           while (optind < argc)
             printf ("%s ", argv[optind++]);
           putchar ('\n');
	}	

	/**
	If opts is empty, print out summary info, and assume sample statistics
	**/
	if((*opts).size()==0){
		(*opts)["sum"] = 1;
		(*opts)["N"] = 1;
		(*opts)["min"] = 1;
		(*opts)["max"] = 1;
		(*opts)["mean"] = 1;
		(*opts)["sd"] = 1;
		(*opts)["sderr"] = 1;
	}
}

int main (int argc, char **argv){
	read_parameters(argc, argv);
	long double sum = 0;
	long double n = 0;
	long double min = INT_MAX;
	long double max = INT_MIN;
	long double mean = 0;
	long double sd = 0;

	(*stats).push_back(sum);
	(*stats).push_back(sum);
	(*stats).push_back(min);
	(*stats).push_back(max); 
	(*stats).push_back(n); 
	(*stats).push_back(mean); 
	(*stats).push_back(sd); 

	while(cin){
		string input_line;
		getline(cin,input_line);
		if(input_line=="") continue;
		long double stat = (long double) ::atof(input_line.c_str());
		(*stats).at(0) = stat;
		compute_line_stats();
		(*points).push_back(stat);
	}
	compute_global_stats();
	print_stats();
	
	delete points;
	delete stats;
	delete global_stats;
	delete opts;
	return 0;
}


