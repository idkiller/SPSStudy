/// ============== user code start ==============
#if 1
typedef struct Data {
	int strike;
	int ball;
} Data;

#endif // 1

extern Data query(int supose[]);

static int numbers[5040][4];
static int *target;

static bool numbers_init = false;

static int *q1[5040];
static int *q2[5040];
static int q1_count = 0;
static int q2_count = 0;

static int checks[10];

bool compare(int a[4], int b[4], int strike, int ball) {
	int strike2 = 0, ball2 = 0;
	for (int i = 0; i < 4; i++) {
		if (a[i] == b[i]) strike2++;
		else if (checks[a[i]] == 1) ball2++;
	}
	return strike2 == strike && ball2 == ball;
}

void tryBest(int suppose[]) {
	if (!numbers_init) {
		int count = 0;
		for (int a = 0; a <= 9; a++) {
			for (int b = 0; b <= 9; b++) {
				for (int c = 0; c <= 9; c++) {
					for (int d = 0; d <= 9; d++) {
						if (a != b && a != c && a != d && b != c && b != d && c != d) {
							numbers[count][0] = a;
							numbers[count][1] = b;
							numbers[count][2] = c;
							numbers[count][3] = d;
							count++;
						}
					}
				}
			}
		}
		numbers_init = true;
	}

	for (int i = 0; i < 5040; i++) {
		q1[i] = numbers[i];
	}


	int **que = q1;
	int **nque = q2;
	int count = 5040;

	for (int i = 0; i < 10; i++) checks[i] = 0;
	while (true) {
		target = que[0];
		for (int i = 0; i < 4; i++) {
			checks[target[i]] = 1;
		}
		Data data = query(target);
		if (data.strike == 4) {
			suppose[0] = target[0];
			suppose[1] = target[1];
			suppose[2] = target[2];
			suppose[3] = target[3];
			break;
		}
		int ncount = 0;
		for (int i = 1; i < count; i++) {
			if (compare(que[i], target, data.strike, data.ball)) {
				nque[ncount++] = que[i];
			}
		}
		count = ncount;
		int **tmp = que;
		que = nque;
		nque = tmp;
		for (int i = 0; i < 4; i++) {
			checks[target[i]] = 0;
		}
	}
}
