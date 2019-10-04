#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int& a, int& b)
{
	int tmp = a;
	a = b;
	b = tmp;
}

void myqsort(int* arr, int start, int end)
{
	if (start >= end) return;

	int pivot = start;
	int left = start + 1;
	int right = end;

	while (left < right)
	{
		while (left <= end && arr[left] <= arr[pivot]) left++;
		while (right >= start && arr[right] > arr[pivot]) right--;

		if (left < right)
		{
			swap(arr[left], arr[right]);
		}
	}

	swap(arr[start], arr[right]);

	myqsort(arr, start, right - 1);
	myqsort(arr, right + 1, end);
}

void print(int* arr, int size)
{
	for (int i = 0; i < size; i++)
	{
		printf("%.2d ", arr[i]);
	}
	printf("\n");
}

void test()
{
	static int count = 0;
	const int arr_size = 20;
	int arr[arr_size];

	srand(time(0) + count++);

	for (int i = 0; i < arr_size; i++)
	{
		arr[i] = rand() % 100;
	}
	//int arr[arr_size] = { 03, 95, 89, 82, 67, 88, 31, 49, 19, 31, 67, 80, 67, 40, 18, 26, 35, 83, 58, 98 };
	print(arr, arr_size);
	myqsort(arr, 0, arr_size - 1);
	print(arr, arr_size);
	printf("\n");
}

int main()
{
	for (int i = 0; i < 1000; i++)
	{
		test();
	}
	return 0;
}
