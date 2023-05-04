#ifndef WORK_H
#define WORK_H

#include "ListLC.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    char *name;
    char *hash;
    int mode;
} WorkFile;

typedef struct {
    WorkFile *tab;
    int size;
    int n;
} WorkTree;

int getChmod(const char *path);
void setMode(int mode, char *path);
int isDir(const char *fileName);

WorkFile *createWorkFile(char *name);
void freeWorkFile(WorkFile *wf);
char *wfts(WorkFile *wf);
WorkFile *stwf(char *ch);

WorkTree *initWorkTree();
void freeWorkTree(WorkTree *wt);
int inWorkTree(WorkTree *wt, char *name);
int appendWorkTree(WorkTree *wt, char *name, char *hash, int mode);

char *wtts(WorkTree *wt);
WorkTree *stwt(char *ch);
int wttf(WorkTree *wt, char *file);
WorkTree *ftwt(char *file);

char *concatenation_chaines(char *ch1, char *ch2);
char *blobWorkTree(WorkTree *wt);
char *saveWorkTree(WorkTree *wt, char *path);
void restoreWorkTree(WorkTree *wt, char *path);

#endif