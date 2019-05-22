/// ============== user code start ==============
typedef struct Data {
	int strike;
	int ball;
} Data;
/// ============== submit code start ==============

extern Data query(int supose[]);

//////////////////////////////////////////////////////////////////////////////////////////
#define MAX_QLEN     5050

#define QUEUE1       0
#define QUEUE2       1

int front[2];
int rear[2];
int queue[2][MAX_QLEN];
int queueState = QUEUE1;

void queue1init(void)
{
	front[0] = 0;
	rear[0] = 0;
}

void queue2init(void)
{
	front[1] = 0;
	rear[1] = 0;
}

void queueInit(void)
{
	queueState = QUEUE1;
	queue1init();
	queue2init();
}

int queueIsEmpty(int n)
{
	return front[n] == rear[n];
}

int queueIsFull(int n)
{
	if ((front[n] + 1) % MAX_QLEN == rear[n])
		return 1;
	else
		return 0;
}

int queueEnqueue(int n, int value)
{
	if (queueIsFull(n)) {
		return 0;
	}

	queue[n][front[n]] = value;
	front[n]++;
	if (front[n] == MAX_QLEN)
	{
		front[n] = 0;
	}

	return 1;
}

int queueDequeue(int n, int *value)
{
	if (queueIsEmpty(n)) {
		return 0;
	}

	*value = queue[n][rear[n]];
	rear[n]++;
	if (rear[n] == MAX_QLEN)
	{
		rear[n] = 0;
	}
	return 1;
}
//////////////////////////////////////////////////////////////////////////////////////////

#define MAX          4

struct BaseballData {
	int data[MAX];
};

int dataVisited[10] = { 0 };

BaseballData baseballData[MAX_QLEN];
int baseballIndex = 0;

int createBaseballData(int a, int b, int c, int d)
{
	int currentIndex = baseballIndex;
	BaseballData *newData = &baseballData[currentIndex];

	newData->data[0] = a;
	newData->data[1] = b;
	newData->data[2] = c;
	newData->data[3] = d;

	baseballIndex++;

	return currentIndex;
}

//////////////////////////////////////////////////////////////////////////////////////////

void initData(int *buf, int index)
{
	if (index >= MAX) {
		int ballIndex = createBaseballData(buf[0], buf[1], buf[2], buf[3]);
		queueEnqueue(queueState, ballIndex);
		
		return;
	}

	for (register int i = 0; i < 10; ++i) {
		if (dataVisited[i] == 0) {
			dataVisited[i] = 1;
			buf[index] = i;
			initData(buf, index + 1);
			dataVisited[i] = 0;
		}
	}
}

void init(void)
{
	queueInit();

	for (register int i = 0; i < 10; ++i)
		dataVisited[i] = 0;

	baseballIndex = 0;
}

void tryBest(int suppose[]) {
	int buf[MAX] = { 0 };
	int index = 0;
	Data aResult;
	BaseballData aData;
	int dataCheck[10];

	init();	
	initData(buf, 0);

	while (1) {
		queueDequeue(queueState, &index);
		aData = baseballData[index];

		for (register int c = 0; c < 10; ++c)
			dataCheck[c] = 0;

		for (register int i = 0; i < MAX; ++i) {
			suppose[i] = aData.data[i];
			dataCheck[aData.data[i]]++;
		}
		aResult = query(suppose);

		if (aResult.strike == 4)
			return;

		while (!queueIsEmpty(queueState)) {
			BaseballData bData;
			Data bResult;

			bResult.strike = 0;
			bResult.ball = 0;

			queueDequeue(queueState, &index);
			bData = baseballData[index];

			for (register int c = 0; c < MAX; ++c) {
				if (aData.data[c] == bData.data[c])
					bResult.strike++;
				else if (dataCheck[bData.data[c]] > 0)
					bResult.ball++;
			}

			if (aResult.strike == bResult.strike &&
				aResult.ball == bResult.ball) {
				queueEnqueue((queueState == QUEUE1) ? QUEUE2 : QUEUE1, index);
			}
		}

		queueState = (queueState == QUEUE1) ? QUEUE2 : QUEUE1;
	}
}