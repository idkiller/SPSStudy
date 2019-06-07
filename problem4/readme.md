# 휘발성 메모리

Time Limit 3000 ms

휘발성 메모리를 가진 장치를 만들려고 한다. 
이 장치는 평소에 전원이나 배터리가 없어 동작을 하지 않다가 명령을 수행할때만 전원이 연결되어 동작한다. 저장공간이 없기 때문에 전원이 끊어진 경우 메모리는 모두 지워진다. 
때문에 중요한 정보는 원격 저장 장치에 저장해야 한다.


**
※ user.cpp 내에 전역변수 및 static 변수를 선언하지 마시오. 
문제 조건인 전원 차단으로 인한 메모리 초기화를 가정하기 위해 전역 변수 및 static 변수를 선언하지 말고 원격 저장 장치(main.cpp)에서 제공하는 메모리를 사용하여 풀이를 진행해야 한다. usr.cpp의 함수 내 변수 선언은 가능하다.
(시스템적으로 제한하고 있지는 않습니다. 풀이 연습을 위해서 사용하지 마시길 바랍니다.)
**



main.cpp에서 제공하는 함수를 이용해서 원격 저장 장치에 접근할 수 있다.
최대 65,536 bytes의 저장공간이 제공된다.
다음 함수는 원격 저장 장치에서 제공되는 함수들이다.


```
bool memRead(unsigned char dest[], int pos, int len);
dest: 읽어들일 공간
pos: 읽어들일 위치 (0~65,535)
len: 읽어들일 크기 (1~65,536)
pos+len <= 65,536
메모리 특정 위치(pos)부터 시작해서 len만큼 읽어 dest에 쓴다.
```


```
bool memWrite(unsigned char src[], int pos, int len);
src: 저장할 data
pos: 저장할 위치 (0~65,535)
len: 저장할 크기 (1~65,536)
pos+len <= 65,536
메모리 특정 위치(pos)에 len만큼 src를 쓴다.
```


이 장치는 총 2,400명의 user data를 원격 저장 장치에 저장해서 관리한다.
user data는 id+name로 구성되어 있으며 id, name는 각각 최소 4bytes 최대 12bytes 길이를 가지는 문자열이다. (MAXLEN == 12)
id, name는 알파벳 대문자, 소문자 그리고 숫자로만 이뤄져 있으며 문자열 끝은 항상 '\0'값을 가진다.
(문자열 끝의 '\0'은 문자열 길이에 포함되지 않는다.)


구현해야 하는 함수는 다음과 같다.
init()을 제외하고 나머지 함수들의 총 호출 횟수 합은 50,000회를 넘지 않는다.


```
void init(int N);
매 test case 처음에 1회 호출된다
N: test case에서 사용되는 user data 중 유효한 user data의 최대 개수를 의미하며, 10 ~ 2,400 사이의 값을 가진다.
```

```
void addUser(unsigned char id[MAXLEN + 1], unsigned char name[MAXLEN + 1]);
id: id를 입력 받는다.
name: name을 입력 받는다.
id와 name은 각각 한 test case내에서 유일한 값이 사용된다. 즉 user data가 삭제되더라도 동일한 id 또는 name 값은 재사용되지 않는다.
```

```
void removeUser(unsigned char id[MAXLEN + 1]);
id: 삭제될 data의 id를 입력 받는다.
id는 반드시 유효한 data의 id이다.
```

```
void getName(unsigned char id[MAXLEN + 1], unsigned char name[MAXLEN + 1]);
id: 유효한 data의 id를 입력 받는다.
주어긴 id와 연결된 name을 반환한다.
```

```
void getID(unsigned char name[MAXLEN + 1], unsigned char id[MAXLEN + 1]);
name: 유효한 data의 name을 입력 받는다.
주어진 name과 연결된 id를 반환한다
```


user.cpp
```c++
extern bool memRead(unsigned char dest[], int pos, int len);
extern bool memWrite(unsigned char src[], int pos, int len);

#define MAXLEN		12

void init(int N){
}

void addUser(unsigned char id[MAXLEN + 1], unsigned char name[MAXLEN + 1]){
}

void removeUser(unsigned char id[MAXLEN + 1]){
}

void getName(unsigned char id[MAXLEN + 1], unsigned char name[MAXLEN + 1]){
}

void getID(unsigned char name[MAXLEN + 1], unsigned char id[MAXLEN + 1]){
}

```



main.cpp
```c++
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>

#define MEMORYSIZE		65536
#define MAXDATA	50000
#define MAXN 2400
#define MAXLEN		12

static unsigned char mem[MEMORYSIZE];

enum { CMD_PUT = 1, CMD_DEL, CMD_GET, CMD_GETKEY };

extern void init(int N);
extern void addUser(unsigned char id[MAXLEN + 1], unsigned char name[MAXLEN + 1]);
extern void removeUser(unsigned char id[MAXLEN + 1]);
extern void getName(unsigned char id[MAXLEN + 1], unsigned char name[MAXLEN + 1]);
extern void getID(unsigned char name[MAXLEN + 1], unsigned char id[MAXLEN + 1]);

bool memRead(unsigned char dest[], int pos, int len) {
	if (pos < 0 || len <= 0 || pos + len > MEMORYSIZE)
		return false;
	for (register int i = 0; i < len; ++i)
		dest[i] = mem[pos + i];
	return true;
}

bool memWrite(unsigned char src[], int pos, int len) {
	if (pos < 0 || len <= 0  || pos + len > MEMORYSIZE)
		return false;
	for (register int i = 0; i < len; ++i)
		mem[pos + i] = src[i];
	return true;
}

static bool compareStr(unsigned char *a, unsigned char *b) {
	while (*a) {
		if (*a != *b) return false;
		++a, ++b;
	}
	if (*b) return false;
	return true;
}

static void copyStr(unsigned char* dest, unsigned char* src) {
	for (int i = 0; i <= MAXLEN; ++i)
		dest[i] = src[i];
}

static unsigned int seed = 54321;
static unsigned int pseudo_rand(int max) {
	seed = ((unsigned long long)seed * 1103515245 + 12345) & 0x7FFFFFFF;
	return seed % max;
}

struct POOL {
	bool used[2];
	unsigned char text[MAXLEN + 1];
};
static POOL pool[MAXDATA];

static void makePool() {
	for (int i = 0; i < MAXDATA; ++i) {
		if (i == 34428) pseudo_rand(13);
		if (i == 46585) pseudo_rand(13);
		int len = pseudo_rand(13) + 4;
		if (len > 12) len = 12;
		for (int j = 0; j < len; ++j) {
			int c = pseudo_rand(62);
			if (c < 10) pool[i].text[j] = c + '0';
			else if (c < 36) pool[i].text[j] = c - 10 + 'A';
			else pool[i].text[j] = c - 36 + 'a';
		}
		pool[i].text[len] = 0;
	}
}

static int valid[MAXDATA][3];
static void initUsed() {
	for (int i = 0; i < MAXDATA; ++i) 
		pool[i].used[0] = pool[i].used[1] = false;
}

static int preQ = 0;
static int preN = 0;

static int run() {
	int Q, N;
	unsigned char id[MAXLEN + 1], name[MAXLEN + 1];
	unsigned char id_r[MAXLEN + 1], name_r[MAXLEN + 1];

	scanf("%d %d %d", &seed, &Q, &N);
	
	init(N);
	initUsed();
	int n = 0;
	int index;
	int usedCnt = 0;
	int score = 100;
	int max = 0;

	for (int q = 0; q < Q; ++q) {
		int cmd;
		if (n == 0)
			cmd = CMD_PUT;
		else {
			do {
				cmd = pseudo_rand(5);
			} while (cmd <= CMD_PUT && n + 1 >= N);
		}
		switch(cmd) {
		case 0:
		case CMD_PUT:
			do {
				index = pseudo_rand(MAXDATA);
			} while (pool[index].used[0]);
			pool[index].used[0] = true;
			valid[usedCnt][0] = index;
			copyStr(id, pool[index].text);
			do {
				index = pseudo_rand(MAXDATA);
			} while (pool[index].used[1]);
			pool[index].used[1] = true;
			valid[usedCnt][1] = index;
			copyStr(name, pool[index].text);
			valid[usedCnt][2] = 1;
			addUser(id, name);
			++n;
			if (n > max) max = n;
			++usedCnt;
			break;
		case CMD_DEL:
			do {
				index = pseudo_rand(usedCnt);
			} while (valid[index][2] == 0);
			valid[index][2] = 0;
			copyStr(id, pool[valid[index][0]].text);
			removeUser(id);
			--n;
			break;
		case CMD_GET:
			do {
				index = pseudo_rand(usedCnt);
			} while (valid[index][2] == 0);
			copyStr(id, pool[valid[index][0]].text);
			getName(id, name_r);
			if (!compareStr(pool[valid[index][1]].text, name_r))
				score = 0;
			break;
		case CMD_GETKEY:
			do {
				index = pseudo_rand(usedCnt);
			} while (valid[index][2] == 0);
			copyStr(name, pool[valid[index][1]].text);
			getID(name, id_r);
			if (!compareStr(pool[valid[index][0]].text, id_r))
				score = 0;
			break;
		default:
			break;
		}
	}
	
	return score;
}

int main() {
    int TC;
	setbuf(stdout, NULL);
    //freopen("input.txt", "r", stdin);
    scanf("%d", &TC);

	makePool();

	int totalscore = 0;
    for (int testcase = 1; testcase <= TC; ++testcase) {
		int score = run();
		printf("#%d : %d\n", testcase, score);
		totalscore += score;
    }
	printf("#total score : %d\n", totalscore / TC);
	if (totalscore / TC != 100) return -1;
    return 0;
}
```
