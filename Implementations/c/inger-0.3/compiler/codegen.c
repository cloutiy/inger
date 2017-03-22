/*************************************************
 *                                               *
 *  Module: codegen.c                            *
 *  Description:                                 *
 *      Generates x86 assembly code.             *
 *  Author: Meffert, D.                          *
 *  Modifications:                               *
 *                                               *
 *************************************************
 *                                               *
 *   This program is free software; you can      *
 *   redistribute it and/or modify  it under     *
 *   the terms of the GNU General Public         *
 *   License as published by the Free            *
 *   Software Foundation; either version 2       *
 *   of the License, or (at your option) any     *
 *   later version.                              *
 *                                               *
 *************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "nodenames.h"
#include "symtab.h"
#include "ast.h"



/*
 * --------------------------------------------
 * 
 * ASM Interface
 * 
 * TODO: put this in a seperate module.
 *
 * -------------------------------------------
 */

/* Data Transfer Instructions. */
#define EMITASM_MOVL( src, dst )            ( fprintf( g_outFile, "\tmovl\t%s, %s\n", src, dst ) )
#define EMITASM_MOVB( src, dst )            ( fprintf( g_outFile, "\tmovb\t%s, %s\n", src, dst ) )
#define EMITASM_CMOVE( src, dst )           ( fprintf( g_outFile, "\tcmove\t%s, %s\n", src, dst ) )
#define EMITASM_CMOVNE( src, dst )          ( fprintf( g_outFile, "\tcmovne\t%s, %s\n", src, dst ) )
#define EMITASM_CMOVG( src, dst)            ( fprintf( g_outFile, "\tcmovg\t%s, %s\n", src, dst ) )
#define EMITASM_CMOVGE( src, dst)           ( fprintf( g_outFile, "\tcmovge\t%s, %s\n", src, dst ) )
#define EMITASM_CMOVL( src, dst)            ( fprintf( g_outFile, "\tcmovl\t%s, %s\n", src, dst ) )
#define EMITASM_CMOVLE( src, dst)           ( fprintf( g_outFile, "\tcmovle\t%s, %s\n", src, dst ) )
#define EMITASM_CMOVNLE( src, dst)          ( fprintf( g_outFile, "\tcmovnle\t%s, %s\n", src, dst ) )
#define EMITASM_CMOVNL( src, dst)           ( fprintf( g_outFile, "\tcmovnl\t%s, %s\n", src, dst ) )
#define EMITASM_CMOVNGE( src, dst)          ( fprintf( g_outFile, "\tcmovnge\t%s, %s\n", src, dst ) )
#define EMITASM_CMOVNG( src, dst)           ( fprintf( g_outFile, "\tcmovng\t%s, %s\n", src, dst ) )
#define EMITASM_PUSHL( reg )                ( fprintf( g_outFile, "\tpushl\t%s\n", reg ) )
#define EMITASM_POPL( reg )                 ( fprintf( g_outFile, "\tpop\t%s\n", reg ) )
#define EMITASM_PUSHAD()                    { fprintf( g_outFile, "\tpusha\n" ); }
#define EMITASM_POPAD()                     { fprintf( g_outFile, "\tpopa\n" ); }
#define EMITASM_XCHGL( src, dst )           ( fprintf( g_outFile, "\txchgl\t%s, %s\n", src, dst ) )

/* Binary Arithmic Instructions. */
#define EMITASM_NEG( reg )                  ( fprintf( g_outFile, "\tneg\t%s\n", reg ) )
#define EMITASM_ADDL( src, dst )            ( fprintf( g_outFile, "\taddl\t%s, %s\n", src, dst ) )
#define EMITASM_SUBL( src, dst )            ( fprintf( g_outFile, "\tsubl\t%s, %s\n", src, dst ) )
#define EMITASM_IMUL( src )                 ( fprintf( g_outFile, "\timul\t%s\n", src ) )
#define EMITASM_IDIV( src )                 ( fprintf( g_outFile, "\tidiv\t%s\n", src ) )
#define EMITASM_XORL( src, dst )            ( fprintf( g_outFile, "\txorl\t%s, %s\n", src, dst ) )
#define EMITASM_CMPL( src, dst )            ( fprintf( g_outFile, "\tcmpl\t%s, %s\n", src, dst ) )
#define EMITASM_ANDL( src, dst )            ( fprintf( g_outFile, "\tandl\t%s, %s\n", src, dst ) )
#define EMITASM_ORL( src, dst )             ( fprintf( g_outFile, "\torl\t%s, %s\n", src, dst ) )
#define EMITASM_XORL( src, dst )            ( fprintf( g_outFile, "\txorl\t%s, %s\n", src, dst ) )

/* Control Transfer Instructions. */
#define EMITASM_CALL( func )                ( fprintf( g_outFile, "\tcall\t%s\n", func ) )
#define EMITASM_JE( dst )                   ( fprintf( g_outFile, "\tje\t%s\n", dst ) )
#define EMITASM_JNE( dst )                  ( fprintf( g_outFile, "\tjne\t%s\n", dst ) )
#define EMITASM_JG( dst )                   ( fprintf( g_outFile, "\tjg\t%s\n", dst ) )
#define EMITASM_JGE( dst )                  ( fprintf( g_outFile, "\tjge\t%s\n", dst ) )
#define EMITSAM_JL( dst )                   ( fprintf( g_outFile, "\tjl\t%s\n", dst ) )
#define EMITSAM_JLE( dst )                  ( fprintf( g_outFile, "\tjle\t%s\n", dst ) )
#define EMITASM_JNG( dst )                  ( fprintf( g_outFile, "\tjng\t%s\n", dst ) )
#define EMITASM_JNGE( dst )                 ( fprintf( g_outFile, "\tjnge\t%s\n", dst ) )
#define EMITSAM_JNL( dst )                  ( fprintf( g_outFile, "\tjnl\t%s\n", dst ) )
#define EMITSAM_JNLE( dst )                 ( fprintf( g_outFile, "\tjnle\t%s\n", dst ) )
#define EMITASM_JMP( dst )                  ( fprintf( g_outFile, "\tjmp\t%s\n", dst ) )

/* Shift and Rotate Instructions. */
#define EMITASM_SALL( src, dst )            ( fprintf( g_outFile, "\tsall\t%s, %s\n", src, dst ) ) 
#define EMITASM_SARL( src, dst )            ( fprintf( g_outFile, "\tsarl\t%s, %s\n", src, dst ) )

/* Converts a register enum to a register string. */
#define ASM_REG( reg )                      ( g_registers[reg] )

/* Generates a new label name and stores it in the string. */
#define GENERATE_LABEL( str )               ( sprintf( str, ".L%d", g_labelCount++ ) )

/* Generates a string for a pointer. */
#define ASM_POINTER( str, reg, offset )     { if( offset == 0 ) sprintf( str, "(%s)", reg ); \
                                              else sprintf( str, "%d(%s)", offset, reg ); }
		
/* Generates a string for the location of a local variable on the stack. */
#define ASM_LOCAL( str, loc )               { sprintf( str, "%d(%%ebp)", loc ); }

enum REGISTERS
{
    EAX = 0,
    AX,
    AL,
    AH,
    EBX,
    BX,
    BL,
    BH,
    ECX,
    CX,
    CL,
    CH,
    EDX,
    DX,
    DL,
    DH,
    ESP,
    EBP,
    EIP
};

char g_registers[19][8] = 
{
    "%eax", "%ax", "%al", "%ah", "%ebx", "%bx", "%bl", "%bh", "%ecx", "%cx", "%cl", "%ch",
    "%edx", "%dx", "%dl", "%dh", "%esp", "%ebp", "%eip"
};


/*
 * Converts an integer to a literal used in assembly code.
 */
static char* IntToAsmLit( int i )
{
    static char str[32];
    sprintf( str, "$%d", i );
    return str;
}

static char *AsmAddressReg( int reg )
{
    static char str[32];
    sprintf( str, "(%s)", g_registers[reg] );
    return str;
}

/* Encodes a floating point value.
 */
static long EncodeFloat( char *val )
{
    return 0x00000000;
}


/******************************************
 *               GLOBALS                  *  
 ******************************************/
FILE *g_outFile;
static int g_labelCount;

/******************************************
 *               FORWARDS                 *  
 ******************************************/
static void GenerateCodeForNode( TreeNode *node );

/* 
 * This functions counts the # of bytes 
 * used by local variables in this function. 
 */
static int CountAllocatedBytes( TreeNode *node, int byteCount )
{
    Symbol *symbol;
    
    int i;
  //  printf( "CountAllocatedBytes: %s\n", GetNodeName(TOAST(node)->id) );
    switch( TOAST( node )->id )
    {

    /* Generate code if this is a global declaration. */
    case NODE_DECLARATION:
	symbol = FindSymbol( GetNameFromDecl( node ) );
	switch( GetTypeFromDecl( node ) )
	{
	case INT:
	case FLOAT:
	    symbol->location = -( byteCount + 4 );
	    byteCount += 4;
	    break;
	case CHAR:
	case BOOLEAN:
	    symbol->location = -( byteCount + 4 );
	    byteCount++;
	    break;
	}
        break;
	
    /* Recurse. */
    default:
	ListFirst( node->children );
        for( i = 0; i < ListSize( node->children ); i++ )
        {
            byteCount = CountAllocatedBytes( ListGet( node->children ), byteCount );
            ListNext( node->children );
        }
    }
    return byteCount;
}


/* 
 * This functions initializes local variables.
 */
static void InitializeLocals( TreeNode *node )
{
    Symbol *symbol;
    int i;
    char str[64];
    
    switch( TOAST( node )->id )
    {

    /* Generate code if this is a global declaration. */
    case NODE_DECLARATION:
	symbol = FindSymbol( GetNameFromDecl( node ) );
	switch( GetTypeFromDecl( node ) )
	{
	case INT:
	    if( GetInitializerFromDecl( node ) != NULL )
	    {
		sprintf( str, "%d(%s)", symbol->location, ASM_REG(EBP) );
		EMITASM_MOVL( IntToAsmLit( atoi(GetInitializerFromDecl( node ) )), str );
	    }
	    break;
	case FLOAT:
	    break;
	case CHAR:
	    break;
	case BOOLEAN:
	    break;
	}
        break;
	
    /* Recurse. */
    default:
	ListFirst( node->children );
        for( i = 0; i < ListSize( node->children ); i++ )
        {
            InitializeLocals( ListGet( node->children ) );
            ListNext( node->children );
        }
    }
}


/*
 * This function generates the code 
 * needed for a function declaration.
 */
static void GenerateFunctionCode( TreeNode *node )
{
    int localBytes, i;
    Symbol *symbol;

    
    /* Generate function start code. */
    fprintf( g_outFile, ".globl %s\n", GetNameFromHeader( GetHeaderFromFunction(node) ) );
    fprintf( g_outFile, "\t.type\t%s,@function\n", GetNameFromHeader( GetHeaderFromFunction(node) ) );
    fprintf( g_outFile, "%s:\n", GetNameFromHeader( GetHeaderFromFunction(node) ) );
    EMITASM_PUSHL( ASM_REG(EBP) );
    EMITASM_MOVL( ASM_REG(ESP), ASM_REG(EBP) );
    EMITASM_PUSHAD();
    
    /* Enter the scope in the symbol table. */
    EnterScope();

    /* Allocate space on the stack for local variables. */
    localBytes = CountAllocatedBytes( GetBlockFromFunction( node ), 0 );
    EMITASM_SUBL( IntToAsmLit(localBytes), ASM_REG(ESP) );
    InitializeLocals( GetBlockFromFunction( node ) );
    //printf( "paramcount: %d\n", GetParamCountFromHeader( GetHeaderFromFunction(node) ) );
    for( i = 0; i < GetParamCountFromHeader( GetHeaderFromFunction( node ) ); i++ )
    {
        symbol = FindSymbol( GetParamNameFromHeader( GetHeaderFromFunction( node ), i ) );
	symbol->location = (i + 2) * 4;
	//printf( "symbol: %d\n", symbol->location );
    }

    /* Generate code for function implementation. */
    GenerateCodeForNode( GetBlockFromFunction( node ) );

    /* Leave scope in the symbol table. */
    ExitScope();

    /* Generate function return code. */
    EMITASM_POPAD();
    //EMITASM_POPL( ASM_REG(EBP) );
    fprintf( g_outFile, "\tleave\n" );
    fprintf( g_outFile, "\tret\n\n" );
}

/*
 *  Generates code for function calling.
 */
static void GenerateApplicationCode( TreeNode *node )
{
    /* Generate the code that pushes the arguments from right 
     * to left on the stack (c-style function calling). */
    int i = 0;
    for( i = GetArgumentCountFromApplication( node )-1; i >= 0; i-- )
    {
        GenerateCodeForNode( GetArgumentFromApplication( node, i ) );
        fprintf( g_outFile, "\tpushl\t%%eax\n" );
    }
    
    /* Generate the code that calls the function, and restore the
     * stack pointer after returning. */
    fprintf( g_outFile, "\tcall\t%s\n", GetNameFromApplication( node ) );
    if( GetArgumentCountFromApplication(node) != 0 )
	fprintf( g_outFile, "\taddl\t$%d, %%esp\n", GetArgumentCountFromApplication( node )*4 );
}



/*
 *  Generates code used by all comparison codes.
 *   (Comparison functions use a conditional move and need a 0
 *   and a 1 in EBX and ECX)
 */
static void GenerateInitializingComparisonCode()
{
    EMITASM_MOVL( IntToAsmLit(0), ASM_REG(EBX) );
    EMITASM_MOVL( IntToAsmLit(1), ASM_REG(ECX) );
}


/* 
 * Recursive function that traverses the AST and
 * generates assembly code for the global
 * declarations.
 */
static void GenerateGlobalDeclCodeForNode( TreeNode *node )
{
    int i;
    SimpleType type;
    char *name;
    Symbol *symbol;

    //printf( "GenerateGlobalDeclCodeForNode: %s\n", GetNodeName( TOAST(node)->id )  );
    switch( TOAST( node )->id )
    {

    /* Generate code if this is a global declaration. */
    case NODE_DECLARATION:
	type = GetTypeFromDecl( node );
	name = GetNameFromDecl( node );
	symbol = FindSymbol( name );
	if( symbol != NULL )
	{
	    fprintf( g_outFile, ".globl %s\n", name );
	    if( type == INT || type== FLOAT )
    	    {
                fprintf( g_outFile, "\t.align 4\n" );
	        fprintf( g_outFile, "\t.type\t%s,@object\n", name );
	        fprintf( g_outFile, "\t.size\t%s,4\n", name );
	        fprintf( g_outFile, "%s:\n", name );
	        if( type == INT )
		{
	    	    fprintf( g_outFile, "\t.long\t%s\n", GetInitializerFromDecl( node ) );
		}
	        else if( type == FLOAT )
		{
	            fprintf( g_outFile, "\t.long\t%lx\n", EncodeFloat( GetInitializerFromDecl( node ) ) );
		}
	    }
	}
        break;
	
    /* Generate no code for other nodes, but recurse for all
     * children. */
    default:
	ListFirst( node->children );
        for( i = 0; i < ListSize( node->children ); i++ )
        {
            GenerateGlobalDeclCodeForNode( ListGet( node->children ) );
            ListNext( node->children );
        }
    }
}



/*
 *  Recursive function that traverses the AST and
 *  generates the assembly code at each node.
 */
static void GenerateCodeForNode( TreeNode *node )
{
    Symbol *symbol;
    int i = 0;
    char label1[32], label2[32];
    char str[32];
    

    /* Determine the type of this node and generate the
     * corresponding code. */
   // printf( "GenerateCodeForNode: %s\n", GetNodeName(TOAST(node)->id) );
    switch( TOAST( node )->id )
    {

    /* Generates code for the function implementation only
     * if it is a function definition. */
    case NODE_FUNCTION:
        if( GetBlockFromFunction( node ) != NULL )
        {
            GenerateFunctionCode( node );
        }
        break;

    /* Generate code for a variable declaration. */
    case NODE_DECLARATION:
	/*
	symbol = FindSymbol( GetNameFromDecl( node ) );
	if( symbol->global == FALSE )
	{
	    
	}
	*/
        break;

    /* Generates code for a function call. */
    case NODE_APPLICATION:
        GenerateApplicationCode( node );
	break;
    
    /* Generates code for an if statement. */
    case NODE_IF:
	GenerateCodeForNode( GetTreeChild( node, 0 ) );
	EMITASM_CMPL( IntToAsmLit(0), ASM_REG(EAX) );
	sprintf( label1, ".L%d", g_labelCount++ );
	if( GetElseBlockFromIf(node) == NULL )
	{
	    EMITASM_JE( label1 );
	    GenerateCodeForNode( GetThenBlockFromIf( node ) );
	    fprintf( g_outFile, "%s:\n", label1 );
	}
	else
	{
	    sprintf( label2, ".L%d", g_labelCount++ );
	    EMITASM_JE( label1 );
	    GenerateCodeForNode( GetThenBlockFromIf( node ) );
	    EMITASM_JMP( label2 );
	    fprintf( g_outFile, "%s:\n", label1 );
	    GenerateCodeForNode( GetElseBlockFromIf( node ) );
	    fprintf( g_outFile, "%s:\n", label2 );
	}
	break;
	
    /* Generate code for a while loop. */
    case NODE_WHILE:
	GENERATE_LABEL( label1 );
	fprintf( g_outFile, "%s:\n", label1 );
	GenerateCodeForNode( GetExpressionFromWhile( node ) );
	EMITASM_CMPL( IntToAsmLit(0), ASM_REG(EAX) );
	GENERATE_LABEL( label2 );
        EMITASM_JE( label2 );
	GenerateCodeForNode( GetBlockFromWhile( node ) );
	EMITASM_JMP( label1 );
	fprintf( g_outFile, "%s:\n", label2 );
	break;
	
    /* Generate code for an assignment. */
    case NODE_ASSIGN:
	/* Put the result of the assignment expression in EBX. */
	GenerateCodeForNode( GetTreeChild( node, 1 ) );
	symbol = FindSymbol( TOAST( GetTreeChild( node, 0 ) )->val.identifier );
	if( symbol->global == TRUE )
	{
	    EMITASM_MOVL( ASM_REG(EAX), TOAST( GetTreeChild( node, 0 ) )->val.identifier );
	}
	else
	{
	    ASM_LOCAL( str, symbol->location );	
	    EMITASM_MOVL( ASM_REG(EAX), str );
	}
	//EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
	/* Get the left hand side of the expression and put it in EAX. */
	//GenerateCodeForNode( GetTreeChild( node, 0 ) );
	/* Move the result of the assignment from EBX to EAX. */
	//ASM_POINTER( str, ASM_REG(EAX), 0 );
	//EMITASM_MOVL( ASM_REG(EBX), str );
	break;

    /* Generates code that stores the result of an expression 
     * in EAX. */
    case NODE_LIT_INT:
        EMITASM_MOVL( IntToAsmLit( TOAST(node)->val.uintvalue ), ASM_REG(EAX) );
        break;

    /* Generates code that stores the value of an identifier
     * in EAX. */
    case NODE_LIT_IDENTIFIER:
	symbol = FindSymbol( GetNameOfIdentifier( node ) );
	if( symbol->global == TRUE )
	{
	    EMITASM_MOVL( symbol->name, ASM_REG(EAX) );
	}
	else
	{
	    EMITASM_MOVL( ASM_REG(EBP), ASM_REG(ECX) );
	    EMITASM_ADDL( IntToAsmLit(symbol->location), ASM_REG(ECX) );
	    EMITASM_MOVL( AsmAddressReg(ECX), ASM_REG(EAX) );
	}
	break;
	
    /* Generates code that negates the value in EAX. */ 
    case NODE_UNARY_SUBTRACT:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_NEG( ASM_REG(EAX) );
        break;

    /* Generates code that 'ands' the result of an expression
     * with the value in EAX. */
    case NODE_BITWISE_AND:
	GenerateCodeForNode( GetTreeChild( node, 0 ) );
	EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
	GenerateCodeForNode( GetTreeChild( node, 1 ) );
	EMITASM_ANDL( ASM_REG(EBX), ASM_REG(EAX) );
	break;
	
    /* Generates code that 'ors' the result of an expression
     * with the value in EAX. */
    case NODE_BITWISE_OR:
	GenerateCodeForNode( GetTreeChild( node, 0 ) );
	EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
	GenerateCodeForNode( GetTreeChild( node, 1 ) );
	EMITASM_ORL( ASM_REG(EBX), ASM_REG(EAX) );
	break;
	
   /* Generates code that 'xors' the result of an expression
     * with the value in EAX. */
    case NODE_BITWISE_XOR:
	GenerateCodeForNode( GetTreeChild( node, 0 ) );
	EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
	GenerateCodeForNode( GetTreeChild( node, 1 ) );
	EMITASM_XORL( ASM_REG(EBX), ASM_REG(EAX) );
	break;
	
   /* Generates code that adds the result of an expression
     * to the value in EAX. */
    case NODE_BINARY_ADD:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_ADDL( ASM_REG(EBX), ASM_REG(EAX) );
        break;

    /* Generates code that subtracts the result of an expression 
     * from the value in EAX. */
    case NODE_BINARY_SUBTRACT:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_XCHGL( ASM_REG(EAX), ASM_REG(EBX) );
        EMITASM_SUBL( ASM_REG(EBX), ASM_REG(EAX) );
        break;

    /* Generates code that multiplies the value in EAX with the
     * result of an expression. */
    case NODE_MULTIPLY:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_IMUL( ASM_REG(EBX) );
        break;

    /* Generates code that divides the value in EAX with the result
     * of an expression. */
    case NODE_DIVIDE:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_XCHGL( ASM_REG(EAX), ASM_REG(EBX) );
        EMITASM_XORL( ASM_REG(EDX), ASM_REG(EDX) ); /* Set EDX to 0. */
        EMITASM_IDIV( ASM_REG(EBX) );
        break;

    /* Generates code that divides the value in EAX with the result of
     * an expression and stores the remainder in EAX. */
    case NODE_MODULUS:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_XCHGL( ASM_REG(EAX), ASM_REG(EBX) );
        EMITASM_XORL( ASM_REG(EDX), ASM_REG(EDX) );
        EMITASM_IDIV( ASM_REG(EBX) );
        EMITASM_MOVL( ASM_REG(EDX), ASM_REG(EAX) );
        break;

    /* Generates code that compares the result of an expression to see
     * if it equals the value in EAX. */
    case NODE_EQUAL:
	GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_CMPL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateInitializingComparisonCode();
        EMITASM_CMOVNE( ASM_REG(EBX), ASM_REG(EAX) );
        EMITASM_CMOVE( ASM_REG(ECX), ASM_REG(EAX) );
	break;
	
    /* Generates code that compares the result of an expression to see
     * if it does not equal the value in EAX. */
    case NODE_NOTEQUAL:
	GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_CMPL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateInitializingComparisonCode();
        EMITASM_CMOVE( ASM_REG(EBX), ASM_REG(EAX) );
        EMITASM_CMOVNE( ASM_REG(ECX), ASM_REG(EAX) );
	break;
	
    /* Generates code that compares the result of an expression to see if
     * it is smaller than the value in EAX. */
    case NODE_LESS:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_CMPL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateInitializingComparisonCode();
        EMITASM_CMOVNL( ASM_REG(EBX), ASM_REG(EAX) );
        EMITASM_CMOVL( ASM_REG(ECX), ASM_REG(EAX) );
        break;

    /* Generates code that compares the result of an expression to see if
     * it is smaller than or equals the value in EAX. */
    case NODE_LESSEQUAL:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_CMPL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateInitializingComparisonCode();
        EMITASM_CMOVNLE( ASM_REG(EBX), ASM_REG(EAX) );
        EMITASM_CMOVLE( ASM_REG(ECX), ASM_REG(EAX) );
        break;

    /* Generates code that compares the result of an expression to see if
     * it is larger than the value in EAX. */
    case NODE_GREATER:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_CMPL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateInitializingComparisonCode();
        EMITASM_CMOVNG( ASM_REG(EBX), ASM_REG(EAX) );
        EMITASM_CMOVG( ASM_REG(ECX), ASM_REG(EAX) );
        break;
	
    /* Generates code that compares the result of an expression to see if
     * it is larger than or equals the value in EAX. */
    case NODE_GREATEREQUAL:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_CMPL( ASM_REG(EAX), ASM_REG(EBX) );
        GenerateInitializingComparisonCode();
        EMITASM_CMOVNGE( ASM_REG(EBX), ASM_REG(EAX) );
        EMITASM_CMOVGE( ASM_REG(ECX), ASM_REG(EAX) );
        break;

    /* Generates code that bitshifts the value in EAX to the left
     * where the # of bits is determined by the result of an expression
     * wich is stored in CL. */
    case NODE_BITWISE_LSHIFT:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(ECX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_XCHGL( ASM_REG(EAX), ASM_REG(ECX) );
        EMITASM_SALL( ASM_REG(CL), ASM_REG(EAX) );
        break;
	
   /* Generates code that bitshifts the value in EAX to the right
    * where the # of bits is determined by the result of an expression
    * wich is stored in CL. */
   case NODE_BITWISE_RSHIFT:
        GenerateCodeForNode( GetTreeChild( node, 0 ) );
        EMITASM_MOVL( ASM_REG(EAX), ASM_REG(ECX) );
        GenerateCodeForNode( GetTreeChild( node, 1 ) );
        EMITASM_XCHGL( ASM_REG(EAX), ASM_REG(ECX) );
        EMITASM_SARL( ASM_REG(CL), ASM_REG(EAX) );
        break;

    /* Generate no code for other nodes, but recurse for all
     * children. */
    default:
        ListFirst( node->children );
        for( i = 0; i < ListSize( node->children ); i++ )
        {
            GenerateCodeForNode( ListGet( node->children ) );
            ListNext( node->children );
        }
    }
}


/*
 * This function initiates the code generation process. 
 */
void GenerateCode( TreeNode *node )
{
    /* Go to the root node in the symbol table. */
    GotoSymbolRoot();

    /* Generate global declaration code. */
    fprintf( g_outFile, ".data\n" );
    GenerateGlobalDeclCodeForNode( node );

    /* Generate code for text section. */
    fprintf( g_outFile, ".text\n" );
    fprintf( g_outFile, "\t.align 4\n" );

    /* Go back to the root node in the symbol table. */
    GotoSymbolRoot();

    /* Generate actual program code. */
    g_labelCount = 3;
    GenerateCodeForNode( node );	

    /* Generate start stub. */
    /*
    fprintf( g_outFile, "main:\n" );
    fprintf( g_outFile, "\tpushl\t%%ebp" );
    fprintf( g_outFile, "\tmovl\t%%esp, %%ebp\n" );
    fprintf( g_outFile, "\tpopl\t%%ebp\n" );
    fprintf( g_outFile, "\tcall\t%s", startFunc );
    fprintf( g_outFile, "\tret\n" );
    fprintf( g_outFile, ".Lfe1:\n" );
    fprintf( g_outFile, "\t.size\tmain,.Lfe1-main\n" );
    */
}
