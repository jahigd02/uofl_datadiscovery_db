#define JUNCTION 1
#define COMPARATOR 2
#define ARGUMENT 3

#define ARG_INT 4
#define ARG_FLOAT 5
#define ARG_STRING 6


#define LT_TOK 7
#define GT_TOK 8
#define EQ_TOK 9
#define LTE_TOK 10
#define GTE_TOK 11
#define NEQ_TOK 12

#define JUNC_OR 13
#define JUNC_AND 14

#define ARG_COL 15

typedef struct Node {
    int node_type; // JUNCTION, COMPARATOR, or ARGUMENT
    int junc_type; // which type of junction?
    //if argument:
    int arg_type; //is it an ARG_INT, ARG_FLOAT, or ARG_STRING -- OR, is it an ARG_COL?
    int ival; //if ARG_INT, integer value
    float fval; //if ARG_FLOAT, float value
    char* strval; //if ARG_STRING, string value

    //if comparator:
    int comparator_type; //LT_TOK ... NEQ_TOK ? (defined above)

    struct Node* left; //left child
    struct Node* right; //right child


    int retval;
} Node;

Node* create_node();