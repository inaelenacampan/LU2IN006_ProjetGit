#ifndef COMMIT_H
#define COMMIT_H

#include "ListLC.h"
#include "Work.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct key_value_pair {
    char *key;
    char *value;
} kvp;

typedef struct hash_table {
    kvp **T;
    int n;
    int size;
} HashTable;

typedef HashTable Commit;

unsigned long HashSdbm(char *str);

kvp *createKeyVal(char *key, char *val);
void freeKeyVal(kvp *kv);

char *kvts(kvp *k);
kvp *stkv(char *str);

Commit *initCommit();
void freeCommit(Commit *c);

void commitSet(Commit *c, char *key, char *value);
Commit *createCommit(char *hash);
char *commitGet(Commit *c, char *key);
char *cts(Commit *c);
Commit *stc(char *ch);
void ctf(Commit *c, char *file);
Commit *ftc(char *file);
char *blobCommit(Commit *c);

#endif