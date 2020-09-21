#define FILE_INPUT ".as"
#define OUTPUT_OB ".ob"
#define OUTPUT_ENTRY ".ent"
#define OUTPUT_EXT ".ext"
#define LINE_LEN 81  /* The length of an assembly code line -including '\n'*/
#define MAX_ARG_SIZE 32 /* The length of an argument -including '\n' */
#define NUM_OPS 16 /* Number of operations */
#define NUM_INST 4 /* Number of instructions */
#define LIMIT_STOCK 8 /*r0...r7*/
#define MAX_DATA 16777215 /* Max data in 24 bit */
#define MIN_DATA -8388608 /* MIN data in 24 bit */
#define BITS_IN_LINE 24 /* Number of bits */
#define MAX_MEMORY 2097152 /* Max memory */
#define NO 0
#define YES 1


/*building structs for table use*/

/**operations**/
typedef enum {SYMBOL, DATA, INSTRUCTION, ERROR} line_opt;

/*info use for each op_code*/
typedef struct operation {
    const char *op_name;
    unsigned int op_code;
    unsigned int func_code;
    int parameters;
} operation;

/*address method*/
typedef enum {CONST_NUM = 0, SYB = 1, DIST = 2, STOCK = 3} prm_type;

/*Absolute, Relocatable, External in 3 bits*/
typedef enum {A=4, R=2, E=1} ARE; 


/**SYMBOL structure**/
typedef struct symbol_struct { 
    char name[MAX_ARG_SIZE];
    unsigned int address; /* address of the symbol*/
    int oper; /* 1 if it's an operation, 0 if not */
    int external; /* 1 if it's an external value, 0  if not */
    int entry; /* 1 if it's an entry value, 0 if not */
} symbol_struct;

typedef struct symbol_node * symbol_ptr;
typedef struct symbol_node {
    symbol_struct symbol;
    symbol_ptr next;
} symbol_node;



/**DATA structure**/
typedef enum {STRING, NUMBER} data_type;
typedef struct data_struct {
    int type; /* type of data saved, STRING or NUMBER */
    char s; /* character variable saved */
    int num; /* int variable saved */
    int address; /* the data address in the memory */
} data_struct;
/*data table*/
typedef struct data_node * data_ptr;
typedef struct data_node {
    data_struct data;
    unsigned int final : 24; 
    data_ptr next;
} data_node;

/**INSTRUCTION structure**/
/*num info*/
typedef struct op_num {
    unsigned int number:21;
    unsigned int ARE : 3;
} op_num;

/*symbol info*/
typedef struct op_symb {
    char *name;
    unsigned int address : 21;
    unsigned int ARE : 3;
} op_symb;

typedef union  op_type {
    op_num num;
    op_symb symbl;
} op_type;

/*instruction info*/
typedef struct instruction_struct {
    unsigned int ARE : 3; /*the ARE in first word*/
    unsigned int funct : 5; /*number of function for operation*/
    unsigned int r_dest : 3; /*stock number if used in origin*/
    unsigned int adrs_dest : 2; /*the address method for the destination*/
    unsigned int r_origin : 3; /*stock number if used in destination*/
    unsigned int adrs_origin : 2; /* the address method for the origin  */
    unsigned int op_code : 6;  /* operations 0-15 */
    unsigned int IC;  /* the IC saved for the word  */
    op_type p1; /*second word address*/
    op_type p2; /*third word address*/
} instruction_struct;

/*for representation of instruction table*/
typedef struct instruction_node * instruction_ptr;
typedef struct instruction_node {
    instruction_struct data;
    unsigned int final_first : 24;   /*instruction word */
    unsigned int final_second : 24; /*second word*/
    unsigned int final_third : 24; /*third word*/
    instruction_ptr next;
} instruction_node;




