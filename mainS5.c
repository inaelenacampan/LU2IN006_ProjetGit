#include "Headers/Commit.h"
#include "Headers/ListLC.h"
#include "Headers/References.h"
#include "Headers/Work.h"
#include "Headers/Merge.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {

    //Test sur la fonction de Merge entre 2 WorkTree

    WorkTree * wt1 = initWorkTree();
    appendWorkTree(wt1, "D.c", "567", 0);
    appendWorkTree(wt1, "B.c", "ghretzbdoz", 0);
    appendWorkTree(wt1, "C.c", "123", 0);
    appendWorkTree(wt1, "A.c", "124", 0);

    WorkTree * wt2 = initWorkTree();
    appendWorkTree(wt2, "B.c", "ghre3", 0);
    appendWorkTree(wt2, "A.c", "124", 0);
    appendWorkTree(wt2, "E.c", "4567", 0);
    appendWorkTree(wt2, "C.c", "125", 0);

    List * conflicts;

    WorkTree * fusion = mergeWorkTrees(wt1, wt2, &conflicts);

    char * chTree = wtts(fusion);
    printf(" Fusion\n%s\n", chTree);
    free(chTree);

    char * chList = ltos(conflicts);
    printf(" Conflicts\n%s\n", chList);
    free(chList);

    freeList(conflicts);
    freeWorkTree(fusion);
    freeWorkTree(wt1);
    freeWorkTree(wt2);
    
    return 0;
}