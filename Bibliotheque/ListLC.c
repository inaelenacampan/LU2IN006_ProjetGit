#include "../Headers/ListLC.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZEMAX_DATA 300
#define SIZEMAX_LIST 50
#define SIZEMAX_CHAR 300

/*
  Question 1.3: ecriture le hash d'un fichier dans une destination
*/

int hashFile(char *source, char *dest) {
    char commande[1000];
    sprintf(commande, "cat %s | sha256sum > %s", source, dest);
    return system(commande);
}

/*
  Question 1.4 : retourne le hash d'un fichier, via un fichier temporaire
*/

char *sha256file(char *file) {

    static char template[] = "/tmp/myfileXXXXXX";

    char fname[1000];
    strcpy(fname, template);

    int fd = mkstemp(fname);

    hashFile(file, fname);

    FILE *test = fopen(fname, "r");
    char *res = malloc(sizeof(char) * SIZEMAX_CHAR);

    fscanf(test, "%s", res);
    fclose(test);

    char commande[1010];
    // res a alloue un espace de 1000*sizeof(char) + "rm -f  "

    sprintf(commande, "rm -f %s", fname);
    system(commande);

    return res;
}

/*
  Question 2.1 : initialisation d'une liste
*/

List *initList() {

    List *L = malloc(sizeof(List));
    *L = NULL;

    return L;
}

/*
  Question 2.2 : allocation mémoire d'une cellule d'une liste,
  ainsi que la libération de la cellule et de la liste
*/

Cell *buildCell(char *ch) {

    Cell *c = malloc(sizeof(Cell));
    c->data = strdup(ch);
    c->next = NULL;

    return c;
}

void freeCell(Cell *c) {

    if (c != NULL) {
        if (c->data != NULL) {
            free(c->data);
        }
        free(c);
    }
}

void freeList(List *L) {

    if (L == NULL) {
        return;
    }

    Cell *cellule = *L;

    while (cellule != NULL) {
        Cell *tmp = cellule->next;
        freeCell(cellule);
        cellule = tmp;
    }

    free(L);
}

/*
  Question 2.3 : fonction d'insertion en tete
*/

void insertFirst(List *L, Cell *C) {

    C->next = *L;
    *L = C;
}

/* 
  Question 2.4 : retourne le champ data d'une cellule
  Pour une liste : Retourne NULL pour une liste vide;
  sinon, une chaine ayant le format : chaine1|chaine2|...|chaineN
*/

char *ctos(Cell *c) {

    if (c->data != NULL) {
        char * data = c->data;
        return data;
    }

    return "";
}

char *ltos(List *L) {

    char *res = malloc(sizeof(char) * SIZEMAX_DATA * SIZEMAX_LIST);

    Cell *tmp = *L;

    if (tmp == NULL) {
        return NULL;
    }

    // la tete de la liste

    strcpy(res, tmp->data);
    tmp = tmp->next;

    while (tmp) {
        strcat(res, "|");
        strcat(res, tmp->data);
        tmp = tmp->next;
    }

    return res;
}

/*
  Question 2.5 : si il existe, retourne l'element d'indice i dans la liste
*/

Cell *listGet(List *L, int i) {

    Cell *tmp = *L;

    if (tmp == NULL) {
        return NULL;
    }

    int j;

    for (j = 0; j < i && tmp != NULL; j++) {
        tmp = tmp->next;
    }

    if (j == i)
        return tmp;

    return NULL;
}

/*
  Question 2.6 : si un element existe, retourne le pointer dans la liste
*/

Cell *searchList(List *L, char *str) {

    if(*L==NULL){
        return NULL;
    }

    Cell *tete = *L;
    while (tete) {

        if (strcmp(tete->data, str) == 0) {
            return tete;
        }

        tete = tete->next;
    }

    return NULL;
}

/*
  Question 2.7
  La chaine donne en parametre a le format : ch1|ch2|...chN
  La liste obtenue est dans le meme ordre que les elements de la chaine donnee
*/

List *stol(char *s) {

    List *L = initList();
    Cell *tete = NULL;

    int debut = 0, i = 0, cpt = 0;

    Cell *C = NULL;

    // correspond a une cellule
    char tmp[SIZEMAX_DATA];

    while (s[i] != '\0') {

        if (s[i] != '|') {
            cpt++;
        }

        else {

            strcpy(tmp, s + debut);
            tmp[cpt] = '\0';

            C = buildCell(tmp);
            if (*L == NULL) {
                insertFirst(L, C);
                tete = *L;
            } else {

                tete->next = C;
                tete = tete->next;
            }

            debut = i + 1;
            cpt = 0;
        }

        i++;
    }

    strcpy(tmp, s + debut);
    tmp[cpt] = '\0';
    C = buildCell(tmp);

    if (*L == NULL) {
        insertFirst(L, C);
    } else {
        tete->next = C;
        tete = tete->next;
    }

    return L;
}

/*
  Question 2.8 : ecriture de la liste dans un fichier
*/

void ltof(List *L, char *path) {

    FILE *f = fopen(path, "w");
    char *chaine = ltos(L);
    fprintf(f, "%s\n", chaine);
    free(chaine);
    fclose(f);
}

/*
  Question 2.9 : recuperer une liste depuis un fichier
*/

List *ftol(char *path) {

    FILE *f = fopen(path, "r");

    if (f == NULL) {
        printf("Erreur a l'ouverture - la source n'existe pas\n");
        return NULL;
    }

    char buffer[SIZEMAX_DATA*SIZEMAX_LIST];
    fgets(buffer, SIZEMAX_DATA*SIZEMAX_LIST, f);
    List *L = stol(buffer);

    return L;
}

/* 
  Question 3.1 : liste du contenu du repertoire donne en parametre
*/

List *listdir(char *root_dir) {

    List *L = initList();
    DIR *dp = opendir(root_dir);

    struct dirent *ep;
    if (dp != NULL) {

        while ((ep = readdir(dp)) != NULL) {
            Cell *c = buildCell(ep->d_name);
            insertFirst(L, c);
        }
        closedir(dp);
    }

    return L;
}

/*
  Question 3.2 : verifie l'existance d'un fichier dans le repertoire courant
*/

int file_exists(char *file) {

    //on se place dans le repertoire courant
    List *L = listdir(".");

    int trouve;

    if (searchList(L, file) == NULL) {
        trouve = 0;
    } else {
        trouve = 1;
    }

    freeList(L);
    return trouve;
}

/*
  Question 3.3 : fonction cp sous linux (copie du contenu d'un fichier source vers dest) 
*/

void cp(char *to, char *from) {

    FILE *source = fopen(from, "r");

    if (source == NULL) {
        printf("Erreur a l'ouverture - la source n'existe pas\n");
        return;
    }

    FILE *dest = fopen(to, "w");
    char buffer[256];

    //copie ligne par ligne
    while (fgets(buffer, 256, source) != NULL) {
        fprintf(dest, "%s", buffer);
    }

    fclose(source);
    fclose(dest);
}

/*
  Question 3.4 : creation de path a partir du hash
*/

char *hashToPath(char *hash) {

    char *res = malloc(sizeof(char) * (SIZEMAX_CHAR + 2));
    sprintf(res, "%c%c/%s", hash[0], hash[1], hash + 2);
    return res;
}

/*
  Question 3.5 : blobFile
*/

void blobFile(char *file) {

    char * hash = sha256file(file);
    char *hashPath = hashToPath(hash);

    char repertoire[3];
    repertoire[0] = hashPath[0];
    repertoire[1] = hashPath[1];
    repertoire[2] = '\0';

    if (!file_exists(repertoire)) {

        char *commande = malloc(15 * sizeof(char));
        sprintf(commande, "mkdir %c%c", hashPath[0], hashPath[1]);
        system(commande);
        free(commande);

    }

    cp(hashPath, file);
    free(hash);
    free(hashPath);
}
