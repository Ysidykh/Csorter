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
#include <pthread.h>
#include "sorter.h"
#include <sys/syscall.h>


int Children = 0;
int Status;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* TestThread(void* X)
{
  printf("Hello, testing thread!\n");
}
struct file_name {
	char *dir;
	char *name;
};
typedef struct file_name FileName;
struct sort_file_struct
{
FileName* Name;
char* outPath;
char* sort;
}typedef SortFile;
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
void sortfile_create(SortFile* sf ,FileName* fn , const char* sort_arg, const char* outPath_arg)

{

  //  printf("FileName size %d", FileName);
  sf->Name = (FileName*) malloc(sizeof(FileName));
  sf->Name->dir = (char*)malloc(sizeof(char) * (strlen(fn->dir)+ 1 ));
  sf->Name->name = (char*)malloc(sizeof(char) * (strlen(fn->name)));
    sf->sort =(char*) malloc(sizeof(char) * ( strlen(sort_arg) + 1 ));
  sf->outPath = (char*) malloc(sizeof(char) * ( strlen(outPath_arg) + 1) );
  memcpy(sf->Name, fn, sizeof(FileName) );  
  strcpy(sf->Name->dir, fn->dir);
  strcpy(sf->Name->name, fn->name);
  strcpy(sf->sort, sort_arg);
  strcpy(sf->outPath, outPath_arg);
  return;
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
	for (i = 0; i < files->length; i++) 
        {
		file_name_destroy(files->arr[i]); //free all space allocated for FileName structs
	}
	free(files->arr); //free array of pointers
	free(files);  //free the structure
}

void files_print(Files * files) 
{ 
//Debug output
	int i;
	for(i = 0; i < files->length; i ++) {
		file_name_print(files->arr[i]);
	}
}

char *dir_strcat(char *buffer, char *start, char *end) { //modified strcat() function for corrected directory paths
	buffer = (char*)realloc(buffer, sizeof(char) * (strlen(end) + strlen(start) + 2)); //allocates space for new string of size start + end + slash
	strcpy(buffer, start);
	int l = strlen(buffer);
	if(buffer[l -1 ] != '/') 
	{  
	    //IF there is no slash after start string, add one
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

	if(dp == NULL) 
	{
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

void* process_file(void* Args) 
{
  
  pthread_mutex_trylock(&mutex);
        SortFile* data = (SortFile*)Args;
        char* sortedStr = "-sorted-";
        char* Extension  = ".csv";
        char* Slash  = "/";
	//pid_t pid = getpid();	
	//printf("%d, ", pid);		
	char in_file_name[strlen(data->Name->dir) + strlen(data->Name->name) + 1];
	char out_file_name[(strlen(data->Name->name)+strlen(data->outPath)+strlen(Slash)+ strlen(sortedStr)+strlen(data->sort) + strlen(Extension))* 2 + ( 1)];
	//create output file name
	strcpy(out_file_name, data->outPath);
	strcat(out_file_name, Slash);
	printf("Out_file name: %s\n", data->Name->name );
	strncat(out_file_name, data->Name->name, strlen(data->Name->name) - 4);
	strcat(out_file_name, sortedStr);
	strcat(out_file_name, data->sort);
	strcat(out_file_name, Extension);
	printf("output file name/path: %s\n", out_file_name);
	//create input file name
	sprintf(in_file_name, "%s/%s", data->Name->dir, data->Name->name);
	//printf("input file name/path: %s\n", in_file_name);
	FILE *in = fopen(in_file_name, "r");
	FILE *out = fopen(out_file_name, "w");
	//	process_stream(in, out, data->sort);
	//free(data);
	printf("Current Thread: %ld\n", (long int)syscall(224));
	 Row *head; //heading (columns)
	char buffer[2048]; //input buffer
	Rows *rows = rows_create(); //rows array

	fgets(buffer,2047, in); //header line (sort categories)
	head = row_create(buffer);

	while(fgets(buffer,2047,in)) {  //read entire stream
		rows_append(rows, row_create(buffer));
	}
	int sort_col_index = row_find_column(head, data->sort);
	mergeSort(rows, 0, rows->length - 1, sort_col_index);
	rows_print_csv(rows, head, out);
	exterminatus(head, rows); //free everything allocated	
	fclose(out);
	fclose(in);
	pthread_mutex_unlock(&mutex);
	pthread_exit(0);
}
 FileName* getFileName(Files * files, int i)
 {
 return(files->arr[i]);
 }
 void setFileName( SortFile* file, FileName* arg_Name)
 {
//file->Name = arg_Name;
 }
void process_dir(char *in_dir_name, char *out_dir_name, const char *sort) 
{
	//pid_t pid = getpid();
	//printf("=== process_dir BEGIN --- pid=%d\n", pid);
	int i;
	printf("Processing dir...");
	char ignore_dir_name[1024];
	Files *files = files_create();  //array of csv filename strings
	sprintf(ignore_dir_name, "%s/%s", in_dir_name, out_dir_name); // don't read output dir
	dir_read(files, in_dir_name, ignore_dir_name);
	//files_print(files); //Debug
	 //	 pthread_t StackThreads[files->length];	
	pthread_t Threads[files->length];
        pthread_t myThreads[files->length];
	for(i = 0;i<files->length;i++)
{
  Threads[i] = &(myThreads[i]);
  printf("pthread stored: %d\n", &Threads[i]);
}
       SortFile** fileArgs = (SortFile**)malloc(sizeof(SortFile) * (files->length));	
       printf("Size of pthread_t %d", sizeof(pthread_t));
       if(Threads == NULL)
	 {
	 printf("why is this NULL!!!!");
         }
        void*(*fp[files->length])(void*);
        for(i =0;i<files->length;i++)
{
  fp[i] = &process_file;
}
       //  fp = process_file;
       //  FileName* temp;
	  printf("I have %d threads\n", files->length);
	for(i = 0; i < files->length; i++) 
	{
		//printf("=== process_dir pid=%d PROCSSING FILE #%d\n", pid, i);
		{
		  sortfile_create(*(fileArgs+i),files->arr[i], sort,out_dir_name);
		    printf("Name:%s\n",fileArgs[i]->Name->name, out_dir_name);
		    if( pthread_create( &(Threads[i]) , NULL, fp[i],(void*)fileArgs[i]) != 0)
{
  printf("There was an error creating thread #%d\n", i);
}	        
		
		    pthread_join(Threads[i], NULL);

} 
	}
	/*
		for(i = 0; i < files->length; i++) 
		{
		   pthread_join(Threads[i], NULL);                   
 		}
	*/
		for(i = 0; i < files->length; i++)
		  {
		    //   free(&(Threads[i]));
		    // free( fileArgs[i]); 
                  }	
		//	free(fileArgs);
               
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
		

			process_dir(input_dir, output_dir, sort_string);					
	}
	 
exit (0);
}
