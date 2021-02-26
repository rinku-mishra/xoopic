#!/usr/bin/python
#----------------------------------------------------------------------------
#
# File:        extract3DData.py
#
# Purpose:
#
# Extracts specific columns of
# data from a *txt file produced with OOPICPro on a 3D plot.
#
# For example, after dumping Ez data. These files have a
# header lines and end lines. Each of these lines starts
# with '*'. These lines must be droped. The rest of the
# lines contain the data. The format is: m, n, x, y, functionValue.
#
# The first two columns, m and n, contain the coordinates of
# function argument on the computational grid. They are integers.
#
# The second two columns, x and y, contain the coordinates in
# physical, real, units.
#
# The last column contains the values of the function at the
# specified coordinates.
#
# Usage: extract3DData.py n0 n1 n2 inputFileName, outputFileName
# where n0, n1, n3 are the columns to extract, these should be integers
# in the range [1,5], the input and output file names.
#
# Version: $Id: extract3DData.py 1926 2002-06-19 16:52:47Z dad $
#
# Copyright 2002 by Tech-X Corporation
#
#----------------------------------------------------------------------------
import sys
import string
import os

#
# check if we have three arguments on the command line
#
comLineArgs = len(sys.argv)
for arg in sys.argv :
  print arg
  
if ( comLineArgs != 6 ):
  print 
  print "USAGE: extract3DData.py n0 n1 n2 InputFileName OutputFileName"
  print "n0, n1, and n3 denote the columns of data to to extract."
  print "These should be integers in the range [1,5]"
  print "InputFileName must be a valid text output from a 3D diagnostics"
  print "produced by OOPICPro. OutputFileName is a user supplied name"
  print "for storing the output data."
  print 
  sys.exit(1)

inputFileName = sys.argv[4]
outputFileName = sys.argv[5]

filein = open(inputFileName,'r')
data = filein.readlines()
filein.close()

fileout = open(outputFileName,'w')

commentStart = '*'
n0 = string.atoi(sys.argv[1]) - 1
n1 = string.atoi(sys.argv[2]) - 1
n2 = string.atoi(sys.argv[3]) - 1

for line in data :
  if line[0] != commentStart :
    fields = string.split(line)
    fileout.write(fields[n0]+'\t'+fields[n1]+'\t'+fields[n2]+'\n')

fileout.close()

