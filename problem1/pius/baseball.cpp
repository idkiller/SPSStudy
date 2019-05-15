
typedef struct Data{
	int strike;
	int ball;
} Data;


extern Data query(int supose[]);

static int numbers[5039];
static int numbers_count = 0;

static int que1[5039];
static int q1_n = 0;
static int que2[5039];
static int q2_n = 0;

static int data[4];

void toArr(int n, int arr[4]) {
	arr[0] = n / 1000;
	arr[1] = (n - (n / 1000) * 1000) / 100;
	arr[2] = (n - (n / 100) * 100) / 10;
	arr[3] = (n - (n / 10) * 10);
}
int checkUnique(int arr[4]) {
	return arr[0] != arr[1] && arr[0] != arr[2] && arr[0] != arr[3] &&
		arr[1] != arr[2] && arr[1] != arr[3] &&
		arr[2] != arr[3];
}
Data status(int target[4], int arr[4]) {
	Data data = { 0, 0 };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (target[i] == arr[j]) {
				if (i == j) {
					data.strike++;
				}
				else {
					data.ball++;
				}
			}
		}
	}
	return data;
}

void permutate() {
	int arr[4] = {};
	for (int i = 123; i < 9876; i++) {
		toArr(i, arr);
		if (checkUnique(arr)) {
			numbers[numbers_count++] = i;
		}
	}
}

void tryBest(int suppose[]) {
	if (numbers_count == 0) {
		permutate();
	}

	int no = 0;
	for (q1_n = 0; q1_n < 5039; q1_n++) {
		que1[q1_n] = numbers[q1_n];
	}
	int arr[4];
	int target[4];
	int* que = que1;
	int* nque = que2;
	int* que_count = &q1_n;
	int* nque_count = &q2_n;
	while (true) {
		toArr(que[0], target);
		Data data = query(target);
		if (data.strike == 4) {
			break;
		}

		*nque_count = 0;

		for (int i = 1; i < *que_count; i++) {
			toArr(que[i], arr);
			Data d = status(target, arr);
			if (d.strike == data.strike && d.ball == data.ball) {
				nque[(*nque_count)++] = que[i];
			}
		}
		int* tmpque = nque;
		nque = que;
		que = tmpque;
		int* tmpque_count = nque_count;
		nque_count = que_count;
		que_count = tmpque_count;
	}
	for (int i = 0; i < 4; i++) {
		suppose[i] = target[i];
	}
}
