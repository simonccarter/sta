#include <iostream>
#include <vector>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <string>
#include <algorithm> 
#include <map>

using namespace std;

/* FLAGS */
static int brief_flag;
static int delimiter_flag;
static int transpose_flag;
static int q1_flag;
static int q3_flag;
static int n_flag;
static int min_flag;
static int max_flag;
static int mean_flag;
static int median_flag;
static int sum_flag;
static int sd_flag;
static int sderr_flag;
static int population_flag;
static int sample_flag;
static int comp_flag;
static int var_flag;
static int help_flag;
static int sort_flag; //redundent?
static int quartiles_flag;
static int all_flag;

/* VARIABLES */
string delimiter = "\t";

long double N = 0, Min = LDBL_MAX, Max = LDBL_MIN, sum = 0;

/* DATA STRUCTURES */
vector<long double> points;
map<string, long double> global_stats;
map<string, int> opts;

/* STATS COMPUTED AT THE LINE */
static void compute_line_stats(long double x){
	sum += x;
	N++;

	if (Min > x){
		Min = x;
	}

	if (Max < x){
		Max = x; 
	}
}

void print_help(){
	cerr << " " << endl;	
	cerr << "Usage: sta [options] < file.txt" << endl;	
	cerr << " " << endl;	
	cerr << "--help		prints this help " << endl;	
	cerr << " " << endl;	
	cerr << "Descriptive Stastistics: " << endl;	
	cerr << "--mean		average" << endl;	
	cerr << "--median	median" << endl;	
	cerr << "--min		min point" << endl;	
	cerr << "--max		max point" << endl;	
	cerr << "--n		sample size" << endl;	
	cerr << "--sd		standard deviation" << endl;	
	cerr << "--sderr	standard error of the mean" << endl;	
	cerr << "--sum		sum" << endl;	
	cerr << "--q		quartiles" << endl;	
	cerr << "--q1		first quartile" << endl;	
	cerr << "--q3		third quartile" << endl;	
	cerr << "--var		variance" << endl;	
	cerr << " " << endl;	
	cerr << "Options: " << endl;	
	cerr << "--brief		brief mode; only values are output" << endl;	
	cerr << "--compensated		compensated variant" << endl;	
	cerr << "--delimiter		specify string to use as delimiter" << endl;	
	cerr << "--population		unbiased estimator (n-1)" << endl;	
	cerr << "--transpose		transpose output" << endl;	
}

void compute_quartiles(long double &n){
	size_t const size = points.size();
	int const Q1 = size / 4;
	int const Q2 = size / 2;
	int const Q3 = Q1 + Q2;

	/** PARTIAL SORT **/
	nth_element(points.begin(),          points.begin() + Q1, points.end());
	nth_element(points.begin() + Q1 + 1, points.begin() + Q2, points.end());
	nth_element(points.begin() + Q2 + 1, points.begin() + Q3, points.end());

	long double firq = points[Q1];
	long double lasq = points[Q3];
	long double median = size%2 != 0 ? points[Q2] : (points[Q2]+points[Q2-1])/2;
	
	global_stats["Q1"] = firq;
	global_stats["median"] = median; 
	global_stats["Q3"] = lasq;
}

/* COMPUTE GLOBAL STATS */
void compute_global_stats(){
	long double mean = sum / N; 

	//sum of squared deviations
	long double ssd = 0;
	//for compensated variant
	long double sum3 = 0;
	for(size_t index = 0; index < points.size(); ++index){
		long double diff = points.at(index) - mean;
		ssd += (diff * diff);
		sum3 += diff;
	}	

	//for compensated variant biased
	long double comp_var_s = (ssd-((sum3*sum3)/N))/N;	

	//for compensated variant unbiased when n is not equal to |population|
	long double comp_var_p = (ssd-((sum3*sum3)/N))/N-1;	

	//sample sd comp
	long double samp_sd_comp = sqrt(comp_var_s);

	//pop sd comp
	long double pop_sd_comp = sqrt(comp_var_p);

	//sample variance
	long double var_s = ssd/N;

	//population variance
	long double var_p = ssd/(N-1);

	//sample sd	
	long double samp_sd = sqrt(var_s);

	//pop sd	
	long double pop_sd = sqrt(var_p);

	if(sort_flag){
		compute_quartiles(N);
	}	

	//standard error of the mean	
	long double sderr_s = (samp_sd / sqrt(N));
	long double sderr_p = (pop_sd / sqrt(N));

	//standard error of the mean, comp variants
	long double sderr_s_c = (samp_sd_comp / sqrt(N));
	long double sderr_p_c = (pop_sd_comp / sqrt(N));
	
	/* INIT GLOBAL STATS */
	global_stats["N"] = N;
	global_stats["mean"] = mean;

	/** HORRIBLE, referening by index position !?!**/
	global_stats["sum"] = sum;
	global_stats["min"] = Min;
	global_stats["max"] = Max;

	//population stats with compensated version
	if(opts.find("population") != opts.end() && opts.find("compensated") != opts.end()){
		global_stats["sd"] = pop_sd_comp;
		global_stats["var"] = comp_var_p;
		global_stats["sderr"] = sderr_p_c;
	}
	//population stats 
	if(opts.find("population") != opts.end() && opts.find("compensated") == opts.end() ) {
		global_stats["sd"] = pop_sd;
		global_stats["var"] = var_p;
		global_stats["sderr"] = sderr_p;
	}
	//sample stats with compensated version
	if(opts.find("population") == opts.end() && opts.find("compensated") != opts.end()){
		global_stats["sd"] = samp_sd_comp;
		global_stats["var"] = comp_var_s;
		global_stats["sderr"] = sderr_s_c;
	}
	//sample stats
	if(opts.find("population") == opts.end() && opts.find("compensated") == opts.end() ) {
		global_stats["sd"] = samp_sd;
		global_stats["var"] = var_s;
		global_stats["sderr"] = sderr_s;
	}

}

void print_stats(){
	vector<string> opts_ordered;
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

	for(vector<string>::iterator ii = opts_ordered.begin(); ii != opts_ordered.end(); ++ii){
		if(opts.find(*ii) == opts.end())
			continue;
		
		if(!brief_flag)	
			cout << *ii << delimiter;
		if(transpose_flag){
			cout << global_stats[*ii] << endl;	
		}
	}
	if(!transpose_flag && !brief_flag)	
		cout << endl;

	if(!transpose_flag){
		for(vector<string>::iterator ii = opts_ordered.begin(); ii != opts_ordered.end(); ++ii){
			ifopts.find(*ii) == opts.end())
				continue;
			cout << global_stats[*ii] << delimiter;
		}	
		cout << endl;
	}
}

void read_parameters(int argc, char **argv){
	int c;
	while (1){
		static struct option long_options[] = {
		       /* These options set a flag. */
		       {"brief",       no_argument, &brief_flag, 1},
		       {"transpose",   no_argument, &transpose_flag, 1},
		       {"sum",         no_argument, &sum_flag, 1},
		       {"help",        no_argument, &help_flag, 1},
		       {"min",         no_argument, &min_flag, 1},
		       {"max",         no_argument, &max_flag, 1},
		       {"mean",        no_argument, &mean_flag, 1},
		       {"median",      no_argument, &median_flag, 1},
		       {"sd",          no_argument, &sd_flag, 1},
		       {"sderr",       no_argument, &sderr_flag, 1},
		       {"var",         no_argument, &var_flag, 1},
		       {"n",           no_argument, &n_flag, 1},
		       {"population",  no_argument, &population_flag, 1},
		       {"compensated", no_argument, &comp_flag, 1},	
		       {"delimiter",   required_argument, 0, 0},	
		       {"q",           no_argument, &quartiles_flag, 1},	
		       {"q1",          no_argument, &q1_flag, 1},	
		       {"q3",          no_argument, &q3_flag, 1},	
		       {"all",         no_argument, &all_flag, 1},	
		       {0, 0, 0, 0}
		};
		int option_index = 0;
		c = getopt_long_only (argc, argv, "",
		    long_options, &option_index);
		if (c == -1)
		     break;

		switch (c){
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;
			if (strcmp(long_options[option_index].name,"delimiter") == 0){
				delimiter = optarg;
			}
		
			//printf ("option %s", long_options[option_index].name);
			//if (optarg)
			// printf (" with arg %s", optarg);
			//printf ("\n");
			break;
		case '?':
		default:
			abort();
		}
	}

	if (help_flag){
		print_help();
		return;
	}
	if(sum_flag)
		opts["sum"] = 1;
	if(n_flag)
		opts["N"] = 1;
	if(min_flag)
		opts["min"] = 1;
	if(max_flag)
		opts["max"] = 1;
	if(sd_flag)
		opts["sd"] = 1;
	if(sderr_flag)
		opts["sderr"] = 1;
	if(comp_flag)
		opts["compensated"] = 1;
	if(sample_flag)
		opts["sample"] = 1;
	if(mean_flag)
		opts["mean"] = 1;
	if(var_flag)
		opts["var"] = 1;
	if(q1_flag){
		opts["Q1"] = 1;
		sort_flag = 1;	
	}
	if(q3_flag){
		opts["Q3"] = 1;
		sort_flag = 1;	
	}
	if(all_flag){
		opts["quartiles"] = 1;
		opts["min"] = 1;
		opts["Q1"] = 1;
		opts["median"] = 1;
		opts["Q3"] = 1;
		opts["max"] = 1;
		opts["var"] = 1;
		opts["mean"] = 1;
		opts["sum"] = 1;
		opts["N"] = 1;
		opts["sd"] = 1;
		opts["sderr"] = 1;
		sort_flag = 1;
	}
	if(quartiles_flag){
		opts["N"] = 1;
		opts["quartiles"] = 1;
		opts["min"] = 1;
		opts["Q1"] = 1;
		opts["median"] = 1;
		opts["Q3"] = 1;
		opts["max"] = 1;
		sort_flag = 1;	
	}
	if(median_flag){
		opts["median"] = 1;
		sort_flag = 1;	
	}
       /* Print any remaining command line arguments (not options). */
	if(optind < argc){
        	printf ("non-option ARGV-elements: ");
        	while(optind < argc)
           		printf ("%s ", argv[optind++]);
        	putchar ('\n');
	}	

	/**
	If opts is empty, print out summary info, and assume sample statistics
	**/
	if(opts.size() == 0){
		opts["sum"] = 1;
		opts["N"] = 1;
		opts["min"] = 1;
		opts["max"] = 1;
		opts["mean"] = 1;
		opts["sd"] = 1;
		opts["sderr"] = 1;
	}
}

int main(int argc, char **argv){
	read_parameters(argc, argv);

	while(cin){
		string input_line;
		getline(cin,input_line);
		if(input_line == "") continue;
		long double stat = (long double)::atof(input_line.c_str());
		compute_line_stats(stat);
		points.push_back(stat);
	}
	compute_global_stats();
	print_stats();
	
	return 0;
}
