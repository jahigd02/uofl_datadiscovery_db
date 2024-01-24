#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "node.h"
#include "lex.yy.h"
#include <ctype.h>
#include "select.tab.h"
#define MAX_ERR_MSG_SIZE 99
#define MAX_ROW_LENGTH 1024

#define TYPE_FLOAT 3
#define TYPE_INT 4
#define TYPE_STR 5

//time awk -F'|' '$15 == "RAIL" || (($15 == "AIR" || $15 == "FOB") && $6 < 28955.64)' short.tbl > awkOUT.out


typedef struct Row_Entry {
    char* strval;
    float fval;
    int ival;

    int entry_type;
} Row_Entry;

int yyparse();
int AST(Node* node, Row_Entry** row_lookup_table);
void error(char* err_msg);
Row_Entry* row_entry_at(char* rowstr_ptr, int index);
int determine_type(char* token);
int entry_comparison(Row_Entry* entry, int comparator_type, char* value);
void main_driver();
void printAST(Node* node, int types);
int opposite_comparator(int compratator_type);
Row_Entry** build_row_lookup_table(char* line);
void free_row_lookup_table(Row_Entry** row_lookup_table);
Row_Entry* row_lookup_table[MAX_ROW_LENGTH];
int type_table[MAX_ROW_LENGTH];

extern Node* root;

int main(int argc, char* argv[]) {
    const char* query = "#15 == \"RAIL\" OR ((#15 == \"AIR\" OR #15 == \"FOB\") AND #6 < 28955.64)";
    yy_scan_string(query);
    yyparse();
    main_driver(argv[1]);
    return 0;
}

void main_driver(char* filepath) {
    //open FILE
    FILE* fp = fopen(filepath, "r");
    if (!fp) { //file error-handling
        char* error_msg = malloc(sizeof(char) * MAX_ERR_MSG_SIZE);
        sprintf(error_msg, "Unable to open file %s.", filepath);
        error(error_msg);
        free(error_msg);
    }
    char* line = malloc(sizeof(char) * MAX_ROW_LENGTH);
    fgets(line, MAX_ROW_LENGTH, fp);
    fgets(line, MAX_ROW_LENGTH, fp); // Hacky solution; what if the file's empty? Above, too.
    char* linecopy = strdup(line); //FREE
    int i = 0;
    char* token = strtok(linecopy, "|\r\n");
    while (token != NULL) { 
        int type = determine_type(token);
        type_table[i] = type;
        token = strtok(NULL, "|\r\n");
        i++;
    }
    free(linecopy);
    
    fseek(fp, 0, SEEK_SET);
    fgets(line, MAX_ROW_LENGTH, fp);
    while (fgets(line, MAX_ROW_LENGTH, fp) != NULL) {
        Row_Entry** row_lookup_table = build_row_lookup_table(line);
        int retval = AST(root, row_lookup_table);
        if (retval) {
            printf("%s", line);
        }
        free_row_lookup_table(row_lookup_table);
    }
    free(line);
    fclose(fp);
}

 //FOR NOW, takes in a string, and returns the result after casting. I think the AST holds other (int, float)
 //literals, though. Either make AST just hold strings, or this accept others.
int entry_comparison(Row_Entry* entry, int comparator_type, char* value) {
    switch(comparator_type) {
        case EQ_TOK:
            if (entry->entry_type == TYPE_FLOAT) {
                return entry->fval == atof(value);
            } else if (entry->entry_type == TYPE_INT) {
                return entry->ival == atoi(value);
            } else if (entry->entry_type == TYPE_STR) {
                return strcmp(entry->strval, value) == 0;
            }
            break;
        case NEQ_TOK:
            if (entry->entry_type == TYPE_FLOAT) {
                return entry->fval != atof(value);
            } else if (entry->entry_type == TYPE_INT) {
                return entry->ival != atoi(value);
            } else if (entry->entry_type == TYPE_STR) {
                return strcmp(entry->strval, value) != 0;
            }
            break;
        case GT_TOK:
            if (entry->entry_type == TYPE_FLOAT) {
                return entry->fval > atof(value);
            } else if (entry->entry_type == TYPE_INT) {
                return entry->ival > atoi(value);
            } else if (entry->entry_type == TYPE_STR) {
                return strcmp(entry->strval, value) > 0;
            }
            break;
        case LT_TOK:
            if (entry->entry_type == TYPE_FLOAT) {
                return entry->fval < atof(value);
            } else if (entry->entry_type == TYPE_INT) {
                return entry->ival < atoi(value);
            } else if (entry->entry_type == TYPE_STR) {
                return strcmp(entry->strval, value) < 0;
            }
            break;
        case GTE_TOK:
            if (entry->entry_type == TYPE_FLOAT) {
                return entry->fval >= atof(value);
            } else if (entry->entry_type == TYPE_INT) {
                return entry->ival >= atoi(value);
            } else if (entry->entry_type == TYPE_STR) {
                return strcmp(entry->strval, value) >= 0;
            }
            break;
        case LTE_TOK:
            if (entry->entry_type == TYPE_FLOAT) {
                return entry->fval <= atof(value);
            } else if (entry->entry_type == TYPE_INT) {
                return entry->ival <= atoi(value);
            } else if (entry->entry_type == TYPE_STR) {
                return strcmp(entry->strval, value) <= 0;
            }
            break;
    }
}

int determine_type(char* token) {
    int len = strlen(token);
    int type = TYPE_INT;
    for (int i = 0; i < len; i++) {
        if (isdigit(token[i]) || token[i] == '.') {
            if (token[i] == '.') {
                type = TYPE_FLOAT;
                break;
            }
        } else {
            type = TYPE_STR;
            break;
        }
    }
    return type;
}

int opposite_comparator(int compratator_type) {
    switch (compratator_type) {
        case LT_TOK:
            return GT_TOK;
        case GT_TOK:
            return LT_TOK;
        case EQ_TOK:
            return EQ_TOK;
        case LTE_TOK:
            return GTE_TOK;
        case GTE_TOK:
            return LTE_TOK;
        case NEQ_TOK:
            return NEQ_TOK;
    }
}

Row_Entry** build_row_lookup_table(char* line) {
    Row_Entry** row_lookup_table = malloc(sizeof(Row_Entry) * MAX_ROW_LENGTH);
    char* linecopy = strdup(line); //FREE
    int i = 0;
    char* token = strtok(linecopy, "|\r\n");

    while (token != NULL) { 
        // create new Row_Entry*, assign it to location in row_lookup_table
        Row_Entry* entry = malloc(sizeof(Row_Entry));
        int row_entry_type = type_table[i];
        switch(row_entry_type) {
            case TYPE_STR:
                entry->strval = strdup(token);
                entry->entry_type = TYPE_STR;
                break;
            case TYPE_FLOAT:
                entry->fval = atof(token);
                entry->entry_type = TYPE_FLOAT;
                break;
            case TYPE_INT:
                entry->ival = atoi(token);
                entry->entry_type = TYPE_INT;
                break;
        }

        row_lookup_table[i] = entry;
        token = strtok(NULL, "|\r\n");
        i++;
    }
    free(linecopy);
    return row_lookup_table;
}

void free_row_lookup_table(Row_Entry** row_lookup_table) {
    int i = 0;
    while (row_lookup_table[i] != NULL) {
        //if (row_lookup_table[i]->strval != NULL)
        free(row_lookup_table[i]);
        i++;
    }
    free(row_lookup_table);
}


int AST(Node* node, Row_Entry** row_lookup_table) {
    if (node->left == NULL && node->right == NULL) { //encountered an ARGUMENT node -- do nothing
        if (node->node_type == ARGUMENT) {
            //..do nothing
        }
    } else { //encountered not an argument -- but rather a JUNCTION node or COMPARATOR node
        AST(node->left, row_lookup_table); // recursively check LEFT child
        AST(node->right, row_lookup_table); // recursively check RIGHT child
        if (node->node_type == JUNCTION) { //encountered a JUNCTION
            switch(node->junc_type) {
                case JUNC_OR:
                    node->retval = (node->left->retval) || (node->right->retval);
                    break;
                case JUNC_AND:
                    node->retval = (node->left->retval) && (node->right->retval);
                    break;
            }
            return node->retval;
        } else if (node->node_type == COMPARATOR) { //encountered a COMPARATOR.
            //a COMPARATOR's column header can be on the left, the right, or not at all --
            //in which case we just boolean compare the two values...
            if (node->left->arg_type == ARG_COL) {
                int index = 0;
                index = atoi(node->left->strval);
                Row_Entry* entry = row_lookup_table[index-1];
                int result = entry_comparison(entry, node->comparator_type, node->right->strval);
                node->retval = result;
                if (entry->entry_type == TYPE_STR) {
                    //free(entry->strval);
                }
                //free(entry);
                return node->retval;
            } else if (node->right->arg_type == ARG_COL) {
                int index = 0;
                index = atoi(node->right->strval);
                Row_Entry* entry = row_lookup_table[index-1];
                int result = entry_comparison(entry, opposite_comparator(node->comparator_type), node->left->strval);
                if (entry->entry_type == TYPE_STR) {
                    //free(entry->strval);
                }
                //free(entry);
                node->retval = result;
                return node->retval;
            } else {
                /////////////////////////////
            }
            
        }
        
    }
}

void error(char* err_msg) {
    printf("Error [%s], quitting.\n", err_msg);
    exit(1);
}