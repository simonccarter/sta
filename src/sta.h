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
#include <sstream>

using namespace std;

class Sta {
	private:
		vector<long double> points;
		map<string, long double> global_stats;
		map<double,long double> percentiles;
		map<string, int> opts;
		long double N = 0, Min = LDBL_MAX, Max = -LDBL_MAX, sum = 0;

	public: 
		void compute_line_stats(long double);
		void compute_quartiles();
		void computer_percentiles();
		void initPercentile(double);
		void compute_percentile(double);
		void compute_global_stats();
		map<double,long double> get_percentiles();
		map<string, long double> get_global_stats();
		Sta(map<string, int>);
};