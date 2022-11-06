/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.tab.h"
#include "sql/parser/lex.yy.h"
// #include "common/log/log.h" // 包含C++中的头文件

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// 保存查询解析上下文的临时结构体，遇到子查询时被入栈
typedef struct QueryContext {
  Query* ssql;
  size_t select_length;
  size_t condition_length;
  size_t having_condition_length;
  size_t from_length;
  size_t value_length;
  Value values[MAX_NUM];
  Condition conditions[MAX_NUM];
  Condition having_conditions[MAX_NUM];
  CompOp comp;
} QueryContext;

typedef struct ParserContext {
  Query * ssql;
  size_t select_length;
  size_t condition_length;
  size_t having_condition_length;
  size_t from_length;
  size_t value_length;
  Value values[MAX_NUM];
  Condition conditions[MAX_NUM];
  Condition having_conditions[MAX_NUM];
  CompOp comp;
  char id[MAX_NUM];
  QueryContext query_stack[MAX_NUM];
  size_t query_stack_depth;
  Value value_list[MAX_NUM];
  size_t value_list_length;
} ParserContext;


void query_stack_push(ParserContext *context)
{
  printf("query_stack_push: \n BEFORE:\n stack-depth:=%d,\n context->condition_length=%d,\n context->value_length=%d\n", context->query_stack_depth, context->condition_length, context->value_length);
  QueryContext *stack = context->query_stack;
  size_t depth = context->query_stack_depth;
  // 保存当前ParserContext状态
  memcpy(&stack[depth], context, sizeof(QueryContext));

  
  // 初始化ParserContext
  context->ssql = query_create();
  context->query_stack_depth++;
  context->select_length  = 0;
  context->condition_length  = 0;
  context->having_condition_length  = 0;
  context->from_length  = 0;
  context->value_length  = 0;
  printf("AFTER: stack-depth:=%d,\n context->condition_length=%d,\n context->value_length=%d\n", context->query_stack_depth, context->condition_length, context->value_length);
}

void query_stack_pop(ParserContext *context)
{
  printf("query_stack_pop:\n BEFORE:\n stack-depth:=%d,\n context->condition_length=%d,\n context->value_length=%d\n", context->query_stack_depth, context->condition_length, context->value_length);
  QueryContext *stack = context->query_stack;
  size_t depth = context->query_stack_depth - 1;
  memcpy(context, &stack[depth], sizeof(QueryContext));
  
  // 删除栈顶子查询
  stack[depth].ssql = NULL;
  stack[depth].select_length  = 0;
  stack[depth].condition_length  = 0;
  stack[depth].having_condition_length  = 0;
  stack[depth].from_length = 0;
  stack[depth].value_length = 0;
  context->query_stack_depth--;
  printf("AFTER: \n stack-depth:=%d,\n context->condition_length=%d,\n context->value_length=%d\n", context->query_stack_depth, context->condition_length, context->value_length);
}

//获取子串
char *substr(const char *s,int n1,int n2)/*从s中提取下标为n1~n2的字符组成一个新字符串，然后返回这个新串的首地址*/
{
  char *sp = malloc(sizeof(char) * (n2 - n1 + 2));
  int i, j = 0;
  for (i = n1; i <= n2; i++) {
    sp[j++] = s[i];
  }
  sp[j] = 0;
  return sp;
}

void yyerror(yyscan_t scanner, const char *str)
{
  ParserContext *context = (ParserContext *)(yyget_extra(scanner));
  query_reset(context->ssql);
  context->ssql->flag = SCF_ERROR;
  context->condition_length = 0;
  context->from_length = 0;
  context->select_length = 0;
  context->value_length = 0;
  for (size_t i = 0; i < context->ssql->sstr.insertion.tuple_num; i++) {
  	context->ssql->sstr.insertion.value_num[i] = 0;
  }
  context->ssql->sstr.insertion.tuple_num = 0;
  printf("parse sql failed. error=%s\n", str);
}

ParserContext *get_context(yyscan_t scanner)
{
  return (ParserContext *)yyget_extra(scanner);
}

#define CONTEXT get_context(scanner)


#line 189 "yacc_sql.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_YACC_SQL_TAB_H_INCLUDED
# define YY_YY_YACC_SQL_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    SEMICOLON = 258,
    CREATE = 259,
    DROP = 260,
    TABLE = 261,
    TABLES = 262,
    INDEX = 263,
    UNIQUE = 264,
    SELECT = 265,
    DESC = 266,
    SHOW = 267,
    SYNC = 268,
    INSERT = 269,
    DELETE = 270,
    UPDATE = 271,
    LBRACE = 272,
    RBRACE = 273,
    COMMA = 274,
    TRX_BEGIN = 275,
    TRX_COMMIT = 276,
    TRX_ROLLBACK = 277,
    INT_T = 278,
    STRING_T = 279,
    TEXT_T = 280,
    FLOAT_T = 281,
    DATE_T = 282,
    HELP = 283,
    EXIT = 284,
    DOT = 285,
    INTO = 286,
    VALUES = 287,
    FROM = 288,
    WHERE = 289,
    AND = 290,
    OR = 291,
    SET = 292,
    ON = 293,
    LOAD = 294,
    DATA = 295,
    INFILE = 296,
    EQ = 297,
    LT = 298,
    GT = 299,
    LE = 300,
    GE = 301,
    NE = 302,
    LIKE_TOKEN = 303,
    NOT_TOKEN = 304,
    HAVING = 305,
    GROUP = 306,
    ORDER = 307,
    BY = 308,
    INNER = 309,
    JOIN = 310,
    ASC = 311,
    NULLABLE = 312,
    NULL_ = 313,
    IS = 314,
    IN_TOKEN = 315,
    EXISTS_TOKEN = 316,
    AS = 317,
    NUMBER = 318,
    FLOAT = 319,
    ID = 320,
    PATH = 321,
    SSS = 322,
    STAR = 323,
    STRING_V = 324,
    EXPRESSION = 325,
    MIN_AGGR = 326,
    MAX_AGGR = 327,
    COUNT_AGGR = 328,
    SUM_AGGR = 329,
    AVG_AGGR = 330
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 187 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
  char *position;
  Query*query;

#line 328 "yacc_sql.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner);

#endif /* !YY_YY_YACC_SQL_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   448

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  76
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  58
/* YYNRULES -- Number of rules.  */
#define YYNRULES  172
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  415

#define YYUNDEFTOK  2
#define YYMAXUTOK   330


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   225,   225,   227,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   252,   257,   262,   268,   274,   280,   286,   292,
     298,   305,   312,   322,   325,   330,   331,   337,   344,   353,
     355,   359,   366,   373,   382,   385,   386,   387,   388,   391,
     392,   393,   397,   398,   399,   402,   411,   428,   433,   435,
     437,   439,   444,   447,   450,   454,   460,   470,   478,   482,
     487,   488,   490,   512,   517,   522,   527,   534,   541,   548,
     555,   563,   564,   565,   566,   567,   569,   571,   578,   585,
     594,   603,   612,   620,   621,   623,   624,   629,   635,   637,
     644,   652,   654,   657,   661,   663,   668,   677,   687,   698,
     717,   719,   723,   725,   728,   733,   754,   774,   794,   816,
     837,   858,   876,   887,   898,   911,   923,   935,   944,   953,
     961,   973,   986,  1000,  1013,  1026,  1039,  1056,  1070,  1077,
    1082,  1083,  1087,  1089,  1094,  1106,  1118,  1130,  1143,  1156,
    1169,  1183,  1185,  1190,  1195,  1201,  1203,  1208,  1216,  1217,
    1218,  1219,  1220,  1221,  1222,  1223,  1225,  1226,  1227,  1228,
    1229,  1230,  1234
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SEMICOLON", "CREATE", "DROP", "TABLE",
  "TABLES", "INDEX", "UNIQUE", "SELECT", "DESC", "SHOW", "SYNC", "INSERT",
  "DELETE", "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN",
  "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T", "TEXT_T", "FLOAT_T",
  "DATE_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM", "WHERE",
  "AND", "OR", "SET", "ON", "LOAD", "DATA", "INFILE", "EQ", "LT", "GT",
  "LE", "GE", "NE", "LIKE_TOKEN", "NOT_TOKEN", "HAVING", "GROUP", "ORDER",
  "BY", "INNER", "JOIN", "ASC", "NULLABLE", "NULL_", "IS", "IN_TOKEN",
  "EXISTS_TOKEN", "AS", "NUMBER", "FLOAT", "ID", "PATH", "SSS", "STAR",
  "STRING_V", "EXPRESSION", "MIN_AGGR", "MAX_AGGR", "COUNT_AGGR",
  "SUM_AGGR", "AVG_AGGR", "$accept", "commands", "command", "exit", "help",
  "sync", "begin", "commit", "rollback", "drop_table", "show_tables",
  "show_index", "desc_table", "create_index", "optional_unique",
  "create_index_attrs", "drop_index", "create_table", "attr_def_list",
  "attr_def", "number", "type", "order_type", "nullable", "ID_get",
  "insert", "insert_tuple", "insert_tuple_list", "value_list", "value",
  "delete", "update", "update_attr", "update_attr_list", "select",
  "select_attr", "aggregate", "attr_list", "alias", "group_by",
  "group_key_list", "rel_list", "join_cond_list", "join_cond", "where",
  "condition_list", "condition", "sub_query", "subquery_value_list",
  "sub_query_init", "having", "having_condition_list", "having_condition",
  "order", "order_attr", "order_list", "comOp", "load_data", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330
};
# endif

#define YYPACT_NINF (-314)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -314,   303,  -314,    40,   170,   203,   -40,   177,    32,    51,
      56,    64,   130,   146,   153,   183,   187,   157,  -314,  -314,
    -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,
    -314,  -314,  -314,  -314,  -314,  -314,  -314,   136,  -314,   201,
     149,   151,    -9,   133,   133,  -314,  -314,  -314,  -314,  -314,
     184,   206,   222,   232,   214,  -314,   171,   186,   212,  -314,
    -314,  -314,  -314,  -314,   217,   242,   197,   257,   260,   199,
     204,   251,   251,   251,   207,   -17,  -314,  -314,   219,   247,
     248,   226,   225,   228,   261,  -314,  -314,   133,  -314,   159,
    -314,  -314,  -314,   133,    97,   282,   298,   288,   292,   307,
     264,   293,   289,  -314,   302,     5,   263,   251,    -7,   133,
     305,     3,   133,    44,   133,  -314,   124,   311,   333,  -314,
    -314,  -314,  -314,  -314,  -314,  -314,   -22,  -314,   295,  -314,
    -314,  -314,  -314,    22,  -314,   133,   195,   127,   195,   203,
     328,  -314,    30,   226,   248,   341,   228,   330,  -314,  -314,
      69,  -314,  -314,    42,   343,  -314,   296,   251,   251,    82,
     304,   312,   248,   251,   345,   350,   251,   355,   288,   373,
    -314,  -314,  -314,  -314,  -314,   313,   195,   195,    78,   292,
     292,  -314,    -4,   344,  -314,  -314,  -314,   293,   376,   315,
     302,   378,   324,  -314,  -314,   320,  -314,   319,   133,  -314,
    -314,   101,   367,   133,   321,   335,  -314,   133,   133,  -314,
     124,   370,   311,  -314,   133,    16,   135,    48,   133,  -314,
     127,   127,    55,  -314,   325,  -314,  -314,   386,  -314,  -314,
    -314,  -314,   374,   372,   251,   133,   329,   133,     3,   133,
     340,   346,   251,   251,   355,  -314,  -314,   195,     9,   133,
     133,  -314,  -314,  -314,    68,   133,  -314,   331,  -314,  -314,
    -314,  -314,   334,  -314,   133,  -314,    69,   336,   377,  -314,
     251,   380,   251,  -314,   362,   337,   351,   353,  -314,  -314,
    -314,    58,   355,  -314,  -314,   342,  -314,  -314,   133,   133,
       3,  -314,   372,   402,  -314,   133,  -314,    94,     6,  -314,
     347,    95,   403,   379,   133,  -314,  -314,  -314,   390,   133,
    -314,  -314,   248,  -314,  -314,   251,   381,   195,   375,  -314,
     348,  -314,   395,    70,   398,   382,  -314,   354,  -314,  -314,
    -314,   346,  -314,   356,   155,    94,     3,     4,   357,  -314,
     358,   397,    86,    95,  -314,   133,   353,   133,   388,  -314,
     375,  -314,   395,     7,   133,   359,  -314,   116,   407,   127,
    -314,   408,   195,   363,  -314,  -314,   364,   395,   397,    73,
     133,   365,   133,  -314,  -314,   223,   133,     4,  -314,  -314,
     366,   397,   195,   409,   195,   404,  -314,  -314,   395,   133,
    -314,   231,   133,   124,   368,  -314,   397,    75,  -314,   195,
    -314,   133,  -314,   371,  -314,   308,  -314,   133,    76,  -314,
    -314,   383,  -314,   133,  -314
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    21,
      20,    15,    16,    17,    18,     9,    10,    11,    12,    13,
      14,     8,     5,     7,     6,     4,    19,     0,    34,     0,
       0,     0,    93,    93,    93,    81,    82,    83,    84,    85,
       0,     0,     0,     0,     0,    24,     0,     0,     0,    25,
      26,    27,    23,    22,     0,     0,     0,     0,     0,     0,
       0,    86,    86,    86,     0,     0,    31,    29,     0,     0,
     110,     0,     0,     0,     0,    28,    37,    93,    94,     0,
      74,    73,    80,    93,     0,     0,     0,     0,     0,     0,
       0,    70,     0,    55,    39,     0,     0,    86,    93,    93,
       0,   101,    93,     0,    93,    30,     0,    58,     0,   158,
     159,   160,   161,   162,   163,   164,     0,    65,   166,   168,
     171,    62,    63,    93,    64,    93,     0,   112,     0,     0,
       0,    66,     0,     0,   110,     0,     0,     0,    45,    46,
      52,    47,    48,    52,     0,    75,     0,    86,    86,     0,
       0,     0,   110,    86,     0,     0,    86,    60,     0,     0,
     139,   165,   169,   170,   167,     0,     0,     0,     0,     0,
       0,   111,     0,     0,   129,    68,    69,    70,     0,     0,
      39,     0,     0,    54,    43,     0,    42,     0,    93,    87,
      92,     0,     0,    93,     0,   151,    78,    93,    93,    76,
       0,     0,    58,    56,    93,     0,     0,    93,    93,   116,
     112,   112,    93,   126,     0,    71,    67,     0,    40,    38,
      53,    44,     0,    35,    86,    93,     0,    93,   101,    93,
       0,    95,    86,    86,    60,    57,    59,     0,     0,    93,
      93,   115,   122,   127,    93,    93,   131,     0,   118,   130,
     113,   114,     0,   124,    93,   172,    52,     0,     0,    88,
      86,     0,    86,   102,     0,     0,     0,   140,    79,    77,
      61,     0,    60,   117,   132,     0,   133,   136,    93,    93,
     101,    41,    35,     0,    90,    93,    89,     0,    49,   152,
       0,     0,     0,     0,    93,   119,   123,   128,     0,    93,
     120,   125,   110,    36,    32,    86,     0,     0,   104,    51,
       0,    50,   155,    93,     0,   142,    72,     0,   134,   138,
     135,    95,    91,     0,     0,     0,   101,    49,     0,   153,
       0,    98,     0,     0,   141,    93,   140,    93,     0,   106,
     104,   103,   155,    49,    93,     0,    96,     0,     0,   112,
     121,     0,     0,     0,   105,   154,     0,   155,    98,    93,
      93,     0,    93,   143,   137,     0,    93,    49,   156,    97,
       0,    98,     0,     0,     0,     0,   107,   108,   155,    93,
      99,     0,    93,     0,     0,   157,    98,    93,   145,     0,
     144,    93,   100,     0,   147,     0,   109,    93,    93,   146,
     148,     0,   149,    93,   150
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,  -314,
    -314,  -314,  -314,  -314,  -314,   128,  -314,  -314,   245,   286,
    -314,  -314,  -313,  -146,  -314,  -314,   269,   227,  -224,  -108,
    -314,  -314,   297,   254,  -314,   299,   -86,   -62,   -43,   111,
    -257,  -222,    93,   109,  -135,  -215,    24,  -128,   164,  -314,
     100,  -314,   104,  -314,  -314,  -311,  -134,  -314
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    39,   268,    30,    31,   147,   104,
     232,   153,   322,   194,   105,    32,   117,   169,   211,   136,
      33,    34,   101,   144,    35,    50,    51,    90,    71,   277,
     356,   162,   336,   318,    99,   181,   137,   138,   253,   139,
     302,   344,   325,   241,   299,   339,   140,    36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      72,    73,   178,   110,   182,   260,   261,   196,   167,   188,
      91,    92,   184,   118,   186,   319,   273,   319,   319,   170,
     280,    69,   160,   156,   352,    52,   171,   205,   148,   149,
     150,   151,   152,   248,   185,    55,   320,   366,   172,   173,
     367,   365,   215,   216,   107,   155,    37,   118,    94,    38,
     111,    95,   175,    70,   223,    70,   378,   161,   308,   195,
     321,   222,   321,   321,   388,   157,   158,   127,   312,   163,
     219,   166,   131,   132,   127,   248,   134,   395,   257,   131,
     132,   249,    56,   134,    70,   262,   250,   252,   127,    57,
     176,   192,   177,   131,   132,   199,   200,   134,   285,   193,
     340,   206,   244,   380,   209,   403,   411,   251,   256,   164,
      70,   379,   165,   281,   351,   112,   127,    70,   192,   235,
     291,   131,   132,   303,   390,   134,   193,   113,   304,    58,
      70,   236,    70,    59,   370,    70,   127,    70,    70,   402,
     282,   131,   132,   217,   373,   134,   371,   201,   218,    60,
     202,   357,   127,   306,   358,   234,    61,   131,   132,   316,
     238,   134,   179,   180,   242,   243,    45,    46,    47,    48,
      49,   247,   269,   305,   258,   259,    40,   331,    41,   263,
     278,   279,   127,   334,    53,    54,    62,   131,   132,   317,
      63,   134,   270,   127,   272,    70,   274,    64,   131,   132,
     254,    65,   134,   220,   221,   255,   283,   284,   294,    66,
     296,   286,   287,   127,    67,   324,    68,    74,   131,   132,
     348,   290,   134,    75,   108,    76,   349,   317,   375,   109,
      45,    46,    47,    48,    49,    77,    79,   119,   120,   121,
     122,   123,   124,   125,   126,   310,   311,    78,   391,    81,
     393,    80,   315,   332,   128,   129,   130,   324,    82,    83,
      85,   328,    84,    86,    87,   405,   330,   386,    42,    88,
      89,    43,    93,    44,    45,    46,    47,    48,    49,    97,
     341,   127,    98,   398,    96,   400,   131,   132,   385,   127,
     134,   100,   102,   103,   131,   132,   397,   409,   134,   106,
     114,   115,   360,     2,   362,   116,   142,     3,     4,   118,
     141,   368,   143,     5,     6,     7,     8,     9,    10,    11,
     145,   146,   159,    12,    13,    14,   381,   382,   154,   384,
     168,    15,    16,   387,   119,   120,   121,   122,   123,   124,
     125,   126,    17,   170,   174,   118,   396,   189,   191,   399,
     127,   128,   129,   130,   404,   131,   132,   133,   406,   134,
     197,   198,   135,   207,   410,   412,   127,   204,   208,   203,
     414,   131,   132,   408,   210,   134,   213,   224,   214,   226,
     227,   229,   230,   231,   233,   237,   239,   240,   245,   265,
     264,   267,   266,   275,   271,   293,   288,   276,   295,   289,
     297,   292,   298,   301,   300,   314,   326,   309,   329,   327,
     335,   333,   323,   337,   338,   342,   355,   343,   363,   345,
     313,   347,   353,   354,   369,   372,   374,   392,   376,   377,
     383,   389,   190,   401,   394,   228,   407,   212,   183,   246,
     187,   225,   346,   364,   350,   307,   361,   359,   413
};

static const yytype_int16 yycheck[] =
{
      43,    44,   136,    89,   138,   220,   221,   153,   116,   144,
      72,    73,   140,    17,   142,    11,   238,    11,    11,    10,
     244,    30,    19,    30,   337,    65,    48,   162,    23,    24,
      25,    26,    27,    17,   142,     3,    30,    30,    60,    61,
     353,   352,   176,   177,    87,   107,     6,    17,    65,     9,
      93,    68,    30,    62,   182,    62,   367,    54,   282,    17,
      56,    65,    56,    56,   377,   108,   109,    58,   290,   112,
     178,   114,    63,    64,    58,    17,    67,   388,    30,    63,
      64,    65,    31,    67,    62,    30,    70,   215,    58,    33,
     133,    49,   135,    63,    64,   157,   158,    67,    30,    57,
      30,   163,   210,    30,   166,    30,    30,   215,   216,    65,
      62,   368,    68,   247,   336,    18,    58,    62,    49,    18,
     266,    63,    64,    65,   381,    67,    57,    30,    70,    65,
      62,    30,    62,     3,    18,    62,    58,    62,    62,   396,
     248,    63,    64,    65,   359,    67,    30,    65,    70,     3,
      68,    65,    58,   281,    68,   198,     3,    63,    64,    65,
     203,    67,    35,    36,   207,   208,    71,    72,    73,    74,
      75,   214,   234,   281,   217,   218,     6,   312,     8,   222,
     242,   243,    58,   317,     7,     8,     3,    63,    64,   297,
       3,    67,   235,    58,   237,    62,   239,    40,    63,    64,
      65,    65,    67,   179,   180,    70,   249,   250,   270,     8,
     272,   254,   255,    58,    65,   301,    65,    33,    63,    64,
      65,   264,    67,    17,    65,     3,   334,   335,   362,    70,
      71,    72,    73,    74,    75,     3,    65,    42,    43,    44,
      45,    46,    47,    48,    49,   288,   289,    33,   382,    37,
     384,    65,   295,   315,    59,    60,    61,   343,    41,    17,
       3,   304,    65,     3,    65,   399,   309,   375,    65,    65,
      19,    68,    65,    70,    71,    72,    73,    74,    75,    32,
     323,    58,    34,   391,    65,   393,    63,    64,    65,    58,
      67,    65,    67,    65,    63,    64,    65,   405,    67,    38,
      18,     3,   345,     0,   347,    17,    42,     4,     5,    17,
       3,   354,    19,    10,    11,    12,    13,    14,    15,    16,
      31,    19,    17,    20,    21,    22,   369,   370,    65,   372,
      19,    28,    29,   376,    42,    43,    44,    45,    46,    47,
      48,    49,    39,    10,    49,    17,   389,     6,    18,   392,
      58,    59,    60,    61,   397,    63,    64,    65,   401,    67,
      17,    65,    70,    18,   407,   408,    58,    55,    18,    65,
     413,    63,    64,    65,    19,    67,     3,    33,    65,     3,
      65,     3,    58,    63,    65,    18,    65,    52,    18,     3,
      65,    19,    18,    53,    65,    18,    65,    51,    18,    65,
      38,    65,    65,    50,    53,     3,     3,    65,    18,    30,
      35,    30,    65,    65,    19,    17,    19,    35,    30,    65,
     292,    65,    65,    65,    65,    18,    18,    18,    65,    65,
      65,    65,   146,    65,    30,   190,    65,   168,   139,   212,
     143,   187,   331,   350,   335,   281,   346,   343,    65
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    77,     0,     4,     5,    10,    11,    12,    13,    14,
      15,    16,    20,    21,    22,    28,    29,    39,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      92,    93,   101,   106,   107,   110,   133,     6,     9,    90,
       6,     8,    65,    68,    70,    71,    72,    73,    74,    75,
     111,   112,    65,     7,     8,     3,    31,    33,    65,     3,
       3,     3,     3,     3,    40,    65,     8,    65,    65,    30,
      62,   114,   114,   114,    33,    17,     3,     3,    33,    65,
      65,    37,    41,    17,    65,     3,     3,    65,    65,    19,
     113,   113,   113,    65,    65,    68,    65,    32,    34,   120,
      65,   108,    67,    65,    95,   100,    38,   114,    65,    70,
     112,   114,    18,    30,    18,     3,    17,   102,    17,    42,
      43,    44,    45,    46,    47,    48,    49,    58,    59,    60,
      61,    63,    64,    65,    67,    70,   105,   122,   123,   125,
     132,     3,    42,    19,   109,    31,    19,    94,    23,    24,
      25,    26,    27,    97,    65,   113,    30,   114,   114,    17,
      19,    54,   117,   114,    65,    68,   114,   105,    19,   103,
      10,    48,    60,    61,    49,    30,   114,   114,   132,    35,
      36,   121,   132,   111,   123,   105,   123,   108,   120,     6,
      95,    18,    49,    57,    99,    17,    99,    17,    65,   113,
     113,    65,    68,    65,    55,   120,   113,    18,    18,   113,
      19,   104,   102,     3,    65,   132,   132,    65,    70,   105,
     122,   122,    65,   123,    33,   109,     3,    65,    94,     3,
      58,    63,    96,    65,   114,    18,    30,    18,   114,    65,
      52,   129,   114,   114,   105,    18,   103,   114,    17,    65,
      70,   105,   123,   124,    65,    70,   105,    30,   114,   114,
     121,   121,    30,   114,    65,     3,    18,    19,    91,   113,
     114,    65,   114,   117,   114,    53,    51,   115,   113,   113,
     104,   132,   105,   114,   114,    30,   114,   114,    65,    65,
     114,    99,    65,    18,   113,    18,   113,    38,    65,   130,
      53,    50,   126,    65,    70,   105,   123,   124,   104,    65,
     114,   114,   117,    91,     3,   114,    65,   105,   119,    11,
      30,    56,    98,    65,   112,   128,     3,    30,   114,    18,
     114,   120,   113,    30,   132,    35,   118,    65,    19,   131,
      30,   114,    17,    35,   127,    65,   115,    65,    65,   105,
     119,   117,    98,    65,    65,    19,   116,    65,    68,   128,
     114,   126,   114,    30,   118,   131,    30,    98,   114,    65,
      18,    30,    18,   121,    18,   132,    65,    65,   131,   116,
      30,   114,   114,    65,   114,    65,   105,   114,    98,    65,
     116,   132,    18,   132,    30,   131,   114,    65,   105,   114,
     105,    65,   116,    30,   114,   132,   114,    65,    65,   105,
     114,    30,   114,    65,   114
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    76,    77,    77,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    90,    91,    91,    92,    93,    94,
      94,    95,    95,    95,    96,    97,    97,    97,    97,    98,
      98,    98,    99,    99,    99,   100,   101,   102,   103,   103,
     104,   104,   105,   105,   105,   105,   106,   107,   108,   108,
     109,   109,   110,   111,   111,   111,   111,   111,   111,   111,
     111,   112,   112,   112,   112,   112,   113,   113,   113,   113,
     113,   113,   113,   114,   114,   115,   115,   115,   116,   116,
     116,   117,   117,   117,   118,   118,   119,   119,   119,   119,
     120,   120,   121,   121,   121,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   123,   124,   125,
     126,   126,   127,   127,   128,   128,   128,   128,   128,   128,
     128,   129,   129,   130,   130,   131,   131,   131,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   133
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     4,     3,
       5,     3,    11,     0,     1,     0,     3,     4,     8,     0,
       3,     6,     3,     3,     1,     1,     1,     1,     1,     0,
       1,     1,     0,     2,     1,     1,     7,     4,     0,     3,
       0,     3,     1,     1,     1,     1,     5,     7,     3,     3,
       0,     3,    11,     3,     3,     5,     6,     8,     6,     8,
       3,     1,     1,     1,     1,     1,     0,     4,     6,     7,
       7,     9,     4,     0,     2,     0,     5,     7,     0,     4,
       6,     0,     4,     8,     0,     3,     3,     6,     6,     9,
       0,     3,     0,     3,     3,     4,     3,     5,     4,     6,
       6,     9,     4,     6,     4,     6,     3,     4,     6,     2,
       4,     4,     5,     5,     7,     7,     5,    10,     4,     2,
       0,     3,     0,     3,     7,     7,     9,     8,    10,    10,
      12,     0,     3,     3,     5,     0,     4,     6,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     2,     1,     2,
       2,     1,     8
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule, void *scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *scanner)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 22:
#line 252 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1804 "yacc_sql.tab.c"
    break;

  case 23:
#line 257 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1812 "yacc_sql.tab.c"
    break;

  case 24:
#line 262 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1820 "yacc_sql.tab.c"
    break;

  case 25:
#line 268 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1828 "yacc_sql.tab.c"
    break;

  case 26:
#line 274 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1836 "yacc_sql.tab.c"
    break;

  case 27:
#line 280 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1844 "yacc_sql.tab.c"
    break;

  case 28:
#line 286 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1853 "yacc_sql.tab.c"
    break;

  case 29:
#line 292 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1861 "yacc_sql.tab.c"
    break;

  case 30:
#line 298 "yacc_sql.y"
                                 {
      CONTEXT->ssql->flag = SCF_SHOW_INDEX;
	  show_index_init(&CONTEXT->ssql->sstr.show_index, (yyvsp[-1].string));
    }
#line 1870 "yacc_sql.tab.c"
    break;

  case 31:
#line 305 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1879 "yacc_sql.tab.c"
    break;

  case 32:
#line 313 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, (yyvsp[-3].string));
		}
#line 1889 "yacc_sql.tab.c"
    break;

  case 33:
#line 322 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 0);
		}
#line 1897 "yacc_sql.tab.c"
    break;

  case 34:
#line 326 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 1);
		}
#line 1905 "yacc_sql.tab.c"
    break;

  case 36:
#line 332 "yacc_sql.y"
                {
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, (yyvsp[-1].string));
		}
#line 1913 "yacc_sql.tab.c"
    break;

  case 37:
#line 338 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1922 "yacc_sql.tab.c"
    break;

  case 38:
#line 345 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1934 "yacc_sql.tab.c"
    break;

  case 40:
#line 355 "yacc_sql.y"
                                   {    }
#line 1940 "yacc_sql.tab.c"
    break;

  case 41:
#line 360 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1951 "yacc_sql.tab.c"
    break;

  case 42:
#line 367 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1962 "yacc_sql.tab.c"
    break;

  case 43:
#line 374 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, CHARS, 4096, (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1973 "yacc_sql.tab.c"
    break;

  case 44:
#line 382 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1979 "yacc_sql.tab.c"
    break;

  case 45:
#line 385 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1985 "yacc_sql.tab.c"
    break;

  case 46:
#line 386 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1991 "yacc_sql.tab.c"
    break;

  case 47:
#line 387 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1997 "yacc_sql.tab.c"
    break;

  case 48:
#line 388 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 2003 "yacc_sql.tab.c"
    break;

  case 49:
#line 391 "yacc_sql.y"
                { (yyval.number)=1; }
#line 2009 "yacc_sql.tab.c"
    break;

  case 50:
#line 392 "yacc_sql.y"
                  { (yyval.number)=1; }
#line 2015 "yacc_sql.tab.c"
    break;

  case 51:
#line 393 "yacc_sql.y"
              { (yyval.number)=0; }
#line 2021 "yacc_sql.tab.c"
    break;

  case 52:
#line 397 "yacc_sql.y"
                    { (yyval.number)=0; }
#line 2027 "yacc_sql.tab.c"
    break;

  case 53:
#line 398 "yacc_sql.y"
                          { (yyval.number)=0; }
#line 2033 "yacc_sql.tab.c"
    break;

  case 54:
#line 399 "yacc_sql.y"
                   { (yyval.number)=1; }
#line 2039 "yacc_sql.tab.c"
    break;

  case 55:
#line 403 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 2048 "yacc_sql.tab.c"
    break;

  case 56:
#line 412 "yacc_sql.y"
                {
			// CONTEXT->values[CONTEXT->value_length++] = *$6;

			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			// CONTEXT->ssql->sstr.insertion.relation_name = $3;
			// CONTEXT->ssql->sstr.insertion.value_num = CONTEXT->value_length;
			// for(i = 0; i < CONTEXT->value_length; i++){
			// 	CONTEXT->ssql->sstr.insertion.values[i] = CONTEXT->values[i];
      // }
			inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-4].string));

      //临时变量清零
      CONTEXT->value_length=0;
    }
#line 2067 "yacc_sql.tab.c"
    break;

  case 57:
#line 428 "yacc_sql.y"
                                       {
		inserts_create_tuple(&CONTEXT->ssql->sstr.insertion, CONTEXT->values, CONTEXT->value_length);
		CONTEXT->value_length = 0;
	}
#line 2076 "yacc_sql.tab.c"
    break;

  case 59:
#line 435 "yacc_sql.y"
                                               {}
#line 2082 "yacc_sql.tab.c"
    break;

  case 61:
#line 439 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 2090 "yacc_sql.tab.c"
    break;

  case 62:
#line 444 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 2098 "yacc_sql.tab.c"
    break;

  case 63:
#line 447 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 2106 "yacc_sql.tab.c"
    break;

  case 64:
#line 450 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 2115 "yacc_sql.tab.c"
    break;

  case 65:
#line 454 "yacc_sql.y"
               {
		value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
	}
#line 2123 "yacc_sql.tab.c"
    break;

  case 66:
#line 461 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 2135 "yacc_sql.tab.c"
    break;

  case 67:
#line 471 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-5].string), CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 2145 "yacc_sql.tab.c"
    break;

  case 68:
#line 478 "yacc_sql.y"
                    {
		updates_attr_init(&CONTEXT->ssql->sstr.update, (yyvsp[-2].string), &CONTEXT->values[0]);
		CONTEXT->value_length = 0;
	}
#line 2154 "yacc_sql.tab.c"
    break;

  case 69:
#line 482 "yacc_sql.y"
                      {
		Query *query = (yyvsp[0].query);
		updates_attr_init_with_subquery(&CONTEXT->ssql->sstr.update, (yyvsp[-2].string), &query->sstr.selection);
	}
#line 2163 "yacc_sql.tab.c"
    break;

  case 71:
#line 488 "yacc_sql.y"
                                                         {}
#line 2169 "yacc_sql.tab.c"
    break;

  case 72:
#line 491 "yacc_sql.y"
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-7].string), (yyvsp[-6].string));

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			selects_append_having_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->having_conditions, CONTEXT->having_condition_length);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			CONTEXT->ssql->sstr.selection.is_subquery=0;
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
#line 2192 "yacc_sql.tab.c"
    break;

  case 73:
#line 512 "yacc_sql.y"
                         {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*", (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2202 "yacc_sql.tab.c"
    break;

  case 74:
#line 517 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2212 "yacc_sql.tab.c"
    break;

  case 75:
#line 522 "yacc_sql.y"
                                    {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2222 "yacc_sql.tab.c"
    break;

  case 76:
#line 527 "yacc_sql.y"
                                                       {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*", (yyvsp[-1].string));
			attr.aggr_type = (yyvsp[-5].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2234 "yacc_sql.tab.c"
    break;

  case 77:
#line 534 "yacc_sql.y"
                                                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-5].string), "*", (yyvsp[-1].string));
			attr.aggr_type = (yyvsp[-7].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2246 "yacc_sql.tab.c"
    break;

  case 78:
#line 541 "yacc_sql.y"
                                                     {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-3].string), (yyvsp[-1].string));
			attr.aggr_type = (yyvsp[-5].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2258 "yacc_sql.tab.c"
    break;

  case 79:
#line 548 "yacc_sql.y"
                                                            {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-5].string), (yyvsp[-3].string), (yyvsp[-1].string));
			attr.aggr_type = (yyvsp[-7].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2270 "yacc_sql.tab.c"
    break;

  case 80:
#line 555 "yacc_sql.y"
                                     {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].string));
			attr.is_complex = 1;
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2281 "yacc_sql.tab.c"
    break;

  case 81:
#line 563 "yacc_sql.y"
               {(yyval.number) = MIN;}
#line 2287 "yacc_sql.tab.c"
    break;

  case 82:
#line 564 "yacc_sql.y"
                   {(yyval.number) = MAX;}
#line 2293 "yacc_sql.tab.c"
    break;

  case 83:
#line 565 "yacc_sql.y"
                     {(yyval.number) = COUNT;}
#line 2299 "yacc_sql.tab.c"
    break;

  case 84:
#line 566 "yacc_sql.y"
                   {(yyval.number) = SUM;}
#line 2305 "yacc_sql.tab.c"
    break;

  case 85:
#line 567 "yacc_sql.y"
                   {(yyval.number) = AVG;}
#line 2311 "yacc_sql.tab.c"
    break;

  case 87:
#line 571 "yacc_sql.y"
                               {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2323 "yacc_sql.tab.c"
    break;

  case 88:
#line 578 "yacc_sql.y"
                                      {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2335 "yacc_sql.tab.c"
    break;

  case 89:
#line 585 "yacc_sql.y"
                                                             {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*", (yyvsp[-1].string));
			attr.aggr_type = (yyvsp[-5].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2349 "yacc_sql.tab.c"
    break;

  case 90:
#line 594 "yacc_sql.y"
                                                           {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-3].string), (yyvsp[-1].string));
			attr.aggr_type = (yyvsp[-5].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2363 "yacc_sql.tab.c"
    break;

  case 91:
#line 603 "yacc_sql.y"
                                                                  {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-5].string), (yyvsp[-3].string), (yyvsp[-1].string));
			attr.aggr_type = (yyvsp[-7].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2377 "yacc_sql.tab.c"
    break;

  case 92:
#line 612 "yacc_sql.y"
                                           {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].string));
			attr.is_complex = 1;
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
      }
#line 2388 "yacc_sql.tab.c"
    break;

  case 93:
#line 620 "yacc_sql.y"
                {(yyval.string) = "";}
#line 2394 "yacc_sql.tab.c"
    break;

  case 94:
#line 621 "yacc_sql.y"
                {(yyval.string) = (yyvsp[0].string);}
#line 2400 "yacc_sql.tab.c"
    break;

  case 96:
#line 624 "yacc_sql.y"
                                       {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2410 "yacc_sql.tab.c"
    break;

  case 97:
#line 629 "yacc_sql.y"
                                                  {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2420 "yacc_sql.tab.c"
    break;

  case 99:
#line 637 "yacc_sql.y"
                                    {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2432 "yacc_sql.tab.c"
    break;

  case 100:
#line 644 "yacc_sql.y"
                                           {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2444 "yacc_sql.tab.c"
    break;

  case 102:
#line 654 "yacc_sql.y"
                              {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-2].string), (yyvsp[-1].string));
		  }
#line 2452 "yacc_sql.tab.c"
    break;

  case 103:
#line 657 "yacc_sql.y"
                                                               {	
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-5].string), (yyvsp[-4].string));
	}
#line 2460 "yacc_sql.tab.c"
    break;

  case 105:
#line 663 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
		  }
#line 2468 "yacc_sql.tab.c"
    break;

  case 106:
#line 669 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 2481 "yacc_sql.tab.c"
    break;

  case 107:
#line 678 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string), (yyvsp[-2].string));
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    	}
#line 2495 "yacc_sql.tab.c"
    break;

  case 108:
#line 688 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-3].string), (yyvsp[-1].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;		
    	}
#line 2510 "yacc_sql.tab.c"
    break;

  case 109:
#line 699 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-8].string), (yyvsp[-6].string), (yyvsp[-5].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-3].string), (yyvsp[-1].string), (yyvsp[0].string));

			// 判断是否同一表中的两个属性。若否，条件加入join中
			if (strcmp((yyvsp[-8].string), (yyvsp[-3].string)) != 0){
				Condition condition;
				condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
				selects_append_joincond(&CONTEXT->ssql->sstr.selection, condition);
			} else {
				Condition condition;
				condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
				CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			}
    	}
#line 2532 "yacc_sql.tab.c"
    break;

  case 111:
#line 719 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2540 "yacc_sql.tab.c"
    break;

  case 113:
#line 725 "yacc_sql.y"
                                   {
				CONTEXT->ssql->sstr.selection.condition_mode = AND_MODE;
			}
#line 2548 "yacc_sql.tab.c"
    break;

  case 114:
#line 728 "yacc_sql.y"
                                      {
				CONTEXT->ssql->sstr.selection.condition_mode = OR_MODE;
			}
#line 2556 "yacc_sql.tab.c"
    break;

  case 115:
#line 734 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-3].string), (yyvsp[-2].string));

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name = NULL;
			// $$->left_attr.attribute_name= $1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
#line 2581 "yacc_sql.tab.c"
    break;

  case 116:
#line 755 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
#line 2605 "yacc_sql.tab.c"
    break;

  case 117:
#line 775 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-4].string), (yyvsp[-3].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[-1].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;

		}
#line 2629 "yacc_sql.tab.c"
    break;

  case 118:
#line 795 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[-1].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp=CONTEXT->comp;
			
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;
		
		}
#line 2655 "yacc_sql.tab.c"
    break;

  case 119:
#line 817 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string), (yyvsp[-2].string));
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp=CONTEXT->comp;
			// $$->right_is_attr = 0;   //属性值
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=NULL;
			// $$->right_value =*$5;			
							
    	}
#line 2680 "yacc_sql.tab.c"
    break;

  case 120:
#line 838 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-3].string), (yyvsp[-1].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;//属性值
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;//属性
			// $$->right_attr.relation_name = $3;
			// $$->right_attr.attribute_name = $5;
									
    	}
#line 2705 "yacc_sql.tab.c"
    break;

  case 121:
#line 859 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-8].string), (yyvsp[-6].string), (yyvsp[-5].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-3].string), (yyvsp[-1].string), (yyvsp[0].string));

			// 判断是否同一表中的两个属性。若否，条件加入join中
			// if (strcmp($1, $6) != 0){
			// 	Condition condition;
			// 	condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			// 	selects_append_joincond(&CONTEXT->ssql->sstr.selection, condition);
			// } else {
				Condition condition;
				condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
				CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// }
    	}
#line 2727 "yacc_sql.tab.c"
    break;

  case 122:
#line 877 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-3].string), (yyvsp[-2].string));
		Query *right_subquery = (yyvsp[0].query);

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
#line 2742 "yacc_sql.tab.c"
    break;

  case 123:
#line 888 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string), (yyvsp[-2].string));
		Query *right_subquery = (yyvsp[0].query);

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
#line 2757 "yacc_sql.tab.c"
    break;

  case 124:
#line 899 "yacc_sql.y"
        {
		RelAttr right_attr;
		relation_attr_init(&right_attr, NULL, (yyvsp[-1].string), (yyvsp[0].string));
		Query *left_subquery = (yyvsp[-3].query);

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, SUB_QUERY, NULL, NULL, &left_subquery->sstr.selection, ATTR, &right_attr, NULL, NULL);
		printf("1condition_length=%d\n", CONTEXT->condition_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(left_subquery);
		printf("2condition_length=%d\n", CONTEXT->condition_length);
	}
#line 2774 "yacc_sql.tab.c"
    break;

  case 125:
#line 912 "yacc_sql.y"
        {
		RelAttr right_attr;
		relation_attr_init(&right_attr, (yyvsp[-3].string), (yyvsp[-1].string), (yyvsp[0].string));
		Query *left_subquery = (yyvsp[-5].query);
		
		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, SUB_QUERY, NULL, NULL, &left_subquery->sstr.selection, ATTR, &right_attr, NULL, NULL);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(left_subquery);
		printf("s op t.a condition_length=%d\n", CONTEXT->condition_length);
	}
#line 2790 "yacc_sql.tab.c"
    break;

  case 126:
#line 924 "yacc_sql.y"
        {
		Query *left_subquery = (yyvsp[-2].query);
		Query *right_subquery = (yyvsp[0].query);
		
		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, SUB_QUERY, NULL, NULL, &left_subquery->sstr.selection, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(left_subquery);
		free(right_subquery);
		printf("s op s condition_length=%d\n", CONTEXT->condition_length);
	}
#line 2806 "yacc_sql.tab.c"
    break;

  case 127:
#line 935 "yacc_sql.y"
                                            {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-3].string), (yyvsp[-2].string));

		Condition condition;
		condition_init_with_value_list(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, CONTEXT->value_list, CONTEXT->value_list_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		CONTEXT->value_list_length = 0;
	}
#line 2820 "yacc_sql.tab.c"
    break;

  case 128:
#line 944 "yacc_sql.y"
                                                   {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string), (yyvsp[-2].string));

		Condition condition;
		condition_init_with_value_list(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, CONTEXT->value_list, CONTEXT->value_list_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		CONTEXT->value_list_length = 0;
	}
#line 2834 "yacc_sql.tab.c"
    break;

  case 129:
#line 953 "yacc_sql.y"
                         {
		Query *right_subquery = (yyvsp[0].query);
		Condition condition;

		condition_init_with_subquery(&condition, CONTEXT->comp, NO_EXPR, NULL, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
#line 2847 "yacc_sql.tab.c"
    break;

  case 130:
#line 962 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[-1].string), (yyvsp[0].string));
			right_attr.is_complex = 1;

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);
		}
#line 2863 "yacc_sql.tab.c"
    break;

  case 131:
#line 974 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-3].string), (yyvsp[-2].string));
			left_attr.is_complex = 1;

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);

		}
#line 2880 "yacc_sql.tab.c"
    break;

  case 132:
#line 987 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-4].string), (yyvsp[-3].string));

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[-1].string), (yyvsp[0].string));
			right_attr.is_complex = 1;

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);

		}
#line 2898 "yacc_sql.tab.c"
    break;

  case 133:
#line 1001 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-4].string), (yyvsp[-3].string));
			left_attr.is_complex = 1;

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[-1].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);
		}
#line 2915 "yacc_sql.tab.c"
    break;

  case 134:
#line 1014 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-3].string));

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[-1].string), (yyvsp[0].string));
			right_attr.is_complex = 1;

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);
    	}
#line 2932 "yacc_sql.tab.c"
    break;

  case 135:
#line 1027 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-6].string), (yyvsp[-5].string));
			left_attr.is_complex = 1;

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-3].string), (yyvsp[-1].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);			
    	}
#line 2949 "yacc_sql.tab.c"
    break;

  case 136:
#line 1040 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-4].string), (yyvsp[-3].string));
			left_attr.is_complex = 1;

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[-1].string), (yyvsp[0].string));
			right_attr.is_complex = 1;

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);
    	}
#line 2967 "yacc_sql.tab.c"
    break;

  case 137:
#line 1056 "yacc_sql.y"
                                                                                   {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-6].string), (yyvsp[-5].string));

		selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

		CONTEXT->ssql->flag=SCF_SELECT;//"select";
		CONTEXT->ssql->sstr.selection.is_subquery=1;
		// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;
		(yyval.query) = CONTEXT->ssql;
        
		query_stack_pop(CONTEXT);
	}
#line 2984 "yacc_sql.tab.c"
    break;

  case 138:
#line 1070 "yacc_sql.y"
                                       {
		subquery_create_value_list(CONTEXT->value_list, CONTEXT->values, CONTEXT->value_length);
		CONTEXT->value_list_length = CONTEXT->value_length;
		CONTEXT->value_length = 0;
	}
#line 2994 "yacc_sql.tab.c"
    break;

  case 139:
#line 1077 "yacc_sql.y"
                 {
	// 将当前状态入栈
	query_stack_push(CONTEXT);
   }
#line 3003 "yacc_sql.tab.c"
    break;

  case 141:
#line 1083 "yacc_sql.y"
                                                        {

	}
#line 3011 "yacc_sql.tab.c"
    break;

  case 143:
#line 1089 "yacc_sql.y"
                                          {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 3019 "yacc_sql.tab.c"
    break;

  case 144:
#line 1095 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, "*", (yyvsp[-2].string));
		left_attr.aggr_type = (yyvsp[-6].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 3035 "yacc_sql.tab.c"
    break;

  case 145:
#line 1107 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-4].string), (yyvsp[-2].string));
		left_attr.aggr_type = (yyvsp[-6].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 3051 "yacc_sql.tab.c"
    break;

  case 146:
#line 1119 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-2].string));
		left_attr.aggr_type = (yyvsp[-8].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 3067 "yacc_sql.tab.c"
    break;

  case 147:
#line 1131 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-5].string), (yyvsp[-3].string));
			left_attr.aggr_type = (yyvsp[-7].number);

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[-1].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 3084 "yacc_sql.tab.c"
    break;

  case 148:
#line 1144 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-7].string), (yyvsp[-5].string));
			left_attr.aggr_type = (yyvsp[-9].number);

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-3].string), (yyvsp[-1].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 3101 "yacc_sql.tab.c"
    break;

  case 149:
#line 1157 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-7].string), (yyvsp[-5].string), (yyvsp[-3].string));
			left_attr.aggr_type = (yyvsp[-9].number);

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[-1].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;						
    	}
#line 3118 "yacc_sql.tab.c"
    break;

  case 150:
#line 1170 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-9].string), (yyvsp[-7].string), (yyvsp[-5].string));
			left_attr.aggr_type = (yyvsp[-11].number);

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-3].string), (yyvsp[-1].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    	}
#line 3135 "yacc_sql.tab.c"
    break;

  case 152:
#line 1185 "yacc_sql.y"
                          {	
		
			}
#line 3143 "yacc_sql.tab.c"
    break;

  case 153:
#line 1190 "yacc_sql.y"
                                {
			OrderAttr attr;
			order_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 3153 "yacc_sql.tab.c"
    break;

  case 154:
#line 1195 "yacc_sql.y"
                                          {
			OrderAttr attr;
			order_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 3163 "yacc_sql.tab.c"
    break;

  case 156:
#line 1203 "yacc_sql.y"
                                    {
			OrderAttr attr;
			order_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
      }
#line 3173 "yacc_sql.tab.c"
    break;

  case 157:
#line 1208 "yacc_sql.y"
                                           {
			OrderAttr attr;
			order_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
  	  }
#line 3183 "yacc_sql.tab.c"
    break;

  case 158:
#line 1216 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 3189 "yacc_sql.tab.c"
    break;

  case 159:
#line 1217 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 3195 "yacc_sql.tab.c"
    break;

  case 160:
#line 1218 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 3201 "yacc_sql.tab.c"
    break;

  case 161:
#line 1219 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 3207 "yacc_sql.tab.c"
    break;

  case 162:
#line 1220 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 3213 "yacc_sql.tab.c"
    break;

  case 163:
#line 1221 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 3219 "yacc_sql.tab.c"
    break;

  case 164:
#line 1222 "yacc_sql.y"
                 { CONTEXT->comp = LIKE; }
#line 3225 "yacc_sql.tab.c"
    break;

  case 165:
#line 1223 "yacc_sql.y"
                           { CONTEXT->comp = NOT_LIKE; }
#line 3231 "yacc_sql.tab.c"
    break;

  case 166:
#line 1225 "yacc_sql.y"
         { CONTEXT->comp = IS_NULL; }
#line 3237 "yacc_sql.tab.c"
    break;

  case 167:
#line 1226 "yacc_sql.y"
                   { CONTEXT->comp = IS_NOT_NULL; }
#line 3243 "yacc_sql.tab.c"
    break;

  case 168:
#line 1227 "yacc_sql.y"
                   {CONTEXT->comp = IN;}
#line 3249 "yacc_sql.tab.c"
    break;

  case 169:
#line 1228 "yacc_sql.y"
                             {CONTEXT->comp = NOT_IN;}
#line 3255 "yacc_sql.tab.c"
    break;

  case 170:
#line 1229 "yacc_sql.y"
                                 {CONTEXT->comp = NOT_EXISTS;}
#line 3261 "yacc_sql.tab.c"
    break;

  case 171:
#line 1230 "yacc_sql.y"
                       {CONTEXT->comp = EXISTS;}
#line 3267 "yacc_sql.tab.c"
    break;

  case 172:
#line 1235 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 3276 "yacc_sql.tab.c"
    break;


#line 3280 "yacc_sql.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1240 "yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, Query *sqls){
	ParserContext context;
	memset(&context, 0, sizeof(context));

	yyscan_t scanner;
	yylex_init_extra(&context, &scanner);
	context.ssql = sqls;
	scan_string(s, scanner);
	int result = yyparse(scanner);
	yylex_destroy(scanner);
	return result;
}
