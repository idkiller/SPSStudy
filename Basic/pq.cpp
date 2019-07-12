#include <stdio.h>
#include <stdlib.h>

struct Foobar {
	int x;
};

static Foobar pq[10000];
static unsigned int pq_size = 0;
static unsigned int pq_max = 10000;

void swap(Foobar &a, Foobar &b) {
	Foobar tmp = a;
	a = b;
	b = tmp;
}

bool compare(Foobar& a, Foobar& b) {
	return a.x < b.x;
}

bool push(Foobar &f) {
	if (pq_size + 1 > pq_max) {
		return false;
	}

	pq[pq_size] = f;

	unsigned int current = pq_size;
	unsigned int parent = (current - 1) >> 1;

	while (current > 0 && compare(pq[current], pq[parent])) {
		swap(pq[current], pq[parent]);
		current = parent;
		parent = (current - 1) >> 1;
	}
	pq_size++;
	return true;
}

bool pop(Foobar &out) {
	if (pq_size < 1) return false;

	out = pq[0];
	pq_size--;

	pq[0] = pq[pq_size - 1];

	unsigned int current = 0;
	unsigned int left = (current << 1) + 1;
	unsigned int right = (current << 1) + 2;
	unsigned int max = current;

	while (left < pq_size) {
		if (!compare(pq[max], pq[left])) {
			max = left;
		}
		if (right < pq_size && !compare(pq[max], pq[right])) {
			max = right;
		}

		if (max == current) {
			break;
		}
		else {
			swap(pq[current], pq[max]);
			current = max;
			left = (current << 1) + 1;
			right = (current << 1) + 2;
		}
	}
	return true;
}

int main() {

	printf("pq test\n");
	srand(49123421);
	Foobar x;
	for (int i = 0; i < 20; i++) {
		x.x = rand() % 100;
		push(x);
		printf("%d ", x.x);
	}
	printf("\n");

	for (int i = 0; i < 20; i++) {
		pop(x);
		printf("%d ", x.x);
	}
	printf("\n");

	return 0;
}
