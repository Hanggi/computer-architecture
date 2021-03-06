//------------------------------------------------------------
//
//  4190.308-002: Computer Architecture (Fall 2016)
//
//  Skeleton code for Lab #2: Great human compiler
//
//  Oct 21, 2016
//  Instructor: Jae W. Lee (jaewlee@snu.ac.kr)
//  Seoul National University
//
//------------------------------------------------------------

#include <stdio.h>
#include <malloc.h>

void get_min_max (int arr[], int n, int* min, int* max);

int main()
{
  int* arr;
  int min = 0;
  int max = 0;
  int i, n;

  printf("Enter the number of integers: ");
  scanf("%d", &n);

  arr = (int*)malloc(sizeof(int)*n);

  printf("Enter the integers: ");

  for(i=0; i<n; i++) scanf("%d", &arr[i]);

  get_min_max (arr, n, &min, &max);

  printf("min: %d, max: %d\n", min, max);

  return 0;
}
