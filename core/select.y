%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./node.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;

void yyerror(const char* s);
Node* root;

Node* create_node() {
    Node* new_node = malloc(sizeof(Node));
    return new_node;
}

%}

%union {
    float fval;
    char* strval;
    float ival;
    struct Node* node;
}

%defines
%token <strval> FLT
%token <strval> INTEGER
%token <strval> STR
%token <strval> COLREF
%token LPAREN
%token RPAREN
%token AND
%token OR
%token EQUALS
%token GT
%token LT
%token GTE
%token LTE
%token N_EQUALS
%token NEWLINE

%type<node> expression
%type<node> comparator
%type<node> comp_arg

%start query
%%

query: line

line: 
    | expression {
                            root = $1;
                            YYACCEPT;
                         }
;

expression: comp_arg comparator comp_arg {
                      Node* node = $2;
                      node->left = $1;
                      node->right = $3;
                      $$ = node;
                      //printf("arg COMP arg\n");
                    }
          | LPAREN expression RPAREN 
                    {
                      $$ = $2;
                      //printf("(expr)\n");
                    }
          | expression OR expression {
                      Node* node = create_node(); 
                      node->node_type = JUNCTION;
                      node->junc_type = JUNC_OR;
                      node->left = $1;
                      node->right = $3;
                      $$ = node;
                      //printf("expr OR expr\n");
                    }
          | expression AND expression {
                      Node* node = create_node(); 
                      node->node_type = JUNCTION;
                      node->junc_type = JUNC_AND;
                      node->left = $1;
                      node->right = $3;
                      $$ = node;
                      //printf("expr AND expr\n");
                    }
;

comparator: EQUALS {
                      Node* node = create_node(); 
                      node->node_type = COMPARATOR;
                      node->comparator_type = EQ_TOK;
                      $$ = node;
                      //printf("EQ\n");
                    }
            | GT {
                      Node* node = create_node(); 
                      node->node_type = COMPARATOR;
                      node->comparator_type = GT_TOK;
                      $$ = node;
                      //printf("NEQ\n");
                    }
            | LT {
                      Node* node = create_node(); 
                      node->node_type = COMPARATOR;
                      node->comparator_type = LT_TOK;
                      $$ = node;
                      //printf("LT\n");
                    }
            | GTE {
                      Node* node = create_node(); 
                      node->node_type = COMPARATOR;
                      node->comparator_type = GTE_TOK;
                      $$ = node;
                      //printf("GTE\n");
                    }
            | LTE {
                      Node* node = create_node(); 
                      node->node_type = COMPARATOR;
                      node->comparator_type = LTE_TOK;
                      $$ = node;
                      //printf("LTE\n");
                    }
            | N_EQUALS {
                      Node* node = create_node(); 
                      node->node_type = COMPARATOR;
                      node->comparator_type = NEQ_TOK;
                      $$ = node;
                      //printf("NEQ\n");
                    }
;

comp_arg: INTEGER {
                    Node* node = create_node();
                    node->node_type = ARGUMENT;
                    node->strval = $1;
                    node->arg_type = ARG_INT;
                    $$ = node;
                    //printf("Int: %d\n", $1);
                  }
        | STR {
                Node* node = create_node();
                node->node_type = ARGUMENT;
                node->strval = $1;
                node->arg_type = ARG_STRING;
                $$ = node;
                //printf("Str: %s\n", $1);
              }
        | FLT {
                Node* node = create_node();
                node->node_type = ARGUMENT;
                node->strval = $1;
                node->arg_type = ARG_FLOAT;
                $$ = node;
                //printf("Float: %f\n", $1);
              }
        | COLREF {
                Node* node = create_node();
                node->node_type = ARGUMENT;
                node->strval = $1;
                node->arg_type = ARG_COL;
                $$ = node;
                }
;

%%


void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}
