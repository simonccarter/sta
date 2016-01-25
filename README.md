sta
==

Simple statistics from the command line interface (CLI), fast.

### Description

This is a lightweight, fast tool for calculating basic descriptive statistics from the command line. Inspired by https://github.com/nferraz/st, this project differs in that it is written in C++, allowing for faster computation of statistics given larger non-trivial data sets.

Additions include the choice of biased vs unbiased estimators and the option to use the compensated variant algorithm. 

Given a file of 1,000,000 ascending numbers, a simple test on a 2.5GHz dual-core MacBook using Bash `time` showed `sta` takes less than a second to complete, compared to 14 seconds using `st`.

### Installing sta

Run `./autogen.sh`, `./configure`, `make`, and `make install`.
 
### Usage

    sta [options] < file

#### Using sta

Imagine you have this sample file:

    $ cat numbers.txt
    1
    2
    3
    4
    5
    6
    7
    8
    9
    10

Running `sta` is simple: 

	$ sta < numbers.txt
	N	max	min	sum	mean	sd	
	100	10	1	55	5.5	2.87228	 

To extract individual bits of information:

	$ sta --sum --sd --var < numbers.txt
	sum	sd	var	
	55	2.87228	8.25

`sta`, by default, assumes you have a population of scores, and thus normalises with N. If in fact you have a sample of scores, and wish to know the expected population standard deviation/variance, i.e. normalise with N-1, then just add the `--sample` flag. See [Standard deviation estimation](http://en.wikipedia.org/wiki/Standard_deviation#Estimation), and [Population variance and sample variance](http://en.wikipedia.org/wiki/Variance#Population_variance_and_sample_variance):

	$ sta --sum --sd --var --sample < numbers.txt
	sum	sd	var	
	55	3.02765	9.16667	

Worried about precision? You can calculate variance instead using the [compensated variant algorithm](http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Compensated_variant): 

	$ sta --var --sample --compensated < numbers.txt

Want to compute quartiles? Run:

	$ sta --q < numbers.txt
	N	min	Q1	median	Q3	max	
	100	1	26	50.5	76	100	

How about percentiles? Run:

	$ sta --p 50,61 < numbers.txt 
	50th	61th	
	51	62

Don't want to see the column names? Run:

	$ sta --q --brief < numbers.txt
	100	1	100	5050	50.5	29.0115

To transpose the output, run:

	$ sta --q --transpose < numbers.txt
	N	100
	min	1
	Q1	26
	median	50.5
	Q3	76
	max	100

To supply your own delimiter, run:

	$ sta --delimiter $'\t\t' --sd --sum < numbers.txt 
	sum		sd		
	55		2.87228

or

	$ sta --delimiter XXX --sd --sum < numbers.txt 
	sumXXXsd
	55XXX2.87228

### Formatting

`sta` works with long doubles, and can process numbers in the following formats:
	
	4.7858757E-39
	4.7858757e-39
	4.7858757

To change the output notation to fixed, supply the --fixed flag.

	$ sta --fixed  < numbers.txt
	N	min	max	sum	mean	sd	sderr	
	10.000000	1.000000	10.000000	55.000000	5.500000	2.872281	0.908295

### Options

	--brief
	--compensated
	--fixed
	--mean
	--median
	--min
	--max
	--percentiles	
	--q
	--q1
	--q3
	--sample
	--sd
	--sderr
	--sum
	--transpose
	
#### Performance Testing.

The example directory contains 2 scripts to create some example files:

	$./examples/create_example_asc.pl n > some_file_with_n_numbers_asc  

and

	$./examples/create_example_rand.pl n > some_file_with_n_numbers_rand  

To see how long `st` or `sta` takes to output the various statistics, call:

	$./examples/time_sta.sh examples/large_file_1m	

and

	$./examples/time_st.sh examples/large_file_1m	

### ToDo

	--Add online variant
	--Add confidence intervals for mean, var, and sd. This should allow for a user supplied interval. 
	
### Contributing

I've not written C++ in a long time, so please do send comments, suggestions and bug reports to:

https://github.com/simonccarter/sta/issues

Or fork the code on github:

https://github.com/simonccarter/sta

#### Testing.

I've recently integrated a basic testing platform using [CxxTest](http://cxxtest.com/). 
You'll need to download CxxTest and set the required environment variables to run the tests.

To build the tests, run the `buildTester.sh` file in the test directory.
Then run `$ ./tester` to run the tests.

Tests are not as extensive as they could be, and contributions are welcome here as well as the main code.
