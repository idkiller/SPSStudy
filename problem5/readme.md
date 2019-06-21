# 가계도 

미소는 여러 집안의 가계도를 관리하는 일을 하고 있다.

이 가계도는 남녀 구분이 없고 성없이 이름(대문자)만으로 관리가 된다.

동일한 이름의 가족은 없다.

혈통 위주로 관리되기 때문에 배우자 관계는 없다.

각 가계도는 연속된 구성원 정보로 구성되어 있으며

각 구성원들은 다음과 같은 구조로 이뤄져 있다.

- 결혼 여부(SOLOTYPE:1, MARRIEDTYPE:2, 1 bytes)

- 이름 길이(최대 값은 11, 2 bytes)

- 이름(최대 길이 11, n bytes)

- 미혼인 경우

 . best friend 이름의 길이(최대 값은 11, 2 bytes)

 . best friend 이름(최대 길이 11, n bytes)

- 기혼인 경우

 . 다음세대 자녀 수(2 bytes)

 . 자녀들의 연속된 데이터(구성원 구조 형태). 나이 순으로 정렬되어 있다. (장자->막내 순)

※ 예를 들어 본인의 이름길이 또는 best friend 이름의 길이 또는 자녀수가 11인 경우 첫 바이트에 int 0이 두번째 바이트에 int 11이 들어간다. 즉 2bytes 데이터이지만 첫번째 byte는 항상 0이 된다.

각 테스트 케이스 처음에 전체 가계도가 한 번 주어진다.

이 가계도를 분석하고 이용해서 다음과 같은 명령을 수행한다.

CHANGE(200): 특정 미혼 구성원의 best friend 이름을 변경한다.

REGISTRATION(300): 특정 기혼 구성원에 자녀를 추가한다. 추가되는 자녀는 미혼이며 기존 자녀들보다 어린 자녀가 된다. best friend를 가진다.

REMOVE(400): 전체 가계도에서 특정 구성원을 포함한 이하 자녀들을 모두 가계도에서 분리(제거)한다.

FAMILY_TREE(500): 전체 가계도 중에서 주어진 특정 구성원을 포함한 그 자손들의 가계도를 parse()함수를 통해서 전달한 orgFamilyTree[] 형태로 newFamily[]에 저장하고 총 size를 반환한다.

END(999): 테스트 케이스를 종료한다.

최대 구성원 수는 200명을 넘지 않으며 최대 세대는 10이다.

main.cpp
```c++
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>

#define CHANGE                 200
#define REGISTERATION       300
#define REMOVE	               400
#define FAMILY_TREE     500
#define END	            		999

#define MAX_FAMILY_TREE_LENGTH 30000
#define MAX_NAME_LENGTH 12

extern void parse(char orgFamilyTree[MAX_FAMILY_TREE_LENGTH], int size);
extern void change(char name[MAX_NAME_LENGTH], char bfName[MAX_NAME_LENGTH]);
extern void registeration(char parent[MAX_NAME_LENGTH], char child[MAX_NAME_LENGTH], char childBfName[MAX_NAME_LENGTH]);
extern void remove(char name[MAX_NAME_LENGTH]);
extern int familyTree(char name[MAX_NAME_LENGTH], char newFamilyTree[MAX_FAMILY_TREE_LENGTH]);

static char orgFamilyTree[MAX_FAMILY_TREE_LENGTH];
static char newFamilyTree[MAX_FAMILY_TREE_LENGTH];

static int point;

static int calcHash(char* data, int len) {
	int ret = 0;
	for (register int i = 1; i < len; ++i) {
		ret += (data[i] * i + i);
		ret &= 0xFFFF;
	}
	return ret;
}

static inline int ch2num(char ch) {
	return ch >= 'A' ? 10 + ch - 'A' : ch - '0';
}

int testcase;
#define SOLOTYPE         (0x01)
#define MARRIEDTYPE            (0x02)
static int len;

static void run() {
	char buf[80];
	scanf("%d", &len);
	for (register int p = 0; p < len;) {
		scanf("%s", buf);
		for (register int i = 0; i < 64 && p < len; i += 2, ++p)
			orgFamilyTree[p] = (ch2num(buf[i]) << 4) | ch2num(buf[i + 1]);
	}
	parse(orgFamilyTree, len);

	while (1) {
		int command;
		char name[MAX_NAME_LENGTH], bfName[MAX_NAME_LENGTH];
		char parent[MAX_NAME_LENGTH], child[MAX_NAME_LENGTH], childBfName[MAX_NAME_LENGTH];
		int hash, length, resultlength, resulthash;

		scanf("%d", &command);
		switch (command) {
		case CHANGE:
			scanf("%s %s", name, bfName);
			change(name, bfName);
			break;
		case REGISTERATION:
			scanf("%s %s %s", parent, child, childBfName);
			registeration(parent, child, childBfName);
			break;
		case REMOVE:
			scanf("%s", name);
			remove(name);
			break;
		case FAMILY_TREE:
			scanf("%s %d %d", name, &length, &hash);
			resultlength = familyTree(name, newFamilyTree);
			if (length == resultlength) {
				resulthash = calcHash(newFamilyTree, length);
				if (resulthash == hash) point += length;
			}
			break;
		}
		if (command == END) break;
	}
}

int main() {
	int T, total_score;

	scanf("%d", &T);

	total_score = 0;
	for (testcase = 1; testcase <= T; ++testcase) {
		int score;
		point = 0;
		run();
		scanf("%d", &score);
		total_score += (point / score) * 100;
	}

	printf("#total score : %d\n", total_score / T);
	if (total_score / T < 100) return -1;
	return 0;
}
```

user.cpp

```c++
#define MAX_FAMILY_TREE_LENGTH 30000
#define MAX_NAME_LENGTH 12

void parse(char orgFamilyTree[MAX_FAMILY_TREE_LENGTH], int size) {
}

void change(char name[MAX_NAME_LENGTH], char bfName[MAX_NAME_LENGTH]) {
}

void registeration(char parent[MAX_NAME_LENGTH], char child[MAX_NAME_LENGTH], char childBfName[MAX_NAME_LENGTH]) {
}

void remove(char name[MAX_NAME_LENGTH]) {
}

int familyTree(char name[MAX_NAME_LENGTH], char newFamilyTree[MAX_FAMILY_TREE_LENGTH]) {
}

```
