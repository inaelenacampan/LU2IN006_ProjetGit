#include "../Headers/Commit.h"
#include "../Headers/ListLC.h"
#include "../Headers/Work.h"
#include "../Headers/References.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZEMAX_CHAR 3000

/*
  Question 7.1 : initialisation des references
*/

void initRefs() {
    if (!file_exists(".refs")) {
        system("mkdir .refs");
        system("touch .refs/HEAD");
        system("touch .refs/master");
    }
}

/* 
  Question 7.2 : modifier le pointeur d'une reference
*/

void createUpdateRef(char *ref_name, char *hash) {
    initRefs();
    
    if ((hash != NULL) && (strlen(hash) != 0)) {

        char chaine[256];
        sprintf(chaine, "echo %s > .refs/%s", hash, ref_name);
        system(chaine);

    } else{
        List * refs = listdir(".refs");
        if(searchList(refs, ref_name)==NULL){
            char commande[256];
            strcpy(commande, "touch ");
            strcat(commande, ".refs/");
            strcat(commande, ref_name);
            system(commande);
        }
    }
}

/*
  Question 7.3 : supprimer une reference
*/

void deleteRef(char *ref_name) {

    char buff[256];
    sprintf(buff, "rm .refs/%s", ref_name);

    List *l = listdir(".refs");

    if (searchList(l, ref_name) != NULL) {
        system(buff);
    }

    freeList(l);
}

/*
  Question 7.4 : obtenir le hash d'une reference
*/

char *getRef(char *ref_name) {

    if (file_exists(".refs")) {

        List *l = listdir(".refs");

        if (searchList(l, ref_name) != NULL) {

            char *buffer = malloc(sizeof(char) * SIZEMAX_CHAR);
            char *path = malloc(sizeof(char) * (SIZEMAX_CHAR + 10));

            strcpy(path, ".refs/");
            strcat(path, ref_name);

            FILE *f = fopen(path, "r");
            if (fgets(buffer, SIZEMAX_CHAR, f) == NULL) {

                freeList(l);
                fclose(f);
                return "";

            } else {

                //le hash est bien retourne sans le saut a la ligne

                if(buffer[strlen(buffer)-1]=='\n'){
                    buffer[strlen(buffer)-1]='\0';
                }

                freeList(l);
                fclose(f);
                return buffer;
            }

        } else {

            freeList(l);
            return NULL;
        }
    }
    return NULL;
}

/*
  Question 7.5 : simulation de la commande git add
*/

void myGitAdd(char *file_or_folder) {

    WorkTree *wt;

    if (!file_exists(".add")) {
        system("touch .add");
        wt = initWorkTree();
    } else {
        wt = ftwt(".add");
    }

    if (file_exists(file_or_folder)) {
        appendWorkTree(wt, file_or_folder, NULL, 0);
        wttf(wt, ".add");

    } else {
        printf("%s n'existe pas!\n", file_or_folder);
    }

    freeWorkTree(wt);
}

/*
  Question 7.6 : simulation de la commande git commit
*/

void myGitCommit(char *branch_name, char *message) {

    if (!file_exists(".refs")) {
        printf("Initialiser d'abord les references du projet!\n");
        return;
    }

    char *path = malloc((SIZEMAX_CHAR + 10) * sizeof(char));
    strcpy(path, ".refs/");
    strcat(path, branch_name);

    List *l = listdir(".refs");

    if (searchList(l, branch_name) == NULL) {
        printf("La branche n'existe pas!\n");
        return;
    }

    char *hashBranch = getRef(branch_name);
    char *hashHEAD = getRef("HEAD");

    if (!((strlen(hashBranch) == 0) && (strlen(hashHEAD) == 0))) {
        if (strcmp(hashBranch, hashHEAD) != 0) {
            printf("HEAD doit pointer sur le dernier commit de la branche!");
            return;
        }
    }

    WorkTree *wt = ftwt(".add");

    char *hashTree = saveWorkTree(wt, "."); //"." signifie le repertoire courant
    
    if(hashTree[strlen(hashTree)-1]=='\n'){
        hashTree[strlen(hashTree)-1]='\0';
    }

    Commit *c = createCommit(hashTree);

    FILE *f = fopen(path, "r");
    char buffer[3* SIZEMAX_CHAR];

    if (strlen(hashBranch) != 0) {
        commitSet(c, "predecessor", hashBranch);
    }

    if (message != NULL) {
        commitSet(c, "message", message);
    }

    commitSet(c, "author", "InaCampan&SarahKaci");

    char *hashCommit = blobCommit(c);

    if(hashCommit[strlen(hashCommit)-1]=='\n'){
        hashCommit[strlen(hashCommit)-1]='\0';
    }
  
    createUpdateRef(branch_name, hashCommit);
    createUpdateRef("HEAD", hashCommit);
    system("rm .add");

    free(path);
    freeList(l);
    free(hashCommit);
    
    free(hashTree);
    freeCommit(c);
    freeWorkTree(wt);
}

/*
  Question 8.1 : initialisation d'un fichier qui contient la branche courrente
*/

void initBranch() {
    FILE *f = fopen(".current_branch", "w");
    fprintf(f, "master");
    fclose(f);
}

/*
  Question 8.2 : teste si la branche existe
*/

int branchExists(char *branch) {

    List *l = listdir(".refs");

    int trouve;

    if (searchList(l, branch) != NULL) {
        trouve = 1;
    } else {
        trouve = 0;
    }

    freeList(l);

    return trouve;
}

/*
  Question 8.3 : creation d'une nouvelle branche
*/

void createBranch(char *branch) {

    if (branchExists(branch) == 1) {
        return;
    }

    char pathHEAD[20] = ".refs/HEAD";

    char chaine[256];
    sprintf(chaine, "cat %s > .refs/%s", pathHEAD, branch);
    
    system(chaine);
}

/*
  Question 8.4 : recuperation de la branche courente
*/

char *getCurrentBranch() {

    FILE *f = fopen(".current_branch", "r");
    char *buffer = malloc(256 * sizeof(char));

    fgets(buffer, 256, f);
    fclose(f);

    return buffer;
}

/*
  Question 8.5 : affichage de toutes les branches du projet
*/

void printBranch(char *branch) {

    char *hashCommit = getRef(branch);

    if (hashCommit == NULL || strlen(hashCommit) == 0) {
        return;
    }

    char *hashPathCommit = malloc(SIZEMAX_CHAR * sizeof(char));
    char *path = malloc((SIZEMAX_CHAR + 3) * sizeof(char));
    strcpy(path, hashToPath(hashCommit));

    sprintf(hashPathCommit, "%s.c", path);
    Commit *c = ftc(hashPathCommit);

    while (c != NULL) {

        if (commitGet(c, "message") != NULL) {

        printf("%s : %s\n", hashCommit, commitGet(c, "message"));
        } else {

        // le message n'est pas renseigne
        printf("%s\n", hashCommit);
        }

        if (commitGet(c, "predecessor") != NULL) {

            hashCommit = commitGet(c, "predecessor");
            strcpy(path, hashToPath(hashCommit));

            sprintf(hashPathCommit, "%s.c", path);
            c = ftc(hashPathCommit);

        } else {

            c = NULL;
        }
    }

    free(hashPathCommit);
    free(path);
    free(hashCommit);
}

/*
  Question 8.6: retourne tous les commits sur une branche
*/

List *branchList(char *branch) {
    List *L = initList();

    char *hashCommit = getRef(branch);

    if(hashCommit == NULL) return NULL;

    char *hashPathCommit = malloc(3000 * sizeof(char));
    char *path = malloc(1000 * sizeof(char));
    strcpy(path, hashToPath(hashCommit));

    sprintf(hashPathCommit, "%s.c", path);
    Commit *c = ftc(hashPathCommit);

    while (c != NULL) {

        Cell *cellule = buildCell(hashCommit);
        insertFirst(L, cellule);

        if (commitGet(c, "predecessor") != NULL) {
            hashCommit = commitGet(c, "predecessor");
            strcpy(path, hashToPath(hashCommit));

            sprintf(hashPathCommit, "%s.c", path);
            c = ftc(hashPathCommit);

        } else {
            c = NULL;
        }
    }
    free(hashPathCommit);
    free(path);
    free(hashCommit);
    return L;
}

/*
  Question 8.7 : retourne tous les commits sur toutes les branches, sans repetition
*/

List *getAllCommits() {

    List *L = initList();
    List *branches = listdir(".refs/");

    Cell *branch = *branches;

    while (branch) {

        if (branch->data[0] != '.') {

            List *listB = branchList(branch->data);
            Cell *cB = *listB;

            while (cB) {
                if (searchList(L, cB->data) == NULL) {
                    insertFirst(L, buildCell(cB->data));
                }
                cB = cB->next;
            }
        }

        branch = branch->next;
    }

    return L;
}

/*  
  Question 9.1 : restore un commit a partir de son hash
*/

void restoreCommit(char *hash_commit) {

    char *hashPathCommit = malloc(SIZEMAX_CHAR * sizeof(char));
    char *path = malloc((SIZEMAX_CHAR + 3) * sizeof(char));

    strcpy(path, hashToPath(hash_commit));

    sprintf(hashPathCommit, "%s.c", path);

    Commit *c = ftc(hashPathCommit);

    char *tree_hashPath = malloc((SIZEMAX_CHAR + 3) * sizeof(char));

    char *tree_hash = commitGet(c, "tree");
    if(tree_hash[strlen(path)-1]=='\n'){
        tree_hash[strlen(path)-1]='\0';
    }

    strcpy(tree_hashPath, hashToPath(tree_hash));
    strcat(tree_hashPath, ".t");

    WorkTree *wt = ftwt(tree_hashPath);
    restoreWorkTree(wt, ".");
}

/*
  Question 9.2 : simulation myGit checkout
*/

void myGitCheckoutBranch(char *branch) {

    FILE *f = fopen(".current_branch", "w");
    fprintf(f, "%s", branch);
    fclose(f);

    char *ref = getRef(branch);
    FILE *b = fopen(".refs/HEAD", "w");
    if(ref!=NULL){
        fprintf(b, "%s\n", ref);
    }
    fclose(b);

    char *refB = getRef(branch);

    //si aucun commit dans la 2e branche, on a rien a recuperer

    if(refB!=NULL && strlen(refB)!=0){
        restoreCommit(refB);
    }
}

/*
  Question 9.3 : applique un filtre sur une liste
*/

List *filterList(List *L, char *pattern) {

    List *filtre = initList();
    int len = strlen(pattern);

    Cell *c = *L;

    while (c) {

        if (strncmp(c->data, pattern, len) == 0) {
            insertFirst(filtre, buildCell(c->data));
        }
        c = c->next;
    }

    return filtre;
}

/*
  Question 9.4 : simulation de la commande myGit checkoutcommit avec un pattern
*/

void myGitCheckoutCommit(char *pattern) {

    List *allC = getAllCommits();
    List *allCPattern = filterList(allC, pattern);
    Cell *c = *allCPattern;
  
    if (c == NULL) {
        printf("PAS DE HASH - ERREUR ! \n");
        return;
    }

    if (c->next == NULL) {
        //Checkout sur la branch du commit
        char * commitHash = (listGet(allCPattern, 0))->data;
  
        FILE *b = fopen(".refs/HEAD", "w");
        fprintf(b, "%s\n", commitHash);
        fclose(b);

        char * branche_currente = getCurrentBranch();
        char path[256];
        sprintf(path, ".refs/%s", branche_currente);

        FILE *br = fopen(path, "w");
        fprintf(br, "%s\n", commitHash);
        fclose(br);

        restoreCommit(commitHash);
    }
    else{

        char * ChList = ltos(allCPattern);
        printf("%s\n", ChList);
        printf("Precisez votre requete ! \n");
        free(ChList);
    }

    freeList(allCPattern);
    freeList(allC);
    
    return;
}