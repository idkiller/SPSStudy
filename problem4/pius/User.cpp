#define MYDEBUG

#ifdef MYDEBUG
#include <stdio.h>
#endif

extern bool memRead(unsigned char dest[], int pos, int len);
extern bool memWrite(unsigned char src[], int pos, int len);

#define MAXLEN		 12
#define OFFSET_DATA  0
#define OFFSET_INDEX 57768

#ifdef MYDEBUG
int dup_id[2407];
int dup_name[2407];
#endif // MYDEBUG

int strlen(unsigned char str[]) {
	int i = 0;
	while (i < MAXLEN && str[i++] != '\0');
	return i;
}

bool streq(unsigned char a[MAXLEN], unsigned char b[MAXLEN]) {
	int i = 0;
	for (int i = 0; i < MAXLEN; i++) {
		if (a[i] == 0 && b[i] == 0) {
			return true;
		}
		if (a[i] != b[i]) {
			return false;
		}
	}
}

unsigned int hashfunc(unsigned char str[MAXLEN + 1]) {
	unsigned int hash = 0;
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		hash = (hash << 5) + str[i];
	}
	return hash %= 2407;
}

unsigned int get_hash(unsigned char id[MAXLEN + 1]) {
	unsigned char dest[MAXLEN];
	unsigned int hash = hashfunc(id);
	do {
		if (hash >= 2407) {
			hash = 0;
		}
		memRead(dest, OFFSET_DATA + hash++ * MAXLEN * 2, MAXLEN);
	} while (dest[0] != 0 && !streq(id, dest));
	return hash - 1;
}

unsigned int get_index_hash(unsigned char name[MAXLEN + 1], unsigned int *id_hash) {
	unsigned char data[2];
	unsigned char tmp[MAXLEN];
	unsigned int hash = hashfunc(name);

	while (true) {
		if (hash >= 2407) {
			hash = 0;
		}
		memRead(data, OFFSET_INDEX + hash * 2, 2);
		if (data[0] == 0 && data[1] == 0) {
			if (id_hash != nullptr) {
 				*id_hash = 0;
			}
			return hash;
		}
		if (id_hash != nullptr) {
			*id_hash = ((data[1] & 0xff) << 8) | data[0] & 0xff;
			memRead(tmp, OFFSET_DATA + (*id_hash) * MAXLEN * 2 + MAXLEN, MAXLEN);
			if (streq(tmp, name)) {
				return hash;
			}
		}
		hash++;
	}

	return hash;
}


void init(int N) {
	unsigned char zero[] = { 0, 0 };
	for (int i = 0; i < 2407; i++) {
		memWrite(zero, OFFSET_DATA + i * MAXLEN, 1);
		memWrite(zero, OFFSET_DATA + i * MAXLEN + MAXLEN, 1);
		memWrite(zero, OFFSET_INDEX + i * 2, 2);
	}

#ifdef MYDEBUG
	printf("\n\nN: %d\n", N);
#endif
}

void addUser(unsigned char id[MAXLEN + 1], unsigned char name[MAXLEN + 1]) {
	unsigned int id_hash = get_hash(id);

	memWrite(id,   OFFSET_DATA + id_hash * MAXLEN * 2         , MAXLEN);
	memWrite(name, OFFSET_DATA + id_hash * MAXLEN * 2 + MAXLEN, MAXLEN);

	unsigned char parts[2] = {id_hash & 0xff, (id_hash >> 8) & 0xff};
	unsigned int name_hash = get_index_hash(name, nullptr);

	memWrite(parts, OFFSET_INDEX + name_hash * 2, 2);

#ifdef MYDEBUG
	printf("[     addUser] [%6d] = {%12s, %12s}, [%6d] = [%6d]\n", id_hash, id, name, name_hash, id_hash);
	unsigned char dup_str[MAXLEN + 1];
	if (dup_id[id_hash] == 1) {
		memRead(dup_str, OFFSET_DATA + id_hash * MAXLEN * 2, MAXLEN);
		dup_str[12] = '\0';
		printf("[   collision] id    \"%12s\" == \"%12s\"\n", id, dup_str);
	}
	if (dup_name[name_hash] == 1) {
		memRead(parts, OFFSET_INDEX + name_hash * 2, 2);
		printf("[  collision] index \"%6d\" == \"%6d\"\n", name_hash, ((parts[1] & 0xff) << 8) | parts[0] & 0xff);
	}
	dup_id[id_hash] = 1;
	dup_name[name_hash] = 1;
#endif
}

void removeUser(unsigned char id[MAXLEN + 1]) {
	unsigned char remove_mark[] = {0xff, 0xff};
	unsigned int id_hash = get_hash(id);
	unsigned char name[MAXLEN + 1];

	memRead(name, OFFSET_DATA + id_hash * MAXLEN * 2 + MAXLEN, MAXLEN);
	unsigned int pointed;
	unsigned int name_hash = get_index_hash(name, &pointed);

	memWrite(remove_mark, OFFSET_DATA + id_hash * MAXLEN * 2, 1);
	memWrite(remove_mark, OFFSET_INDEX + name_hash * 2, 2);

#ifdef MYDEBUG
	printf("[  removeUser] [%6d], [%6d]\n", id_hash, name_hash);
#endif
}

void getName(unsigned char id[MAXLEN + 1], unsigned char name[MAXLEN + 1]) {
	unsigned int id_hash = get_hash(id);
	memRead(name, OFFSET_DATA + id_hash * MAXLEN * 2 + MAXLEN, MAXLEN);
	name[12] = '\0';

#ifdef MYDEBUG
	printf("[     getName] %12s = %6d, %12s\n", id, id_hash, name);
#endif
}

void getID(unsigned char name[MAXLEN], unsigned char id[MAXLEN]) {
	unsigned int id_hash;
	unsigned int name_hash = get_index_hash(name, &id_hash);
	memRead(id, OFFSET_DATA + id_hash * MAXLEN * 2, MAXLEN);
	id[12] = '\0';

#ifdef MYDEBUG
	printf("[       getID] %12s = %6d, %12s = %d\n", name, name_hash, id, id_hash);
#endif
}
