%{
#include <stdio.h>
#include <string.h>

#include "select.tab.h"    
%}

/*
((1=="Utica") AND (3<3.5)) OR (2 > 100.2)

                            OR
                 AND                                        >
     ==[comp]                     <[comp]           2[arg]         100.2[arg]
1[arg]        "Utica"[arg]      2[arg]   100.2[arg]
So what type of nodes will we have? What will we hold?
The layout of a tree will be such that the comparisons are leafs (the comparator is the
parent node of the two argument leafs.) Thus at the very least we have nodes which represent
comparators, and nodes that represent arguments. We will also hold junction nodes for AND/OR.
If both leafs of a node are NULL, then we do nothing with the nodes (these are arguments).
When we get to a comparator, we use our lookup function on the first argument, the column.
So, we do something like lookup(row, column) and do comparison between that and 2nd argument.
[Fill in here to figure out actual walking logic.]

*/

%%
[ \t] {;}
[0-9]+\.[0-9]+ {yylval.strval = strdup(yytext); return FLT;}
[0-9]+ {yylval.strval = strdup(yytext); return INTEGER;}
\"[A-Za-z0-9]+\" {
                    yylval.strval = yytext;
                    char* copy = strdup(yytext + 1);
                    copy[strlen(copy) - 1] = '\0';
                    yylval.strval = copy;
                    return STR;
                 }
"#"[A-Za-z0-9]+ {
                           char* copy = strdup(yytext+1);
                           yylval.strval = copy;
                           return COLREF;
                        }
"(" {return LPAREN;}
")" {return RPAREN;}
"AND" {return AND;}
"OR" {return OR;}
"==" {return EQUALS;}
"<" {return LT;}
">" {return GT;}
"<=" {return LTE;}
">=" {return GTE;}
"!=" {return N_EQUALS;}
\n {return NEWLINE;}
<<EOF>> { return 0; }
%%