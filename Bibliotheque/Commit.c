#include "../Headers/Commit.h"
#include "../Headers/ListLC.h"
#include "../Headers/Work.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define TAILLE_MAX 1000
#define SIZEMAX_CHAR 300

/*
  Questionn 6.4 : Choix de la fonction de hachage
*/

unsigned long HashSdbm(char *str) {
    unsigned long hash = 0;
    int c;
    while (c = *str++) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

/*
  Question 6.1 : creation d'une paire key-value
*/

kvp *createKeyVal(char *key, char *val) {
    kvp *kv = (kvp *)malloc(sizeof(kvp));

    if (key != NULL) {
        kv->key = strdup(key);
    }else{
        kv->key = NULL;
    }

    if (val != NULL) {
        kv->value = strdup(val);
    }else{
        kv->value = NULL;
    }

    return kv;
}

/*
  Liberation de l'espace memoire d'une paire key-value
*/

void freeKeyVal(kvp *kv) {

    if (kv->key != NULL) {
        free(kv->key);
    }

    if (kv->value != NULL) {
        free(kv->value);
    }

    free(kv);
}

/*
  Question 6.2 : chaine de caracteres à partir d'un key-value, ainsi que la fonction inverse
*/

char *kvts(kvp *k) {

    char *res = (char *) malloc(3000 * sizeof(char));

    strcpy(res, k->key);
    strcat(res, " :");

    if (k->value != NULL) {
        strcat(res, k->value);
    }
    return res;
}

kvp *stkv(char *str) {

    char *key = malloc(sizeof(char) * SIZEMAX_CHAR);
    char *value = malloc(sizeof(char) * SIZEMAX_CHAR);

    int n = sscanf(str, "%s :%s", key, value);
    kvp *res;

    if(n==2){
        res = createKeyVal(key, value);
    }

    else{
        sscanf(str, "%s :", key);
        res = createKeyVal(key, NULL);
    }

    if(value!=NULL){
        free(value);
    }
    
    free(key);

    return res;
}

/*
  Question 6.3 : initialisation de la table de Commit et sa fonction de free
*/

Commit *initCommit() {
    Commit *c = (Commit *)malloc(sizeof(Commit));

    c->size = TAILLE_MAX;
    c->n = 0;

    c->T = (kvp **)malloc((c->size) * sizeof(kvp *));

    int i;
    for (i = 0; i < (c->size); i++) {
        c->T[i] = NULL;
    }
    return c;
}

void freeCommit(Commit *c) {

    for (int i = 0; i < (c->size); i++) {
        if (c->T[i] != NULL) {
            freeKeyVal(c->T[i]);
        }
    }

    free(c->T);
    free(c);
}

/*
  Question 6.5 : ajout d'une paire key-value dans la table commit
*/

void commitSet(Commit *c, char *key, char *value) {

    int index = HashSdbm(key) % (c->size);
    // la capacité maximale est atteinte
    if (c->n == c->size) {
        return;
    }

    while (c->T[index] != NULL) {
        index = (index + 1) % (c->size);
    }

    kvp *elem = createKeyVal(key, value);
    c->n++;
    c->T[index] = elem;

    return;
}

/*
  Question 6.6 : creation d'un commit, le hash est associé à la clée tree
*/

Commit *createCommit(char *hash) {

    Commit *c = initCommit();
    commitSet(c, "tree", hash);

    return c;
}

/*
  Question 6.7 : pour recuperer la valeur d'une cle
*/

char *commitGet(Commit *c, char *key) {

    int index = HashSdbm(key) % (c->size);
    int i = 0;
    int n = c->size;

    while (i < n) {

        if (c->T[index] != NULL) {
            if (strcmp(c->T[index]->key, key) == 0) {
                return c->T[index]->value;
            }
        }

        index = (index + 1) % (c->size);
        i++;
    }

    return NULL;
}

/*
  Question 6.8 : création d'une chaine de caracteres a partir d'un commit et l'inverse
*/

char *cts(Commit *c) {
    char *res = malloc(TAILLE_MAX * TAILLE_MAX * sizeof(char));

    if (c->n == 0) {
        return NULL;
    }

    char *tmp;
    int premier = 0;

    for (int i = 0; i < c->size; i++) {

        if (c->T[i] != NULL) {

        tmp = kvts(c->T[i]);
        if (premier == 0) {

            strcpy(res, tmp);
            premier = 1;

        } else {

            strcat(res, "\n");
            strcat(res, tmp);
        }

        free(tmp);
        }
    }
    return res;
}

Commit *stc(char *ch) {

    Commit *c = initCommit();
    int debut = 0, i = 0, cpt = 0, position = 0;

    // récuperation d'une paire key-value
    char tmp[1000];

    while (ch[i] != '\0') {

        if (ch[i] != '\n') {

            cpt++;
        } else {

            strcpy(tmp, ch + debut);
            tmp[cpt] = '\0';

            kvp *new = stkv(tmp);
            commitSet(c, new->key, new->value);

            freeKeyVal(new);

            debut = i + 1;
            cpt = 0;
        }
        i++;
    }

    strcpy(tmp, ch + debut);
    tmp[cpt] = '\0';

    kvp *new = stkv(tmp);
    commitSet(c, new->key, new->value);

    freeKeyVal(new);

    return c;
}

/*
  Question 6.9 : ecriture dans un fichier / lecture depuis un fichier d'un commit
*/

void ctf(Commit *c, char *file) {
    FILE *f = fopen(file, "w");
    char *chaine = cts(c);
    fprintf(f, "%s", chaine);
    fclose(f);
    free(chaine);
}

Commit *ftc(char *file) {

    FILE *source = fopen(file, "r");
    if (source == NULL) {
        printf("Erreur a l'ouverture - la source n'existe pas\n");
        return NULL;
    }

    char buffer[256];
    char *ch = malloc(TAILLE_MAX * 3000 * sizeof(char));

    // pour gerer la création de la chaine résultat
    int premier = 0;

    while (fgets(buffer, 256, source) != NULL) {
        if (premier == 0) {
            strcpy(ch, buffer);
            premier = 1;
        } else {
            strcat(ch, buffer);
        }
    }

    fclose(source);
    Commit *c = stc(ch);
    free(ch);
    return c;
}

/*
  Question 6.10 : blobCommit - meme principe que pour BlobFile
*/

char *blobCommit(Commit *c) {

    static char template[] = "/tmp/myfileXXXXXX";
    char fname[1000];
    strcpy(fname, template);
    fname[17] = '\0';

    int fd = mkstemp(fname);
    ctf(c, fname);

    char *hash = sha256file(fname);
    char *hashPath = hashToPath(hash);

    char repertoire[3];
    repertoire[0] = hashPath[0];
    repertoire[1] = hashPath[1];
    repertoire[2] = '\0';

    if (!file_exists(repertoire)) {

        char *commande = malloc(sizeof(char) * 256);

        sprintf(commande, "mkdir %c%c", hashPath[0], hashPath[1]);
        system(commande);
        free(commande);
    }

    strcat(hashPath, ".c");

    cp(hashPath, fname);
    free(hashPath);
    return hash;
}