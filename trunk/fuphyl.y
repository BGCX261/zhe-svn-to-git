// %lex-param   {void *stream}
// %parse-param {void *stream}

%{

#define TESTING

%}

%defines
%locations

%token SYN_TYPE
%token SYN_COND
%token SYN_END
%token SYN_RESULT
%token SYN_ASSIGN
%token SYN_SEP

%token TUPLE_BEGIN
%token TUPLE_END
%token ARRAY_BEGIN
%token ARRAY_END
%token LIST_BEGIN
%token LIST_END

%token KEY_IF
%token KEY_ELSE

%token LIT_NUMBER
%token LIT_STRING
%token LIT_ATOM
%token LIT_IDENTIFIER

%left LOG_OR LOG_ORELSE LOG_XOR
%left LOG_AND LOG_ANDALSO
%nonassoc REL_LT REL_LTE REL_GT REL_GTE REL_EQ REL_NEQ
%left OP_ADD OP_SUBTRACT
%left OP_MULTIPLY OP_DIVIDE
%right OP_POWER

/* unary ops */
%left OP_POSITIVE OP_NEGATIVE LOG_NOT

%left OP_LITERAL

%token ERROR_STRING
%token ERROR_ATOM

%token DUMMY

%start expr_seq

%%

else_cond : KEY_ELSE SYN_COND
          | KEY_ELSE
          ;

conditional : KEY_IF expr_seq SYN_COND
                  statement
              else_cond
                  statement
            ;

argument_tuple : '1' ;
type_expr : '2' ;

statement : SYN_END
          | simple_expr_seq SYN_END
          | conditional
          | simple_expr_seq SYN_SEP conditional
          ;

type_specifier : /* empty */
               | SYN_TYPE type_expr
               ;

visibility_modifier : /* empty */ 
                    ;

function : visibility_modifier LIT_IDENTIFIER argument_tuple 
               type_specifier SYN_ASSIGN
		       statement
		 ;
variable : visibility_modifier LIT_IDENTIFIER
               type_specifier SYN_ASSIGN
		       statement
		 ;

definition : function
           | variable
           ;

lit_tuple : TUPLE_BEGIN TUPLE_END
          | TUPLE_BEGIN SYN_SEP TUPLE_END
          | TUPLE_BEGIN simple_expr_seq TUPLE_END
          | TUPLE_BEGIN simple_expr_seq SYN_SEP TUPLE_END
          ;
lit_array : ARRAY_BEGIN ARRAY_END
          | ARRAY_BEGIN SYN_SEP ARRAY_END
          | ARRAY_BEGIN simple_expr_seq ARRAY_END
          | ARRAY_BEGIN simple_expr_seq SYN_SEP ARRAY_END
          ;
lit_list  : LIST_BEGIN LIST_END
          | LIST_BEGIN SYN_SEP LIST_END
          | LIST_BEGIN simple_expr_seq LIST_END
          | LIST_BEGIN simple_expr_seq SYN_SEP LIST_END
          ;

expr : LIT_NUMBER
	 | LIT_STRING
	 | LIT_ATOM

     | expr LOG_AND expr
     | expr LOG_ANDALSO expr
     | expr LOG_OR expr
     | expr LOG_ORELSE expr
     | expr LOG_XOR expr
     | LOG_NOT expr

     | expr REL_LT expr
     | expr REL_LTE expr
     | expr REL_GT expr
     | expr REL_GTE expr
     | expr REL_EQ expr
     | expr REL_NEQ expr

     | expr OP_ADD expr
     | expr OP_SUBTRACT expr
     | expr OP_MULTIPLY expr
     | expr OP_DIVIDE expr
     | expr OP_POWER expr

     | lit_tuple %prec OP_LITERAL
     | lit_array %prec OP_LITERAL
     | lit_list  %prec OP_LITERAL

     | OP_SUBTRACT expr %prec OP_NEGATIVE
     | OP_ADD expr %prec OP_POSITIVE
	 ;

simple_expr_seq : expr
	            | simple_expr_seq SYN_SEP expr
                ;

fancy_expr : expr SYN_SEP
           | definition
		   ;

expr_seq : /* empty */
         | SYN_SEP
         | expr_seq fancy_expr
         ;
