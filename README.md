# Notes

*Please read https://github.com/jahigd02/uofl_datadiscovery_db/blob/main/DBDocument.pdf for a detailed explanation of the research, and the results of benchmarks.*

## Directory Structure
All files related to running the program are in _spring_24/core_.

Files related to generating and re-formatting TCP-H tables are in _spring_24/dbgen_.

Files related to testing are in _spring_24/testing_.
- file1.tbl is a generic testing file.

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


## File types and generation using *dbgen*

The program expects the files to be in a certain format. Specifically, here's a truncated example:
```
l_orderkey|l_partkey|l_suppkey|l_linenumber|l_quantity|l_extendedprice|l_discount|l_tax|l_returnflag|l_linestatus|l_shipdate|l_commitdate|l_receiptdate|l_shipinstruct|l_shipmode|l_comment
1|155190|7706|1|17|21168.23|0.04|0.02|N|O|1996-03-13|1996-02-12|1996-03-22|DELIVER IN PERSON|TRUCK|egular courts above the
...
```
Requirements:

1. The separator is |
2. There is a header
3. There is no separator at the end of a line; just a newline character

For the purposes of this research, I generate test files using the TCP-H benchmarking tool *dbgen*. It generates tables according to some specifics, including type and size. It's a standard from TCP-H, so the details are largely irrelevant to me other than that I'm to generate from the standard table 'lineitem', and use a variable size for my purposes.

Generate files using the _dbgen_ binary in the directory _spring_24/dbgen_. The process currently to generate testing files is a little messy. Here's how it goes:

1. Generate a _lineitem_ table file of scale factor _S_ (which is in terms of GBs) with _./dbgen -T L -s [S]
2. This will generate a file with this appearance:
    ```
    1|36901|O|173665.47|1996-01-02|5-LOW|Clerk#000000951|0|nstructions furiously among |
    2|78002|O|46929.18|1996-12-01|1-URGENT|Clerk#000000880|0| foxes. accounts at the |
    ..
    ```
    This is not in the correct format, so it needs to be sanitized.
3. Remove trailing | by running python script ___
4. Add the header by concatenating this file onto a file with just the header string.
## TODOs
- Allow queries to be defined in the command line
