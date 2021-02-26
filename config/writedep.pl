#!/bin/perl
# **********************************************************************
#
# Copyright (c) 2000
# Object Oriented Concepts, Inc.
# Billerica, MA, USA
#
# All Rights Reserved
#
# **********************************************************************

$top_srcdir=$ARGV[0];
open(UNIX, "> .depend") || die "Can't open .depend: $!";
# print "top_srcdir = ";
# print $top_srcdir;
# print "\n";

while(<STDIN>)
{
    if(/\\$/)
    {
	chop;
	chop;
	$line = $line . $_;
        # print $line; print "\n";
    }
    else
    {
	$_ = $line . $_;
        # print $_;
	
	# ($t, @d) = split(/ /);
	($t, @d) = split();
        # print $t . "\n";
        # print @d;

	foreach (@d)
	{
            # print $_ . "\n";
	    # if(!/^\//)
	    # {
                # print $_ . "\n";
		if( !/^\//  or
		    substr($_, 0, length("$top_srcdir/../")) eq "$top_srcdir/../" or
		    substr($_, 0, length("$top_srcdir/")) eq "$top_srcdir/" )
		{
		    $map{$t} .= " $_";
                    # print UNIX "$t $_" . "\n";
                    # print "$t $_" . "\n";
		}
	    # }
	}

	$line = ""
    }
    # print $line;
}

# open(WIN, "> .windep") || die "Can't open .windep: $!";

while(($key,$value) = each %map)
{
    print UNIX "$key$value\n";
    # print "$key$value\n";
    # $key =~ s/\.o/.obj/;
    # print WIN "$key$value\n";
}

close(UNIX);
# close(WIN);
