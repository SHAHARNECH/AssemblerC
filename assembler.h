#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instructions_operations.h"
#include <ctype.h>


/**
 **********************************************************************
 * functions
 **********************************************************************
 **/

/**basic functions**/
int start_assembly(int , char *[]); /*in fileManager.c*/

int start_passings(FILE *fptr, char * file_name); /*in passings.c*/


/**functions for parsing the lines taken from file- in parseLine.c file**/
int parse_line(char *file_name,  int line_index, char *file_line, symbol_ptr *curr_symbol, data_ptr *curr_data, instruction_ptr *curr_instruction, int *entry_flag, int *extern_flag, unsigned int *IC, unsigned int *DC);

int parse_data(char *file_name, int line_index, char* file_line, int type, data_ptr *curr_data,unsigned int *IC, unsigned int *DC);

int parse_entry(char *file_name, int line_index, char* file_line, symbol_ptr *curr_symbol, unsigned int *DC);

int parse_extern(char *file_name, int line_index, char* file_line, symbol_ptr *curr_symbol);

int skip_white_spaces(const char* file_line); 

int is_symbol(char *file_name, int line_index, const char *word);

int is_data(const char* word);

operation *is_instruction(const char* word);

int is_comma(char *file_name, int line_index, const char* file_line);

int is_end(const char *line);

int is_valid(char *file_name, int line_index, char *arg);


/**functions for updating and using data after parsed- in data.c file**/
void update_data(data_ptr *curr_data, unsigned int *IC, unsigned int *DC);

int update_symbol_table(symbol_ptr *curr_symbol, unsigned int *IC);

int update_instruction_table(char *file_name, int line_index, symbol_ptr *curr_symbol, instruction_ptr *curr_instruction);

int add_symbol(char *file_name, int line_index, char *name, symbol_ptr *curr_symbol, unsigned int IC, int oper, int entry, int external);

symbol_struct *find_symbol(char* name, symbol_ptr *curr_symbol);

int num_data(char *file_name, int line_index, int number,data_ptr *curr_data, unsigned int *IC, unsigned int *DC );

int char_data(char *file_name, int line_index, char s, data_ptr *curr_data, unsigned int *IC, unsigned int *DC);

int add_instruction(char* file_name, int line_index, char* str, instruction_ptr* curr_instruction, symbol_ptr* curr_symbol, unsigned int op_code,  unsigned int function, int parameters, unsigned int* IC, unsigned int* DC);

void free_symbol_list(symbol_ptr *curr_symbol);

void free_data_list(data_ptr *curr_data);

void free_instruction_list(instruction_ptr *curr_instruction);

void free_lists(symbol_ptr *curr_symbol, data_ptr *curr_data, instruction_ptr *curr_instruction);

/*updating data to binary (machine code) -in data.c file*/
void binary_instruction(instruction_ptr ptr);

void binary_data(data_ptr ptr);

void intToBase16(unsigned int num, char *result);

unsigned int complete_two_instruction(int num);



/**output data to files- in outputManager.c file**/
int create_entry(char *file_name, symbol_ptr *curr_symbol);

int create_extern(char *file_name, instruction_ptr *curr_instruction);

int create_ob(char *file_name, data_ptr *curr_data, instruction_ptr *curr_instruction, unsigned int *IC, unsigned int *DC);

int write_output_file(char *file_name, symbol_ptr *curr_symbol, data_ptr *curr_data, instruction_ptr *curr_instruction, unsigned int *IC, unsigned int *DC, int *entry_flag, int *extern_flag);

int error_output(char *file_name, int line_index, char *sentence_err);



/*end*/




