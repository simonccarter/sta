#ifndef STA_H
#define STA_H

#include <iostream>
#include <vector>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
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
	void clear_global_stats();
	void clear_percentiles();
public: 
	void compute_line_stats(long double);
	void compute_quartiles();
	void compute_percentiles();
	void initPercentile(double);
	void compute_percentile(double);
	void compute_global_stats();
	map<double,long double> get_percentiles();
	map<string, long double> get_global_stats();
	void set_opts(map<string, int>);
	~Sta();
};

#endif