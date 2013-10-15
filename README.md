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

`sta` runs in sample mode by default (meaning we normalise with N). If you want to use unbiased statistics, i.e. normalise with N-1, then just add the `--population` flag

	$ sta --sum --sd --var --population < numbers.txt
	sum	sd	var	
	55	3.02765	9.16667	

Worried about precision? You can calculate variance instead using the [compensated variant algorithm](http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Compensated_variant): 

	$ sta --var --population --compensated < numbers.txt

Want to compute quartiles? Run:

	$ sta --q < numbers.txt
	N	min	Q1	median	Q3	max	
	100	1	26	50.5	76	100	

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

### Formatting

`sta` works with long doubles, and can process numbers in the following formats:
	
	4.7858757E-39
	4.7858757e-39
	4.7858757

### Options

	--sum
	--mean
	--min
	--max
	--q
	--q1
	--q3
	--sd
	--sderr
	--population
	--compensated
	--brief

#### Testing.

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
	
### Contributing

I've not written C++ in a long time, so please do send comments, suggestions and bug reports to:

https://github.com/simonccarter/sta/issues

Or fork the code on github:

https://github.com/simonccarter/sta
