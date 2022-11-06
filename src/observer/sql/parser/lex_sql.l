%{
#include<string.h>
#include<stdio.h>
#include "parse_defs.h"

struct ParserContext;

#include "yacc_sql.tab.h"
extern int atoi();
extern double atof();

#if YYDEBUG > 0
#define debug_printf  printf
#else
#define debug_printf(...)
#endif // YYDEBUG

#define RETURN_TOKEN(token) debug_printf("%s\n",#token);return token
%}

/* Prevent the need for linking with -lfl */
%option noyywrap
%option bison-bridge
%option reentrant

WHITE_SAPCE      [\ \t\b\f]
DIGIT            [0-9]+

ID               [A-Za-z_]+[A-Za-z0-9_]*
DOT              \.
QUOTE            [\'\"]
%x STR

%%

{WHITE_SAPCE}                                        // ignore whitespace
\n                                                   ;

[\-]?{DIGIT}+                                        yylval->number=atoi(yytext); RETURN_TOKEN(NUMBER);
[\-]?{DIGIT}+{DOT}{DIGIT}+                           yylval->floats=(float)(atof(yytext)); RETURN_TOKEN(FLOAT);

";"                                                  RETURN_TOKEN(SEMICOLON);
{DOT}                                                RETURN_TOKEN(DOT);
"*"                                                  RETURN_TOKEN(STAR);
[Ee][Xx][Ii][Tt]                                     RETURN_TOKEN(EXIT);
[Hh][Ee][Ll][Pp]                                     RETURN_TOKEN(HELP);
[Dd][Ee][Ss][Cc]                                     RETURN_TOKEN(DESC);
[Cc][Rr][Ee][Aa][Tt][Ee]                             RETURN_TOKEN(CREATE);
[Dd][Rr][Oo][Pp]                                     RETURN_TOKEN(DROP);
[Tt][Aa][Bb][Ll][Ee]                                 RETURN_TOKEN(TABLE);
[Tt][Aa][Bb][Ll][Ee][Ss]                             RETURN_TOKEN(TABLES);
[Ii][Nn][Dd][Ee][Xx]                                 RETURN_TOKEN(INDEX);
[Uu][Nn][Ii][Qq][Uu][Ee]                             RETURN_TOKEN(UNIQUE);
[Oo][Nn]                                             RETURN_TOKEN(ON);
[Ss][Hh][Oo][Ww]                                     RETURN_TOKEN(SHOW);
[Ss][Yy][Nn][Cc]                                     RETURN_TOKEN(SYNC);
[Ss][Ee][Ll][Ee][Cc][Tt]                             RETURN_TOKEN(SELECT);
[Ff][Rr][Oo][Mm]                                     RETURN_TOKEN(FROM);
[Ww][Hh][Ee][Rr][Ee]                                 RETURN_TOKEN(WHERE);
[Aa][Nn][Dd]                                         RETURN_TOKEN(AND);
[Oo][Rr]                                             RETURN_TOKEN(OR);
[Ii][Nn][Ss][Ee][rR][tT]                             RETURN_TOKEN(INSERT);
[Ii][Nn][Tt][Oo]                                     RETURN_TOKEN(INTO);
[Vv][Aa][Ll][Uu][Ee][Ss]                             RETURN_TOKEN(VALUES);
[Dd][Ee][Ll][Ee][Tt][Ee]                             RETURN_TOKEN(DELETE);
[Uu][Pp][Dd][Aa][Tt][Ee]                             RETURN_TOKEN(UPDATE);
[Ss][Ee][Tt]                                         RETURN_TOKEN(SET);
[Bb][Ee][Gg][Ii][Nn]                                 RETURN_TOKEN(TRX_BEGIN);
[Cc][Oo][Mm][Mm][Ii][Tt]                             RETURN_TOKEN(TRX_COMMIT);
[Rr][Oo][Ll][Ll][Bb][Aa][Cc][Kk]                     RETURN_TOKEN(TRX_ROLLBACK);
[Ii][Nn][Tt]                                         RETURN_TOKEN(INT_T);
[Cc][Hh][Aa][Rr]                                     RETURN_TOKEN(STRING_T);
[Tt][Ee][Xx][Tt]                                     RETURN_TOKEN(TEXT_T);
[Ff][Ll][Oo][Aa][Tt]                                 RETURN_TOKEN(FLOAT_T);
[Dd][Aa][Tt][Ee]                                     RETURN_TOKEN(DATE_T);
[Ll][Oo][Aa][Dd]                                     RETURN_TOKEN(LOAD);
[Dd][Aa][Tt][Aa]                                     RETURN_TOKEN(DATA);
[Ii][Nn][Ff][Ii][Ll][Ee]                             RETURN_TOKEN(INFILE);
[Ll][Ii][Kk][Ee]                                     RETURN_TOKEN(LIKE_TOKEN);
[Nn][Oo][Tt]                                         RETURN_TOKEN(NOT_TOKEN);
[Oo][Rr][Dd][Ee][Rr]                                 RETURN_TOKEN(ORDER);
[Bb][Yy]                                             RETURN_TOKEN(BY);
[Aa][Ss][Cc]                                         RETURN_TOKEN(ASC);
[Mm][Ii][Nn]                                         RETURN_TOKEN(MIN_AGGR);
[Mm][Aa][Xx]                                         RETURN_TOKEN(MAX_AGGR);
[Cc][Oo][Uu][Nn][Tt]                                 RETURN_TOKEN(COUNT_AGGR);
[Ss][Uu][Mm]                                         RETURN_TOKEN(SUM_AGGR);
[Aa][Vv][Gg]                                         RETURN_TOKEN(AVG_AGGR);
[Hh][Aa][Vv][Ii][Nn][Gg]							 RETURN_TOKEN(HAVING);
[Gg][Rr][Oo][Uu][Pp]                                 RETURN_TOKEN(GROUP);
[Jj][Oo][Ii][Nn]                                     RETURN_TOKEN(JOIN);
[Ii][Nn][Nn][Ee][Rr]                                 RETURN_TOKEN(INNER);
[Nn][Uu][Ll][Ll]                                     RETURN_TOKEN(NULL_);
[Nn][Uu][Ll][Ll][Aa][Bb][Ll][Ee]                     RETURN_TOKEN(NULLABLE);
[Ii][Ss]                                             RETURN_TOKEN(IS);
[Ii][Nn]											 RETURN_TOKEN(IN_TOKEN);
[Ee][Xx][Ii][Ss][Tt][Ss]							 RETURN_TOKEN(EXISTS_TOKEN);
[Aa][Ss]                                             RETURN_TOKEN(AS);



{ID}                                                 yylval->string=strdup(yytext); RETURN_TOKEN(ID);
"("                                                  RETURN_TOKEN(LBRACE);
")"                                                  RETURN_TOKEN(RBRACE);

","                                                  RETURN_TOKEN(COMMA);
"="                                                  RETURN_TOKEN(EQ);
"<="                                                 RETURN_TOKEN(LE);
"<>"                                                 RETURN_TOKEN(NE);
"!="                                                 RETURN_TOKEN(NE);
"<"                                                  RETURN_TOKEN(LT);
">="                                                 RETURN_TOKEN(GE);
">"                                                  RETURN_TOKEN(GT);
{QUOTE}[\0-\46\50-\177]*{QUOTE}            yylval->string=strdup(yytext); RETURN_TOKEN(SSS);
([\-]?[\+\-\*\/\(\)A-Za-z0-9\._]+[\+\-\*/]+[A-Za-z0-9\._]+[\(\)A-Za-z0-9\.\+\-\*/_]*)|(\-+[\(\)A-Za-z0-9\._]+) yylval->string=strdup(yytext);RETURN_TOKEN(EXPRESSION);

.                                                    printf("Unknown character [%c]\n",yytext[0]); return yytext[0];
%%

void scan_string(const char *str, yyscan_t scanner) {
  yy_switch_to_buffer(yy_scan_string(str, scanner), scanner);
}
