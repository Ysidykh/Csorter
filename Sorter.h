#ifndef SORTINGLIB_H
#define SORTINGLIB_H 
#define MakeStringType  typedef char* String;
void merge(int arr[], int leftHead, int Middle, int rightHead);
void mergeSort(int arr[], int leftHead, int rightHead);
void copyArray(int Source[], int Destination[], int N);
 typedef struct MovieStruct
 {
   char* Color;
   char* Director;
   float NumberOfCritics;
   float Duration;
   float DirectorFacebookLikes;
   float Actor3FaceBookLikes;
   char* Actor2Name;
   float Actor1FacebookLikes;
   float Gross;
   char* Genre;
   char* Actor1Name;
   char* MovieTitle;
   float NumberOfVotedUsers;
   float CastTotalFacebookLikes;
   char* Actor3Name;
   float FacesOnPoster;
   char* Keywords;
   char* IMDBLink;
   float UserReviews;
   char* Language;
   char* Country;
   double ContentRating;
   float Budget;
   float TitleYear;
   float Actor2FacebookLikes;
   float IMDBScore;
   float AspectRatio;
   float MovieFaceBookLikes;
 }Movie;
typedef enum{INTEGER, FLOAT, STRING} Type;
typedef enum{TRUE, FALSE} Bool;
#endif
