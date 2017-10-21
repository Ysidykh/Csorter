
typedef enum{INTEGER, FLOAT, STRING} Type;

void merge( int *arr, int leftHead, int Middle, int rightHead, Type type)
{
  int i, j, k;
  int LeftTempSize = Middle - leftHead + 1;
  int RightTempSize =  rightHead - Middle;
  /* create temp arrays */
  int LeftTempArray[LeftTempSize], RightTempArray[RightTempSize];
  /* Copy data to temp arrays L[] and R[] */
  for (i = 0; i < LeftTempSize; i++)
    LeftTempArray[i] = arr[leftHead + i];
  for (j = 0; j <RightTempSize; j++)
    RightTempArray[j] = arr[Middle + 1+ j];
  /* Merge the temp arrays back into arr[l..r]*/
  i = 0; // Initial index of first subarray
  j = 0; // Initial index of second subarray
  k = leftHead; // Initial index of merged subarray
  while (i <LeftTempSize  && j < RightTempSize)
    {
      if (LeftTempArray[i] <= RightTempArray[j])
        {
	  arr[k] = LeftTempArray[i];
	  i++;
        }
      else
        {
	  arr[k] = RightTempArray[j];
	  j++;
        }
      k++;
    }
 
  /* Copy the remaining elements of L[], if there
     are any */
  while (i < LeftTempSize)
    {
      arr[k] = LeftTempArray[i];
      i++;
      k++;
    }
 
  /* Copy the remaining elements of R[], if there
     are any */
  while (j < RightTempSize)
    {
      arr[k] = RightTempArray[j];
      j++;
      k++;
    }
} 

void mergeSort(int *arr, int leftHead, int rightHead, Type type)
{
  
  if (leftHead < rightHead)
    {
      // Same as (l+r)/2, but avoids overflow for
      // large l and h
      int Middle = leftHead+(rightHead-leftHead)/2;
      // Sort first and second halves
      mergeSort(arr, leftHead, Middle, type);
      mergeSort(arr, Middle+1, rightHead, type);
      merge(arr, leftHead, Middle, rightHead, type);
    }
}

int compare(int x, int y)
{
  return(x>y);
}
/* 
int compare(float x, float y)
{
  return(x>y);
}

int compare(char x, char y)
{
  return(x>y);
}
*/
