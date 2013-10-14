#! /usr/bin/perl -w

use Scalar::Util qw(looks_like_number); 
$n = 100000;
if($#ARGV > 1){
        $n = $ARGV[1];
}
if(defined($ARGV[0])){
	$n = $ARGV[0];
	if (!looks_like_number($n)){
		print $ARGV[0] , " is not a number\n";
		exit()
	}
}

for(1..$n){
	print rand(), "\n";
}
