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
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>



int Children = 0;
int Status;
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
	  //printf("%.1f is greater than %.1f\n", str1Num, str2Num);
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
	int i;
	for(i = 0; i < cols->length; i++){
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

int row_find_column(Row *row, const char *str_to_find) { //returns index of str_to_find OR -1 if not found
	int i;
	for(i = 0; i < row->cols->length; i++) {
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

void rows_print(Rows* rows) { //prints entire rows structure (debug)

	printf("length of rows array: %d\n", rows->length);
	int i;
	for(i = 0; i < rows->length; i++) {
		printf("row %d: ",i+1);
		row_print(rows->arr[i]);
	}
}

void rows_print_csv(Rows *rows, Row *head, FILE *out) { //final formatted output
	int i;
	//print header line
	for(i = 0; i < head->cols->length; i++){
			fprintf(out, "%s",head->cols->arr[i]);
			if (i < head->cols->length - 1){
				fprintf(out, ",");
			}

		}
	fprintf(out, "\n");

	for (i = 0; i < rows->length; i++) {
		int j;
		for(j = 0; j < rows->arr[i]->cols->length; j++) {
			fprintf(out, "%s",rows->arr[i]->cols->arr[j]);
			if (j < rows->arr[i]->cols->length - 1){
				fprintf(out, ",");
			}
		}
		fprintf(out, "\n");
	}

}

struct file_name {
	char *dir;
	char *name;
};
typedef struct file_name FileName;

FileName *file_name_create(char *dir, char *name) {
	FileName *fn = malloc(sizeof(FileName));
	fn->dir = (char*)malloc(sizeof(char) * (strlen(dir) + 1)); //allocate space for the dir
	fn->name = (char*)malloc(sizeof(char) * (strlen(name) + 1)); //allocate space for name
	strcpy(fn->dir, dir);
	strcpy(fn->name, name);
	return fn;
}

void file_name_destroy(FileName *fn) {
	free(fn->name);
	free(fn->dir);
	free(fn);
}

void file_name_print(FileName *fn) {
	printf(" ----   FileName: dir=[%s], name=[%s]\n", fn->dir, fn->name);
}


struct file_names_array { //file name string storage array
	FileName **arr;
	int length;

};
typedef struct file_names_array Files;

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



///////////////////DIRECTORY PROCESS FUNCTIONS AND STRUCTS////////////////////

Files *files_create() {
	Files *f = (Files*)malloc(sizeof(Files));
	f->length = 0; //length of zero
	f->arr = NULL; //points to nothing
	return f;
}

int files_append(Files *files, char *dir, char *name) {
	FileName *fn = file_name_create(dir, name);
	files->arr = realloc(files->arr, sizeof(char*) * (files->length + 1)); //just add a pointer
	files->arr[files->length] = fn; //put FileName pointer into arr (filename string)
	files->length++;
	return files->length;
}

void files_destroy(Files *files) {
	int i;
	for (i = 0; i < files->length; i++) {
		file_name_destroy(files->arr[i]); //free all space allocated for FileName structs
	}
	free(files->arr); //free array of pointers
	free(files);  //free the structure
}

void files_print(Files * files) { //Debug output
	int i;
	for(i = 0; i < files->length; i ++) {
		file_name_print(files->arr[i]);
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

Files *dir_read(Files *files, char *dir_string, char *ignore_dir_name) {

	char *dirname_buffer = malloc(sizeof(char) * (strlen(dir_string) + 1 ));
	char *filename_buffer = NULL;
	strcpy(dirname_buffer, dir_string);
	DIR *dp = opendir(dir_string);
	struct dirent *ep;

	if(strcmp(dir_string, ignore_dir_name) == 0) {
		//printf(" === !!!=== Won't read output dir '%s'\n", ignore_dir_name);
		return NULL;
	}

	if(dp == NULL) {
		printf("Could not open directory '%s', try again please. \n", dir_string);
		return NULL;
	}
	//printf("--- PROCESSING DIRECTORY [%s]\n", dir_string);
	while ((ep = readdir(dp))) {
		//append the file name to the dir_string if the last 4 letters are '.csv' and then add it to the files array
		int len = strlen(ep->d_name);
		if(ep->d_type == DT_REG) {
			//printf("%s, %d This is a regular file.",ep->d_name,ep->d_type);
			if(len >= 5) {
				char *s = ep->d_name + len - 4;
				if (strcmp(s,".csv") == 0) { //TODO:  take care of case sensitivity
					//printf(" It is a CSV!\n");
					files_append(files, dir_string, ep->d_name);
					//printf("Files array reallocated.  New length: %d\n",files_length);
					//files_print(files); // debug
				}  else {
					//printf(" It is not a CSV...\n");
				};
			}
		} else if(ep->d_type == DT_DIR) {
			if(!((strcmp(ep->d_name, ".") == 0) || (strcmp(ep->d_name, "..")== 0))) {
				//printf("%s, %d This is a directory to process.\n",ep->d_name,ep->d_type);
				dir_read(files, dir_strcat(dirname_buffer, dir_string, ep->d_name), ignore_dir_name);
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
      //printf("Sorting %s\n", arr->arr[leftHead]->cols->arr[ColumnIndex]);
      mergeSort(arr, leftHead, Middle, ColumnIndex);
      mergeSort(arr, Middle+1, rightHead, ColumnIndex);
      merge(arr, leftHead, Middle, rightHead, ColumnIndex);
    }
}

void process_stream(FILE *in, FILE *out, const char *sort) {

	Row *head; //heading (columns)
	char buffer[2048]; //input buffer
	Rows *rows = rows_create(); //rows array

	fgets(buffer,2047, in); //header line (sort categories)
	head = row_create(buffer);

	while(fgets(buffer,2047,in)) {  //read entire stream
		rows_append(rows, row_create(buffer));
	}

	int sort_col_index = row_find_column(head, sort);
	mergeSort(rows, 0, rows->length - 1, sort_col_index);
	rows_print_csv(rows, head, out);
	exterminatus(head, rows); //free everything allocated
}

void process_stdin(const char *sort) {
	process_stream(stdin, stdout, sort);
}

void process_file(FileName *fn, char *out_dir, const char *sort) {

	pid_t pid = getpid();
	printf("%d, ", pid);

	char in_file_name[1024];
	char out_file_name[1024];

	//create output file name
	strcpy(out_file_name, out_dir);
	strcat(out_file_name, "/");
	strncat(out_file_name, fn->name, strlen(fn->name) - 4);
	strcat(out_file_name, "-sorted-");
	strcat(out_file_name, sort);
	strcat(out_file_name, ".csv");
	//printf("output file name/path: %s\n", out_file_name);
	//create input file name
	sprintf(in_file_name, "%s/%s", fn->dir, fn->name);
	//printf("input file name/path: %s\n", in_file_name);

	FILE *in = fopen(in_file_name, "r");
	FILE *out = fopen(out_file_name, "w");

	process_stream(in, out, sort);

	fclose(out);
	fclose(in);

}

void process_dir(char *in_dir_name, char *out_dir_name, const char *sort) {

	//pid_t pid = getpid();

	//printf("=== process_dir BEGIN --- pid=%d\n", pid);

	int i;
	char ignore_dir_name[1024];

	Files *files = files_create();  //array of csv filename strings

	sprintf(ignore_dir_name, "%s/%s", in_dir_name, out_dir_name); // don't read output dir
	dir_read(files, in_dir_name, ignore_dir_name);

	//files_print(files); //Debug

	for(i = 0; i < files->length; i++) {
		//printf("=== process_dir pid=%d PROCSSING FILE #%d\n", pid, i);
		pid_t pid = fork();
		if(pid >= 0) {
			// child
			//printf("%d, ",pid);
			waitpid(pid, &Status, 0);
			Children += WEXITSTATUS(Status);
			process_file(files->arr[i], out_dir_name, sort);
		} else {
			// parent
			wait(NULL);
		}
	}
	//printf("=== process_dir END --- pid=%d\n", pid);
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
		
		pid_t pid = getpid();
		pid_t InitialId  = pid;
		printf("Initial PID: %d", pid);
		printf("PIDs of all child processes: ");
		if(pid != 0) 
		{
			process_dir(input_dir, output_dir, sort_string);
			
			printf("\n");
			}
			if(getpid()!=InitialId)
	 {
		Children++;
	 }
	 else
	 {
	 printf("Number of Children: %d", Children);
	 }
	}
	 
//	exit(0);
exit (Children);
}
