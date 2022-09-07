#ifndef CODE_GENERATION_X86_H
#define CODE_GENERATION_X86_H

#include <stdio.h>
#include <stdlib.h>

#include "definitions.h"
#include "symbol.h"
#include "types.h"

#define NUM_FREE_REGISTERS 4        // Registers that can be used freely by the program
#define FIRST_PARAMETER_REGISTER 9  // Register that is the first one used for parameters (according to calling convention)

// Generates a label to which a jump can be executed.
void cglabel(int l);

// Emits a jump instruction which jumps to the label with the given index.
void cgjump(int l);

// Assembly code that is executed first in a program.
void cgpreamble(void);

// Assembly code executed at the end of program.
void cgpostamble(void);

// Load integer value into register.
// Returns the index of the register into which the value was loaded.
int cgloadint(int value);

// Load value from a variable into a register.
// Returns the index of the register in which the value was loaded.
int cgloadglob(t_symbol_entry* symbol, int op);

// Store register value in variable.
int cgstoreglob(int r, t_symbol_entry* symbol);


// Load a value from a local variable into a register. The register in which
// the value was loaded is returned. A local variable is relative to the base pointer.
int cgloadlocal(t_symbol_entry* symbol, int op);

// Store the value of a local variable into a register.
int cgstorelocal(int r, t_symbol_entry* symbol);

// Widen a value in register from oldtype to newtype.
// Return register which contains original value converted to newtype.
int cgwiden(int r, int oldtype, int newtype);

// Generate a global symbol that holds a value (byte/long/quad).
void cgglobsym(t_symbol_entry* symbol);

// Store value contained in register r1 at memory location which
// is stored in register r2.
int cgstorderef(int r1, int r2, int type);


// Add two values, stored in register r1 and r2, together and return index
// of register with the result.
int cgadd(int r1, int r2);

// Multiply two values, stored in register r1 and r2, together and return index
// of register with the result.
int cgmul(int r1, int r2);

// Divide value in register r1 by the value in register r2 and return index of
// a register which holds the result.
int cgdiv(int r1, int r2);

// Subtract value in register r1 by the value in register r2 and return index of
// a register which holds the result.
int cgsub(int r1, int r2);

// Load the address of an identifier into a register and return this register.
int cgaddress(t_symbol_entry* symbol);

// Load the value which is stored at memory location in register r and return
// a register which contains this value.
int cgderef(int r, int type);

// Setup assembly code for entering a function
void cgfunctionpreamble(t_symbol_entry* symbol);

// Clean-up code for returning from a function
void cgfunctionpostamble(t_symbol_entry* symbol);

// Shift a register left by a constant
int cgshlconst(int r, int val);

int cgcompare_and_set(int ASTop, int r1, int r2);
int cgcompare_and_jump(int ASTop, int r1, int r2, int label);

// Freeing all registers.
void free_all_registers(void);

void generate_preamble();
void generate_postamble();
void generate_free_registers();
void generate_printint(int reg);

// Return the size of a primitive type in bytes
int cgprimsize(int type);

// Calls a function with the given id
int cgcall(t_symbol_entry* symbol, int argc);

// Emits assembly instructions for returning from a function
void cgreturn(int reg, t_symbol_entry* symbol);


void cgglobstr(int label, char* text);
int cgloadglobstr(int label);

// Binary/ unary operations available in C mapped to assembly instructions
int cgshift_l(int r1, int r2);
int cgshift_r(int r1, int r2);
int cg_or(int r1, int r2);
int cg_and(int r1, int r2);
int cg_negate(int r1);
int cg_logic_not(int r1);
int cg_invert(int r1);
int cgxor(int r1, int r2);
int cgequal(int r1, int r2);
int cgnotequal(int r1, int r2);
int cglessthan(int r1, int r2);
int cggreaterthan(int r1, int r2);
int cglessequal(int r1, int r2);
int cggreaterequal(int r1, int r2);

void cg_reset_locals(void);
int cg_get_local_offset(int type, int isparam);

void cg_copy_argument(int r, int arg_position);

extern FILE* outfile;

#endif