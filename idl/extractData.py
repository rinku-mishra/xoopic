#!/usr/bin/python
#data = open("
import sys

#
# after the line that starts with the start string
# the data follows
#
start = '#####'

#
# check if we have three arguments on the command line
#
comLineArgs = len(sys.argv)

if ( comLineArgs != 4 ):
    print "The number of arguments is ", comLineArgs
    print "This number must be equal to 4"
    print "usage: $extractDataSet Number InputFileName OutputFileName"
    print "Number: the number of the data set to extract, e.g. 1,"
    print "InputFileName: the file that contains all data sets,"
    print "OutputFileName: the file to write the extracted set to."

#
# open the file name to read the data set
#
print
print "Opening file: ", sys.argv[2], " for extraction of set ", sys.argv[1]
print
setNumber = int(sys.argv[1])

data = open(sys.argv[2],'r')
dataLines = data.readlines()

i = 0
lineCounter = -1
numberDataLines = len(dataLines)

for line in dataLines :
    lineCounter = lineCounter + 1
    if (line[0:5] == start) : 
        i = i + 1
        if ( i == setNumber ) :
            #
            # open the output file to write
            #
            outFile = open(sys.argv[3],'w')
            print "File ", sys.argv[3], " opened for writing of set ", i
            lineNumber = lineCounter+1
            while ( lineNumber < numberDataLines and len(dataLines[lineNumber]) > 1 ) :
                outFile.write(dataLines[lineNumber])
                lineNumber = lineNumber + 1
            outFile.close()
data.close()

#for s in sys.argv :

