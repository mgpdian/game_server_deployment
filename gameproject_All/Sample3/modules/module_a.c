/*这是module_a.c的内容*/ 
#include <stdio.h> 
#include "module_a.h" 
#include "../utils/convert.h" 
void PrintUpperCass(char _letter) 
{ 
    char upper = ConvertUpper(_letter); 
    printf("%c\n", upper); 
}
