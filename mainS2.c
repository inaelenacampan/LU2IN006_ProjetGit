#include "Headers/ListLC.h"
#include "Headers/Work.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {

    // Tests effectues pour la semaine n°2
    // Creation de WorkFile, WorkTree
    // Tests pour sauvegarde d'un WorkTree

    WorkFile *wf1 = createWorkFile("testwf1.txt");
    char *chWf1 = wfts(wf1);
    printf("%s\n", chWf1);

    free(chWf1);
    freeWorkFile(wf1);

    char chWf[] = "testFile.txt\t\t675";

    WorkFile *wf2 = stwf(chWf);
    char *chWf2 = wfts(wf2);

    printf("%s\n", chWf2);

    free(chWf2);
    freeWorkFile(wf2);

    WorkTree *wt = initWorkTree();
    appendWorkTree(wt, "mainS1.c", NULL, 777);
    appendWorkTree(wt, "Bibliotheque/Work.c", NULL, 777);
    appendWorkTree(wt, "testSaveWT", NULL, 777);
    char * hashSave = saveWorkTree(wt, ".");
    freeWorkTree(wt);
    free(hashSave);
    
    return 0;
}