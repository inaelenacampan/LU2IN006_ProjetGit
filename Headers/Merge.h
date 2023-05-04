#ifndef MERGE_H
#define MERGE_H

#include "Commit.h"
#include "ListLC.h"
#include "References.h"
#include "Work.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

WorkTree *mergeWorkTrees(WorkTree *wt1, WorkTree *wt2, List **conflicts);
WorkTree *recuperer_WorkTree(char *branch);
List *merge(char *remote_branch, char *message);
void createDeletionCommit(char *branch, List *conflicts, char *message);
void divideList(char * branch, List *conflicts, List **conflicts_current,
                List **conflicts_remote);

#endif