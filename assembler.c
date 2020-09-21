#include "assembler.h"
/**
 * main function that starts the assembler program
 * @param from command line, name of assembler and files to be opened
 * if file (.as) not inserted will print error
 * @return 0 when ends
 */

int main(int argc, char *argv[])
{
	
	if (argc < 2)
	{
	    fprintf(stdout, "ERROR- no file given");
		return 0;
	}
		
	else
	    return start_assembly(argc, argv);
		
		


}



/*end assembler */
