#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * starts the two passing's, if successfully closes and free filename allocated else prints error
* @param current file reading from, name of current file
 * return 1 if success and 0 if error
*/
int start_passings(FILE *fptr, char * file_name)
{
    unsigned int IC=100; /*IC start from 100*/
    unsigned int DC=0;
    int entry_flag = NO;
    int extern_flag = NO;
    int line_index=0;
    int  error=NO;
    char file_line[LINE_LEN];
  
  symbol_ptr curr_symbol;
  data_ptr curr_data;
  instruction_ptr curr_instruction;
  data_ptr ptr_d;

    /*initializing to NULL*/
    curr_symbol = NULL, curr_data = NULL, curr_instruction = NULL;

	/*extracting line from file until end of file*/
	while((fgets(file_line, LINE_LEN, fptr)!=NULL))
	{
	    line_index++; /*start from line #1 */

	        /*start parsing line if successful 0 else -1*/
      		if(!parse_line(file_name, line_index, file_line, &curr_symbol, &curr_data, &curr_instruction, &entry_flag, &extern_flag, &IC, &DC))
      		{	
      	    		error=YES; /*parse returned no so error flag up*/
      		}
	}

/*first passing did not go successfully so free lists and stop here*/
if(error)
	{
        fprintf(stdout, "\nERROR FIRST PASS-file not written correctly\n");
	    free_lists(&curr_symbol, &curr_data, &curr_instruction);
	    return 0;
	}

/*first passing went successfully- continue*/
else{
    	update_data(&curr_data, &IC, &DC); /*update data table*/
    	ptr_d = curr_data;
   		 while(ptr_d)     /*writing data in binary machine code*/
   		    {
      		 binary_data(ptr_d);
       		 ptr_d = ptr_d->next;
   		    }
       
	    if(!update_symbol_table(&curr_symbol, &IC))  /*update data table*/
		error=YES;

        /*update instruction table, 1 successful 0-> error flag*/
        if(!update_instruction_table(file_name, line_index, &curr_symbol, &curr_instruction))
		    error=YES;
}
    /*second passing did not go successfully so free lists and stop here*/
    if(error)
    {
        fprintf(stdout, "\nERROR SECOND PASS-file not written correctly\n");
	    free_lists(&curr_symbol,&curr_data, &curr_instruction);
	    return 0;
    }

    /*second passing went successfully- write output files*/
    else
	  write_output_file(file_name, &curr_symbol, &curr_data, &curr_instruction,&IC, &DC, &entry_flag, &extern_flag);
	free_lists(&curr_symbol, &curr_data, &curr_instruction);
	return 1;
}

/*end passings*/


