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
	std::vector<long double> points;
	std::map<std::string, long double> global_stats;
	std::map<double,long double> percentiles;
	std::map<std::string, int> opts;
public: 
	void compute_line_stats(long double);
	void compute_quartiles();
	void computer_percentiles();
	void initPercentile(double);
	void compute_percentile(double);
	void compute_global_stats();
	map<double,long double> get_percentiles();
	map<std::string, long double> get_global_stats();
	Sta(map<string, int>);
};