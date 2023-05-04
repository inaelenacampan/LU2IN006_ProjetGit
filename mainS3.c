#include "Headers/ListLC.h"
#include "Headers/Work.h"
#include "Headers/Commit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char ** argv){

    //Tests pour la semaine n°3

    //Fonctions liees a key-value

    kvp * test = stkv("text :message");
    char * ch = kvts(test);

    printf("%s\n", ch);

    freeKeyVal(test);
    free(ch);

    //Creation d'un commit

    Commit * c = createCommit("abcdefgh");
    commitSet(c, "commit1", "message");
    commitSet(c, "test", "updateC");
    commitSet(c, "partie2", "free");

    char * chCommit = cts(c);
    printf("%s\n\n", chCommit);
    freeCommit(c);
    
    Commit * copy = stc(chCommit);
    char * chCopy = cts(copy);

    printf("%s\n", chCopy);
    free(chCopy);
    
    //Gestion dans un fichier

    ctf(copy, "testS3/ecritureCommit.txt");
    free(chCommit);

    char * hash = blobCommit(copy);
    printf("Hash Commit : %s\n", hash);

    free(hash);
    freeCommit(copy);
    
    return 0;
}