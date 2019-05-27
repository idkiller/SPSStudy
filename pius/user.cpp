#include <stdio.h>
#include <time.h>

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

	void print() {
		for (unsigned int i = 0; i < size; i++) {
			printf("%d: %d, %d, %d, %d\n", data[i].furnitureID, data[i].requestTime, data[i].price, data[i].makingTime, data[i].waitingTime);
		}
	}

	T data[N];
	unsigned int size = 0;
};

static Furniture furnitures[10000];
static int craftsman_count = 0;
static int currentTime = 0;
static int craftman[31];
static int restman = 0;

bool work_comp(int& a, int& b) {
	return a < b;
}

bool stock_comp(int& a, int& b) {
	Furniture fa = furnitures[a];
	Furniture fb = furnitures[b];

	int va = fa.price / fa.makingTime;
	int vb = fb.price / fb.makingTime;
	return va > vb;
}


void sleep(int ms) {
	clock_t goal = ms + clock();
	while (goal > clock());
}

static PriorityQue<int, work_comp, 31> working;
static PriorityQue<int, stock_comp, 10000> stock;

static int COUNT = 0;
void init(int N) {
	printf("%d\n", COUNT++);
	craftsman_count = N;
	restman = craftsman_count;
	working.reset();
	stock.reset();

	for (int i = 0; i < craftsman_count; i++) {
		craftman[i] = -1;
	}
}

void request(int furnitureID, int requestTime, int price, int makingTime, int waitingTime) {
	furnitures[furnitureID] = {furnitureID, requestTime, price, makingTime, waitingTime, -1, false};
	stock.push(furnitureID);
	printf("------------------- [%d] request - %d, %d, %d, %d\n", currentTime, furnitureID, price, makingTime, waitingTime);
	//stock.print();
	//sleep(2000);
}

#define TIMEOUTED(fid) ((furnitures[fid].requestTime + furnitures[fid].waitingTime) < (currentTime + furnitures[fid].makingTime))

inline void startMakingEx(int mid, int fid) {
	if (craftman[mid] == -1) restman--;
	craftman[mid] = fid;
	startMaking(mid + 1, fid);
	furnitures[fid].makingTime--;
}

void tick(int t) {
	currentTime = t;
	int fid;

	for (int i = 0; i < craftsman_count; i++) {
		if (craftman[i] == -1) {
			if (stock.pop(fid) && !TIMEOUTED(fid)) {
				startMakingEx(i, fid);
			}
			else {
				break;
			}
		}
		else {
			if (furnitures[craftman[i]].makingTime == 0) {
				furnitures[craftman[i]].done = true;
				completeMaking(i + 1);
				craftman[i] = -1;
				restman++;
				if (stock.pop(fid) && !TIMEOUTED(fid)) {
					startMakingEx(i, fid);
				}
			}
			else if (furnitures[craftman[i]].makingTime > 0 && restman == 0) {
				if (!stock.empty() && stock_comp(stock.data[0], craftman[i])) {
					stock.pop(fid);
					if (!TIMEOUTED(fid)) {
						startMakingEx(i, fid);
					}
				}
				else {
					furnitures[craftman[i]].makingTime--;
				}
			}
		}
	}
}