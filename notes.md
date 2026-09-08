
"If you start with the rules, you can use them to generate strings that are in the grammar. Strings created this way are called derivations because each is derived from the rules of the grammar. In each step of the game, you pick a rule and follow what it tells you to do. Most of the lingo around formal grammars comes from playing them in this direction. Rules are called productions because they produce strings in the grammar."

Terminal is a letter from the grammars' alphabet.
    - Literal value
    - In syntactic grammar; individual lexemes; tokens; "if" or 1234
    - "Terminal" because endpoint. No further "moves"

Non-terminal is a named reference to another rule in the grammar
    - "Play that rule and insert whatever it produces here"



Subset to implement first:

    Literals: Numbers, strings, booleans
    Unary expressions: ! (logical 'not'), - (negate a number)
    Binary expressions: Infix arithmetic (+, -, *, /),
                        Logical operators (==, !=, <, <= , >, >=)
    Parentheses: ( )


### 5.1.3
expression ->    literal
               | unary
               | binary
               | grouping ;

literal    ->    INT | FLOAT | CHAR | STRING | "true" | "false" ;
grouping   ->    "(" expression ")" ;
unary      ->    ( "-" | "!" ) expression ;
binary     ->    expression operator expression ;
operator   ->    "==" | "!=" | "<" | "<=" | ">" | ">="
               | "+"  | "-"  | "*" | "/" ;

### 6.1

expression ->   equality

equality   ->   comparison ( ( "!=" | "==" ) comparison )* ;

comparison ->   term ( ( ">" | ">=" | "<" | "<=" ) term )* ;

term       ->   factor ( ( "-" | "+" ) factor )* ;

factor     ->   unary ( ( "/" | "*" ) unary )* ;

unary      ->   ( "!" | "-" ) unary
                | primary;

primary    ->   INT | FLOAT | CHAR | STRING | "true" | "false" | "null" | "(" expression ")" ;


Equality      == !=      Left
Comparison  > >= < <=    Left
Term           - +       Left
Factor         / *       Left
Unary          ! -       Right

### 8.1 Statements and State

program -> declaration* EOF ;

declaration -> varDecl | statement ;

varDecl -> type IDENTIFIER ( "=" expression )? ";" ;

type -> "int" | "float" | "char" | "string" | "bool" | "auto" | IDENTIFIER ;

statement -> exprStmt | block ;

exprStmt -> expression ";" ;

block -> "{" declaration "}" ;


