#include "assembler.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*valid stock*/ 
char *stocks_r[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"}; 



/**
 ***************************************************************************************
 * functions for inserting data to tables in first pass and updating them in second pass
 ***************************************************************************************
 **/


/*
  * update_data runs through data table and updates address
  * @param pointer of current data, pointer to IC, pointer to DC)
  * @return void
  */
void update_data(data_ptr *curr_data, unsigned int *IC,  unsigned int *DC){
    data_ptr dat;
    unsigned int dat_memory;
    dat = *curr_data;
    dat_memory=(*IC);
    while (dat){
       dat ->data.address= dat_memory++;
       dat = dat->next;
    }
}

/*
  * update_symbol_table runs through symbol table and updates address
  * @param pointer of current symbol, pointer to IC)
  * @return 1 if successful 0 if not
  */
int update_symbol_table(symbol_ptr *curr_symbol, unsigned int *IC){
    symbol_ptr ptr_s;
    ptr_s = *curr_symbol;
    while(ptr_s)
    {
	if(ptr_s->symbol.entry && !ptr_s->symbol.address)
		 {  fprintf(stdout,"\nSymbol %s not declared",ptr_s->symbol.name);
			return 0; }
	if(ptr_s->symbol.external) /* if the symbol is external, the address needs to be set to zero */
            ptr_s->symbol.address = 0;
        else if(!ptr_s->symbol.oper)
	    ptr_s->symbol.address += (*IC); /*add IC to address with DC**/
	
        ptr_s = ptr_s->next;
    }
	return 1;
}

/*
  * update_instruction_table runs through instruction table
  * and updates address depending on address origin and destination
  * @param file name, line number, pointer of current symbol,pointer of current instruction)
  * @return 1 if successful else return 0
  */
int update_instruction_table(char *file_name, int line_index, symbol_ptr *curr_symbol, instruction_ptr *curr_instruction){
    instruction_ptr ptr_i;
    symbol_struct *current_symbol;
    ptr_i = *curr_instruction;
    while(ptr_i)
    { /* runs through the instruction list to update all the nodes */
        if(ptr_i->data.adrs_dest == DIST) /* the origin argument is a &distance */
        {   
            current_symbol = find_symbol(ptr_i->data.p1.symbl.name,  curr_symbol);
            if(!current_symbol){
		 fprintf(stdout,"\nSymbol %s not declared",ptr_i->data.p1.symbl.name);
                return 0;
            }
            else
                {
                if(current_symbol->external)
                     fprintf(stdout,"\nSymbol %s cannot be extern",ptr_i->data.p1.symbl.name);
                else
                   {
		    ptr_i->data.p1.symbl.address=complete_two_instruction((current_symbol->address)-(ptr_i->data.IC));
		    ptr_i->data.p1.symbl.ARE = A;
		   }
               }
        }
        else
            { /* the origin argument is a symbol */
            if(ptr_i->data.adrs_origin == SYB)
            {  
                current_symbol = find_symbol(ptr_i->data.p1.symbl.name,  curr_symbol);
                if(!current_symbol)
                { 
                     fprintf(stdout,"\nSymbol %s not declared",ptr_i->data.p1.symbl.name);
			return 0;
                }
                else
                    {
                        if(current_symbol->external)
                        {
                            ptr_i->data.p1.symbl.address = CONST_NUM; /*set address to zero*/
                            ptr_i->data.p1.symbl.ARE = E;
                        }
                        else
                        {
                            ptr_i->data.p1.symbl.ARE = R;
			                if(!ptr_i->data.p1.symbl.address)
				                ptr_i->data.p1.symbl.address = (current_symbol->address);
                        }

                    }
            }

            if(ptr_i->data.adrs_dest == SYB)
            {    /* the destination argument is a symbol */
		current_symbol=NULL;
		if(!ptr_i->data.p2.symbl.ARE && !ptr_i->data.p2.num.ARE)
		         {  /* no p2 only p1 */ 
		            current_symbol = find_symbol(ptr_i->data.p1.symbl.name , curr_symbol);
                    		if(!current_symbol)
				{
                    	 	fprintf(stdout,"\nSymbol %s not declared",ptr_i->data.p1.symbl.name);
				return 0;}
                    		else
                       		 {
                        	ptr_i->data.p1.symbl.address = current_symbol->address;
                        		if(current_symbol->external)
                           	 		ptr_i->data.p1.symbl.ARE = E;
                        		else
                            			ptr_i->data.p1.symbl.ARE = R;
                       		 }
			}

                else
		        {  
			    current_symbol= find_symbol(ptr_i->data.p2.symbl.name , curr_symbol);
				 
		            if(!current_symbol){
				 fprintf(stdout,"\nSymbol %s not declared",ptr_i->data.p2.symbl.name);
                    		return 0;}
                   	    else
                           {
                        	if(current_symbol->external)
                          		  ptr_i->data.p2.symbl.ARE = E;
			        else
			        {
					ptr_i->data.p2.symbl.ARE = R;
					ptr_i->data.p2.symbl.address = (current_symbol->address);
				}
		           }
		        }
		      
               
             }
	}
        /*instruction table has been updated now the binary representation should be updated */
        binary_instruction(ptr_i);
        ptr_i = ptr_i->next;
        }
	
	return 1;
    }

/* binary_instruction function updates the instruction table to machine code for 24 bits*/
void binary_instruction(instruction_ptr ptr){
    unsigned int temp = 0;
    /* set the binary representation for the operation */

    temp |= ptr->data.op_code; /* takes up 6 bits */
    temp <<= 2;
    temp |= ptr->data.adrs_origin; /* takes up 2 bits */
    temp <<= 3;	
    temp |= ptr->data.r_origin;  /* takes up 3 bits */
    temp <<= 2;
    temp |= ptr->data.adrs_dest;  /* takes up 2 bits */
    temp <<= 3;
    temp |= ptr->data.r_dest;  /* takes up 3 bits */
    temp <<= 5;
    temp |= ptr->data.funct;  /* takes up 5 bits */
    temp <<= 3;
    temp |= ptr->data.ARE; /* takes up 3 bits */
    ptr->final_first = temp;
    temp = 0;

    switch (ptr->data.adrs_origin){
        case (SYB): {
            temp |= ptr->data.p1.symbl.address; /* takes up 21 bits */
            temp <<= 3;
            temp |= ptr->data.p1.symbl.ARE; /* takes up 3 bits */
            break;
        }
        
        case (CONST_NUM): {
	        if(ptr->data.p1.symbl.ARE)
	        {
	            temp |= ptr->data.p1.symbl.address; /* takes up 21 bits */
	            temp <<= 3;
                temp |= ptr->data.p1.symbl.ARE; /* takes up 3 bits */
                break;
	        }
	        else
	            {
                temp |= ptr->data.p1.num.number; /* takes up 21 bits */
                temp <<= 3;
                temp |= ptr->data.p1.num.ARE;   /* takes up 3 bits */
                break;
	            }
        }
        default:break;
    }/*end switch adrs origin*/
	ptr->final_second = temp;
    temp = 0;

  if(ptr->data.adrs_origin <3) /*p1 already taken*/
    {
      switch (ptr->data.adrs_dest){
        case(SYB):{
	        if(ptr->data.p2.symbl.ARE){
                temp |= ptr->data.p2.symbl.address; /* takes up 21 bits */
                temp <<= 3;
                temp |= ptr->data.p2.symbl.ARE; /* takes up 3 bits */
                break;
            }
	    }
        case(STOCK):{
           break;
        }
        case(CONST_NUM):{
            temp |= ptr->data.p2.num.number; /* takes up 21 bits */
            temp <<= 3;
            temp |= ptr->data.p2.num.ARE;  /* takes up 3 bits */
            break;
        }
        default:break;
      }/*end switch*/
    }/*end if*/
  else  /*ptr->data.adrs_origin>3*/
  {
     switch (ptr->data.adrs_dest){
        case(DIST):{
            break;
        }
	    case(SYB):{
            temp |= ptr->data.p1.symbl.address; /* takes up 21 bits */
            temp <<= 3;
            temp |= ptr->data.p1.symbl.ARE; /* takes up 3 bits */
            break;
        }
        case(STOCK):{
           break;
        }
        case(CONST_NUM):{
            temp |= ptr->data.p1.num.number; /* takes up 21 bits */
	        temp <<= 3;
            temp |= ptr->data.p1.num.ARE;   /* takes up 3 bits */
            break;
        }
        default:break;
      }/*end switch*/
  }/*end else*/
  ptr->final_third = temp;
}

/* binary_data function updates the data table to machine code for 24 bits*/
void binary_data(data_ptr ptr){
    unsigned int temp = 0;
    switch (ptr->data.type){
        case STRING:{
            temp |= (unsigned int) ptr->data.s; /* cast the character value stored in the data table to number by ascii */
            break;
        }
        case NUMBER:{
            temp |= ptr->data.num; /* save the number value stored in the data table */
            break;
        }
        default:break;
    }
    ptr->final = temp;
}

/* add_symbol function adds the symbol to the symbol table */
int add_symbol(char *file_name, int line_index, char *name, symbol_ptr *curr_symbol, unsigned int address, int oper, int entry, int external) {
    symbol_ptr *head= curr_symbol;
    symbol_ptr add_info, curr;

    name = strtok(name, ":");
    if(strlen(name)>MAX_ARG_SIZE)
	    return error_output(file_name, line_index, "Symbol too long");
    add_info = (symbol_ptr) malloc(sizeof(symbol_node));
    if (!add_info)
        return error_output(file_name, line_index, "Out of memory");

    strcpy(add_info->symbol.name, name);
    add_info->symbol.address = address;
    add_info->symbol.oper = oper;
    add_info->symbol.external = external;
    add_info->symbol.entry = entry;
    add_info->next = NULL;

    if(*head == NULL)
        *head = add_info;
    else
        {
        curr = *head;
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = add_info;
        }
    return 1;
}

/* symbol_struct function to find a symbol in the symbol table using the symbol name
 * returns a pointer to the symbol in table or NULL if doesn't exist */
symbol_struct *find_symbol(char* name, symbol_ptr *curr_symbol){
    symbol_ptr curr;
    curr = *curr_symbol;
  
    sscanf(name, "%s", name);
    while(curr){
        if( !strcmp(curr->symbol.name, name))
        {   
            return &curr->symbol; /* return the existing symbol with that name */
        }
        curr = curr->next;
    }
    return NULL;
}


/* num_data function adds a number data to data table */
int num_data(char *file_name, int line_index, int number,data_ptr *curr_data, unsigned int *IC, unsigned int *DC ){
     data_ptr *headd= curr_data;
     data_ptr add_info, curr;

    if((*IC + *DC) > MAX_MEMORY){
        return error_output(file_name, line_index,"Out of memory");
    }
    else
        {
	    if(number>MAX_DATA && number <MIN_DATA)
	         return error_output(file_name, line_index,"Number too big");
        add_info = (data_ptr)malloc(sizeof(data_node));
        if(!add_info){
             return error_output(file_name, line_index,"Out of memory");
        }
        if(number < 0)
           
	number=complete_two_instruction(number);
        add_info->data.type = NUMBER;
        add_info->data.num = number;
        add_info->next = NULL;
		(*DC)++;
	
        if(*headd == NULL)
            *headd = add_info;
        else
            {
            curr = *headd;
            while (curr->next)
            {
                curr = curr->next;
            }
            curr->next = add_info;
             }
    }
  return 1;
}

/* char_data function adds a string data to data table */
int char_data(char *file_name, int line_index, char s, data_ptr *curr_data, unsigned int *IC, unsigned int *DC){
    data_ptr *data_p= curr_data;
    data_ptr add_info, curr;
    if((*IC + *DC) > MAX_MEMORY){
        return error_output(file_name, line_index,"Out of memory");
    }
    add_info = (data_ptr)malloc(sizeof(data_node));
    if(!add_info){
        return error_output(file_name, line_index,"Out of memory");
    }
    
    add_info->data.type = STRING;
    add_info->data.s = s;
    add_info->next = NULL;
	(*DC)++;
    if(*data_p == NULL)
        *data_p = add_info;
    else {
        curr = *data_p;
        while (curr->next)
        {
            curr = curr->next;
        }
        curr->next = add_info;
    }
    return 1;
}

/*
* no_param, one_param, two_param functions used to seperate cases in add_instruction function
* to deal with data in line by how many parameters the operation receives
*/
int no_param(char* file_name, int line_index, instruction_ptr new_data, char *str,  unsigned int *IC)
{
    int i=0;
    instruction_ptr add_info=new_data;

    add_info->data.adrs_origin = CONST_NUM;
    add_info->data.adrs_dest = CONST_NUM;
    add_info->data.r_origin = CONST_NUM;
    add_info->data.r_dest = CONST_NUM;

    i =skip_white_spaces(str);
	if (str[i] == '\n'||str[i] == '\t'||str[i]== '\0') 
		return 1;
	/*parameter when in zero parameter operation*/
	else return error_output(file_name, line_index, "Too many parameters");
}

int one_param(char* file_name, int line_index, instruction_ptr new_data, symbol_ptr* curr_symbol, char *str, unsigned int *IC)
{
symbol_struct *symbol_declared;
instruction_ptr add_info=new_data;
int temp,i=0,j=0;
char *stemp;
stemp= str;
        i += skip_white_spaces(str);
        if (str[i] == 'r')            /* the argument is a stock */
        {
            if (add_info->data.op_code == 9) { /*cant be stock*/
               return error_output(file_name, line_index, "Invalid argument");
            }
            sscanf(str,"%s",str);
            while (stocks_r[j]) {
                if (!strcmp(str, stocks_r[j]))
                { 
                    add_info->data.adrs_origin = CONST_NUM;
                    add_info->data.adrs_dest = STOCK;
                    add_info->data.r_dest = j;
                   
                    /* check for errors in the end of the input */
                    i += strlen(stocks_r[j]);
                    i += skip_white_spaces(str+i);
                    if(str[i] == ','){
                       return error_output(file_name, line_index, "Too many parameters");
                    }
                    if(!is_end(str+i)){
                       return error_output(file_name, line_index, "Too many parameters");
                    }
                    break;
                }
                j++;
            }/*end while*/
            if (j == LIMIT_STOCK) /*bigger than stocks given*/
                return error_output(file_name, line_index, "Invalid stock");
        } /*not stock*/
        else
        {
            if (*(str) == '&')
            {
                if ( add_info->data.op_code != 9) {  /*only operation 9 can do &distance*/
                   return error_output(file_name, line_index, "Invalid argument");
                }
                str++;
                if (is_symbol(file_name, line_index, str)>0) /*valid symbol*/
                {
                    add_info->data.adrs_dest = DIST;
		            add_info->data.p1.symbl.ARE = A;
                    add_info->data.p1.symbl.name = (char*)malloc(MAX_ARG_SIZE);
                    if (!add_info->data.p1.symbl.name)
                        return error_output(file_name, line_index, "Out of Memory");
                    sscanf(str, "%s", str);
                    strcpy(add_info->data.p1.symbl.name, str);
                    (*IC)++;
                }
                if (!is_end(stemp+strlen(str)+1)) {
                    return error_output(file_name, line_index, "Extra comma, missing parameter");
                }
            }/*end &dist*/
            else { /*not stock=3, not &dist=2, either symbol=1 or const=0*/
                if (is_symbol(file_name, line_index, str)>0) /*symbol=1*/
                {
                    add_info->data.adrs_dest = SYB;
                    if (!is_end(stemp+strlen(str))) {
                       return error_output(file_name, line_index, "missing parameter");
                    }

                    add_info->data.p1.symbl.name = (char*)malloc(MAX_ARG_SIZE);
                    if (!add_info->data.p1.symbl.name)
                        return error_output(file_name, line_index, "Out of Memory");
                    sscanf(str, "%s", str);
                    strcpy(add_info->data.p1.symbl.name, str);
		             symbol_declared=find_symbol(add_info->data.p1.symbl.name,  curr_symbol);
            		if(symbol_declared){
			            add_info->data.p1.symbl.address= symbol_declared->address;
			            }
                    (*IC)++;
                }
                else {  /*const=0*/
		
                    if (is_valid(file_name, line_index, str))
                    {
                        if (add_info->data.op_code != 13) {/*only operation that can be const number*/
                           return error_output(file_name, line_index, "Invalid argument");
                        }

                        add_info->data.adrs_dest = CONST_NUM;
                        str++;
                        temp=atoi(str);
                        add_info->data.p1.num.number = complete_two_instruction(temp);
			            add_info->data.p1.num.ARE=A;
                        (*IC)++;
                        if(!is_end(stemp+strlen(str))) {
                            return error_output(file_name, line_index, "missing parameter");
                        }
                    }
                }
            }
        }
	 return 1;
}/*end one*/

int two_param(char* file_name, int line_index, instruction_ptr new_data, symbol_ptr* curr_symbol, char *str, unsigned int *IC)
{
symbol_struct *symbol_declared;
instruction_ptr add_info=new_data;
int i,j=0;
char * token=NULL;
char temp[MAX_ARG_SIZE]={'\0'};
str+=skip_white_spaces(str);

	if(!strcmp(str, "\n"))
  		 { return error_output(file_name, line_index, "Missing two parameters");}

    if (!is_comma(file_name, line_index, str)) {
        
        token = strtok(str, ","); /* cut out the first argument */
	    i=skip_white_spaces(token);
	    token=token+i;
	
        if (token) {
            if (token[0] == 'r') { /*is stock*/
                if (add_info->data.op_code == 4) { /*operation cannot be stock*/
                    return error_output(file_name, line_index, "Invalid argument");
                }
		
                while (stocks_r[j]) {	
                    if (!strncmp(token, stocks_r[j], 2))
                    {
                        add_info->data.adrs_origin = STOCK;
                        add_info->data.r_origin = j;
			            break;
                    }
                    j++;
                }/*end while*/
                if (j == LIMIT_STOCK)
                   return  error_output(file_name, line_index, "Invalid stock");
            }/*end if stock*/
            else {/* symbol or constant number */
                if (is_symbol(file_name, line_index, token)>0)
                { /*symbol*/
                    add_info->data.adrs_origin = SYB;
                    add_info->data.p1.symbl.name = (char*)malloc(MAX_ARG_SIZE);
                    if (!add_info->data.p1.symbl.name)
                        return error_output(file_name, line_index, "Out of Memory");
                    strcpy(add_info->data.p1.symbl.name, token);
		            symbol_declared=find_symbol(add_info->data.p1.symbl.name,  curr_symbol);
		            if(symbol_declared){
			                add_info->data.p1.symbl.address= symbol_declared->address;
		            }
                    (*IC)++;
                }
                else /*constant number*/
                {
                    if (is_valid(file_name, line_index, token))
                    {	
                        add_info->data.adrs_origin = CONST_NUM;
                        add_info->data.p1.num.number =complete_two_instruction(atoi(token+1));
			add_info->data.p1.num.ARE=A;
                        (*IC)++;
                    }
                    else {
                       return  error_output(file_name, line_index, "Invalid argument");
                    }
                }
            }
        }
        /*------second arg------*/
    token = strtok(NULL, ",");
    j = 0;
    i=skip_white_spaces(token);
    token=token+i;
    strcpy(temp, token);
    
    if (token[0] == 'r') { /*stock*/
	sscanf(token, "%s", token);
	
        while (stocks_r[j]) {
            if (!strcmp(token, stocks_r[j])) {
                add_info->data.adrs_dest = STOCK;
                add_info->data.r_dest = j;
		
              	break;
            }
            j++;
        } /*end while*/
	if (j == LIMIT_STOCK)
	    return error_output(file_name, line_index, "Invalid stock");
    }/*end if stock*/
    else
    { /*not stock- can be symbol, constant number*/
        sscanf(token, "%s", token);
        if (is_symbol(file_name, line_index, token)>0) { /* the argument is a reference to a symbol */
            add_info->data.adrs_dest = SYB;
            if(add_info->data.adrs_origin <2) /*p1 already taken*/
            {
                add_info->data.p2.symbl.ARE = R;
                add_info->data.p2.symbl.name = (char*)malloc(MAX_ARG_SIZE);
                if (!add_info->data.p2.symbl.name)
                    return error_output(file_name, line_index, "Out of Memory");
                strcpy(add_info->data.p2.symbl.name, token);
		        symbol_declared=find_symbol(add_info->data.p2.symbl.name,  curr_symbol);
            		if(symbol_declared){
			                add_info->data.p2.symbl.address= symbol_declared->address;
            		}
            		(*IC)++;
            }
            else
            { /*p1 is empty*/
                add_info->data.p1.symbl.name = (char*)malloc(MAX_ARG_SIZE);
                if (!add_info->data.p1.symbl.name)
                    return error_output(file_name, line_index, "Out of Memory");
                strcpy(add_info->data.p1.symbl.name, token);
		        symbol_declared=find_symbol(add_info->data.p1.symbl.name,  curr_symbol);
		        if(symbol_declared){
			        add_info->data.p1.symbl.address= symbol_declared->address;
			    }
                (*IC)++;
            }
        }/*end symbol*/
        else {
            if (is_valid(file_name, line_index, token))  /*constant num*/
            {
                if (add_info->data.op_code == 1)  /*only cmp can receive destination arg as const*/
                {
                    add_info->data.adrs_dest = CONST_NUM;
                    if (add_info->data.adrs_origin == STOCK) /*no need for p2*/
                    {
                        add_info->data.p1.num.number = complete_two_instruction(atoi(token+1));
                        add_info->data.p1.num.ARE=A;
			            (*IC)++;
                    }
                    else
                    {
                        add_info->data.p2.num.number = complete_two_instruction(atoi(token+1));
			            add_info->data.p2.num.ARE=A;
                        (*IC)++;
                    }
                }
                else
                    return error_output(file_name, line_index, "Invalid argument");
            }
            else {  /* the argument doesn't fit */
                return error_output(file_name, line_index, "Invalid argument");
            }
        }/*end if-else if*/
        /*checking for extra data in line*/

        }
	
       
        if (!is_end(temp+strlen(token))) {
            return error_output(file_name, line_index, "Too many parameters");
   } 
}/*end if is_comma*/
    else {
            return error_output(file_name, line_index, "Missing comma");
        }
   return 1;
}/*end two_param*/

/*
  * add_instruction receives line parsed data and inserts to instruction table by no_param, one_param, two_param functions
  * that treats each operation differently by how many parameters can receive
  * @param file name, line number, current line, pointer to line, pointer of current instruction,
  * pointer of current symbol ,number of operation, number of function, pointer to IC, pointer to DC
  * @return 1 if successful or 0 if error
  */
int add_instruction(char* file_name, int line_index, char *str, instruction_ptr* curr_instruction, symbol_ptr* curr_symbol, unsigned int op_code,  unsigned int function, int parameters, unsigned int* IC, unsigned int* DC)
{
   instruction_ptr add_info;
   instruction_ptr current;

   if ((*IC + *DC) > MAX_MEMORY) {
        return error_output(file_name, line_index, "Out of memory");
    }
	
   add_info = (instruction_ptr)malloc(sizeof(instruction_node));
        if (!add_info) {
            return error_output(file_name, line_index, "Out of memory");
        }

        add_info->data.ARE = A;
        add_info->data.funct = function;
	add_info->data.r_dest = 0;
	add_info->data.adrs_dest = 0;
	add_info->data.r_origin = 0;
	add_info->data.adrs_origin = 0;
	add_info->data.op_code = op_code; /* the number of the command */
        add_info->data.IC = *IC;
        add_info->next = NULL;
        (*IC)++;
   
switch (parameters) {
            case 0: { /*receives 0 parameters*/
                if(op_code >13)
                   {
                     if(!no_param(file_name, line_index, add_info, str, IC))
			            {
                          
                         return 0;
			            }
			        }
                else{
                    return error_output(file_name, line_index, "Wrong operation");
                    }
                break;
            }/*end case 0*/
            case 1: { /*receives 1 parameters*/
                if (op_code > 4 && op_code < 14)
                {
                    if(!one_param(file_name,line_index, add_info, curr_symbol, str, IC))
                    {
                      
			            return 0;
                    }
                }
                else{
                    return error_output(file_name, line_index, "Wrong operation");
                }
                break;
            }/*end case 1*/
            case 2: {/*receives 2 parameters*/
                if (op_code < 5 ) 
                    {
                        if(!two_param(file_name,line_index, add_info, curr_symbol, str, IC))
			            {
                            
			                return 0;
			            }
			        }
                else{
                        return error_output(file_name, line_index, "Wrong operation");
                    }
                break;
            } /*end case 2*/
            default:break;
        }/*end switch*/

        /*update instruction table*/
	if(*curr_instruction == NULL)
            *curr_instruction = add_info;
	else {
            current = (*curr_instruction)->next;
            while (current) {
                curr_instruction = &(*curr_instruction)->next;
                current = current->next;
            }
            (*curr_instruction)->next = add_info;
        }
return 1;
}


/**
 *****************************************************
 * functions for general convertion and freeing data
 *****************************************************
 **/


/* intToBase16 function to convert a number from decimal to base 16 and return it as a string */
void intToBase16(unsigned int num, char *result){ 
    char *converter = "0123456789abcdef"; /* base 16 characters */
    char d[7]={0};
    int i = 0, j, x;
    if(num == 0)
    {
        strcpy(result, "0");
        return;
    }
     while(num)
     {
        d[i] = converter[num % 16];
        i++;
        num = num/16; /* divide the number by 16 to convert the rest of the number to base 16 */
    }
    /* the d string needs to be reversed to hold the correct value */
    for(x=0, j=i-1; j>=0; x++,j--){
        result[x] = d[j];
    }
    result[x] = '\0';
}

/* return the representation of a number in a 2's complement for the representation in 24 bits*/
unsigned int complete_two_instruction(int num) {
unsigned int new_num;
    if(num < 0) {
       new_num=(unsigned int)num *(-1);
	new_num= ~new_num;
        return new_num+1;
    }
    return (unsigned int)num;
}


/*frees data in symbol table*/
void free_symbol_list(symbol_ptr *curr_symbol){
    symbol_ptr *head= curr_symbol;
    symbol_ptr curr;
    while(*head){
        curr = (*head)->next;
        free(*head);
        *head = curr;
    }
}
/*frees data in data table*/
void free_data_list(data_ptr *curr_data){
    data_ptr *head= curr_data;
    data_ptr curr;
    while(*head){
        curr = (*head)->next;
	free(*head);
        *head = curr;
    }
}
/*frees data in instruction table*/
void free_instruction_list(instruction_ptr *curr_instruction){
    instruction_ptr *head= curr_instruction;
    instruction_ptr cur;
    while(*head){
        cur = (*head)->next;
	

	 free(*head);
        *head = cur;
    }
}
/*function to use all three free functions*/
void free_lists(symbol_ptr *curr_symbol, data_ptr *curr_data, instruction_ptr *curr_instruction){
    free_data_list(curr_data);

    free_instruction_list(curr_instruction);

    free_symbol_list(curr_symbol);

}

/*end data*/
