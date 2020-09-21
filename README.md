# AssemblerC
simulates a two pass assembler in C for 'System programming Lab' course - Open University -

* Course: 20465 final project- C assembler
* Semester: 2020b
* Student: Shahar Nechushtan 


/**** FILE TIMELINE
 *
 * assembler->fileManager->passings->parseLine->data     ---for parsing lines in first pass
 * passings->data                                        ---for updating tables in second pass
 * passings->outputManager                               ---making output files
 * passings_>fileManager->assembler                      --- closing program
****/
