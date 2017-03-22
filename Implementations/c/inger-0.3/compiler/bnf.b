/*!bnf_ */
Subcollections 
  SET_TYPES = { "bool", "char", "float", "int", "string" }
  SET_LITERALS = { <boolean_literal>, <char_literal>, <float_literal>, <int_literal>, <string_literal> }
  SET_INSTRUCTIONS = { "label", "break", "continue", "if", "goto", "while", "do", "switch", "return", ";" }
  SET_UNARIES = { "&", "*", "~", "+", "-", "!" }

/*!bnf_module*/
module 
    module:               "module" <identifier> ";" globals.
/*!*/

/*!bnf_globals*/
globals 
    globals:              e.
    globals:              global globals.
    globals:              "extern" global globals.
/*!*/

/*!bnf_global*/
global 
    global:               function.
    global:               declaration.
/*!*/

/*!bnf_function*/
function 
    function:             functionheader functionrest.
/*!*/

/*!bnf_functionheader*/
functionheader 
    functionheader:       modifiers <identifier> ":" paramlist "->" returntype.
/*!*/

/*!bnf_functionrest*/
functionrest 
    functionrest:         ";".
    functionrest:         block.
/*!*/

/*!bnf_modifiers*/
modifiers: 
    modifiers:            e.
    modifiers:            "start".
/*!*/

/*!bnf_paramlist*/
paramlist 
    paramlist:            "void".
    paramlist:            paramblock moreparamblocks.
/*!*/

/*!bnf_moreparamblocks*/
moreparamblocks 
    moreparamblocks:      e.
    moreparamblocks:      ";" paramblock moreparamblocks.
/*!*/

/*!bnf_paramblock*/
paramblock 
    paramblock:           type param moreparams.
/*!*/

/*!bnf_moreparams*/
moreparams 
    moreparams:           e.
    moreparams:           "," param moreparams.
/*!*/

/*!bnf_param*/
param 
    param:                reference <identifier> dimensionblock.
/*!*/

/*!bnf_returntype*/
returntype 
    returntype:           type reference dimensionblock.
/*!*/

/*!bnf_reference*/
reference 
    reference:            e.
    reference:            "*" reference.
/*!*/

/*!bnf_dimensionblock*/
dimensionblock 
    dimensionblock:       e.
    dimensionblock:       "[" "]" dimensionblock.
/*!*/

/*!bnf_block*/
block 
    block:                "{" code "}".
/*!*/

/*!bnf_code*/
code 
    code:                 e.
    code:                 block code
    code:                 statement code.
/*!*/

/*!bnf_statement*/
statement 
    statement:            "label" <identifier> ";"
    statement:            ";"
    statement:            "break" ";"
    statement:            "continue" ";"
    statement:            expression ";"
    statement:            declarationblock ";"
    statement:            "if" "(" expression ")" block elseblock
    statement:            "goto" <identifier> ";"
    statement:            "while" "(" expression ")" "do" block
    statement:            "do" block "while" "(" expression ")" ";"
    statement:            "switch" "(" expression ")" "{" switchcases "default" block "}"
    statement:            "return" "(" expression ")" ";".
/*!*/

/*!bnf_elseblock*/
elseblock 
    elseblock:            e.
    elseblock:            "else" block.
/*!*/

/*!bnf_switchcases*/
switchcases 
    switchcases:          e.
    switchcases:          "case" <int_literal> block swithcases.
/*!*/

/*!bnf_declarationblock*/
declarationblock 
    declarationblock:     type declaration restdeclarations.
/*!*/

/*!bnf_restdeclarations*/
restdeclarations 
    restlocals:           e.
    restlocals:           "," declaration restdeclarations.
/*!*/

/*!bnf_declaration*/
declaration 
    local:                reference <identifier> indexblock initializer.
/*!*/

/*!bnf_indexblock*/
indexblock 
    indexblock:           e.
    indexblock:           "[" <int_literal> "]" indexblock.
/*!*/

/*!bnf_initializer*/
initializer 
    initializer:          e.
    initializer:          "=" expression.
/*!*/

/*!bnf_expression*/
expression 
    expression:           logicalor restexpression.
/*!*/

/*!bnf_restexpression*/
restexpression 
    restexpression:       e.
    restexpression:       "=" logicalor restexpression.
/*!*/

/*!bnf_logicalor*/
logicalor 
    logicalor:            logicaland restlogicalor.
/*!*/

/*!bnf_restlogicalor*/
restlogicalor 
    restlogicalor:        e.
    restlogicalor:        "||" logicaland restlogicalor.
/*!*/

/*!logicaland*/
logicaland 
    logicaland:           bitwiseor restlogicaland.
/*!*/

/*!bnf_restlogicaland*/
restlogicaland 
    restlogicaland:       e.
    restlogicaland:       "&&" bitwiseor restlogicaland.
/*!*/

/*!bnf_bitwiseor*/
bitwiseor 
    bitwiseor:            bitwisexor restbitwiseor.
/*!*/

/*!bnf_restbitwiseor*/
restbitwiseor 
    restbitwiseor:        e.
    restbitwiseor:        "|" bitwisexor restbitwiseor.
/*!*/

/*!bnf_bitwisexor*/
bitwisexor 
    bitwisexor:           bitwiseand restbitwisexor.
/*!*/

/*!bnf_restbitwisexor*/
restbitwisexor 
    restbitwisexor:       e.
    restbitwisexor:       "^" bitwiseand restbitwisexor.
/*!*/

/*!bnf_bitwiseand*/
bitwiseand 
    bitwiseand:           equality restbitwiseand.
/*!*/

/*!bnf_restbitwiseand*/
restbitwiseand 
    restbitwiseand:       e.
    restbitwiseand:       "&" equality restbitwiseand.
/*!*/

/*!bnf_equality*/
equality 
    equality:             relation restequality.
/*!*/

/*!bnf_restequality*/
restequality 
    restequality:         e.
    restequality:         equalityoperator relation restequality.
/*!*/

/*!bnf_equalityoperator*/
equalityoperator 
    equalityoperator:     "==".
    equalityoperator:     "!=".
/*!*/

/*!bnf_relation*/
relation 
    relation:             shift restrelation.
/*!*/

/*!bnf_restrelation*/
restrelation 
    restrelation:         e.
    restrelation:         relationoperator shift restrelation.
/*!*/

/*!bnf_relationoperator*/
relationoperator 
    relationoperator:     "<".
    relationoperator:     "<=".
    relationoperator:     ">".
    relationoperator:     ">=".
/*!*/

/*!bnf_shift*/
shift 
    shift:                addition restshift.
/*!*/

/*!bnf_restshift*/
restshift 
    restshift:            e.
    restshift:            shiftoperator addition restshift.
/*!*/

/*!bnf_shiftoperator*/
shiftoperator 
    shiftoperator:        "<<".
    shiftoperator:        ">>".
/*!*/

/*!bnf_addition*/
addition 
    addition:             multiplication restaddition.
/*!*/

/*!bnf_restaddition*/
restaddition 
    restaddition:         e.
    restaddition:         additionoperator multiplication restaddition.
/*!*/

/*!bnf_additionoperator*/
additionoperator 
    additionoperator:     "+".
    additionoperator:     "-".
/*!*/

/*!multiplication*/
multiplication 
    multiplication:       unary3 restmultiplication.
/*!*/

/*!bnf_restmultiplication*/
restmultiplication 
    restmultiplication:   e.
    restmultiplication:   multiplicationoperator unary3 restmultiplication.
/*!*/

/*!bnf_multiplicationoperator*/
multiplicationoperator 
    multiplicationoperator: "*".
    multiplicationoperator: "/".
    multiplicationoperator: "%".
/*!*/

/*!bnf_unary3*/
unary3 
    unary3:               unary2
    unary3:               unary3operator unary3.
/*!*/

/*!bnf_unary3operator*/
unary3operator: 
    unary3operator:       "&".
    unary3operator:       "*".
    unary3operator:       "~".
/*!*/

/*!bnf_unary2*/
unary2 
    unary2:               factor.
    unary2:               unary2operator unary2.
/*!*/

/*!bnf_unary2operator*/
unary2operator 
    unary2operator:       "+".
    unary2operator:       "-".
    unary2operator:       "!".
/*!*/

/*!bnf_factor*/
factor 
    factor:               <identifier> application.
    factor:               immediate.
    factor:               "(" expression ")".
/*!*/

/*!bnf_application*/
application 
    application:          e.
    application:          "[" expression "]" application.
    application:          "(" expression moreexpressions ")".
/*!*/

/*!bnf_moreexpressions*/
moreexpressions 
    moreexpressions:      e.
    moreexpressions:      "," expression morexpressions.
/*!*/

/*!bnf_type*/
type 
    type:                 "bool".
    type:                 "char".
    type:                 "float".
    type:                 "int".
    type:                 "string".
/*!*/

/*!bnf_immediate*/
immediate 
    immediate:            <boolean_literal>.
    immediate:            <char_literal>.
    immediate:            <float_literal>.
    immediate:            <int_literal>.
    immediate:            <string_literal>.
/*!*/
