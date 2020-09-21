assembler: fileManager.o passings.o parseLine.o outputManager.o data.o assembler.o
	gcc -g -ansi -Wall -pedantic fileManager.o passings.o parseLine.o outputManager.o data.o assembler.o -o assembler

assembler.o: assembler.c assembler.h instructions_operations.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

data.o: data.c assembler.h instructions_operations.h
	gcc -c -ansi -Wall -pedantic data.c -o data.o

outputManager.o: outputManager.c assembler.h instructions_operations.h
	gcc -c -ansi -Wall -pedantic outputManager.c -o outputManager.o

parseLine.o: parseLine.c assembler.h instructions_operations.h
	gcc -c -ansi -Wall -pedantic parseLine.c -o parseLine.o

passings.o: passings.c assembler.h instructions_operations.h
	gcc -c -ansi -Wall -pedantic passings.c -o passings.o

fileManager.o: fileManager.c assembler.h instructions_operations.h
	gcc -c -ansi -Wall -pedantic fileManager.c -o fileManager.o

clean:
	rm -rf *.o
