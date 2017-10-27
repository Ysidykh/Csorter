NAME
		
				sorter.c -- sorts CSV files
				
SYNOPSIS

	A continuation of project0, this new program maintains the same functionality,
	and extends it depending on what parameters are passed into it.

	
USAGE
	SINGLE CSV FILE:
	cat [input_file.csv] | /a.out -c  [SORTING COLUMN]
	
	Takes a csv file that's piped into STNDIN as input, and outputs every row
	sorted according to the SORTING COLUMN which must match exactly with one of
	the 'header' strings stored in the first row of the CSV to STNDOUT.
	
	MULTIPLE CSV FILES:
	/a.out -c  [SORTING COLUMN] -d [INPUT DIRECTORY] -o [OUTPUT DIRECTORY]
	
	Finds all CSV files with the same amount of columns in the INPUT DIRECTORY	
	and all of its children, and starts a new process that parses, sorts, and
	outputs every file found as a new CSV into the OUTPUT DIRECTORY.
	
DATA STRUCTURE
	Either from a file or from STNDIN, each CSV file is read line by line into a
	buffer string.  Each string is put into a ROW element as is.  Each of the
	rows is then put into a scalable array of row pointers.  This is the basic
	structure.
	From there, each comma in the CSV file that seperates the individual data 
	fields is replaced with a end of line character.  This new internal row 
	buffer is passed through character by character, and each time an EOL char
	is found, a pointer is created and stored at that location in a new Col
	structure.  These pointers are stored in a scalable array of Col pointers.
	So internally, there is a growable pointer array that points to each row,
	and inside of each row, there is an array of pointers that points to each
	data field.
	This is a completely generic structure, and will function for CSV files of
	any size and any length, assuming that the length of each individual line
	of the original input does not exceed the char limit of the initial 
	read-in buffer (2047 chars).  Which should be plenty.  Should be.
	
SORTING ALGORITHM
	Yeah I'll get to that later.  Probably tomorrow.  Merge sort.  yay.
	
FORKING
	If the program finds 7 argv parameters, it will iterate itself through 
	the specificed INPUT DIRECTORY and find all CSV files that share the same
	header length (# of sortable columns) that also contains the SORTING COLUMN.
	Each file will be sorted individually and output into a new CSV file named
	[INPUTFILE]-sorted-[SORTING COLUMN].csv.
	These new files will not be printed, but when the program ends, it will print:
	
	Initial PID: XXXXX
	PIDS of all child processes: AAA,BBB,CCC,DDD,EEE,FFF, etc
	Total number of processes: ZZZZZ
	Have nice day.
	
TESTING PROCEDURE
	The most difficult part of this assignment was parsing a string into a scalable,
	lossless, discrete, and completely generic data structure that can hold
	CSV files of any size and length, and also be easy to sort through.
	Many things were tested for parsing: empty data fields, multiple commas, 
	commas at the end of every input line, escape characters within data fields, 
	different languages (cyrillic characters)...  We attempted to account for 
	every possible form of data corruption and grader trickyness.
	
	The crux of the entire parsing function is the first line of the CSV, which 
	is referred to as the HEAD.  It was also tested extensively for quotes, extra commas,
	case sensitivity, and null strings.  The head is what determines if a CSV is 
	"compatible" in the multiple directory sort.  The first line of every CSV found is
	stored in a row_array.  The program iterates through every head, dumps the files that
	don't contain the SORTING COLUMN, and forks a new sort for each one, with the added
	output file generated.
	