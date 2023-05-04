#ifndef REFERENCES_H
#define REFERENCES_H

#include "Commit.h"
#include "ListLC.h"
#include "Work.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Semaine 3
void initRefs();
void createUpdateRef(char *ref_name, char *hash);
void deleteRef(char *ref_name);
char *getRef(char *ref_name);
void myGitAdd(char *file_or_folder);
void myGitCommit(char *branch_name, char *message);

// Semaine 4
void initBranch();
int branchExists(char *branch);
void createBranch(char *branch);
char *getCurrentBranch();
void printBranch(char *branch);
List *branchList(char *branch);
List *getAllCommits();
void restoreCommit(char *hash_commit);
void myGitCheckoutBranch(char *branch);
List *filterList(List *L, char *pattern);
void myGitCheckoutCommit(char *pattern);

#endif