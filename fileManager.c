#include "assembler.h"
/**
 * start_assembly function allocates memory and opens the file
 * reads from file line by line, if unsuccessful sends error, else closes file
 * @param main, argc argv[]
 * @return - if memory malfunction or after closing file returns 0
 */

int start_assembly(int argc, char *argv[])
{
    FILE *src_file;
    int i;
    char *file_name;
    file_name = (char *)malloc(strlen(FILE_INPUT));
        if(!file_name)
                { fprintf(stdout, "ERROR-failed to allocate memory\n");
		return 0;}
        
    /* Scanning argv from the second argument, first file to be parsed */
    for(i=1; i < argc; i++)
    {
        file_name = (char *)realloc(file_name, strlen(argv[i]) + strlen(FILE_INPUT));
        if(!file_name)
                { fprintf(stdout, "ERROR-failed to allocate memory\n");
		return 0;}
      

        strcpy(file_name, argv[i]);
        strcat(file_name,FILE_INPUT);

        /*Opens the file to read from*/
        src_file = fopen(file_name, "r+");
        if(!src_file) /* If the wasn't found or it isn't allowed to read the file pointer is NULL */
        { /*file could not open*/
          fprintf(stdout, "ERROR-file could not open successfully\n");
          continue;
        }
	else{
       /*start passings -if successful closes file and free name else print error*/
        if(start_passings(src_file, file_name)) /* success in parsing and passings*/
            fprintf(stdout, "File %s has been read and written successfully\n",file_name);
        fclose(src_file); /* Closes the file after reading and frees the file_name string for the next file name */
       }

    }
 free(file_name);
return 0;
}





