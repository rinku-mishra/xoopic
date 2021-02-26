#!/usr/bin/perl -w
#
# This script constructs a file 'Makefile.depend' that describes
# all dependencies among a list of Fortran90 files given on the
# command line.
#
  
# for handling command-line options
use Getopt::Long;
 
# usage
$usage=join('',
"\n",
"Usage: depend.pl [-h,--help] [-o output-file] f90-source-files\n",
"\n");
$usageFull=join('',$usage,
"This script constructs a file 'Makefile.depend' that describes\n",
"all dependencies among a list of Fortran90 files given on the\n",
"command line.  It does this by examining all files for lines\n",
"of the form\n",
"\n",
"    module my_module\n",
"\n",
"and\n",
"\n",
"    use my_module\n",
"\n",
"The dependencies are given in the form\n",
"   file1.o file2.o : modfile1.o\n",
"   file1.o file3.o file4.o : modfile2.o\n",
"   ...\n",
"\n",
"  -h,--help      : print a short or verbose help message and quit\n",
"  -o output-file : write results to named file; if this option is\n",
"                   absent, results are written to \'Makefile.depend\'.\n",
"\n");
 
unless(@ARGV) {die $usage;}
  
# parse command-line
#   options
Getopt::Long::Configure("bundling","permute");
if (!GetOptions("h"     => \$help,
                "help"  => \$verbose_help,
                "o=s"   => \$outfile)) {die $usage;}
#   help
if ($help) {die $usage;}
if ($verbose_help) {die $usageFull;}
#   output file-name
if (defined($outfile)) {
  unless ($outfile=~/^[\w-]+\.?[\w-]+$/) {
    print STDERR "\n\"$outfile\" is not a valid filename.\n",
                 "Filename must match /^[\\w-]+\.?[\\w-]+\$/.\n";
    die $usage;
  }
} else {$outfile="Makefile.depend";}
#   source files
unless(@ARGV) {die $usage;}
@srcfiles=(); @junk=();
while ($arg=shift @ARGV) {
  if (-e $arg) {push @srcfiles,$arg;}
  else {push @junk,$arg;}
}
if (@junk) {
  print STDERR "Warning: The following files don't exist:\n";
  print STDERR "  @junk\n";
}
 
# parse source files for information about modules and use of modules
%modules = ();  # key<-module file name, val=ref to module name array
%needs = ();    # key<-module name, val=ref to file name array
foreach $file (@srcfiles) {
  open(FILE,$file) or die "Can't read file $file: $!.\n";
  @modules = ();
  while(<FILE>){
    if (/^\s*module (\w*)\s*/i) {push(@{$modules{$file}},$1);}
    if (/^\s*use (\w*)\s*/i) {push(@{$needs{$1}},$file)}
  }
  close(FILE);
}

# open output file
open (OUT,"> $outfile") or die "Can't open file $outfile for writing: $!.\n";
$outFH = *OUT;
 
# parse %modules and %needs to extract and write dependencies
foreach $modfile (sort keys %modules) {
  @uniqmods=unique(@{$modules{$modfile}});
  @files=();
  foreach $module (@uniqmods) {
    push(@files,@{$needs{$module}});
  }
  @uniqfiles=unique(@files);
  foreach $file (sort @uniqfiles) {
    $file=~s/\.f$/.o/i;
    $file=~s/\.f90$/.o/i;
    print $outFH "$file ";
  }
  $modfile=~s/\.f$/.o/i;
  $modfile=~s/\.f90$/.o/i;
  print $outFH ": $modfile\n\n";
}
 
# close output file
close($outFH);
 
#
# subroutines
#

# return array containing unique elements of argument array
sub unique {
  my @array=@_;
  my %seen = ();
  my @uniq = ();
  foreach $a (@array) {
    push (@uniq, $a) unless $seen{$a}++;
  }
  return @uniq;
}

