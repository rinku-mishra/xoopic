#!/bin/bash

if [ "$1" == "help" ]; then
  # display short help
  echo "Script parses all CPP files in current directory and displays all blocks in input file and parameters for each block. For each parameter is displayed: "
  echo "  - parameter name"
  echo "  - parameter default value (if defined in source file)"
  echo "  - short comment about parameter purpose use (if defined in source file)"
  echo
  echo "Run script inside 'advisor' directory in XOOPIC to get all variables and their default values that can be set in input file."
  echo "You can redirect script's output to a file or via pipe to less (e.g $0 | less)."
  echo
  exit
fi

F1=__list1.tmp
F2=__c_files.tmp

# get files that have parameters
grep -n -e "setNameAndDescription" *.cpp | awk -F: '{ print $1 }' | sort -u > ${F2}

for i in `cat ${F2}`; do
	# go through all files
		# get class in current file
		#CLASS=`cat ${i} | grep "::" | head -1 | sed "s/::.*//"`
	# get name of input file block
	CLASS=`cat ${i} | grep "name = " | head -1 | sed 's/.*"\(.*\)\".*/\1/'`
	
	if [ -z "$CLASS" ]; then
	  # empty name (name not define - probably class providing basic functionality and other classes derive from current class); get class name
      CLASS=`cat ${i} | grep "::" | head -1 | sed "s/::.*//"`
      CLASS=`echo "$CLASS		// this is NOT input file name but Class name!"`
    fi
    
	echo "$CLASS"
	echo "{"
	
	# get parameters
	grep -n -e "setNameAndDescription" ${i} | awk -F: '{ print $1 }' > ${F1}

	for j in `cat ${F1}`; do
		# display all parameters
		LINE=`cat $i | head -${j} | tail -1`
		INPUT_FILE_NAME=`echo $LINE | sed 's/.*setNameAndDescription.*("\([a-zA-Z0-9_-]*\)"[)]*,.*/\1/g'`
		VARNAME=`echo $LINE | sed 's/\(.*\).setNameAndDescription.*/\1/g'`
		# get variable default value
		DEFAULT_VALUE=`cat $i | head -$(($j+10)) | tail -10 | grep -e "^[[:space:]]*${VARNAME}.setValue" | head -1 | sed 's/.*setValue(\(.*\));/\1/g' | sed 's/.*[(]*"\(.*\)"[)]*/\1/g'`
		
		echo -n "  ${INPUT_FILE_NAME} = ${DEFAULT_VALUE}	// "

		IS_ALL=`echo $LINE | sed "s/.*\();\)/\1/"`

		# for others, comment is last parameter
		if [ "${IS_ALL}" == ");" ]; then
			# all is defined in current line
			COMM=`echo $LINE | sed 's/.*\"\(.*\)\"[)]*;.*/\1/'`
		else
			#  comment is defined in next line
			NUM=`echo $j | awk -F: '{ print $1 }'`
			NUM=$(($NUM+1))
			LINE=`cat $i | head -${NUM} | tail -1`
			COMM=`echo $LINE | sed 's/.*\"\(.*\)\"[)]*;.*/\1/'`
		fi

		echo ${COMM}
	done

	echo -e "}\n"
done

rm -f ${F1} ${F2}

