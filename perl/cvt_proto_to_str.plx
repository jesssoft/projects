#!/usr/bin/perl
#
# jesssoft
#
# usage: make_defs.plx defs.h > out.c
#
use warnings;
use strict;

my $c_comment_block = 0;

print "/*\n";
print " * 2015.07.14 jesssoft\n";
print " */\n";
print "#include \"proto_str.h\"\n";
print "\n";
print "const char *\n";
print "get_proto_str(uint16_t proto_no)\n";
print "{\n";
print "\tconst char *str;\n";
print "\n";
print "\tstr = 0;\n";
print "\tswitch(proto_no) {\n";
while (<>) {
	chomp;

	my $len = length $_;
	my @words = split(/[ \t]+/);

	if ($c_comment_block == 1) {
		# still in a comment block.
		if (/\*\//) {
			$c_comment_block = 0;
		}
		next;
	}

	# bypass elements (comments, empty lines)
	if ($len == 0) { 
		# don't delete (empty line)
		next;
	}
	
	my $comment = substr($words[0], 0, 2);
	if ($comment eq "//") {
		 # don't delete (comment)
		next;
	} elsif ($comment eq "/*") {
		# don't delete (c style comment)
		if (/\*\//) {
			# nothing to do - one line comment.
		} else {
			$c_comment_block = 1;
		}
		next;
	}	

	# substitue
	if (@words == 3 and $words[0] eq "#define") {
		my $len = length($words[1]);
		if ($len <= 6) {
			next;
		}

		my $prefix = substr($words[1], 0, 3);
		my $suffix = substr($words[1], -3, 3);
		my $ok = 0;

		if ($suffix eq "REQ") { $ok = 1; }
		if ($suffix eq "ACK") { $ok = 1; }
		if ($suffix eq "NTY") { $ok = 1; }
		if ($suffix eq "VAL") { $ok = 1; }
		if ($suffix eq "ASK") { $ok = 1; }
		if ($suffix eq "ANS") { $ok = 1; }
		if ($suffix eq "CMD") { $ok = 1; }

		if ($ok == 0) {
			next;
		}

		print "\tcase $words[2]:\n";
		print "\t\tstr = \"$words[1]\";\n";
		print "\t\tbreak;\n";
		next;
	}
}
print "\tdefault:\n";
print "\t\tstr = \"Unknown\";\n";
print "\t\tbreak;\n";
print "\t}\n";
print "\n";
print "\treturn str;\n";
print "}\n";


