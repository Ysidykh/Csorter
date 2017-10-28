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
#include <ctype.h>                                                                                                                                                                                                

int isLatinScript( short C)
{
  return (C>=32 && C<=126);
}     
/*
 Compares two strings.
Compares X to Y.
If X is less than or equal to Y, then the function returns 1. Otherwise it returns 0.
This function expects X and Y to be in the context of the Latin Script; the only input that would be valid is anything that belongs to the Latin Scipr in the unicode set.   
 */             
int Compare(char *X, char *Y)
{
  int Comparison = 1;
  float XNum;
  float YNum;
  int i = 0;
  int j = 0;
  char str1[strlen(X)+1];
  char str2[strlen(Y)+1];
  if( ( (isdigit(X[0]) && (isdigit(X[strlen(X)-1])))) &&( (isdigit(Y[0])) && (isdigit(Y[strlen(X) - 1]))) )
    {
      XNum = strtof( X, NULL);
      YNum = strtof(Y, NULL);
      if(XNum>YNum)
	{
	  Comparison = 0;
	}
    }
  else
    {
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
//TODO:  cols destroy function

void cols_print(Cols* cols) { //prints columns arr
  int i =0;
	for(i = 0; i < cols->length; i++)
       {
		printf("column %d: %s\n",i,cols->arr[i]);
	}

}

void cols_populate(Cols *cols, char *buf) {
	char *p  = buf; //points to start of buf
	char *col = buf; //initial column pointer
	int in_quote = 0; //custom boolean value
	while(*p) {
		if(*p == '"') 
                {
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

	//TODO: parse and store column
	cols_populate(r->cols, r->buf);




	return r;
}
void row_destroy(Row* row) { //frees memory, contents first, and then structure
	free(row->buf);
	free(row);
}

void row_print(Row* row) { //prints out the contents of the row
	printf("Row has %d columns\n", row->cols->length);
	cols_print(row->cols);
	//printf("---- row ---- buf: >>[%s]<<\n", row->buf);
}

int row_find_column(Row *row, char *str_to_find) { //returns index of str_to_find OR -1 if not found
  int i= 0;
       for(i = 0; i < row->cols->length; i++) 
                {
		if(strcmp(row->cols->arr[i],str_to_find) == 0) {
			return i;
		}
		else
{
  // printf("Comparing %s and %s\n", str_to_find, row->cols->arr[i]);
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
	free(rows->arr[i]);
	i++;
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
        int i;
	for(i = 0; i < rows->length; i++) 
       {
		printf("row %d: ",i+1);
		row_print(rows->arr[i]);
	}
}
void num_rows_print(Rows* rows, int num)
{
  int i = 0;
  for(i = 0; i < num; i++)
    {
      printf("row %d: ",i+1);
      row_print(rows->arr[i]);
    }
}
////////////////////////MAIN FUNCTION //////////////////////

void print_usage(char *program_name) {
	printf("EXAMPLE USAGE:  cat input.file | %s -c  movie_title\n", program_name);
	printf("EXAMPLE USAGE:  %s -c  movie_title -d thisdir -o thatdir", program_name);
}

////////////////////////////////////////Generic Compare //////////////////////                                                                                                                                                       
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
      printf("Comparing %s and %s\n", LeftTempArray[i]->cols->arr[ColumnIndex], RightTempArray[j]->cols->arr[ColumnIndex]);
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

int main(int argc, char **argv) 
      {
	//IF there are 2 parameters, expect STDIN to run simple sort (project 0)
	//EXAMPLE USAGE:  cat input.file | ./sorter -c  movie_title

	//IF 6 parameters, run multiple CSV sort (project 1)
	//EXAMPLE USAGE:   ./sorter -c  movie_title -d thisdir -o thatdir
   
	Row *head; //heading (columns)
	//read first line into buffer
	char buffer[2048]; //input buffer
        printf("And she's like so whatever....");
	//analyze input parameters
	if(!(argc ==3 || argc == 7)) {
		print_usage(argv[0]);
		exit(-1);
	}


	//arc == 3 case
		//TODO: check first parameter is "-c"

		if(strcmp(argv[1], "-c")) {
			print_usage(argv[0]);
			exit(-1);
		}

		fgets(buffer,2047,stdin); //header line (sort caregories)
		head = row_create(buffer);
		//row_print(head); //DEBUG

		//take and store the 3rd paramter to sort by
		if(row_find_column(head, argv[2]) < 0) { //checks return value of the function
			printf("PROGRAM TERMINATED\nSorting variable not found in column headers\n");
			print_usage(argv[0]);
			exit(-1);

		}

		//	exit(42);

		//read data
		//sort data
		//output data into stdout
		int Column =  row_find_column(head, argv[2]);
		printf("Colunm Number: %d", Column);


	fgets(buffer,2047,stdin); //header line (sort caregories)
	head = row_create(buffer);
	//row_print(head); //DEBUG
	//	exit(42);
	Rows* rows = rows_create(); //create empty rows structure with empty array
	//Read the rest of the file line by line into a rows_array structure

	while(fgets(buffer,2047,stdin)) 
         {
		rows_append(rows, row_create(buffer));
	}
		mergeSort(rows, 0, 250, Column);
		num_rows_print(rows, 251);
	//DEBUG
	//		rows_print(rows);
	//free all allocated memory
        /*
	for(int i=0; i< rows->length; i++) {
		printf("row %d has %d columns\n",i, rows->arr[i]->cols->length);
		if (rows->arr[i]->cols->length != 28) {
			printf("NOT 28");
		}

	}
	*/
       

	row_destroy(head);
	rows_destroy(rows);

	//done
	return EXIT_SUCCESS;


}

