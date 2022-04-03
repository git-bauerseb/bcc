## Grammar

global-declarations ::= global-declarations
                     | global-declaration global-declarations
                     ;

global-declaration ::= function-declaration | var-declaration;


type ::= type-keyword opt-pointer;

type-keyword ::= 'void' | 'char' | 'int' | 'long'
               ;

opt-pointer ::= <empty> | '*' opt-pointer;

statements ::= statement
             | statement statements
             ;

statement ::= 'print' expression ';'
            | 'int' identifier ';'
            | identifier '=' expression ';'
            | if-statement
            | while-statement
            | for-statement
            ;

return-statement ::= 'return' (expression ?) ';'
                   ;

function-call ::= identifier '(' expression ')'
                ;

function-declaration ::= 'void' identifier '(' ')' compound-statement
                       ;

for-statement ::= 'for' '(' init-statement ';' bool-expression ';' 
                     post-statement  ')' compound-statement
                ;

while-statement ::= 'while' '(' expression ')' compound-statement
                  ;

if-statement ::= if-head
               | if-head 'else' compound-statement
               ;

if-head ::= 'if' '(' expression ')' compound-statement;

compound-statement ::= '{' '}'
                     | '{' statements '}'
                     ;


identifier ::= T_IDENT
             ;

expression ::= equals;

equals ::= comparison ('==' comparison ?)
         | comparison ('!=' comparison ?)
         ;


comparison ::= term ('<' term ?)
             | term ('<=' term ?)
             | term ('>' term ?)
             | term ('>=' term ?)
             ;


term ::= factor ('+' factor ?)
       | factor ('-' factor ?)
       ;

factor ::= number '*' number
         | number '/' number
         ;

number ::= T_INTLIT
         ;


## Assembly