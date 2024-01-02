
%{
#include "linexp.tab.h"
%}

%%

[\n]					{
							return 0; 
						}

[+]						{
							//printf(" PLUS \n"); 
							return(PLUS); 
						}

[*]						{	
							//printf(" MULT \n");
							return(MULT);
						}

[=]						{	
							//printf(" EQ \n");
							return(EQ);
						}

"<="					{
							//printf(" LEQ \n");
							return(LEQ);
						}
">="					{
							//printf(" GEQ \n");
							return(GEQ);
						}

[\t\r ]+				{;}

[uU][0-9]*				{	
							//printf(" UVAR \n");
							linexplval.token_str = strdup(linexptext);
							return(UVAR);
						}
						
[yY][0-9]*				{
							//printf(" YVAR \n");
							linexplval.token_str = strdup(linexptext);
							return(YVAR);
						}
						
([a-zA-Z_]+[0-9]*)* 		{
							//printf(" VAR \n");
							linexplval.token_str = strdup(linexptext);
							return(VAR);
						}
				
[-]?[0-9]+[.]?[0-9]*	{
							//printf(" CONST \n");	
							linexplval.const_val = atof(linexptext);
							return(CONST);
						}
[-]						{
							//printf(" MINUS \n");
							return(MINUS);
						}	
[()']					{;}	
%%

int yywrap()
{
	return 0;
}
