//#include <stdio.h>

extern bool startMaking(int masterID, int furnitureID);
extern bool completeMaking(int masterID);

struct Furniture {
	int furnitureID;
	int requestTime;
	int price;
	int makingTime;
	int waitingTime;
	int masterID;
	bool done;
};

template<typename T>
void swap(T& a, T& b) {
	T tmp = a;
	a = b;
	b = tmp;
}

template<typename T, bool C(T&, T&), unsigned int N>
class PriorityQue {
public:
	bool push(T& value) {
		if (size + 1 > N)
		{
			return false;
		}

		data[size] = value;
		unsigned int current = size;
		unsigned int parent = (size - 1) >> 1;

		while (current > 0 && C(data[current], data[parent])) {
			swap(data[current], data[parent]);
			current = parent;
			parent = (size - 1) >> 1;
		}
		size++;
		return true;
	}

	bool pop(T& out) {
		if (size < 1) return false;

		out = data[0];
		size--;

		data[0] = data[size];
		unsigned int current = 0;
		unsigned int left = (current << 1) + 1;
		unsigned int right = (current << 1) + 2;
		unsigned int max = current;

		while (left < size) {
			if (!C(data[max], data[left])) {
				max = left;
			}
			if (right < size && !C(data[max], data[right])) {
				max = right;
			}
			if (max == current) {
				break;
			}
			else {
				swap(data[current], data[max]);
				current = max;
				left = (current << 1) + 1;
				right = (current << 1) + 2;
			}
		}

		return true;
	}

	bool empty() {
		return size == 0;
	}

	void reset() {
		size = 0;
	}

	T data[N];
	unsigned int size = 0;
};

static Furniture furnitures[10000];
static int craftsman_count = 0;
static int currentTime = 0;
static int craftman[32];
static int restman = 0;

#define STOCK_WEIGHT(fid) ((furnitures[fid].price * 1000) / furnitures[fid].makingTime)
bool stock_comp(int& a, int& b) {
	return STOCK_WEIGHT(a) > STOCK_WEIGHT(b);
}

static PriorityQue<int, stock_comp, 10000> stock;

static int pcraftman[32];

bool pwork_comp(int& a, int& b) {
	int ma = pcraftman[a];
	int mb = pcraftman[b];

	Furniture *fa = &furnitures[craftman[ma]];
	Furniture *fb = &furnitures[craftman[mb]];

	int ca = (fa->makingTime ? fa->price / fa->makingTime : 0) * (fa->requestTime + fa->waitingTime - currentTime);
	int cb = (fb->makingTime ? fb->price / fb->makingTime : 0) * (fb->requestTime + fb->waitingTime - currentTime);

	return ca > cb;
}

void craftmanSort(int start, int end) {
	if (start >= end) {
		return;
	}

	int left = start;
	int right = end;
	int pivot = left;
	int tmp;

	while (left <= right) {
		while (left <= end && !pwork_comp(left, pivot)) {
			left++;
		}
		while (right > start && pwork_comp(right, pivot)) {
			right--;
		}

		if (left > right) {
			tmp = pcraftman[right];
			pcraftman[right] = pcraftman[pivot];
			pcraftman[pivot] = tmp;
		}
		else {
			tmp = pcraftman[left];
			pcraftman[left] = pcraftman[right];
			pcraftman[right] = tmp;
		}
	}

	craftmanSort(start, right - 1);
	craftmanSort(right + 1, end);
}

void init(int N) {
	craftsman_count = N;
	restman = craftsman_count;
	stock.reset();

	for (int i = 0; i < craftsman_count; i++) {
		craftman[i] = -1;
	}
}

void request(int furnitureID, int requestTime, int price, int makingTime, int waitingTime) {
	furnitures[furnitureID] = {furnitureID, requestTime, price, makingTime, waitingTime, -1, false};
	stock.push(furnitureID);
	//printf("------------------- [%d] request - %d, %d, %d, %d\n", currentTime, furnitureID, price, makingTime, waitingTime);
	//stock.print();
	//sleep(2000);
}

#define TIMEOUTED(fid) ((furnitures[fid].requestTime + furnitures[fid].waitingTime) <= (currentTime + furnitures[fid].makingTime))

inline void startMakingEx(int mid, int fid) {
	if (craftman[mid] == -1) restman--;
	craftman[mid] = fid;
	startMaking(mid + 1, fid);
}

/*
#include <time.h>

void sleep(int ms) {
	clock_t goal = ms + clock();
	while (goal > clock());
}
*/

void tick(int t) {
	currentTime = t;
	int fid;

	for (int i = 0; i < craftsman_count; i++) {
		pcraftman[i] = i;
	}
	craftmanSort(0, craftsman_count - 1);

	for (int m = 0; m < craftsman_count; m++) {
		int i = pcraftman[m];
		if (craftman[i] == -1) {
			if (stock.pop(fid) && !TIMEOUTED(fid)) {
				startMakingEx(i, fid);
			}
			else {
				continue;
			}
		}
		else {
			fid = craftman[i];
			if (furnitures[fid].makingTime == 0) {
				furnitures[fid].done = true;
				completeMaking(i + 1);
				craftman[i] = -1;
				restman++;
				if (stock.pop(fid) && !TIMEOUTED(fid)) {
					startMakingEx(i, fid);
				}
				/*
				else {
					printf("%d is timeouted\n", fid);
				}
				*/
			}
			else if (furnitures[fid].makingTime > 0 && restman == 0) {
				int tmp = fid;
				if (!stock.empty() && stock_comp(stock.data[0], fid)) {
					stock.pop(fid);
					if (!TIMEOUTED(fid)) {
						//printf("[%d] changed %d(%d) > %d(%d)\n", currentTime, fid, WORK_WEIGHT(fid), tmp, WORK_WEIGHT(tmp));
						startMakingEx(i, fid);
					}
					/*
					else {
						printf("%d is timeouted\n", fid);
					}
					*/
				}
			}
		}
		furnitures[fid].makingTime--;
	}
}