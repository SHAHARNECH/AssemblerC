#include "assembler.h"
#include <ctype.h>

 /*
  * operation struct by operation name, operation number, function number, how many parameters receives
  * struct in instructions_operations.h
  */
operation op_code[NUM_OPS] = {
			    {"mov", 0, 0, 2},
                            {"cmp", 1, 0, 2},
                            {"add", 2, 1, 2},
                            {"sub", 2, 2, 2},
                            {"lea", 4, 0, 2},
			    {"clr", 5, 1, 1},
                            {"not", 5, 2, 1},
                            {"inc", 5, 3, 1},
                            {"dec", 5, 4, 1},
                            {"jmp", 9, 1, 1},
                            {"bne", 9, 2, 1},
		            {"jsr", 9, 3, 1},
                            {"red", 12, 0, 1},
                            {"prn", 13, 0,1},
                            {"rts", 14, 0, 0},
                            {"stop", 15, 0, 0}
                            };
/*operation types*/
char *instructions[] =  {".data", ".string", ".entry", ".extern", NULL};



/**
***************************************************************************************************
* line parsing functions by data type
***************************************************************************************************
*/

/*
  * parse_line passes through each argument in line and roots it depending on instruction type
  * @param file name, line number, current line, pointer of current symbol , pointer of current data , pointer of current instruction,
  * pointer to entry flag, pointer to extern flag, pointer to IC, pointer to DC)
  * @return 1 if successful or 0 if error
  */
int parse_line(char *file_name,  int line_index, char *file_line, symbol_ptr *curr_symbol, data_ptr *curr_data, instruction_ptr *curr_instruction, int *entry_flag, int *extern_flag, unsigned int *IC, unsigned int *DC)
 {
    int var, dat;
    char *symbol,*token=NULL;
    symbol_struct *current;
    int status = ERROR;
    char check_length[LINE_LEN]={'\0'};
    char first_word[MAX_ARG_SIZE]={'\0'};
    char sec_word[MAX_ARG_SIZE]={'\0'};
    operation *op_ptr;

    /*last character in line is not \n therefore line is not 80 characters max*/
    if(file_line[strlen(file_line)-1]!='\n')
        return error_output(file_name, line_index, "Line too long");
    /* if line is blank space or ';' ignore and get next*/
    var = skip_white_spaces(file_line);
    if(file_line[var] == ';' || file_line[var] == '\n')
       { return 1;}

    /* exclude first word from line and check if it is 80 or less than characters*/
    else if(sscanf((file_line + var), "%s", check_length)){
      if(strlen(check_length)>MAX_ARG_SIZE){
			return error_output(file_name, line_index, "Argument too long");}

		strcpy(first_word, check_length);

    /*first word is symbol declaration*/
    if(first_word[(int)(strlen(first_word)-1)] == ':')
    {
        token=strtok(first_word," :");
        /*declaration: symbol cannot be assembler word*/
   	    if((is_data(first_word)<NUM_INST) || (is_instruction(first_word)))
            return error_output(file_name, line_index, "Symbol cant be instruction or operation");
        /*declaration: symbol has to start with letter*/
   	    if(!isalpha(first_word[0]))
             return error_output(file_name, line_index, "Symbol doesnt start with letter");
        /*if symbol returns i else 0*/
		if((is_symbol(file_name, line_index, token)>0))
         	  { status = SYMBOL;}
		else return error_output(file_name, line_index, "Invalid symbol");
	} /*end of declaration symbol*/

	/*first word is data inserted*/
	else if(is_data(first_word)<NUM_INST)
            {status = DATA;}
	/*first word is an instruction*/
	else if(is_instruction(first_word))
            {status = INSTRUCTION;}
	/*first word unknown*/
	else 
	  {return error_output(file_name, line_index, "Invalid input");}
    }

    switch (status){
        case SYMBOL: {  /*symbol declared*/
            /* cut out the next word to sec_word */
            file_line+=strlen(first_word)+1; /*first word length +':'*/
            var = skip_white_spaces(file_line);
	        file_line+=var;
            strcpy(sec_word, file_line);
            token=strtok(sec_word,"\" #");
            strcpy(sec_word, token);

            if(*sec_word){
		
                dat = is_data(sec_word);
                switch (dat){
                    case 0: { /* symbol declared then .data */
                        
                        strcpy(first_word,strtok(first_word,":")); /*extracting symbol*/
                        /* search if symbol already declared*/
                        if((current = find_symbol(first_word, curr_symbol))){
                          if(current->address == 0) {
                                current->address = (*DC);
                                /*parse the rest of the line and add the data to data list*/
                                return parse_data(file_name, line_index, file_line + var + strlen(sec_word), dat, curr_data, IC, DC);
                            }
                            else{
                              return error_output(file_name, line_index, "Symbol already declared");
                                 }  
                         }
                         else { /* the symbol wasn't declared so add to symbol table*/
                            if(add_symbol(file_name, line_index, first_word, curr_symbol, (*DC), 0, 0, 0))
                           	        return parse_data(file_name, line_index, file_line + var + strlen(sec_word), dat, curr_data, IC, DC);
			                 else return 0;
                             }
                    }/*end case .data*/

                    case 1: { /* symbol declared then .string */
                        symbol = strtok(first_word, ":");
                        var=skip_white_spaces(file_name+strlen(sec_word));
                        if((current = find_symbol(symbol,  curr_symbol))){
                            if(current->address == 0){
                                current->address = (*DC);
                               /* parse the rest of the line and add the data to data list*/
				                return parse_data(file_name, line_index, file_line + var + strlen(sec_word), dat, curr_data, IC, DC);
                            }
                            else{
                                return  error_output(file_name, line_index,"Symbol already declared");
                               
                            }
                        }
                        else {
                            /* the symbol wasn't declared so add to symbol table*/
                            if(add_symbol(file_name, line_index, symbol, curr_symbol, (*DC), 0, 0, 0))
                            	return parse_data(file_name, line_index, file_line + var + strlen(sec_word), dat, curr_data, IC, DC);
			                else return 0;
                        }
                    }/*end case .string*/

                    case 2: { /* symbol declared then .entry not valid*/
                       var=skip_white_spaces(file_line);
			var+= strlen(sec_word);
			file_line+=var;
			var=skip_white_spaces(file_line);
			
                        error_output(file_name, line_index,"Symbol before instruction ignored");
			if(parse_entry(file_name, line_index, file_line+var, curr_symbol, DC))
                    	{
                       		*entry_flag = YES;
                        	return 1;
			}
		            else return 0;
                    }

                    case 3: { /* symbol declared then .extern not valid*/
                        var=skip_white_spaces(file_line);
			var+= strlen(sec_word);
			file_line+=var;
			var=skip_white_spaces(file_line);
                         error_output(file_name, line_index,"Symbol before instruction ignored");
			/*parse extern data inserted and extern flag up*/
                    	if(parse_extern(file_name, line_index, file_line+var, curr_symbol))
                    	{
                        	*extern_flag = YES;
                       		return 1;
                    	}
		            else return 0;
                    }

                    default:break;
                }/*end switchcase*/
		sscanf(sec_word, "%s", sec_word);
                /* symbol declared then instruction operation*/
                if((op_ptr = is_instruction(sec_word)))
		{
                    if(!find_symbol(first_word, curr_symbol))
                     {		 if(!add_symbol(file_name, line_index, first_word, curr_symbol, (*IC), 1, 0, 0))
					        {return 0;}
			        /*get data after operation*/
                    var = strlen(sec_word);
                    var += skip_white_spaces(file_line+var);
                    file_line += var;
	
                    /*  add the command to the instruction list */
                    return add_instruction(file_name, line_index, file_line, curr_instruction, curr_symbol, op_ptr->op_code,  op_ptr->func_code, op_ptr->parameters, IC, DC);
		     }
                    else
                         return error_output(file_name, line_index,"Symbol already declared");
	        }
		  
           }
	   return error_output(file_name, line_index,"Symbol declared empty");
        }/*end case symbol*/

        case DATA: { /*start with data instruction*/
            dat = is_data(first_word);
	    var = skip_white_spaces(file_line);
	    file_line+=var;
	    file_line+=strlen(first_word); /*first word length */
            var = skip_white_spaces(file_line);
	   strcpy(sec_word, file_line+var);
         
        if(sec_word[0]==' ' ||sec_word[0]=='\t'||sec_word[0]=='\n')
		return error_output(file_name, line_index," missing data");
            switch (dat){
                case 0: { /*.data */
		  
		
                    /*parse data inserted and save room in address for it*/
                    if(parse_data(file_name, line_index, file_line, dat, curr_data, IC, DC))
			            {
                        
                        return 1;
			            }
                    else return 0;
                }
                case 1: { /* .string */
		
		 
                    /*parse data inserted and save room in address for it*/
                    if(parse_data(file_name, line_index, file_line, dat, curr_data, IC, DC))
		             {
			           
			            return 1;
		             }
                    else return 0;
                }
                case 2: { /* .entry */
                    /*parse entry data inserted and entry flag up*/
                   if(parse_entry(file_name, line_index, file_line, curr_symbol, DC))
                    {
                       *entry_flag = YES;
                        return 1;
			        }
		            else return 0;
                }
                case 3: { /* .extern */
                    /*parse extern data inserted and extern flag up*/
                    if(parse_extern(file_name, line_index, file_line, curr_symbol))
                    {
                        *extern_flag = YES;
                       return 1;
                    }
		            else return 0;
                }
                default:break;
            }/*end case data*/
          
        }/*end case data*/
	
	/*start with operation instruction*/
        case INSTRUCTION: {
            op_ptr =  is_instruction(first_word); /*valid operation*/
            var += strlen(first_word);
            var += skip_white_spaces(file_line+var);
            file_line += var;
            /*get data after and insert to instruction table*/
           return add_instruction(file_name, line_index, file_line, curr_instruction, curr_symbol, op_ptr->op_code, op_ptr->func_code,  op_ptr->parameters, IC, DC);
        }
        default:break;
    }/*end switchcase*/
	return 0;
}/*end func*/


/*
  * parse_data passes through data after instruction and inserts to the right table\address if valid
  * @param file name, line number, current line, pointer of current data ,pointer to IC, pointer to DC)
  * @return 1 if successful or 0 if error
  */
int parse_data(char *file_name, int line_index, char* file_line, int type, data_ptr *curr_data, unsigned int *IC, unsigned int *DC)
{
    int var, count,i, temp=0, cstring=0;
    char *num, *str;
	str = (char *) malloc(sizeof(char)*strlen(file_line));
    if (!str) {
        return error_output(file_name, line_index, "Out of memory");
    }
    switch (type)
    {
        case 0: { /* .data */
		
            /*extract the number value from the line */
            num = strtok(file_line, " ,\n");
	        count=0;
            /* while to get all numbers in the line that are separated by comma*/
            while (num != NULL) 
	        {
		
                var =  skip_white_spaces(num);
			if(!is_valid(file_name, line_index, num))
				return 0;
		        temp=atoi(num);
		        if(!num_data(file_name, line_index, temp, curr_data, IC, DC))  
			 return 0;
		        count++;
		        num = strtok(NULL, ",");
            }/*end while*/

		 /*   if(count-=1) at least one or more numbers
			   (*IC)+=count;*/
			
		    
            break;
        }/*case .data*/
        case 1: { /* .string */
            count = 0;
		    i=0;
            /*extract the string value from the line */
            var = skip_white_spaces(file_line);
		    file_line+=var;
		    strcpy(str, file_line);
		    str=strtok(str, "\n");
            /* loop going through all characters in word and counting apostrophes*/
	        for(i=0;i<strlen(str);i++)
		    {
			    if(str[i]=='"')
		  	    {
				    count++;	/*count apostrophes*/
			    }
		    cstring++; /*counting all characters in word*/
		    }
		
	   if(count>2) /*too many apostrophes*/
	   {
	       free(str);
			return error_output(file_name, line_index, "Cannot have apostrophes inside string");
	   }
	
	   if(!count)  /*missing both apostrophes*/
	   {
	       free(str);
	       return error_output(file_name, line_index, "Missing both apostrophes");
	   }

	   if(str[0] != '"')  /*missing apostrophe*/
       	   {
	       free(str);
		   return error_output(file_name, line_index, "Missing apostrophes at the beginning");
	   }
		for(i=1; i<strlen(str); i++)
		{ if(str[(strlen(str)-i)] !=' ' && str[(strlen(str)-i)] != '\t')
		 {
			break;}
	   	}

	   if(str[(strlen(str)-i)] != '"')  /*missing apostrophe*/
	   {
		   free(str);
		   return error_output(file_name, line_index, "Missing apostrophes at the end");
	   }
	   /*correct apostrophes, now insert each char to data table*/
	   for(i=1; i<(cstring-1); i++)
	    {
	     if(str[i]!='"')
	     {
	       if(!char_data(file_name, line_index, str[i], curr_data, IC, DC))
	       {
	           free(str);
	           return 0;
	       }
	     }
	    }
	   /*insert last char '\0' to data table*/
	   if(char_data(file_name, line_index,'\0', curr_data, IC, DC))
		{
	       /*(*IC)+=(cstring-2);*/
		
	       break;
		}
	   else
	   {
		    free(str);
		    return 0;
           }
        }/*end case .string*/
        default:break;
    }/*end switch*/
   free(str);
   return 1;
}/*end func*/

/*
  * parse_entry passes through data after .entry and inserts to the symbol table with entry flag up
  * @param file name, line number, current line, pointer of current symbol , pointer to DC)
  * @return 1 if successful or 0 if error
  */
int parse_entry(char *file_name, int line_index, char* file_line, symbol_ptr *curr_symbol, unsigned int *DC){
    char word[LINE_LEN];
    char *token;
    sscanf(file_line, "%s",word);
    token=strtok(file_line, " \t\n");
    token=strtok(NULL, " \t\n");
 

	if(word[0]==' '||word[0]=='\n')
		return error_output(file_name, line_index, "No symbol inserted");
        /*checking if there is data after symbol name*/
	if(token)
    		 return error_output(file_name, line_index, "Too many parameters");
	/*checking second word is symbol*/
	if(find_symbol(word, curr_symbol))
	{
	    find_symbol(word, curr_symbol)->entry = 1;
            return 1;
	}
	else{ /*symbol wasnt declared yet so add to symbol table*/
	    return (add_symbol(file_name, line_index, word, curr_symbol, *DC, 0, 1, 0));
        }
}

/*
  * parse_extern passes through data after .extern and inserts to the symbol table with extern flag up
  * @param file name, line number, current line, pointer of current symbol)
  * @return 1 if successful or 0 if error
  */
int parse_extern(char *file_name, int line_index, char* file_line, symbol_ptr *curr_symbol){
    char word[LINE_LEN];
    char *token;
    sscanf(file_line, "%s",word);
    if(word[0]==' '||word[0]=='\n')
	return error_output(file_name, line_index, "No symbol inserted");
    token=strtok(file_line, " \t\n");
    token=strtok(NULL, " \t\n");
    /*checking if there is data after symbol name*/
	if(token)
    		 return error_output(file_name, line_index, "Too many parameters");

	return add_symbol(file_name, line_index, word, curr_symbol, 0, 0, 0, 1);
}



/**
 ***************************************************************************************************
 * general parsing functions
 ***************************************************************************************************
 */


/*function to skip white spaces, returns the number of steps to skip */
int skip_white_spaces(const char* line){
    int i = 0;
    /* skip spaces and tabs */
    while(line[i] == ' ' || line[i] == '\t')
        i++;
    return i;
}

/* is_symbol function to check if the word given is a correct written symbol or not, returns the length of the symbol*/
int is_symbol(char *file_name, int line_index, const char *word){
    int i = 0, count=0;
   /* symbol begins with a letter and can contain only letters and digits*/
    if(isalpha(word[0]))
    {
        i++;
	    count++;
        while(isdigit(word[i]) || isalpha(word[i]))
            i++;

        while(word[count])
	    {
		    if(word[count]=='\n'|| word[count]=='\t' || word[count]==32)
		        break;
		count++;
	    }
    }

   if(count==i)
        return i;
   else return 0;  
}

/* is_data function checks if the word is a data instruction and returns location in instruction array*/
int is_data(const char* word){
int i=0;
    while(instructions[i]){
        if(!strcmp(word, instructions[i]))
            return i;
        i++;
    }
    return i;
}
/* is_instruction function checks if the word is an operation, if it is it returns the op code of the operation*/
operation *is_instruction(const char* word){
    int i = 0;
    /* comparing to each word in the op_code array */
    while(i < NUM_OPS )
    {
        if(!strncmp(word, op_code[i].op_name,4))
           {return &op_code[i];}
        i++;
    }
    return NULL;
}

/* is_comma function checks if there's a comma in the line to know is there are multiple arguments*/
int is_comma(char *file_name, int line_index, const char* file_line){
int count=0;
    if(*file_line == ',')  /* the first character is a comma so an argument is missing */
    {
        return -1;
    }
    file_line++;
    while(*file_line)
    {
        if(*file_line == ',')
        {
		    count++;
            if(*(file_line+1) == '\0')  /*argument is missing */
                return-1;
            if(*(file_line+1) == ',')   /* multiple commas */
                return -1;
        }
        file_line++;
    }
if(count)
    return 0;
else
     return -1;
}

/* is_end function checks if the line has ended, 1 if end 0 if not*/
int is_end(const char *line){
    int i = 0;

i += skip_white_spaces(line);
    if(line[i] == '\n'||line[i]=='\0' )
        return 1;
    else     
	return 0;
}

/* is_valid function checks if a given argument is a legal parameter*/
int is_valid(char *file_name, int line_index, char *arg) {

    if (*arg == '\n' || *arg == '\0' || *arg == EOF || arg[0] == ';')  /* skip line*/
        return 0;

while((*arg)== '#'||(*arg)== ' '||(*arg)== '+'||(*arg)== '-'||(*arg)== '\t')
       {arg++;}
		
        while (*arg)
        {
            if (isdigit(*arg))
	        arg++;
            else { 
	            if (is_end(arg))
                        return 1;
                    else
                        return error_output(file_name, line_index, "Wrong paremeter");      
                 }
       }/*end while*/
 return 1;
}

/*end parse*/
