#include "../Headers/Merge.h"
#include "../Headers/Commit.h"
#include "../Headers/ListLC.h"
#include "../Headers/Work.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
  Question 11.1 : fusion de 2 WorkTree et creation d'une liste de conflicts
*/

WorkTree *mergeWorkTrees(WorkTree *wt1, WorkTree *wt2, List **conflicts) {

    List * tmp_conflicts = initList();
    int len1 = wt1->n;
    int len2 = wt2->n;

    WorkTree *fusion = initWorkTree();
    
    int index;
    int i;

    // Premier WorkTree : on le parcours et on vérifie si le fichier courant existe dans le 2e WT
    // On effectue des tests sur la présence du fichier en question dans la liste des conflicts / dans le WT de fussion

    for (i = 0; i < len1; i++) {

        index = inWorkTree(wt2, wt1->tab[i].name);

        if(index<0){
            if(inWorkTree(fusion, wt1->tab[i].name)==-1){
                appendWorkTree(fusion, wt1->tab[i].name, wt1->tab[i].hash, wt1->tab[i].mode);
            }
        }

        else if(strcmp(wt1->tab[i].hash, wt2->tab[index].hash) == 0){
            if(inWorkTree(fusion, wt1->tab[i].name)==-1){
                appendWorkTree(fusion, wt1->tab[i].name, wt1->tab[i].hash, wt1->tab[i].mode);
            }
        }

        else{
            if(searchList(tmp_conflicts, wt1->tab[i].name)==NULL){
                insertFirst(tmp_conflicts, buildCell(wt1->tab[i].name));
            }
        }

    }

    // Deuxieme WorkTree : pareil

    for (i = 0; i < len2; i++) {

        index = inWorkTree(wt1, wt2->tab[i].name);

        if(index<0){
            if(inWorkTree(fusion, wt2->tab[i].name)==-1){
                appendWorkTree(fusion, wt2->tab[i].name, wt2->tab[i].hash, wt2->tab[i].mode);
            }
        }

        else if(strcmp(wt2->tab[i].hash, wt1->tab[index].hash) == 0){
            if(inWorkTree(fusion, wt2->tab[i].name)==-1){
                appendWorkTree(fusion, wt2->tab[i].name, wt2->tab[i].hash, wt2->tab[i].mode);
            }
        }

        else{
            if(searchList(tmp_conflicts, wt2->tab[i].name)==NULL){
                insertFirst(tmp_conflicts, buildCell(wt2->tab[i].name));
            }
        }

    }
  
    *conflicts = tmp_conflicts;
    return fusion;
}

/*
  Question 11.2 : merge 2 branches qui n'ont aucun conflict
*/

/*
  Fonction intermediaire pour retrouver le Worktree associé à son hash depuis le hash d'un commit
*/

WorkTree *recuperer_WorkTree(char *branch) {

    char *hashCommit = getRef(branch);
    char *hashPathCommit = malloc(3000 * sizeof(char));
    char *pathC = hashToPath(hashCommit);

    sprintf(hashPathCommit, "%s.c", pathC);

    Commit *commit = ftc(hashPathCommit);

    char *tree_hash = malloc(300 * sizeof(char));

    strcpy(tree_hash, hashToPath(commitGet(commit, "tree")));

    strcat(tree_hash, ".t");
    WorkTree *wt = ftwt(tree_hash);

    free(tree_hash);
    free(hashPathCommit);
    free(pathC);
    freeCommit(commit);

    return wt;
}

List *merge(char *remote_branch, char *message) {
  
    char *current_branch = getCurrentBranch();
    char *hashCommitRemote = getRef(remote_branch);
    char *hashCommitCurrent = getRef(current_branch);

    // Branch Remote
    WorkTree *wtR = recuperer_WorkTree(remote_branch);

    // Current Branch
    WorkTree *wtC = recuperer_WorkTree(current_branch);

    List *conflicts = initList();
    WorkTree *fusion = mergeWorkTrees(wtR, wtC, &conflicts);

    if (*conflicts == NULL) {

        char * hashNewTree = blobWorkTree(fusion);
        
        // Creation du commit

        Commit * newC = createCommit(hashNewTree);
        commitSet(newC, "predecessor", hashCommitCurrent);
        commitSet(newC, "merged_predecessor", hashCommitRemote);
        commitSet(newC, "message", message);

        char * hashNewC = blobCommit(newC);
        
        FILE *head = fopen(".refs/HEAD", "w");
        fprintf(head, "%s\n", hashNewC);
        fclose(head);

        char path[256];
        sprintf(path, ".refs/%s", current_branch);

        FILE *br = fopen(path, "w");
        fprintf(br, "%s\n", hashNewC);
        fclose(br);

        restoreWorkTree(fusion, ".");
        deleteRef(remote_branch);
        free(hashNewTree);
    }
    return conflicts;
}

/*
  Question 11.3 : creation du commit a partir des conflicts qui reste
*/

void createDeletionCommit(char *branch, List *conflicts, char *message) {
  
    char *branche_initiale = getCurrentBranch();

    myGitCheckoutBranch(branch);

    WorkTree *wtC = recuperer_WorkTree(branch);

    deleteRef(".add");

    // Pour tester le fait qu'il existe au moins un fichier dans le commit de deletion
    int vide = 1;

    for (int i = 0; i < wtC->n; i++) {

        char *nom_fichier = (wtC->tab[i]).name;
        if (searchList(conflicts, nom_fichier) == NULL) {
            myGitAdd(nom_fichier);
            vide = 0;
        }
    }

    if(vide==0){
        if (file_exists(".add")) {
            WorkTree *wt = ftwt(".add");
            freeWorkTree(wt);
        }
        myGitCommit(branch, message);
    }

    myGitCheckoutBranch(branche_initiale);
    freeWorkTree(wtC);
}

/*
  Fonction auxiliare pour donner le choix a l'utilisateur de diviser les conflicts en 2,
  selon la version qu'il souhaite garder
*/

void divideList(char *branch, List *conflicts, List **conflicts_current, List **conflicts_remote) {
  
    *conflicts_current = initList();
    *conflicts_remote = initList();
    
    Cell *fichier = *conflicts;
    printf("Branches : current - %s et remote - %s\n", getCurrentBranch(), branch);
    printf("Les choix sont 1 ou 2\n");
  
    int choix;
    while (fichier != NULL) {
        printf("%s depuis quelle branche ? : ", fichier->data);

        scanf("%d", &choix);

        if (choix == 1) {
            insertFirst(*conflicts_current, buildCell(fichier->data));
        } else if (choix == 2) {
            insertFirst(*conflicts_remote, buildCell(fichier->data));
        } else {
            printf("la valeur selectionnée n'est pas une option possible, le fichier sera positionné dans la liste de la branche courante par défaut\n");
            insertFirst(*conflicts_current, buildCell(fichier->data));
        }

        fichier = fichier->next;
    }
}
