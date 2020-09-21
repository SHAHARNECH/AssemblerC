#include "assembler.h"
/**
 *************************************************
 * functions for managing stdout and output files
 *************************************************
 **/



/**
 * error_output function prints to stdout error sentence
 * @param file name, line number and error sentence
 * @returns 0
 */
int error_output(char *file_name, int line_index, char *sentence_err)
{
    fprintf(stdout, "ERROR-in %s line %d: %s\n", file_name, line_index, sentence_err);
    return 0;
}

/**
 * write_output_file function uses create functions and makes the output files
 * @param file name, pointer to symbol table, pointer to data table, pointer to instruction table,
 * pointer to IC,pointer to DC, pointer to entry flag, pointer to extern flag)
 * @return 0 if memory malfunction or 1 after creating files
 */
int write_output_file(char *file_name, symbol_ptr *curr_symbol, data_ptr *curr_data, instruction_ptr *curr_instruction, unsigned int *IC,unsigned int *DC, int *entry_flag, int *extern_flag)
  {  
	
	if(!create_ob(file_name, curr_data, curr_instruction, IC, DC))
		return 0;
	if(*entry_flag){
	    if(!create_entry(file_name, curr_symbol))
	        return 0; }
	if(*extern_flag){
	    if(!create_extern(file_name, curr_instruction))
	        return 0;}
   
	return 1;
}

/*create_entry function to create .ent file*/
int create_entry(char *file_name, symbol_ptr *curr_symbol){
    size_t length = (strlen(file_name)-strlen(FILE_INPUT)+strlen(OUTPUT_ENTRY));
    symbol_ptr ptr_s;
    FILE *entry_file;
    char *str;
    str = (char *)malloc(sizeof(char)*length);
    if(!str) {
        return error_output(file_name, 0,"Error Failed to allocate memory for a new string");
    }
    /* create the file name */
    strncpy(str, file_name, strlen(file_name)-strlen(FILE_INPUT));
    strcat(str, OUTPUT_ENTRY);

    /*Open the file for writing*/
    entry_file = fopen(str, "w+");

    if(!entry_file)
      {return error_output(file_name, 0,"Error Failed to open and write in file");
	}
    ptr_s = *curr_symbol;
    while(ptr_s){
        if(ptr_s->symbol.entry)
            fprintf(entry_file,"%s\t%.6d\n", ptr_s->symbol.name, ptr_s->symbol.address);
        ptr_s = ptr_s->next;
    }
    free(str);
    fclose(entry_file);
    return 1;
}

/*create_extern function to create .ext file*/
int create_extern(char *file_name, instruction_ptr *curr_instruction){
    instruction_ptr ptr_i;
    
    FILE *extern_file;
    char *str;
    str = (char *)malloc(sizeof(char)*strlen(file_name)-1);
    if(!str) {
        return error_output(file_name, 0,"Error Failed to allocate memory for a new string");  
    }
    strcpy(str,file_name);
    
    /* create the file name */
    str=strtok(str,".");
    strcat(str, OUTPUT_EXT);

    /*Open the file for writing*/
    extern_file = fopen(str, "w+");

    if(!extern_file)
         {return error_output(file_name, 0,"Error Failed to open and write in file");
		}
    ptr_i = *curr_instruction;
    while(ptr_i){
      if(ptr_i->data.p1.symbl.ARE){ /*if there is second word*/
		if(ptr_i->data.p2.symbl.ARE) /*if there is third word symbol*/
		    {
			if(ptr_i->data.p1.symbl.ARE==E) /*if second word is extern*/
				fprintf(extern_file, "%s\t%.6d\n", ptr_i->data.p1.symbl.name, (ptr_i->data.IC)-2);
			if(ptr_i->data.p2.symbl.ARE==E) /*if third word is extern*/
				fprintf(extern_file, "%s\t%.6d\n", ptr_i->data.p2.symbl.name, (ptr_i->data.IC)-1);
			}		
		else {/*if there is third word number*/
		    if (ptr_i->data.p2.num.ARE){ /*if there is third word*/
			if(ptr_i->data.p1.symbl.ARE==E) /*if second word is extern*/
				fprintf(extern_file, "%s\t%.6d\n", ptr_i->data.p1.symbl.name, (ptr_i->data.IC)-2);}
		    else 
			{ /*no third word*/
		        if(ptr_i->data.p1.symbl.ARE==E) /*if second word is extern*/
				fprintf(extern_file, "%s\t%.6d\n", ptr_i->data.p1.symbl.name, (ptr_i->data.IC)-1);}
		     }
	 }
	else{
		if(ptr_i->data.p2.symbl.ARE)
			{
		    if(ptr_i->data.p2.symbl.ARE==E)
				fprintf(extern_file, "%s\t%.6d\n", ptr_i->data.p2.symbl.name, (ptr_i->data.IC)-2);
			}
   		}
	ptr_i = ptr_i->next;
    }
    free(str);
    fclose(extern_file);
    return 1;
}

/*create_ob function to create .ob file*/
int create_ob(char *file_name, data_ptr *curr_data, instruction_ptr *curr_instruction, unsigned int *IC, unsigned int *DC){
	unsigned int r;
    instruction_ptr  headi= *curr_instruction;
    data_ptr headd = *curr_data;
    size_t length = strlen(file_name)-strlen(FILE_INPUT)+strlen(OUTPUT_OB);
    FILE *object_file;
    char *str = (char *)malloc(sizeof(char)*length);
   
    if(!str)
    {
      return  error_output(file_name, 0,"Error Failed to allocate memory for a new string");
    }
    /* create file name */
    strncpy(str, file_name, strlen(file_name)-strlen(FILE_INPUT));
    strcat(str, OUTPUT_OB);

    /*Open the file for writing*/
    object_file = fopen(str, "w+");
    /* print to file the number of instruction lines and data line in memory */
    fprintf(object_file, "%d %d\n", (*IC)-100, *DC);
    
    /* loop for instruction list */
	while(headi){
        if(headi->final_first != 0){
            r=(headi->final_first);                                    
            fprintf(object_file, "%.7d \t %.6x\n",(headi->data.IC)++,r);
            if(headi->final_second != 0)
            {
                 r=headi->final_second;                                    
                 fprintf(object_file, "%.7d \t %.6x\n",(headi->data.IC)++,r);
            }
            if(headi->final_third != 0)
            {
                r=headi->final_third;
                fprintf(object_file, "%.7d \t %.6x\n",(headi->data.IC)++,r);
            }
        }
        headi = headi->next;
    }
    /*loop for data list*/
    while(headd){
        r=headd->final;
        fprintf(object_file, "%.7d \t %.6x\n", headd->data.address, r);
        headd = headd->next;
    }

 free(str);
 fclose(object_file);
 return 1;
}


/*end ouputManager*/
