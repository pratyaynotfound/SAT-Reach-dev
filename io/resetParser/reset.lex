
%{
#include "reset.tab.h"
%}

%%

[\n]							{//printf("EOL token\n");
										return 0; 
									}

[+]								{//printf("PLUS token\n"); 
										return(PLUS); 
									}

[*]								{//printf("MULT token\n");
										return(MULT);
									}

[=]								{//printf("EQ token\n");
										return(EQ);
									}

[[:space:]]+			{;}


([a-zA-Z_]+[0-9]*)* 	{
										//printf("VAR token\n");
										resetlval.token_str = strdup(resettext);
										return(VAR);
									}
				
[-]?[0-9]+[.]?[0-9]*				{
															//printf("CONST token\n");
															resetlval.const_val = atof(resettext);
															return(CONST);
														}
['()]							{;}		
%%

int resetwrap()
{
	return 0;
}
