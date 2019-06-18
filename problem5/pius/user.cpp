//#define MYDEBUG

#ifdef MYDEBUG
#include <stdio.h>
#endif // MYDEBUG

#define MAX_FAMILY_TREE_LENGTH 30000
#define MAX_NAME_LENGTH 12

struct Person {
	bool married;
	int name_length;
	char name[MAX_NAME_LENGTH];
	int length;
	char best_friend[MAX_NAME_LENGTH];
	Person* parent;
	Person* children[199];
};

static Person people[200];
static int people_count = 0;
static Person *tree_root;

int strlen(const char *str) {
	int count = 0;
	while (str[count++] != '\0');
	return count - 1;
}

bool streq(const char *a, int a_size, const char *b, int b_size) {
	if (a_size != b_size) return false;
	for (int i = 0; i < a_size; i++) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}

void strcpy(char dest[MAX_NAME_LENGTH], char *src, int size) {
	for (int i = 0; i < size; i++) {
		dest[i] = src[i];
	}
	dest[size] = '\0';
}

unsigned int hashfunc(const char str[MAX_NAME_LENGTH], int size) {
	unsigned int hash = 5381;
	for (int i = 0; i < size; i++) {
		hash += (hash << 5) + str[i];
	}
	return hash % 200;
}

void ht_init() {
	for (int i = 0; i < 200; i++) {
		people[i].name_length = -1;
	}
}

Person* ht_set(Person &person) {
	int hash = hashfunc(person.name, person.name_length);
	while (people[hash].name_length != -1) {
		hash++;
		if (hash == 200) hash = 0;
	}
	people[hash] = person;
	return &people[hash];
}
Person* ht_get(const char* name, int size) {
	int hash = hashfunc(name, size);
	while (people[hash].name_length < 0 || !streq(people[hash].name, people[hash].name_length, name, size)) {
		hash++;
		if (hash == 200) hash = 0;
	}
	return &people[hash];
}

#ifdef MYDEBUG
static const char* spaces = "                                                                                                                                                      ";
void printTree(Person *person, int depth) {
	Person *parent = person->parent;
	printf("%.*s \"%.*s\"%s ", depth * 4, spaces, person->name_length, person->name, person->married ? "/M" : "");
	if (person->married) {
		printf("[%d]\n", person->length);
		for (int i = 0; i < person->length; i++) {
			printTree(person->children[i], depth + 1);
		}
	}
	else {
		printf(" - %.*s\n", person->length, person->best_friend);
	}
}

void printData(char data[MAX_FAMILY_TREE_LENGTH], int size) {
	for (int i = 0; i < size; i++) {
		printf("%3d ", data[i]);
		if (i % 10 == 0) {
			printf("\n");
		}
	}
	printf("\n");
}

bool printDataDiff(char dest[MAX_FAMILY_TREE_LENGTH], int dest_size, char src[MAX_FAMILY_TREE_LENGTH], int src_size) {
	int min = dest_size < src_size ? dest_size : src_size;
	if (dest_size != src_size) {
		return false;
	}
	for (int i = 0; i < min; i++) {
		if (dest[i] != src[i]) return false;
	}
	return true;
}
#endif // MYDEBUG

Person* parsePerson(char data[MAX_FAMILY_TREE_LENGTH], int start, int size, int *consumed, int depth, Person *parent) {
	int offset = 0;
	if (size == 0) {
		return nullptr;
	}

	Person person;
	person.parent = parent;
	person.married = data[start + offset++] == 2;
	offset++;
	person.name_length = data[start + offset++];
	strcpy(person.name, &data[start + offset], person.name_length);
	offset += person.name_length + 1;
	person.length = data[start + offset++];

	Person *who = ht_set(person);

	if (person.married) {
		for (int i = 0; i < person.length; i++) {
			int child_consumed = 0;
			who->children[i] = parsePerson(data, start + offset, size - offset, &child_consumed, depth + 1, who);
			offset += child_consumed;
		}
	}
	else {
		strcpy(who->best_friend, &data[start + offset], person.length);
		offset += person.length;
	}
	*consumed = offset;
	return who;
}

void removeTree(Person* root) {
	root->name_length = -1;
	if (root->married) {
		for (int i = 0; i < root->length; i++) {
			removeTree(root->children[i]);
		}
	}
}

int dumpTree(Person* root, char *data) {
	int offset = 0;
	if (root == nullptr) return 0;
	data[offset++] = root->married ? 2 : 1;
	data[offset++] = 0;
	data[offset++] = root->name_length;
	strcpy(&data[offset], root->name, root->name_length);
	offset += root->name_length;
	data[offset++] = 0;
	data[offset++] = root->length;
	if (root->married) {
		for (int i = 0; i < root->length; i++) {
			offset += dumpTree(root->children[i], &data[offset]);
		}
	}
	else {
		strcpy(&data[offset], root->best_friend, root->length);
		offset += root->length;
	}
	return offset;
}


void parse(char orgFamilyTree[MAX_FAMILY_TREE_LENGTH], int size) {
	int consumed = 0;
	ht_init();
	Person *root = parsePerson(orgFamilyTree, 0, size, &consumed, 0, nullptr);
	tree_root = root;

#ifdef MYDEBUG
	printf("total %d read\n", consumed);
	printTree(root, 0);
	printf("------------------------------------------\n");
	char data[MAX_FAMILY_TREE_LENGTH];
	int dump_size = dumpTree(root, data);
	printf("%s\n", printDataDiff(orgFamilyTree, size, data, dump_size) ? "SAME DATA" : "NOT SAME DATA");
	printf("------------------------------------------\n");
#endif // MYDEBUG
}

void change(char name[MAX_NAME_LENGTH], char bfName[MAX_NAME_LENGTH]) {
	Person *who = ht_get(name, strlen(name));
	who->length = strlen(bfName);
	strcpy(who->best_friend, bfName, who->length);

#ifdef MYDEBUG
	printf("CHANGE %.*s => %.*s\n", strlen(name), name, strlen(bfName), bfName);
	printTree(who->parent, 0);
	printf("------------------------------------------\n");
#endif // MYDEBUG
}

void registeration(char parent[MAX_NAME_LENGTH], char child[MAX_NAME_LENGTH], char childBfName[MAX_NAME_LENGTH]) {
	Person *who = ht_get(parent, strlen(parent));
	Person person;
	person.parent = who;
	person.married = false;
	person.name_length = strlen(child);
	strcpy(person.name, child, person.name_length);
	person.length = strlen(childBfName);
	strcpy(person.best_friend, childBfName, person.length);
	who->children[who->length++] = ht_set(person);

#ifdef MYDEBUG
	printf("registration to %.*s / %.*s - %.*s\n", strlen(parent), parent, strlen(child), child, strlen(childBfName), childBfName);
	printTree(who, 0);
	printf("------------------------------------------\n");
#endif // MYDEBUG
}

void remove(char name[MAX_NAME_LENGTH]) {
	Person *who = ht_get(name, strlen(name));
	Person *parent = who->parent;
	int pos = 200;
	for (int i = 0; i < parent->length - 1; i++) {
		if (parent->children[i] == who) {
			pos = i;
		}
		if (pos <= i) {
			parent->children[i] = parent->children[i + 1];
		}
	}
	parent->length--;
	parent->children[parent->length] = nullptr;
	removeTree(who);

#ifdef MYDEBUG
	printf("remove %.*s\n", strlen(name), name);
	printTree(tree_root, 0);
	printf("------------------------------------------\n");
#endif // MYDEBUG
}

int familyTree(char name[MAX_NAME_LENGTH], char newFamilyTree[MAX_FAMILY_TREE_LENGTH]) {
	Person *who = ht_get(name, strlen(name));
	int dump_size = dumpTree(who, newFamilyTree);

#ifdef MYDEBUG
	printf("dump tree\n");
	printTree(who, 0);
	printf("------------------------------------------\n");
#endif // MYDEBUG
	return dump_size;
}
