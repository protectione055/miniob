cd src/observer/sql/parser
flex  --header-file=lex.yy.h lex_sql.l
bison -d -b yacc_sql yacc_sql.y
# bison -d --debug --verbose -b yacc_sql yacc_sql.y