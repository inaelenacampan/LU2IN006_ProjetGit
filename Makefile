all : mainS1 mainS2 mainS3 mainS5 myGit

mainS1.o : mainS1.c Headers/ListLC.h
	gcc -g -o mainS1.o -c mainS1.c

mainS2.o : mainS2.c Headers/ListLC.h Headers/Work.h
	gcc -g -o mainS2.o -c mainS2.c

mainS3.o : mainS3.c Headers/ListLC.h Headers/Work.h Headers/Commit.h
	gcc -g -o mainS3.o -c mainS3.c

mainS5.o : mainS5.c Headers/ListLC.h Headers/Work.h Headers/Commit.h Headers/References.h Headers/Merge.h
	gcc -g -o mainS5.o -c mainS5.c

myGit.o : myGit.c Headers/ListLC.h Headers/Work.h Headers/Commit.h Headers/References.h Headers/Merge.h
	gcc -g -o myGit.o -c myGit.c

Bibliotheque/ListLC.o : Bibliotheque/ListLC.c Headers/ListLC.h
	gcc -g -o Bibliotheque/ListLC.o -c Bibliotheque/ListLC.c

Bibliotheque/Work.o : Bibliotheque/Work.c Headers/Work.h Headers/ListLC.h
	gcc -g -o Bibliotheque/Work.o -c Bibliotheque/Work.c

Bibliotheque/Commit.o : Bibliotheque/Commit.c Headers/Commit.h Headers/Work.h Headers/ListLC.h
	gcc -g -o Bibliotheque/Commit.o -c Bibliotheque/Commit.c

Bibliotheque/References.o : Bibliotheque/References.c Headers/References.h Headers/Commit.h Headers/Work.h Headers/ListLC.h
	gcc -g -o Bibliotheque/References.o -c Bibliotheque/References.c

Bibliotheque/Merge.o : Bibliotheque/Merge.c Headers/Merge.h Headers/References.h Headers/Commit.h Headers/Work.h Headers/ListLC.h
	gcc -g -o Bibliotheque/Merge.o -c Bibliotheque/Merge.c

mainS1 : mainS1.o Bibliotheque/ListLC.o
	gcc -o mainS1 mainS1.o Bibliotheque/ListLC.o

mainS2 : mainS2.o Bibliotheque/ListLC.o Bibliotheque/Work.o
	gcc -o mainS2 mainS2.o Bibliotheque/ListLC.o Bibliotheque/Work.o

mainS3 : mainS3.o Bibliotheque/ListLC.o Bibliotheque/Work.o Bibliotheque/Commit.o
	gcc -o mainS3 mainS3.o Bibliotheque/ListLC.o Bibliotheque/Work.o Bibliotheque/Commit.o

mainS5 : mainS5.o Bibliotheque/ListLC.o Bibliotheque/Work.o Bibliotheque/Commit.o Bibliotheque/References.o Bibliotheque/Merge.o
	gcc -o mainS5 mainS5.o Bibliotheque/ListLC.o Bibliotheque/Work.o Bibliotheque/Commit.o Bibliotheque/References.o Bibliotheque/Merge.o

myGit : myGit.o Bibliotheque/ListLC.o Bibliotheque/Work.o Bibliotheque/Commit.o Bibliotheque/References.o Bibliotheque/Merge.o
	gcc -o myGit myGit.o Bibliotheque/ListLC.o Bibliotheque/Work.o Bibliotheque/Commit.o Bibliotheque/References.o Bibliotheque/Merge.o

clean :
	rm -f *.o Bibliotheque/*.o mainS1 mainS2 mainS3 mainS5 myGit
