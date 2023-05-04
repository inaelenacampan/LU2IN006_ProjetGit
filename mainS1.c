#include "Headers/ListLC.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char ** argv){

    //Tests effectues pour la semaine n°1

    char * hash = sha256file("testS1/sourceH.txt");
    printf("Test hashage par fichier temporel : %s\n", hash);
    free(hash);


    List * L = initList();
    Cell * c1 = buildCell("L1");
    Cell * c2 = buildCell("L2");
    Cell * c3 = buildCell("L3");
    Cell * c4 = buildCell("L4");

    insertFirst(L, c1);
    insertFirst(L, c2);
    insertFirst(L, c3);
    insertFirst(L, c4);

    char * listeCh = ltos(L);
    printf("Test creation de list : %s\n",listeCh);

    free(listeCh);

    Cell * tmp = listGet(L, 2);
    if(tmp!=NULL){
        printf("Element present dans la liste sur la 3e position : %s\n", ctos(tmp));
    }else{
        printf("Indice qui depasse le nombre d'element dans la liste!\n");
    }
    
    Cell * search = searchList(L, "L3");
    if(search!=NULL){
        printf("Element trouve : %s\n", ctos(search));
    }else{
        printf("L'element ne figure pas dans la liste ! \n");
    }
    freeList(L);

    
    List * List2 = stol("ch1|ch2|ch3|ch4|ch5");
    char * chList2 = ltos(List2);
    printf("Chaine obtenue a partir d'une liste : %s\n",chList2);

    ltof(List2, "testS1/ecritureList.txt");

    free(chList2);
    freeList(List2);

    List * listFile = ftol("testS1/lectureList.txt");
    char * chFile = ltos(listFile);
    printf("Chaine obtenue a partir d'une liste d'un file : %s\n", chFile);

    free(chFile);
    freeList(listFile);

    List * repertoire_courant = listdir(".");
    char * chRepertoire_courant = ltos(repertoire_courant);
    printf("Dans le repertoire courant : %s\n", chRepertoire_courant);

    free(chRepertoire_courant);
    freeList(repertoire_courant);

    int statut = file_exists("mainS1.c");
    if(statut==1){
        printf("Le fichier mainS1.c existe dans le repertoire courant ! \n");
    }else{
        printf("Le fichier mainS1.c n'existe pas dans le repertoire courant ! \n");
    }
    
    //Test de la fonction blob
    
    blobFile("testS1/testBlob.txt");
    
    return 0;
}