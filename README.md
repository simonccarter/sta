sta
==

simple statistics from the command line interface (CLI), fast.

### Description

This tool is a lightweight, basic, fast tool for calculating basic descriptive stastics from the command line. Inspired by https://github.com/nferraz/st, this project differs in that it is written in c++, allowing for the fast computation of statistics given large data sets. 

#### Using st

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

To run sta, 

	$ cat numbers.txt | sta
	N	max	min	sum	mean	sd	
	0	10	1	55	5.5	2.87228	 









