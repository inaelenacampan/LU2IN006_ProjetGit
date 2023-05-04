#include "../Headers/Work.h"
#include "../Headers/ListLC.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SIZEMAX_TREE 1000
#define SIZEMAX_CHAR 300

int getChmod(const char *path) {
    struct stat ret;
    if (stat(path, &ret) == -1) {
        return -1;
    }
    return (ret.st_mode & S_IRUSR) | (ret.st_mode & S_IWUSR) |
            (ret.st_mode & S_IXUSR) | /*owner*/
            (ret.st_mode & S_IRGRP) | (ret.st_mode & S_IWGRP) |
            (ret.st_mode & S_IXGRP) | /*group*/
            (ret.st_mode & S_IROTH) | (ret.st_mode & S_IWOTH) |
            (ret.st_mode & S_IXOTH); /*other*/
}

void setMode(int mode, char *path) {
    char buff[100];
    //gerer le probleme du octal
    sprintf(buff, "chmod %o %s ", mode, path);
    system(buff);
}

/*
  Retourne 0 si c'est un fichier et 1 pour un repertoire
*/

int isDir(const char *fileName) {
    struct stat path;
    stat(fileName, &path);
    return S_ISDIR(path.st_mode);
}

/*
  Question 4.1 : creation d'un WorkFile et sa fonction de liberation memoire
*/

WorkFile *createWorkFile(char *name) {

    WorkFile *wf = (WorkFile *)malloc(sizeof(WorkFile));
    wf->name = strdup(name);
    wf->hash = NULL;
    wf->mode = 0;

    return wf;
}

void freeWorkFile(WorkFile *wf) {

    free(wf->name);
    if (wf->hash != NULL) {
        free(wf->hash);
    }
    free(wf);
}

/*
  Question 4.2 :creation d'une chaine de caractere a partir d'un WorkFile
*/

char *wfts(WorkFile *wf) {

    char *res = malloc(SIZEMAX_CHAR * 3 * sizeof(char));

    strcpy(res, wf->name);
    strcat(res, "\t");

    //cas particulier : le hash est encore a NULL

    if (wf->hash != NULL) {
        strcat(res, wf->hash);
    }

    strcat(res, "\t");

    char buffer[5];
    sprintf(buffer, "%d", wf->mode);

    strcat(res, buffer);

    return res;
}

/*
  Question 4.3 : Format chaine en parametre: chaine1 \t chaine2 \t chaine3
*/

WorkFile *stwf(char *ch) {

    WorkFile *wf = (WorkFile *)malloc(sizeof(WorkFile));

    char *name = malloc(sizeof(char) * SIZEMAX_CHAR);
    char *hash = malloc(sizeof(char) * SIZEMAX_CHAR);
    char *mode = malloc(sizeof(char) * SIZEMAX_CHAR);

    int n = sscanf(ch, "%s\t%s\t%s", name, hash, mode);

    if (n == 3) {

        wf->name = strdup(name);
        wf->hash = strdup(hash);
        wf->mode = atoi(mode);

    } else {

        sscanf(ch, "%s\t\t%s", name, mode);
        wf->name = strdup(name);
        wf->hash = NULL;
        wf->mode = atoi(mode);
    }
    if (hash != NULL) {
        free(hash);
    }
    free(mode);
    free(name);

    return wf;
}

/*
  Question 4.4 : initialisation d'un WorkTree et sa liberation
*/

WorkTree *initWorkTree() {

    WorkTree *wt = (WorkTree *)malloc(sizeof(WorkTree));
    wt->tab = (WorkFile *)malloc(SIZEMAX_TREE * sizeof(WorkFile));
    wt->size = SIZEMAX_TREE;
    wt->n = 0;

    return wt;
}

void freeWorkTree(WorkTree *wt) {

    for(int i = 0; i< wt->n; i++){
        if(wt->tab[i].hash!=NULL){
            free(wt->tab[i].hash);
        }
        free(wt->tab[i].name);
    }
    
    free(wt->tab);
    free(wt);
}

/*
  Question 4.5 : tester si un fichier/dossier se trouve dans le WorkTree
  La fonction retourne -1 si le nom n'existe pas
*/

int inWorkTree(WorkTree *wt, char *name) {

    for (int i = 0; i < wt->n; i++) {
        if (strcmp((wt->tab[i].name), name) == 0) {
            return i;
        }
    }

    return -1;
}

/* 
  Question 4.6 Si le fichier existe, on fait rien (pareil si le WorkTree est plein)
*/

int appendWorkTree(WorkTree *wt, char *name, char *hash, int mode) {

    int index = inWorkTree(wt, name);

    if (index != -1) {
        return 0;
    }

    if (wt->n == wt->size) {
        return 0;
    }

    wt->tab[wt->n].name = strdup(name);

    if(hash!=NULL){
        wt->tab[wt->n].hash = strdup(hash);
    }else{
        wt->tab[wt->n].hash = NULL;
    }

    wt->tab[wt->n].mode = mode;

    (wt->n)++;
    return 1;
}

/*
  Question 4.7 : chaine de caracteres correspondant au WorkTree
*/

char *wtts(WorkTree *wt) {

    char *res = (char *)malloc(sizeof(char) * SIZEMAX_TREE * SIZEMAX_CHAR * 3);

    if (wt->n == 0) {
        return NULL;
    }

    char * s;

    s = wfts(&(wt->tab[0]));
    strcpy(res, s);
    free(s);

    for (int i = 1; i < wt->n; i++) {

        strcat(res, "\n");
        s = wfts(&(wt->tab[i]));
        strcat(res, s);
        free(s);
    }

    strcat(res, "\n");
    
    return res;
}

/* 
  Question 4.8
  Format : WorkFile1 \n WorkFile2 \n ... WorkFileN \n
*/

WorkTree *stwt(char *ch) {

    WorkTree *wt = initWorkTree();

    int debut = 0, i = 0, cpt = 0, position = 0;
    char *tmp = malloc(3* SIZEMAX_CHAR * sizeof(char));

     while (ch[i] != '\0') {

        if (ch[i] != '\n') {
            cpt++;
        } else {

            strcpy(tmp, ch + debut);
            tmp[cpt] = '\0';

            WorkFile *new = stwf(tmp);

            appendWorkTree(wt, new->name, new->hash, new->mode);
            //freeWorkFile(new);
            debut = i + 1;
            cpt = 0;
        }

        i++;
    }

    return wt;
}

/*
  Question 4.9 : Ecriture dans un fichier
*/

int wttf(WorkTree *wt, char *file) {

    FILE *dest = fopen(file, "w");
    char *chaine = wtts(wt);

    fprintf(dest, "%s", chaine);
    fclose(dest);
    free(chaine);

    return 1;
}

/*
  Question 4.10 : recuperation d'un worktree depuis un fichier
*/

WorkTree *ftwt(char *file) {

    FILE *source = fopen(file, "r");
    if (source == NULL) {
        printf("Erreur a l'ouverture - la source n'existe pas\n");
        return NULL;
    }

    char buffer[3*SIZEMAX_CHAR];

    WorkTree * wt = initWorkTree();

    while (fgets(buffer, 3*SIZEMAX_CHAR, source) != NULL) {

        WorkFile * wf = stwf(buffer);
        appendWorkTree(wt, wf->name, wf->hash, wf->mode);
        freeWorkFile(wf);
    }

    fclose(source);
    return wt;
}

/*
  Question 5.1 : principe du blobFile
*/

char *blobWorkTree(WorkTree *wt) {

    static char template[] = "/tmp/myfileXXXXXX";
    char fname[1000];
    strcpy(fname, template);
    fname[17] = '\0';

    int fd = mkstemp(fname);
    wttf(wt, fname);

    char *hash = sha256file(fname);
    char *hashPath = hashToPath(hash);

    char repertoire[3];
    repertoire[0] = hashPath[0];
    repertoire[1] = hashPath[1];
    repertoire[2] = '\0';

    if (!file_exists(repertoire)) {

        char *commande = malloc(sizeof(char) * 15);

        sprintf(commande, "mkdir %c%c", hashPath[0], hashPath[1]);
        system(commande);
        free(commande);
    }

    strcat(hashPath, ".t");
    cp(hashPath, fname);
    setMode(0777, hashPath);
    free(hashPath);

    return hash;
}

/*
  Question 5.2 : sauvegarde d'un workTree de maniere recursive
*/

char *concatenation_chaines(char *ch1, char *ch2) {
    char *res = malloc((strlen(ch1) + strlen(ch2) + 2) * sizeof(char));
    strcpy(res, ch1);
    strcat(res, "/");
    strcat(res, ch2);
    return res;
}

char *saveWorkTree(WorkTree *wt, char *path) {

    for (int i = 0; i < wt->n; i++) {

        char *pathTotal = concatenation_chaines(path, wt->tab[i].name);

        if (isDir(pathTotal) == 1) { // c'est un repertoire

            List *list = listdir(pathTotal);

            WorkTree *newWT = initWorkTree();
            Cell *tmp = *list;

            while (tmp) {
                if (tmp->data[0] != '.') {
                    appendWorkTree(newWT, tmp->data, NULL, 0);
                }
                tmp = tmp->next;
            }

            freeList(list);
            
            wt->tab[i].mode = getChmod(pathTotal);
            char * hashNewWT = saveWorkTree(newWT, pathTotal);
            wt->tab[i].hash = strdup(hashNewWT);
            free(hashNewWT);

            freeWorkTree(newWT);

        } else { // c'est un fichier

            blobFile(pathTotal);
            wt->tab[i].mode = getChmod(pathTotal);
            char * hashFile = sha256file(pathTotal);
            wt->tab[i].hash = strdup(hashFile);
            free(hashFile);
        }

        free(pathTotal);
    }
  
    return blobWorkTree(wt);
}

/*
  Question 5.3 : restore d'un WorkTree
*/

int isWorkTree(char *hash) {

    char *path = hashToPath(hash);
    int trouve = -1;
    
    char *nom_fichier = path + 3;
    char *nom_fichier_tree = malloc(sizeof(char) * (SIZEMAX_CHAR + 4));
    sprintf(nom_fichier_tree, "%s.t", nom_fichier);

    char repertoire[3];
    repertoire[0] = hash[0];
    repertoire[1] = hash[1];
    repertoire[2] = '\0';

    List *list = listdir(repertoire);

    if (searchList(list, nom_fichier_tree) != NULL) {
        trouve = 1;
    }

    else if (searchList(list, nom_fichier) != NULL) {
        trouve = 0;
    }

    free(path);
    free(nom_fichier_tree);
    freeList(list);
    
    return trouve;
}

void restoreWorkTree(WorkTree *wt, char *path) {

    for (int i = 0; i < wt->n; i++) {

        char *pathTotal = concatenation_chaines(path, wt->tab[i].name);
        char *hash = wt->tab[i].hash;
        char *hashCopy = hashToPath(hash);

        if (isWorkTree(hash) == 1) { // c'est un repertoire

            strcat(hashCopy, ".t");
            WorkTree *newWT = ftwt(hashCopy);

            restoreWorkTree(newWT, pathTotal);
            setMode(getChmod(hashCopy), pathTotal);
        }

        else if (isWorkTree(hash) == 0) { // c'est un fichier
            cp(pathTotal, hashCopy);
            setMode(getChmod(hashCopy), pathTotal);
        }

        free(pathTotal);
    }
}
