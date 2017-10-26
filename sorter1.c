/*
 ============================================================================
 Name        : project1.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

///////////////////////// COLUMNS /////////////////////////////////

struct cols_array { //contains all data fields in one row
	char **arr; //array of data field pointers
	int length;
};

typedef struct cols_array Cols;

Cols * cols_create() { //allocate memory for the array of columns (empty initially)
	Cols *c = (Cols*)malloc(sizeof(Cols));
	c->length = 0; //length of zero
	c->arr = NULL; //points to nothing
	return c;
}

int cols_append(Cols *cols, char *c) {
	cols->arr = realloc(cols->arr, sizeof(char*)*(cols->length + 1)); //grows the array arr to accomodate a new pointer to a column
	cols->arr[cols->length] = c; //populates the newly allocated element within the array
	cols->length++;
	return cols->length; //TODO: returns an int for error handing (improper memory allocation)
}

void cols_print(Cols* cols) { //prints columns arr
	for(int i = 0; i < cols->length; i++){
		printf("column %d: %s\n",i,cols->arr[i]);
	}

}

void cols_populate(Cols *cols, char *buf) {
	char *p = buf; //points to start of buf
	char *col = buf; //initial column pointer
	int in_quote = 0; //custom boolean value
	while(*p) {
		if(*p == '"') {
			in_quote = 1 - in_quote; //toggle value of in_quote
		}
		if(*p == ',' && !in_quote) {
			*p = 0x00; //replaces comma with end of line char (0)
			cols_append(cols, col); //append to the cols array
			col = p + 1; //reposition col to point to next char after 0
		}

		p++;
	}
	cols_append(cols, col); //append to the cols array (for the final column)
}

void cols_destroy(Cols *cols) { //frees allocated col_arr memory
	free(cols->arr); //frees the memory allocated for the array arr
	free(cols);//frees the pointer to the array structure
}

///////////////////////////////// ROW STRUCTURE /////////////////////////
struct row {
	char *buf;
	Cols *cols;
};
typedef struct row Row; //struct 'row' alias
//ROW FUNCTIONS

Row * row_create(char *buffer) {
	//allocate memory for the row structure
	Row *r = (Row*)malloc(sizeof(Row));
	//allocate memory for the row's buf
	r->buf = (char*)malloc(strlen(buffer) + 1); //length + null byte
	//copy data from buffer into buf
	strcpy(r->buf, buffer);  //copy buffered chars into row's buf
	int l = strlen(r->buf); //removes end of line character
	if(r->buf[l-1] == 0x0a) { //checks CRLF (windows and Linux)
		//if(r->buf[strlen(r->buf)-1] == '\n') THIS ALSO WORKS
		r->buf[l-1] = 0;
	}
	if(r->buf[l-2] == 0x0d) { //checks CRLF (windows only)
		//if(r->buf[strlen(r->buf)-1] == '\r') THIS ALSO WORKS
		r->buf[l-2] = 0;
	}

	r->cols = cols_create();
	cols_populate(r->cols, r->buf);

	return r;
}
void row_destroy(Row* row) { //frees memory, contents first, and then structure
	cols_destroy(row->cols);
	free(row->buf);
	free(row);
}

void row_print(Row* row) { //prints out the contents of the row
	printf("Row has %d columns\n", row->cols->length);
	cols_print(row->cols);
	//printf("---- row ---- buf: >>[%s]<<\n", row->buf);DEBUG
}

int row_find_column(Row *row, char *str_to_find) { //returns index of str_to_find OR -1 if not found
	for(int i = 0; i < row->cols->length; i++) {
		if(strcmp(row->cols->arr[i],str_to_find) == 0) {
			return i;
		}
	}
	return -1;
}



/////////////////////APPENDABLE ARRAY OF ROWS //////////////////////

struct rows_array {
	Row **arr; //array of pointers to pointers to rows
	int length; //number of elements in array arr
};
typedef struct rows_array Rows; //rows_array alias

Rows * rows_create() { //allocate memory for the array of rows (empty initially)
	Rows * r = (Rows*)malloc(sizeof(Rows));
	r->length = 0; //length of zero
	r->arr = NULL; //points to nothing
	return r;
}

void rows_destroy(Rows* rows) { //frees memory of rows array
	//frees the memory that the Rows in the array point to
	int i = 0;
	while(i < rows->length) { //does nothing if length is zero (empty)
		row_destroy(rows->arr[i++]);
	}
	free(rows->arr); //frees the memory allocated for the array arr
	//frees the pointer to the array structure
	free(rows);
}

int rows_append(Rows* rows, Row* row) { //adds a row to the array and returns the new length of the array arr
	rows->arr = realloc(rows->arr, sizeof(*row)*(rows->length + 1)); //grows the array arr to accomodate a new pointer to a row
	rows->arr[rows->length] = row; //populates the newly allocated element with the row in the parameter
	rows->length++;
	return rows->length; //TODO: returns an int for error handing (improper memory allocation)
}

void rows_print(Rows* rows) { //prints entire rows structure

	printf("length of rows array: %d\n", rows->length);
	for(int i = 0; i < rows->length; i++) {
		printf("row %d: ",i+1);
		row_print(rows->arr[i]);
	}
}

////////////////////////MAIN FUNCTIONS //////////////////////

void print_usage(char *program_name) { //Default error string to print in case of improper program call
	printf("EXAMPLE USAGE:  cat input.file | %s -c  movie_title\n", program_name);
	printf("EXAMPLE USAGE:  %s -c  movie_title -d thisdir -o thatdir", program_name);
}

void exterminatus(Row *head, Rows* rows) {  //destroys everything and quits the program
	row_destroy(head);
	rows_destroy(rows);
}

int check_params(int argc, char **argv) {
	if(!(argc ==3 || argc == 7)) {
			print_usage(argv[0]);
			return -1;
		}
	if(strcmp(argv[1], "-c")) {
			print_usage(argv[0]);
			return -1;
		}
	return 1;
	//TODO:  FINISH THIS FUNCTION
}

void process_stream(FILE *fp, const char *sort) {
	Row *head; //heading (columns)
	char buffer[2048]; //input buffer
	Rows *rows = rows_create(); //rows array

	fgets(buffer,2047,fp); //header line (sort caregories)
	head = row_create(buffer);

	while(fgets(buffer,2047,fp)) {  //read entire stream
		rows_append(rows, row_create(buffer));
	}

	rows_print(rows); //placeholder behavior
	//TODO:  sort and output new rows array
	exterminatus(head, rows); //free everything allocated

}

void process_stdin(const char *sort) {
	process_stream(stdin, sort);
}

void process_file(char *file_name, const char *sort) {
	FILE *fp = fopen(file_name, "r");
	process_stream(fp, sort);
	fclose(fp);

}

void process_dir(char *in_dir_name, char *out_dir_name, const char *sort) {

}



///////////////////MAIN/////////////////////////////////////

int main(int argc, char **argv) {
	int check_params_result = check_params(argc, argv); //checks usage

	char *prog_name = argv[0];
	char *sort_string = argv[2];
	char *input_dir = argv[4];
	char *output_dir = argv[6];

	if(check_params_result < 0) { //error in usage
		print_usage(prog_name);
		exit(-1);
	} else if(check_params_result == 1){ //argc = 3 case
		process_stdin(sort_string);
	} else if(check_params_result == 2){  //argc = 7 case
		process_dir(input_dir, output_dir, sort_string);
	}
	exit(0);
}







/*    DEBUG THINGS BELOW

	//analyze input parameters
	if(!(argc ==3 || argc == 7)) {
		print_usage(argv[0]);
		exit(-1);
	}

	if(row_find_column(head, argv[2]) < 0) { //checks return value to be > 0 to ensure that it is part of the col->arr
		printf("PROGRAM TERMINATED\nSorting variable not found in column headers\n");
		print_usage(argv[0]);
		exit(-1);

	}



	//////////////////////////////////argc == 3 case

	if(strcmp(argv[1], "-c")) {
		print_usage(argv[0]);
		exit(-1);
	}

	//read head
	Row *head; //heading (columns)
	char buffer[2048]; //input buffer

	fgets(buffer,2047,stdin); //header line (sort caregories)
	head = row_create(buffer);
	//row_print(head); //DEBUG

	//take and store the 3rd paramter to sort by
	if(row_find_column(head, argv[2]) < 0) { //checks return value to be > 0 to ensure that it is part of the col->arr
		printf("PROGRAM TERMINATED\nSorting variable not found in column headers\n");
		print_usage(argv[0]);
		exit(-1);

	}


	//read data
	//TODO: sort data
	//output data into stdout



	fgets(buffer,2047,stdin); //header line (sort caregories)
	head = row_create(buffer);
	row_print(head); //DEBUG
	exit(42);


	Rows* rows = rows_create(); //create empty rows structure with empty array

	//Read the rest of the file line by line into a rows_array structure

	while(fgets(buffer,2047,stdin)) {
		rows_append(rows, row_create(buffer));
	}
	//DEBUG
	//rows_print(rows);
	//free all allocated memory

	for(int i=0; i< rows->length; i++) {
		printf("row %d has %d columns\n",i, rows->arr[i]->cols->length);
		if (rows->arr[i]->cols->length != 28) {
			printf("NOT 28");
		}

	}

	row_destroy(head);
	rows_destroy(rows);

	//done
	return EXIT_SUCCESS;


}

*/
