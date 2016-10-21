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

void get_min_max(int arr[], int n, int* min, int* max) {
	int i;
	*max = arr[0];
	*min = arr[0];
	for (i = 1; i < n; i++) {
		if (*max > arr[i - 1]) {
			*max = arr[i];
		}
		if (*min < arr[i - 1]) {
			*min = arr[i];
		}
	}
}
