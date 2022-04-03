#ifndef CG_H
#define CG_H

#include <stdio.h>
#include <stdlib.h>

#include "definitions.h"
#include "symbol.h"

void cglabel(int l);
void cgjump(int l);

void cgpreamble(void);
void cgpostamble(void);

/*
    Load an integer value into a register.
    Return index of the register into which value was loaded.
*/
int cgloadint(int value);

/*
    Load value of global variable into a register.
*/
int cgloadglob(int id);

/*
    Store register value into a variable.
*/
int cgstoreglob(int r, int id);

int cgwiden(int r, int oldtype, int newtype);

/*
    Generate a global symbol.
*/
void cgglobsym(int id);


void cgprintint(int r);

/*
    Add two registers together and return
    the index of the register with the result.
*/
int cgadd(int r1, int r2);
int cgmul(int r1, int r2);
int cgdiv(int r1, int r2);
int cgsub(int r1, int r2);

int cgaddress(int id);
int cgderef(int r, int type);

int cgequal(int r1, int r2);
int cgnotequal(int r1, int r2);
int cglessthan(int r1, int r2);
int cggreaterthan(int r1, int r2);
int cglessequal(int r1, int r2); 
int cggreaterequal(int r1, int r2);


void cgfunctionpreamble(int id);
void cgfunctionpostamble(int id);

// Shift a register left by a constant
int cgshlconst(int r, int val);

/*

*/
int cgcompare_and_set(int ASTop, int r1, int r2);
int cgcompare_and_jump(int ASTop, int r1, int r2, int label);

/*
    Setting all registers to be free.
*/
void free_all_registers(void);

void generate_preamble();
void generate_postamble();
void generate_free_registers();
void generate_printint(int reg);

/*
    Returns the size of a primitive type
    (in bytes).
*/
int cgprimsize(int type);

int cgcall(int r, int id);
void cgreturn(int reg, int function_id);

extern FILE* outfile;


#endif