#include <iostream>
#include <vector>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <string>
#include <map>
#include <sstream>
#include "sta.h"

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
static int sample_flag;
static int comp_flag;
static int var_flag;
static int fixed_flag;
static int help_flag;
static int sort_flag; //redundent?
static int quartiles_flag;
double percentiles_flag;
static int all_flag;

/* VARIABLES */
string delimiter = "\t";

/* DATA STRUCTURES */
vector<string> opts_ordered;
map<string, int> opts;

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
	cerr << "--p		comma seperated list of percentiles" << endl;
	cerr << " " << endl;
	cerr << "Options: " << endl;
	cerr << "--brief		brief mode; only values are output" << endl;
	cerr << "--compensated		compensated variant" << endl;
	cerr << "--delimiter		specify string to use as delimiter" << endl;
	cerr << "--sample		unbiased estimator (n-1) to use with sample scores" << endl;
	cerr << "--transpose		transpose output" << endl;
}

void print_stats(Sta* sta){
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

	if(percentiles_flag){
		map<double, long double> percentiles = sta->get_percentiles();
		for(map<double,long double>::iterator ii = percentiles.begin(); ii != percentiles.end(); ++ii){
			string String = static_cast<ostringstream*>( &(ostringstream() << ii->first) )->str();
			opts_ordered.push_back(String + "th");
			opts[String + "th"] = 1;
		}
	}

    if(fixed_flag){
      cout << fixed;
    }

	map<string, long double> global_stats = sta->get_global_stats();
	for(vector<string>::iterator ii = opts_ordered.begin(); ii != opts_ordered.end(); ++ii){
		if(opts.find(*ii) == opts.end()){
			continue;
		}
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
			if(opts.find(*ii) == opts.end())
				continue;
			cout << global_stats[*ii] << delimiter;
		}
		cout << endl;
	}
}

void read_parameters(int argc, char **argv, Sta* sta){
	int c;
	while (1){
		static struct option long_options[] = {
		       /* These options set a flag. */
		       {"brief", 		no_argument,       &brief_flag, 1},
		       {"transpose", 	no_argument,       &transpose_flag, 1},
		       {"sum", 			no_argument,       &sum_flag, 1},
		       {"help", 		no_argument,       &help_flag, 1},
		       {"min",   		no_argument,       &min_flag, 1},
		       {"max",   		no_argument,       &max_flag, 1},
		       {"mean",   		no_argument,       &mean_flag, 1},
		       {"median",   	no_argument,       &median_flag, 1},
		       {"sd",   		no_argument,       &sd_flag, 1},
		       {"sderr",   		no_argument,       &sderr_flag, 1},
		       {"var",   		no_argument,       &var_flag, 1},
		       {"n",   			no_argument,       &n_flag, 1},
		       {"sample",   	no_argument,       &sample_flag, 1},
		       {"compensated",  no_argument,       &comp_flag, 1},
		       {"delimiter",   	required_argument, 0, 0},
		       {"fixed",        no_argument,       &fixed_flag, 1},
		       {"q",           	no_argument, &quartiles_flag, 1},
		       {"q1",          	no_argument, &q1_flag, 1},
		       {"q3",          	no_argument, &q3_flag, 1},
		       {"all",         	no_argument, &all_flag, 1},
		       {"percentiles",   required_argument, 0, 0},
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
			if (strcmp(long_options[option_index].name,"percentiles") == 0){
				string numbers = optarg;
				stringstream ss(numbers);
				double i;
				while (ss >> i){
					if(i<0 || i>99 || fmod (i,1)!=0){
						cout << "Error. Percentile " << i << " shoud be an integer between 0 and 99."<< endl;
						exit(0);
					}
					sta->initPercentile(i);
					if (ss.peek() == ',')
						ss.ignore();
				}
				percentiles_flag=1;
				opts["percentiles"] = 1;
			}
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
	if(sd_flag){
		opts["sd"] = 1;
	}
	if(sderr_flag)
		opts["sderr"] = 1;
	if(comp_flag)
		opts["compensated"] = 1;
	if(transpose_flag)
		opts["transpose"] = 1;
	if(brief_flag)
		opts["brief"] = 1;
	if(sample_flag){
		opts["sample"] = 1;
	}
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

	int subtract = 0;
	if(brief_flag)
		subtract++;
	if(comp_flag)
		subtract++;
	if(sample_flag)
		subtract++;
	if(transpose_flag)
		subtract++;

	/**
	If opts is empty, print out summary info, and assume population of scores
	**/
	if(opts.size()==subtract){
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
	Sta * sta = new Sta();
	read_parameters(argc, argv, sta);
	sta->set_opts(opts);
	while(cin){
		string input_line;
		getline(cin,input_line);
		if(input_line == "") continue;
		long double stat = (long double)::atof(input_line.c_str());
		sta->compute_line_stats(stat);
	}
	sta->compute_global_stats();
	if (percentiles_flag) sta->compute_percentiles();
	if (sort_flag) sta->compute_quartiles();
	print_stats(sta);
	return 0;
}
