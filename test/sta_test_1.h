#include <./cxxtest/TestSuite.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "../src/sta.cpp"

using namespace std;

class TestRunner1 : public CxxTest::TestSuite {
	Sta * sta;
	map<string, long double>  stats;
	map<string, int> opts;
public:
	void setUp() {
		opts["sum"] = 1;
		opts["N"] = 1;
		opts["min"] = 1;
		opts["max"] = 1;
		opts["mean"] = 1;
		opts["sd"] = 1;
		opts["sderr"] = 1;
		opts["var"] = 1;
		init_sta();	
	}
	void tearDown(){
		delete sta;
	}
	void test_n(){
		TS_ASSERT_EQUALS(stats["N"], 10);
	}
	void test_min(){
		TS_ASSERT_EQUALS(stats["min"], 1.0);
	}
	void test_max(){
		TS_ASSERT_EQUALS(stats["max"], 10);
	}
	void test_mean(){
		TS_ASSERT_EQUALS(stats["mean"], 5.5);
	}
	void test_sum(){
		TS_ASSERT_EQUALS(stats["sum"], 55);
	}
	void test_sd(){
		TS_ASSERT_DELTA(stats["sd"], 2.87228, 1e-5);
	}
	void test_sderr(){
		TS_ASSERT_DELTA(stats["sderr"], 0.908295, 1e-6);
	}
	void test_var(){
		TS_ASSERT_DELTA(stats["var"], 8.25, 1e-2);
	}
	void test_q1(){
		TS_ASSERT_EQUALS(stats["Q1"], 3);
	}
	void test_q3(){
		TS_ASSERT_EQUALS(stats["Q3"], 8);
	}
	void test_percentiles(){
		sta->initPercentile(0);
		sta->initPercentile(44);
		sta->initPercentile(99);
		sta->compute_percentiles();
		stats = sta->get_global_stats();
		TS_ASSERT_EQUALS(stats["0th"], 1);
		TS_ASSERT_EQUALS(stats["44th"], 5);
		TS_ASSERT_EQUALS(stats["99th"], 10);
	}
	void test_sd_sample(){
		delete sta;
		opts["sample"] = 1;
		init_sta();
		opts.erase("sample");
		TS_ASSERT_DELTA(stats["sd"], 3.02765, 1e-6);
	}
	void test_sd_sample_comp(){
		delete sta;
		opts["sample"] = 1;
		opts["compensated"] = 1;
		init_sta();
		opts.erase("sample");
		opts.erase("compensated");
		TS_ASSERT_DELTA(stats["sd"], 3.02765, 1e-5);
	}
	void test_sd_pop_comp(){
		delete sta;
		opts["compensated"] = 1;
		init_sta();
		opts.erase("compensated");
		TS_ASSERT_DELTA(stats["sd"], 2.87228, 1e-5);
	}
	void test_var_sample(){
		delete sta;
		opts["sample"] = 1;
		init_sta();
		opts.erase("sample");
		TS_ASSERT_DELTA(stats["var"], 9.16667, 1e-5);
	}
	void test_var_sample_comp(){
		delete sta;
		opts["sample"] = 1;
		opts["compensated"] = 1;
		init_sta();
		opts.erase("sample");
		opts.erase("compensated");
		TS_ASSERT_DELTA(stats["var"], 9.16667, 1e-5);
	}
	void test_var_pop_comp(){
		delete sta;
		opts["compensated"] = 1;
		init_sta();
		opts.erase("compensated");
		TS_ASSERT_DELTA(stats["var"], 8.25, 1e-5);
	}
	void test_sderr_sample(){
		delete sta;
		opts["sample"] = 1;
		init_sta();
		opts.erase("sample");
		TS_ASSERT_DELTA(stats["sderr"], 0.957427, 1e-6);
	}
	void test_sderr_sample_comp(){
		delete sta;
		opts["sample"] = 1;
		opts["compensated"] = 1;
		init_sta();
		opts.erase("sample");
		opts.erase("compensated");
		TS_ASSERT_DELTA(stats["sderr"], 0.957427, 1e-6);
	}
	void test_sderr_pop_comp(){
		delete sta;
		opts["compensated"] = 1;
		init_sta();
		opts.erase("compensated");
		TS_ASSERT_DELTA(stats["sderr"], 0.908295, 1e-6);
	}
	void insert_data (int begin, int end){
		for(int i = begin; i<=end; i++){
			sta->compute_line_stats(i);
		}
	}
	void init_sta(){
		sta = new Sta();
		sta->set_opts(opts);
		insert_data(1,10);
		sta->compute_global_stats();
		sta->compute_quartiles();
		sta->compute_percentiles();
		stats = sta->get_global_stats();
	}
	void print_global_stats(){
		cout << "In print_global_stats" << '\n';
		for(map<string,long double>::iterator ii = stats.begin(); ii != stats.end(); ++ii){
			cout << ii->first << ' ' << ii->second << '\n';
		}
	}
};