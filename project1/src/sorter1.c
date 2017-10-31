/*
 ============================================================================
 Name        : project1.c
 Author      : Yevgeniy Sidykh/Lorenzo Gomez
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h> //why not
#include <ctype.h>

int isLatinScript( short C)
{
  return (C>=32 && C<=126);
}
/*
 Compares two strings.
Compares X to Y.
If X is less than or equal to Y, then the function returns 1. Otherwise it returns 0.
This function expects X and Y to be in the context of the Latin Script; the only input that would be valid is anything that belongs to the Latin Script in the unicode set.
 */
int Compare(char *X, char *Y)
{
  int Comparison = 1;
  float str1Num;
  float str2Num;
  int i = 0;
  int j = 0;
  char str1[strlen(X)+1];
  char str2[strlen(Y)+1];
  for(i = 0;i<strlen(X);i++)
    {
      if(isLatinScript(X[i])&& isalnum(X[i]))
	{
	  str1[j] = X[i];
	  j++;
	}
    }
  j = 0;
  for(i =0; i<strlen(Y);i++)
    {
      if(isLatinScript(Y[i]) && isalnum(Y[i]))
	{
	  str2[j] = Y[i];
	  j++;
	}
    }
  if( ( (isdigit(str1[0]) && (isdigit(str1[strlen(str1)-1])))) &&( (isdigit(str2[0])) && (isdigit(str2[strlen(str2) - 1]))) )
    {
      str1Num = strtof( str1, NULL);
      str2Num = strtof(str2, NULL);
      if(str1Num>str2Num)
	{
	  Comparison = 0;
	  printf("%.1f is greater than %.1f\n", str1Num, str2Num);
	}
    }
  else
    {

	       int temp =  strcmp(str1, str2);
	       if(temp>0)
		 {
                   Comparison= 0;
		 }
    }

  return Comparison;
}

///////////////////////// COLUMNS /////////////////////////////////

struct cols_array { //contains all data fields in one row
	char **arr; //array of data field pointers
	int length;
};

typedef struct cols_array Cols;

Cols *cols_create() { //allocate memory for the array of columns (empty initially)
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

Row *row_create(char *buffer) {
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

Rows *rows_create() { //allocate memory for the array of rows (empty initially)
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

int check_params(int argc, char **argv) {  //checks for existence and proper sequence of parameters

	char *sort_string = argv[2];
	char *input_dir = argv[4];
	char *output_dir = argv[6];

	if(!(argc ==3 || argc == 7)) { //check number of parameters
			print_usage(argv[0]);
			return -1;
		}
	//3 case, check for -c and sorting_column is not empty (done for 3 AND 7)
	if(strcmp(argv[1], "-c") || (*sort_string == '\0')) {
			print_usage(argv[0]);
			return -1;
		};

	if(argc == 7) {  //if 7, also check for -d -o and directory strings
		if(strcmp(argv[3], "-d") || strcmp(argv[5], "-o")) {
			print_usage(argv[0]);
			return -1;
		}
		if ((*input_dir == '\0') || (*output_dir == '\0')) {
			return -1;
		}
	}
	if(argc == 3) {return 1;};
	if(argc == 7) {return 2;};
	return -1;
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

///////////////////DIRECTORY PROCESS FUNCTIONS AND STRUCTS////////////////////

struct file_names_array { //file name string storage array
	char ** arr;
	int length;

};
typedef struct file_names_array Files;

Files *files_create() {
	Files *f = (Files*)malloc(sizeof(Files));
		f->length = 0; //length of zero
		f->arr = NULL; //points to nothing
		return f;
}

int files_append(Files *files, char *filename) {
	char *str = (char*)malloc(sizeof(char) * (strlen(filename) + 1)); //allocate space (somewhere) for the filename char*
	strcpy(str, filename);
	files->arr = realloc(files->arr, sizeof(char*) * (files->length + 1)); //just add a pointer
	files->arr[files->length] = str; //put str pointer into arr (filename string)
	files->length++;
	return files->length;
}

void files_destroy(Files * files) {
	for (int i = 0; i < files->length; i++) {
		free(files->arr[i]); //free all space allocated for file name strings
	}
	free(files->arr); //free array of pointers
	free(files);  //free the structure
}

void files_print(Files * files) {
	for(int i = 0; i < files->length; i ++) {
		printf("CSV %d: %s\n",i+1, files->arr[i]);
	}

}

char *dir_strcat(char *buffer, char *start, char *end) { //modified strcat() function for corrected directory paths
	buffer = (char*)realloc(buffer, sizeof(char) * (strlen(end) + strlen(start) + 2)); //allocates space for new string of size start + end + slash
	strcpy(buffer, start);
	int l = strlen(buffer);
	if(buffer[l -1 ] != '/') {  //IF there is no slash after start string, add one
		buffer[l + 1] = 0x00;
		buffer[l] = '/';
	}
	strcat(buffer, end);
	return buffer;
}

Files *dir_read(Files *files, char *dir_string) {

	char *dirname_buffer = malloc(sizeof(char) * (strlen(dir_string) + 1 ));
	char *filename_buffer = NULL;
	strcpy(dirname_buffer, dir_string);
	DIR *dp = opendir(dir_string);
	struct dirent *ep;

	if(dp == NULL) {
		printf("Could not open directory '%s', try again please. \n", dir_string);
		return NULL;
	}
	printf("-------------PROCESSING DIRECTORY---------%s----\n", dir_string);
	while ((ep = readdir(dp))) {
		//append the file name to the dir_string if the last 4 letters are '.csv' and then add it to the files array
		int len = strlen(ep->d_name);
		if(ep->d_type == DT_REG) {
			printf("%s, %d This is a regular file.",ep->d_name,ep->d_type);
			if(len >= 5) {
				char *s = ep->d_name + len - 4;
				if (strcmp(s,".csv") == 0) { //TODO:  take care of case sensitivity
					printf(" It is a CSV!\n");
					int files_length = files_append(files, dir_strcat(filename_buffer, dir_string, ep->d_name));
					printf("Files array reallocated.  New length: %d\n",files_length);
				}  else {
					printf(" It is not a CSV...\n");
				};
			}
		} else if(ep->d_type == DT_DIR) {
			if(!((strcmp(ep->d_name, ".") == 0) || (strcmp(ep->d_name, "..")== 0))) {
				printf("%s, %d This is a directory to process.\n",ep->d_name,ep->d_type);

				dir_read(files, dir_strcat(dirname_buffer, dir_string, ep->d_name));
			}
		} else {
			printf("%s, %d This is some other thing  It gets ignored...\n",ep->d_name,ep->d_type);
		}
	}

	closedir(dp);
	free(dirname_buffer);
	free(filename_buffer);
	return files;
}





void process_dir(char *in_dir_name, char *out_dir_name, const char *sort) {
	Files *files = files_create();  //array of csv filename strings
	dir_read(files, in_dir_name);

	files_print(files);

	//read dir into internal storage array of file names
	//loop through internal storage array
	//if another dir is found, go into it and restart

}



//////////////////SORT/////////////////////////////////////

void merge(Rows *arr, int leftHead, int Middle, int rightHead, int ColumnIndex)
{
  int i, j, k;
  int LeftTempSize = Middle - leftHead + 1;
  int RightTempSize =  rightHead - Middle;
  /* create temp arrays */
  Row *LeftTempArray[LeftTempSize], *RightTempArray[RightTempSize];
  /* Copy data to temp arrays L[] and R[] */
  for (i = 0; i < LeftTempSize; i++)
    LeftTempArray[i] = arr->arr[leftHead + i];
  for (j = 0; j <RightTempSize; j++)
    RightTempArray[j] = arr->arr[Middle + 1+ j];
  /* Merge the temp arrays back into arr[l..r]*/
  i = 0; // Initial index of first subarray
  j = 0; // Initial index of second subarray
  k = leftHead; // Initial index of merged subarray
  while (i <LeftTempSize  && j < RightTempSize)
    {
      //      printf("Comparing %s and %s\n", LeftTempArray[i]->cols->arr[ColumnIndex], RightTempArray[j]->cols->arr[ColumnIndex]);
      if (Compare(LeftTempArray[i]->cols->arr[ColumnIndex], RightTempArray[j]->cols->arr[ColumnIndex]))
        {
	  arr->arr[k] = LeftTempArray[i];
	  i++;
        }
      else
        {
	  arr->arr[k] = RightTempArray[j];
	  j++;
        }
      k++;
    }

  /* Copy the remaining elements of L[], if there
     are any */
  while (i < LeftTempSize)
    {
      arr->arr[k] = LeftTempArray[i];
      i++;
      k++;
    }

  /* Copy the remaining elements of R[], if there
     are any */
  while (j < RightTempSize)
    {
      arr->arr[k] = RightTempArray[j];
      j++;
      k++;
    }
}
/*
 */
void mergeSort(Rows *arr, int leftHead, int rightHead, int ColumnIndex)
{
  if (leftHead < rightHead)
    {
      // Same as (l+r)/2, but avoids overflow for
      // large l and h
      int Middle = leftHead+(rightHead-leftHead)/2;
      // Sort first and second halves
      printf("Sorting %s\n", arr->arr[leftHead]->cols->arr[ColumnIndex]);
      mergeSort(arr, leftHead, Middle, ColumnIndex);
      mergeSort(arr, Middle+1, rightHead, ColumnIndex);
      merge(arr, leftHead, Middle, rightHead, ColumnIndex);
    }
}



///////////////////MAIN/////////////////////////////////////

int main(int argc, char **argv) {
	int check_params_result = check_params(argc, argv); //checks usage
	printf("\n----PARAMS RESULT------->%d\n", check_params_result);


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
