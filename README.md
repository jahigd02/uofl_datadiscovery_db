# Notes


## Core Structure

The structure of the *select* program is as follows:
The main entry point is _driver.c_. This is ran as _./select *file*_, where *file* is a file to run a query on.

For now, queries are defined in the driver.c file at the top.

The program uses Flex and Bison to define the grammar and write/handle the parsing. _select.y_ is the Bison file, and _select.l_ is the Flex file. To compile the program, one must first compile the Flex file, then the Bison file. This is not strictly required unless either is modified, but I always do it anyways for good measure.

This is a standard compilation of the program:
```
flex --header-file=lex.yy.h ./select.l 
bison -d ./select.y
flex --header-file=lex.yy.h ./select.l 
gcc -pg -o select driver.c select.tab.c lex.yy.c -lfl
```

_-pg_ is a temporary flag I'm using in gcc for allowing profiling using GNU gprof.\
_-lfl_ is a required flag for generating code in tandem with Flex.
## TODOs
- Allow queries to be defined in the command line