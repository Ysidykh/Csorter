#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"Sorter.h"
#include <string.h>
//void copyArray(int Source[], int Destination[], int N);
//void mergeSort(int **Source,int l, int r, int Columns);
//void TopDownSplitMerge(int WorkArray[], int iBegin, int iEnd, int SourceArray[]);
//void TopDownMerge(int SourceArray[], int iBegin, int iMiddle, int iEnd,int WorkArray[]);
//int  min(int Num1, int Num2);
int main(int argc, char* argv[])
{
  printf("Hi, I am running"); 
  void *VoidVar = malloc(100);
  char X = 'P';
  char Y = 'K';
  VoidVar = &(X);
  VoidVar++;
  VoidVar = &(Y);   
  printf("Void Data: %d\n", &VoidVar);
  printf("Void Data Adress #2:%d \n", &VoidVar+2);
  MakeStringType;
  int Array2D[3][3]= {{1,0,-2},
                    {76,23,5},
		    {3,10,8}};
  typedef enum{False, True} bool; 
  int NumberOfRows = 1;
  int NumberOfColumns = 0;
  int NumberOfChars = 100;
  int FileCode;  
  char* input = (char*) malloc(sizeof(char) * NumberOfChars);
  char* Data;
  char* tempMemory = NULL;
  int i  = 0;
  for(i =0;i<NumberOfChars;i++)
    {
      input[i] = '\0';
    }
  FileCode = getchar();
  if(FileCode <1)
    {
      printf("There has be an error while reading your file.");
      printf("\n");
      printf("Please try again later.\n");  
      return 0;
    } 
  *input = FileCode;
  i =1; 
  /*
  typedef struct MovieStruct
  {
    String Color;
    String Director;
    float NumberOfCritics;
    float Duration;
    float DirectorFacebookLikes;
    float Actor3FaceBookLikes;
    String Actor2Name;
    float Actor1FacebookLikes;
    float Gross;
    String Genre;
    String Actor1Name;
    String MovieTitle;
    float NumberOfVotedUsers;
    float CastTotalFacebookLikes;
    String Actor3Name;
    float FacesOnPoster;
    String Keywords;
    String IMDBLink;
    float UserReviews;
    String Language;
    String Country;
    double ContentRating;
    float Budget;
    float TitleYear;
    float Actor2FacebookLikes;
    float IMDBScore;
    float AspectRatio;
    float MovieFaceBookLikes;   
  }Movie;
  */
  bool NewLine = False;
  do { 
    FileCode =  getchar();
    input[i] = FileCode;
    if(input[i] == '\n')
      {
	//	printf("New Line On Input\n");
	NumberOfRows++;
      }
    if((NewLine == False) && (input[i]== ','))
      {
	NumberOfColumns++;
      }
    else if(input[i]== '\n')
      {
	NewLine = True;
      }
    i++;
    if(i==NumberOfChars)
      {
	char* tempMemory = (char*) realloc(input, ( (sizeof(char))* (NumberOfChars *2)));
        while(tempMemory== NULL)
	  { 
	    // printf("Alllocating Memory");
	  }
        input = tempMemory;
	NumberOfChars = (NumberOfChars * 2);
      }
  }
  while (((i<NumberOfChars) &&(!(FileCode<1))) && (FileCode!=EOF));
  NumberOfColumns = NumberOfColumns+1;
  Data = malloc(sizeof(char) * NumberOfChars);
  i =0;
  int LoopNum = NumberOfChars; 
  for(i = 0; i<LoopNum;i++)
{
  Data[i] = '\0';
}
  i = 0;
  char  placeHolderChar = '0';
  char* placeHolder = &placeHolderChar; 
  int  j = 0;
  int tempIndex = 0;
  for(i =0;i<LoopNum || input[i]!='\0';i++)
{
  if(Data[i] == *placeHolder)
    {
      // if(i<(NumberOfChars-1))
	//     printf("Character after: %c", Data[i+1]);	
      continue;
    }
  else if(i<(LoopNum-1))
    {
      if(input[i] == '\n')
      {
	//	printf("New Line\n");
      }
      Data[i] = input[j];
     if((input[j] == '\n' && input[j+1]==','))
    { 
      Data[i+1] = placeHolderChar;
      Data[i+2] = ',';
      printf("Checking: First Case \n");
      i = (i+2);
      j = (j+1);
      if(i<30000)
	{ 
	  //printf( "%s \n", Data);
        }
    }
  else if((input[j] == ',' && input[j+1] == ','))
   {
        for(tempIndex  = j ;input[tempIndex] == ',' && input[tempIndex+1]== ','; tempIndex++)
    {
     Data[i+1] = placeHolderChar;
     Data[i+2] =input[tempIndex];
     i = (i+2);
    }
	j = tempIndex;
    // j = tempIndex;
	// j = (j+1);
     //     printf("Checking:Second Ccase \n");
     //     printf("%s", Data);    
   }
  else if(( (Data[i] == ',') && (Data[i+1] ==placeHolderChar)) ||((Data[i]==placeHolderChar) && (Data[i+1] == ',')) )
    {
    }
       
  else if(( (Data[i] == ',') && (Data[i+1] ==placeHolderChar)) ||((Data[i]==placeHolderChar) && (Data[i+1] == ',')) )
    {

    } 
    } 
  else if(( (Data[i] == ',') && (Data[i+1] ==placeHolderChar)) ||((Data[i]==placeHolderChar) && (Data[i+1] == ',')) )
    {
    }

  else if(( (Data[i] == ',') && (Data[i+1] ==placeHolderChar)) ||((Data[i]==placeHolderChar) && (Data[i+1] == ',')) )
    {

    }
  else
  { 
    Data[i] = input[i];
  }
  j++;
}
  //  for( i =0;i<LoopNum;i++)
{
  // printf("%c",Data[i]);
}
 
  //   printf("Number of rows:%d", NumberOfRows) ;
  // printf("Number of Columns:%d", NumberOfColumns);
  /*
   Store the whole file into a two-dimensional array. This way we can acess all of the rows/colums by referring to them as rows/columns.
  */
// for(i  =0; i<3000;i++)
   // printf("%c", Data[i]);
   
   char*  ListOfRows[NumberOfRows+2][NumberOfColumns+2];
    i =0; 
    j =0;
  int k =0;
  i =0;
  ListOfRows[i][j] = strtok (Data ,",\n");
  j++;  
  String temp;
  do {
    // printf ("%s\n",pch);       
    // printf("Indecies, %d, %d ", i, j);
    // String temp[1]
    ListOfRows[i][j] = strtok (NULL, ",\n");
    temp = ListOfRows[i][j];
    if(*temp == '"')
     {
       printf("\nQuotes:%s\n",temp);
     }
    j++;
    //  printf("%s", ListOfRows[i][j]); 
    if(j == NumberOfColumns)
      {
	j=0;
        i++;
      }
  }while(temp != NULL); 
  i =0;
  j =0;  
  // printf("%s", ListOfRows[1][2]);
  
  //  for(i = 0;i)
  {
  }
  String ListOfColumns[NumberOfColumns+2];
  /* 
 for(i =0;i<10;i++)
{

   printf("Row#%d", i+1);
  for(j =0;j<NumberOfColumns;j++)
{
  printf("%s,",ListOfRows[i][j]);
}
  printf("\n");
}  
  */
Movie* MyMovies = malloc(sizeof(Movie)*NumberOfRows);
 printf("Size of Movie Struct:%d", sizeof(Movie));
   i = 0;
   j = 0;
   printf("Data went well\n");
   for(i=1;i<NumberOfRows;i++) 
    {
      MyMovies[j].Color = ListOfRows[i][0];   
   MyMovies[j].Director = ListOfRows[i][1];
   MyMovies[j].NumberOfCritics = atof( ListOfRows[i][2]);
   MyMovies[j].Duration = atof(ListOfRows[i][3]);
   MyMovies[j].DirectorFacebookLikes = atof(ListOfRows[i][4]);
   MyMovies[j].Actor3FaceBookLikes =atof( ListOfRows[i][5]);
   MyMovies[j].Actor2Name = ListOfRows[i][6];
   MyMovies[j].Actor1FacebookLikes =atof( ListOfRows[i][7]);
   MyMovies[j].Gross =atof(ListOfRows[i][8]);
   MyMovies[j].Genre = ListOfRows[i][9];
   MyMovies[j].Actor1Name = ListOfRows[i][10];
   MyMovies[j].MovieTitle = ListOfRows[i][11];
   MyMovies[j].NumberOfVotedUsers = atof(ListOfRows[i][12]);
   MyMovies[j].CastTotalFacebookLikes = atof(ListOfRows[i][13]);
   MyMovies[j].Actor3Name = ListOfRows[i][14];
   MyMovies[j].FacesOnPoster =atof(ListOfRows[i][15]);
   MyMovies[j].Keywords =ListOfRows[i][16];
   MyMovies[j].IMDBLink = ListOfRows[i][17];
   MyMovies[j].UserReviews = atof(ListOfRows[i][18]);
   MyMovies[j].Language = ListOfRows[i][19];
   MyMovies[j].Country = ListOfRows[i][20];
   MyMovies[j].ContentRating = atof(ListOfRows[i][21]);
   MyMovies[j].Budget =atof( ListOfRows[i][22]);
   MyMovies[j].TitleYear =atof( ListOfRows[i][23]);
   MyMovies[j].Actor2FacebookLikes =atof( ListOfRows[i][24]);
   MyMovies[j].IMDBScore = atof(ListOfRows[i][25]);
   MyMovies[j].AspectRatio = atof(ListOfRows[i][26]);
   MyMovies[j].MovieFaceBookLikes = atof(ListOfRows[i][27]);
   j++;   
}
   printMovies(MyMovies, 64,64);
  ///  printf("size of command line argument: %ud", sizeof(argv[0]));
  // printf(DoesThisColumnExist(argv[1],ListOfRows[0], sizeof(argv[])));
  //  mergeSort((int**) *ListOfRows, 0, NumberOfRows, compareColumns(argv[1], ListOfRows[0], NumberOfColumns));
//  mergeSort(Array2D, 0, 2, 3, 1);
  i =0;
  j =0;
  //  printMovies((MyMovies),3,3);
  //   printf("%s ,%s,%d, %d, ",MyMovies[4].Color ,MyMovies[4].Director ,MyMovies[4].NumberOfCritics, MyMovies[4].Duration);  
  
 // for(i =0;i<NumberOfRows;i++)
	// {
  //   for(j =0;j<NumberOfColumns;j++)
	 // printf("%s", ListOfRows[i][j]);
	 //   printf("\n");
	 //    }
      // printMovies(MyMovies);
  // printf("Comparing Columns %d", compareColumn(argv[]));  
  free(Data);
  free(input);
  return 0;
}

void nullTerminate(char* strPointer, int start, int finish)
{
  int i = start;
  for(i =start;i<=finish;i++)
    {
      strPointer[i] = '\0';
     
    }

}
int compareColumns(char* column, char* Columns[], int NumberOfColumns)
{
  int i =0;
  for(i =0;i<NumberOfColumns;i++)
    {
      if(strcmp(column, Columns[i]) == 0)
	{
	  return i;
	}
    }
  i = -1;
  return i;


}

void printMovies(struct MovieStruct  *Movieptr, int beginning, int ending)
{ 
  int j = beginning;
  for(j  = beginning;j<=ending;j++)
{
  printf("%s, %s, %f, %f, %f, %f, %s, %f, %f, %s, %s, %s, %f, %f, %s, %f, %s, %s, %f, %s, %s, %s, %f, %f, %f, %f, %f, %f \n", 
         Movieptr[j].Color,
	 Movieptr[j].Director ,
	 Movieptr[j].NumberOfCritics,
	 Movieptr[j].Duration,
	 Movieptr[j].DirectorFacebookLikes,
	 Movieptr[j].Actor3FaceBookLikes,
	 Movieptr[j].Actor2Name,
	 Movieptr[j].Actor1FacebookLikes,
	 Movieptr[j].Gross,
	 Movieptr[j].Genre,
	 Movieptr[j].Actor1Name,
	 Movieptr[j].MovieTitle,
	 Movieptr[j].NumberOfVotedUsers,
	 Movieptr[j].CastTotalFacebookLikes,
	 Movieptr[j].Actor3Name,
	 Movieptr[j].FacesOnPoster,
	 Movieptr[j].Keywords,
	 Movieptr[j].IMDBLink,
	 Movieptr[j].UserReviews,
	 Movieptr[j].Language,
	 Movieptr[j].Country,
	 Movieptr[j].ContentRating,
	 Movieptr[j].Budget,
	 Movieptr[j].TitleYear,
	 Movieptr[j].Actor2FacebookLikes,
	 Movieptr[j].IMDBScore,
	 Movieptr[j].AspectRatio,
	 Movieptr[j].MovieFaceBookLikes);
}

}
