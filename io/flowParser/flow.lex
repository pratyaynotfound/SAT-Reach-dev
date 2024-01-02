
%{
#include "flow.tab.h"
%}

%%

[\n]							{
										return 0; 
								}

[+]								{ 
										return(PLUS); 
								}

[*]								{
										return(MULT);
								}

[=]								{
										return(EQ);
								}

[[:space:]]+					{;}

[uU][0-9]*						{
									yylval.token_str = strdup(yytext);
									return(UVAR);
								}

([a-zA-Z_]+[0-9]*)* 				{
									yylval.token_str = strdup(yytext);
									return(VAR);
								}
				
[+-]?[0-9]+[.]?[0-9]*			{
									yylval.const_val = atof(yytext);
									return(CONST);
								}
[-]								{
									return(MINUS);
								}														
		
%%

int yywrap()
{
	return 0;
}
