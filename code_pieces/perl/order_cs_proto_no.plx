#!/usr/bin/perl
#
# jesssoft
#
# usage: order_cs_proto_no.plx defs.h > out.cs
#
use warnings;
use strict;

my $no = 1;
my $c_comment_block = 0;
while (<>) {
	chomp;

	my $seed_sign;
	my $len = length $_;
	my @words = split(/[ \t]+/);

	if ($c_comment_block == 1) {
		# still in a comment block.
		if (scalar(@words) >= 3) {
			$seed_sign = substr($words[2], 0, 1);
			if ($seed_sign eq "@") {
				# get the seed number for protocol no.
				$no = int($words[3]);
			}
		}

		print ("$_\n");
		if (/\*\//) {
			$c_comment_block = 0;
		}
		next;
	}

	#
	# bypass elements (comments, empty lines)
	#
	if ($len == 0) { 
		# don't delete (empty line)
		print "\n";
		next;
	}
	
	$seed_sign = substr($words[0], 0, 4);
	if ($seed_sign eq "///@") {
		# get the seed number for protocol no.
		$no = int($words[1]);
		print "$_\n";
		next;
	}
	
	my $comment = substr($words[0], 0, 2);
	if ($comment eq "//") { 
		# don't delete (c++ style comment)
		print "$_\n";
		next;
	} elsif ($comment eq "/*") {
		# don't delete (c style comment)
		if (/\*\//) {
			# nothing to do - one line comment.
		} else {
			$c_comment_block = 1;
		}
		print "$_\n";
		next;
	}	

	#
	# substitue
	#
	if (@words == 3 and $words[0] eq "#define") {
		my $len = length($words[1]);
		if ($len <= 6) {
			print "$_\n";
			next;
		}

		my $prefix = substr($words[1], 0, 3);
		my $suffix = substr($words[1], -3, 3);
		if ($prefix ne "CS_" and $prefix ne "DB_" and 
		    $prefix ne "AS_" and $prefix ne "HP_") {
			print "$_\n";
			next;
		}

		my $ok = 0;
		if ($suffix eq "REQ") { $ok = 1; }
		if ($suffix eq "ACK") { $ok = 1; }
		if ($suffix eq "NTY") { $ok = 1; }
		if ($suffix eq "VAL") { $ok = 1; }
		if ($suffix eq "ASK") { $ok = 1; }
		if ($suffix eq "ANS") { $ok = 1; }
		if ($suffix eq "CMD") { $ok = 1; }

		if ($ok == 0) {
			print "$_\n";
			next;
		}

		my $org = $words[$#words];
		my $val = $_;
		$val =~ s/$org/$no/g;

		print "$val\n";
		$no += 1;

		next;
	}

	print "$_\n";
}

