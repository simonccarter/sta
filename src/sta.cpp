#include "sta.h"

long double N = 0, Min = LDBL_MAX, Max = -LDBL_MAX, sum = 0;

Sta::~Sta(){
	N = 0; Min = LDBL_MAX, Max = -LDBL_MAX, sum = 0;
	clear_global_stats();
	clear_percentiles();
}; 

void Sta::set_opts(map<string, int> opts){
	this->opts = opts;	
}

void Sta::compute_line_stats(long double x){
	sum += x;
	N++;
	if (Min > x){
		Min = x;
	}

	if (Max < x){
		Max = x; 
	}
	points.push_back(x);
}

void Sta::initPercentile(double i){
	percentiles[i] = 0.0;
}

void Sta::compute_quartiles(){
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

void Sta::compute_percentile(double p){
	double percentile = (p/100) * points.size();
	nth_element( points.begin(), points.begin()+percentile, points.end() );
	string String = static_cast<ostringstream*>( &(ostringstream() << p) )->str();
	global_stats[String + "th"] = points[percentile]; 
}

void Sta::compute_percentiles(){
	for(map<double,long double>::iterator ii = percentiles.begin(); ii != percentiles.end(); ++ii){
		compute_percentile(ii->first);
	}
}

void Sta::compute_global_stats(){
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
	long double comp_var_p = (ssd-((sum3*sum3)/N))/N;	

	//for compensated variant unbiased when n is not equal to |population|
	long double comp_var_s = (ssd-((sum3*sum3)/N))/(N-1);	

	//pop sd comp
	long double p_sd_comp = sqrt(comp_var_p);

	//sample sd comp
	long double s_sd_comp = sqrt(comp_var_s);

	//pop variance
	long double var_p = ssd/N;

	//sample variance
	long double var_s = ssd/(N-1);

	//pop sd	
	long double p_sd = sqrt(var_p);

	//sample sd	
	long double s_sd = sqrt(var_s);

	//standard error of the mean	
	long double sderr_p = (p_sd / sqrt(N));
	long double sderr_s = (s_sd / sqrt(N));

	//standard error of the mean, comp variants
	long double sderr_p_c = (p_sd_comp / sqrt(N));
	long double sderr_s_c = (s_sd_comp / sqrt(N));

	/* INIT GLOBAL STATS */
	global_stats["N"] = N;
	global_stats["mean"] = mean;
	global_stats["sum"] = sum;
	global_stats["min"] = Min;
	global_stats["max"] = Max;

	//sample stats with compensated version
	if(opts.find("sample") != opts.end() && opts.find("compensated") != opts.end()){
		global_stats["sd"] = s_sd_comp;
		global_stats["var"] = comp_var_s;
		global_stats["sderr"] = sderr_s_c;
	}
	//sample stats 
	if(opts.find("sample") != opts.end() && opts.find("compensated") == opts.end() ) {
		global_stats["sd"] = s_sd;
		global_stats["var"] = var_s;
		global_stats["sderr"] = sderr_s;
	}
	//population stats with compensated version
	if(opts.find("sample") == opts.end() && opts.find("compensated") != opts.end()){
		global_stats["sd"] = p_sd_comp;
		global_stats["var"] = comp_var_p;
		global_stats["sderr"] = sderr_p_c;
	}
	//population stats
	if(opts.find("sample") == opts.end() && opts.find("compensated") == opts.end() ) {
		global_stats["sd"] = p_sd;
		global_stats["var"] = var_p;
		global_stats["sderr"] = sderr_p;
	}
}

map<double,long double> Sta::get_percentiles(){
	return percentiles;
}

map<string, long double> Sta::get_global_stats(){
	return global_stats;
}

void Sta::clear_global_stats(){
	global_stats.clear();
}

void Sta::clear_percentiles(){
	percentiles.clear();
}