#include <./cxxtest/TestSuite.h>
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
#include <iostream>
#include "../src/sta.cpp"

using namespace std;

std::string a = "hello";
std::map<std::string, int> opts;

class TestRunner : public CxxTest::TestSuite {
	Sta * sta;
	map<string, long double>  stats;
public:
	void setUp() {
		opts["sum"] = 1;
		opts["N"] = 1;
		opts["min"] = 1;
		opts["max"] = 1;
		opts["mean"] = 1;
		opts["sd"] = 1;
		opts["sderr"] = 1;
		sta = new Sta(opts);
		insert_data(1,10);
		sta->compute_global_stats();
		stats = sta->get_global_stats();
	}
	void test_min(){
		TS_ASSERT_EQUALS(stats["min"], 1.0);
	}
	void insert_data (int begin, int end){
		for(int i = begin; i<=end; i++){
			sta->compute_line_stats(1);
		}
	}
};