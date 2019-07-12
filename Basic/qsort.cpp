#include <stdio.h>
#include <stdlib.h>

void printIntArr(int* arr, int size) {
	for (int i = 0; i < size; i++) {
		printf("%.2d ", arr[i]);
	}
	printf("\n");
}

void swap(int* arr, int a, int b) {
	int tmp = arr[a];
	arr[a] = arr[b];
	arr[b] = tmp;
}

void qsort(int* arr, int start, int end) {
	if (start >= end) {
		return;
	}

	int pivot = start;
	int left = start;
	int right = end;

	while (left <= right) {
		while (left <= end && arr[left] <= arr[pivot]) left++;
		while (right > start && arr[right] >= arr[pivot]) right--;

		if (left <= right) {
			swap(arr, left, right);
		}
		else {
			swap(arr, pivot, right);
		}
	}
	qsort(arr, start, right - 1);
	qsort(arr, right + 1, end);
}


#define ARR_MAX 20
int main() {	
	int arr[ARR_MAX];

	srand(4912421);

	for (int i = 0; i < ARR_MAX; i++) {
		arr[i] = rand() % 100;
	}
	printIntArr(arr, ARR_MAX);
	qsort(arr, 0, ARR_MAX - 1);
	printIntArr(arr, ARR_MAX);

	return 0;
}
