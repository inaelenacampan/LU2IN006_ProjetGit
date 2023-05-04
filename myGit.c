#include "Headers/Commit.h"
#include "Headers/ListLC.h"
#include "Headers/Merge.h"
#include "Headers/References.h"
#include "Headers/Work.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {

    switch (argc) {

    case 2:

        if (strcmp(argv[1], "init") == 0) {
            initRefs();
            initBranch();

        } 
        
        else if (strcmp(argv[1], "list-refs") == 0) {

                List *l = listdir(".refs/");
                printf("Refs : \n");

                Cell *c = *l;
                while (c) {
                    if (c->data[0] != '.') {
                    printf("%s \n", c->data);
                    }
                    c = c->next;
                }
                freeList(l);

        }

        else if (strcmp(argv[1], "list-add") == 0) {
            if (file_exists(".add")) {
                WorkTree *wt = ftwt(".add");
                printf("%s", wtts(wt));
            }
        }

        else if (strcmp(argv[1], "clear-add") == 0) {
            deleteRef(".add");
            system("rm .add");
        }

        else if (strcmp(argv[1], "get-current-branch") == 0) {
            char *b = getCurrentBranch();
            printf("%s\n", b);
            free(b);
        }

        break;

    case 3:

        if (strcmp(argv[1], "delete-ref") == 0) {
            deleteRef(argv[2]);
        }

        else if (strcmp(argv[1], "branch") == 0) {
            createBranch(argv[2]);
        }

        else if (strcmp(argv[1], "add") == 0) {
            myGitAdd(argv[2]);
        }

        else if (strcmp(argv[1], "commit") == 0) {
            myGitCommit(argv[2], NULL);
        }

        else if (strcmp(argv[1], "create-ref") == 0) {
            createUpdateRef(argv[2], NULL);

        } else if (strcmp(argv[1], "branch-print") == 0) {

            if (branchExists(argv[2]) == 1) {
                printBranch(argv[2]);
            } else {
                printf("Erreur, la branche n'existe pas");
            }

        } else if (strcmp(argv[1], "checkout-branch") == 0) {

            if (branchExists(argv[2]) == 1) {
                myGitCheckoutBranch(argv[2]);
            } else {
                printf("Erreur, la branche n'existe pas");
            }

        } else if (strcmp(argv[1], "checkout-commit") == 0) {
            myGitCheckoutCommit(argv[2]);
        }

        break;

    case 4:

        if (strcmp(argv[1], "create-ref") == 0) {
            createUpdateRef(argv[2], argv[3]);

        } 
        
        else if (strcmp(argv[1], "add") == 0) {
            myGitAdd(argv[2]);
            myGitAdd(argv[3]);
        }

        else if (strcmp(argv[1], "merge") == 0) {

            char *current_branch = getCurrentBranch();
            List *conflicts = merge(argv[2], argv[3]);

            if (*conflicts == NULL) {
                printf("La fusion s'est bien passée\n");
            } else {
                printf("La fusion ne s'est pas passée comme prévu, choisir une option "
                    "parmi les suivantes:\n");

                printf("1. Garder les fichiers de la branche courante\n");
                printf("2. Garder les fichiers de la branche %s\n", argv[2]);
                printf("3. Choix manuel, conflit par conflit\n");
                printf("Choix entre 1 et 3 : \n");

                int choix;
                scanf("%d", &choix);

                if (choix == 1) {
                
                    createDeletionCommit(argv[2], conflicts, argv[3]);
                    merge(argv[2], argv[3]);
                    printf("La fusion s'est bien passée (pour le choix 1)\n");

                } else if (choix == 2) {

                    createDeletionCommit(getCurrentBranch(), conflicts, argv[3]);
                    merge(argv[2], argv[3]);
                    printf("La fusion s'est bien passée (pour le choix 2)\n");

                } else if (choix == 3) {

                    List *conflicts_current = initList();
                    List *conflicts_remote = initList();

                    divideList(argv[2], conflicts, &conflicts_current, &conflicts_remote);

                    char * current = getCurrentBranch();
                    
                    createDeletionCommit(current, conflicts_remote, argv[3]);
                    createDeletionCommit(argv[2], conflicts_current, argv[3]);
                    
                    merge(argv[2], argv[3]);
                    printf("La fusion s'est bien passée (pour le choix 3) \n");

                    freeList(conflicts_current);
                    freeList(conflicts_remote);

                    } else {
                    printf("Option invalide\n");
                }
            }
            freeList(conflicts);
        }

        break;

    default:

        if (strcmp(argv[1], "add") == 0) {
            for (int i = 2; i < argc; i++) {
                myGitAdd(argv[i]);
            }
        }

        else if (strcmp(argv[1], "commit") == 0) {
            myGitCommit(argv[2], argv[4]);
        }

        break;
    }
    
    return 0;
}