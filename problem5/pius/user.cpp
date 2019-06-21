//#define MYDEBUG

#ifdef MYDEBUG
#include <stdio.h>
#endif // MYDEBUG

#define MAX_FAMILY_TREE_LENGTH 30000
#define MAX_NAME_LENGTH 12

#define MAX_PEOPLE_COUNT 400

struct Person {
	bool married;
	int name_length;
	char name[MAX_NAME_LENGTH];
	long long name_serial;
	int length;
	char best_friend[MAX_NAME_LENGTH];
	Person* parent;
	Person* nextSibling;
	Person* prevSibling;
	Person* firstChild;
	Person* lastChild;
};

static Person people[MAX_PEOPLE_COUNT];
static int people_count = 0;
static Person *tree_root;

int strlen(const char *str) {
	int count = 0;
	while (str[count++] != '\0');
	return count - 1;
}

void strcpy(char dest[MAX_NAME_LENGTH], char *src, int size) {
	for (int i = 0; i < size; i++) {
		dest[i] = src[i];
	}
	dest[size] = '\0';
}

long long serialize_name(const char str[MAX_NAME_LENGTH], int size) {
	long long serial = 0;
	for (int i = 0; i < size; i++) {
		serial = (serial << 5) | (str[i] - 'A' + 1);
	}
	return serial;
}

unsigned int hashfunc(const char str[MAX_NAME_LENGTH], int size) {
	unsigned int hash = 5381;
	for (int i = 0; i < size; i++) {
		hash += (hash << 5) + str[i];
	}
	return hash % MAX_PEOPLE_COUNT;
}

void ht_init() {
	for (int i = 0; i < MAX_PEOPLE_COUNT; i++) {
		people[i].name_length = -1;
	}
}

Person* ht_set(Person &person) {
	int hash = hashfunc(person.name, person.name_length);
	while (people[hash].name_length != -1) {
		hash++;
		if (hash == MAX_PEOPLE_COUNT) hash = 0;
	}
	people[hash] = person;
	return &people[hash];
}
Person* ht_get(const char* name, int size) {
	int hash = hashfunc(name, size);
	long long serial = serialize_name(name, size);
	while (people[hash].name_length < 0 || people[hash].name_serial != serial) {
		hash++;
		if (hash == MAX_PEOPLE_COUNT) hash = 0;
	}
	return &people[hash];
}

#ifdef MYDEBUG
static const char* spaces = "                                                                                                                                                      ";
void printTree(Person *person, int depth) {
	Person *parent = person->parent;
	printf("%.*s \"%.*s\"%s ", depth * 4, spaces, person->name_length, person->name, person->married ? "/M" : "");
	Person *child = person->firstChild;
	if (person->married) {
		printf("[%d]\n", person->length);
		while (child != nullptr) {
			printTree(child, depth + 1);
			child = child->nextSibling;
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
	person.name_serial = serialize_name(person.name, person.name_length);
	offset += person.name_length + 1;
	person.length = data[start + offset++];
	person.nextSibling = person.prevSibling = person.firstChild = person.lastChild = nullptr;

	Person *who = ht_set(person);
	Person *child = nullptr;

	if (person.married && person.length > 0) {
		int child_consumed = 0;
		who->firstChild = parsePerson(data, start + offset, size - offset, &child_consumed, depth + 1, who);
		child = who->firstChild;
		offset += child_consumed;
		for (int i = 1; i < person.length; i++) {
			child->nextSibling = parsePerson(data, start + offset, size - offset, &child_consumed, depth + 1, who);
			child->nextSibling->prevSibling = child;
			child = child->nextSibling;
			offset += child_consumed;
		}
		who->lastChild = child;
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
	Person *child = root->firstChild;
	if (child != nullptr) {
		while (child != nullptr) {
			removeTree(child);
			child = child->nextSibling;
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
		Person *child = root->firstChild;
		while (child != nullptr) {
			offset += dumpTree(child, &data[offset]);
			child = child->nextSibling;
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
	person.name_serial = serialize_name(person.name, person.name_length);
	person.length = strlen(childBfName);
	strcpy(person.best_friend, childBfName, person.length);
	person.nextSibling = person.prevSibling = person.firstChild = person.lastChild = nullptr;
	Person *last = who->lastChild;
	last->nextSibling = ht_set(person);
	who->lastChild = last->nextSibling;
	who->lastChild->prevSibling = last;
	who->length++;


#ifdef MYDEBUG
	printf("registration to %.*s / %.*s - %.*s\n", strlen(parent), parent, strlen(child), child, strlen(childBfName), childBfName);
	printTree(who, 0);
	printf("------------------------------------------\n");
#endif // MYDEBUG
}

void remove(char name[MAX_NAME_LENGTH]) {
	Person *who = ht_get(name, strlen(name));
	if (who->nextSibling == nullptr) {
		who->parent->lastChild = who->prevSibling;
	}
	if (who->prevSibling == nullptr) {
		who->parent->firstChild = who->nextSibling;
	}
	else {
		who->prevSibling->nextSibling = who->nextSibling;
	}
	who->parent->length--;
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
