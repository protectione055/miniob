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
    NUMBER = 317,
    FLOAT = 318,
    ID = 319,
    PATH = 320,
    SSS = 321,
    STAR = 322,
    STRING_V = 323,
    MIN_AGGR = 324,
    MAX_AGGR = 325,
    COUNT_AGGR = 326,
    SUM_AGGR = 327,
    AVG_AGGR = 328
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 186 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
  char *position;
  Query*query;

#line 326 "yacc_sql.tab.c"

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
#define YYLAST   396

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  58
/* YYNRULES -- Number of rules.  */
#define YYNRULES  163
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  359

#define YYUNDEFTOK  2
#define YYMAXUTOK   328


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
      65,    66,    67,    68,    69,    70,    71,    72,    73
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   222,   222,   224,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   249,   254,   259,   265,   271,   277,   283,   289,
     295,   302,   309,   319,   322,   327,   328,   334,   341,   350,
     352,   356,   363,   370,   379,   382,   383,   384,   385,   388,
     389,   390,   394,   395,   396,   399,   408,   425,   430,   432,
     434,   436,   441,   444,   447,   451,   457,   467,   475,   479,
     484,   485,   487,   509,   514,   519,   524,   531,   538,   545,
     554,   555,   556,   557,   558,   560,   562,   569,   576,   585,
     594,   604,   605,   610,   616,   618,   625,   633,   635,   638,
     642,   644,   648,   650,   655,   664,   674,   685,   704,   706,
     710,   712,   715,   720,   741,   761,   781,   803,   824,   845,
     863,   874,   885,   898,   910,   922,   931,   940,   950,   964,
     971,   976,   977,   981,   983,   988,  1000,  1012,  1024,  1037,
    1050,  1063,  1077,  1079,  1084,  1089,  1095,  1097,  1102,  1110,
    1111,  1112,  1113,  1114,  1115,  1116,  1117,  1119,  1120,  1121,
    1122,  1123,  1124,  1128
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
  "EXISTS_TOKEN", "NUMBER", "FLOAT", "ID", "PATH", "SSS", "STAR",
  "STRING_V", "MIN_AGGR", "MAX_AGGR", "COUNT_AGGR", "SUM_AGGR", "AVG_AGGR",
  "$accept", "commands", "command", "exit", "help", "sync", "begin",
  "commit", "rollback", "drop_table", "show_tables", "show_index",
  "desc_table", "create_index", "optional_unique", "create_index_attrs",
  "drop_index", "create_table", "attr_def_list", "attr_def", "number",
  "type", "order_type", "nullable", "ID_get", "insert", "insert_tuple",
  "insert_tuple_list", "value_list", "value", "delete", "update",
  "update_attr", "update_attr_list", "select", "select_attr", "aggregate",
  "attr_list", "group_by", "group_key_list", "rel_list", "join_list",
  "join_cond_list", "join_cond", "where", "condition_list", "condition",
  "sub_query", "subquery_value_list", "sub_query_init", "having",
  "having_condition_list", "having_condition", "order", "order_attr",
  "order_list", "comOp", "load_data", YY_NULLPTR
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
     325,   326,   327,   328
};
# endif

#define YYPACT_NINF (-294)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -294,   166,  -294,    24,   123,   196,   -35,    82,    31,    40,
       6,    12,   103,   120,   127,   139,   141,    51,  -294,  -294,
    -294,  -294,  -294,  -294,  -294,  -294,  -294,  -294,  -294,  -294,
    -294,  -294,  -294,  -294,  -294,  -294,  -294,    73,  -294,   140,
      86,   101,     8,   133,  -294,  -294,  -294,  -294,  -294,   142,
     150,   170,   171,   151,  -294,   119,   121,   152,  -294,  -294,
    -294,  -294,  -294,   163,   174,   160,   228,   230,   206,   168,
    -294,  -294,   172,   -15,  -294,  -294,   173,   202,   204,   185,
     184,   187,   217,  -294,  -294,    88,   244,   133,    13,    23,
     246,   259,   254,    37,   269,   231,   255,   250,  -294,   263,
     273,   219,   225,  -294,    90,  -294,   226,   239,   204,   133,
     155,   133,  -294,   106,   287,   297,  -294,  -294,  -294,  -294,
    -294,  -294,  -294,    -5,  -294,   260,  -294,  -294,  -294,  -294,
     169,  -294,   198,    79,   198,   196,   291,  -294,    77,   185,
     204,   304,   187,   293,  -294,  -294,    56,  -294,  -294,     2,
     295,   133,    30,   296,    13,   249,   264,  -294,   299,   300,
    -294,   301,   254,   312,  -294,  -294,  -294,  -294,  -294,   257,
      58,    83,    37,    37,  -294,    -2,   286,  -294,  -294,  -294,
     255,   319,   261,   263,   320,   266,  -294,  -294,   265,  -294,
     262,  -294,   133,   267,   133,  -294,   290,   276,   279,   133,
     133,   106,   314,   287,  -294,   198,    11,  -294,  -294,  -294,
    -294,   303,  -294,    79,    79,   305,  -294,   270,  -294,  -294,
     333,  -294,  -294,  -294,  -294,   321,   318,  -294,   322,  -294,
     134,   274,   288,   292,  -294,  -294,   301,  -294,  -294,    70,
     301,   280,  -294,  -294,   281,    13,  -294,    56,   282,   325,
     133,   317,   198,   313,     5,  -294,   285,   232,   347,  -294,
     323,  -294,  -294,  -294,   334,  -294,  -294,   204,  -294,   318,
     348,  -294,   294,   144,   134,   302,  -294,   298,  -294,   335,
      89,   338,   324,  -294,   306,  -294,   279,  -294,  -294,   198,
     327,  -294,   313,   308,  -294,     9,   307,  -294,   309,   310,
    -294,   159,   232,  -294,  -294,   292,   190,   311,  -294,   315,
     335,     7,   108,   341,    74,   343,    79,   346,   336,  -294,
    -294,   329,  -294,   316,   335,   326,  -294,  -294,   198,   328,
     198,  -294,  -294,   330,   134,     9,  -294,   341,   222,   350,
     106,  -294,   313,   335,  -294,   339,  -294,   198,  -294,   302,
    -294,   331,   229,  -294,  -294,   342,  -294,   332,  -294
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
       0,     0,    85,    85,    80,    81,    82,    83,    84,     0,
       0,     0,     0,     0,    24,     0,     0,     0,    25,    26,
      27,    23,    22,     0,     0,     0,     0,     0,     0,     0,
      74,    73,     0,     0,    31,    29,     0,     0,   108,     0,
       0,     0,     0,    28,    37,    85,     0,    85,    97,     0,
       0,     0,     0,     0,     0,     0,    70,     0,    55,    39,
       0,     0,     0,    86,     0,    75,     0,     0,   108,    85,
       0,    85,    30,     0,    58,     0,   149,   150,   151,   152,
     153,   154,   155,     0,    65,   157,   159,   162,    62,    63,
       0,    64,     0,   110,     0,     0,     0,    66,     0,     0,
     108,     0,     0,     0,    45,    46,    52,    47,    48,    52,
       0,    85,     0,     0,    97,     0,   142,    78,     0,     0,
      76,    60,     0,     0,   130,   156,   160,   161,   158,     0,
       0,     0,     0,     0,   109,     0,     0,   127,    68,    69,
      70,     0,     0,    39,     0,     0,    54,    43,     0,    42,
       0,    87,    85,     0,    85,    98,     0,     0,    91,    85,
      85,     0,     0,    58,    56,     0,     0,   115,   113,   120,
     125,   116,   114,   110,   110,   122,   124,     0,    71,    67,
       0,    40,    38,    53,    44,     0,    35,    89,     0,    88,
       0,     0,     0,   131,    79,    77,    60,    57,    59,     0,
      60,     0,   111,   112,     0,    97,   163,    52,     0,     0,
      85,     0,     0,   102,    49,   143,     0,     0,     0,    61,
       0,   117,   121,   126,     0,   118,   123,   108,    41,    35,
       0,    90,     0,     0,     0,   100,    51,     0,    50,   146,
      94,     0,   133,    72,     0,   129,    91,    36,    32,     0,
       0,   104,   102,     0,    99,    49,     0,   144,     0,     0,
      92,     0,     0,   132,   119,   131,     0,     0,   103,     0,
     146,    49,    94,    94,     0,     0,   110,     0,     0,   105,
     106,     0,   145,     0,   146,     0,    95,    93,     0,     0,
       0,   134,   128,     0,     0,    49,   147,    94,     0,     0,
       0,   107,   102,   146,    96,   138,   136,     0,   135,   100,
     148,     0,     0,   101,   139,   140,   137,     0,   141
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -294,  -294,  -294,  -294,  -294,  -294,  -294,  -294,  -294,  -294,
    -294,  -294,  -294,  -294,  -294,    96,  -294,  -294,   193,   235,
    -294,  -294,  -269,  -136,  -294,  -294,   216,   178,  -212,  -113,
    -294,  -294,   243,   203,  -294,   251,   -67,   -41,    98,  -290,
    -143,    36,  -278,  -262,  -104,  -204,   -17,  -130,   148,  -294,
      84,  -294,    91,  -294,  -294,  -293,  -127,  -294
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    39,   249,    30,    31,   143,    99,
     225,   149,   279,   187,   100,    32,   114,   163,   202,   132,
      33,    34,    96,   140,    35,    49,    50,    70,   233,   300,
     108,   294,   275,   253,    94,   174,   133,   134,   210,   135,
     258,   303,   282,   198,   255,   297,   136,    36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     161,    86,    71,   170,   156,   171,   177,   175,   179,   242,
     243,   195,   292,   189,   308,   115,   276,   322,   276,   188,
     276,   164,   326,   327,   259,   178,   310,    68,   264,    51,
      37,   336,   106,    38,    54,   277,   181,   323,    69,    56,
     209,   109,   324,   165,   103,   216,   105,   344,   192,    89,
     350,   185,    90,   110,   115,   166,   167,   208,   212,   186,
     193,   278,   215,   278,   349,   278,   343,   107,   157,   124,
     160,    55,   342,   128,   129,   206,    57,   131,   239,   116,
     117,   118,   119,   120,   121,   122,   123,   206,   236,    52,
      53,    63,   328,   240,   115,   124,   125,   126,   127,   128,
     129,   130,   267,   131,   329,   185,    58,    68,   298,   262,
     191,   268,   331,   186,   172,   173,   124,   252,   102,   299,
     128,   129,   207,    59,   131,   273,   261,   298,   124,    40,
      60,    41,   128,   129,   260,   124,   131,    64,   325,   128,
     129,   124,    61,   131,    62,   128,   129,   211,    65,   131,
      66,   227,    68,   229,   152,   213,   214,   153,   234,   235,
     291,   252,   306,   286,   124,    67,     2,    73,   128,   129,
       3,     4,   131,    74,    75,    72,     5,     6,     7,     8,
       9,    10,    11,    77,    76,    78,    12,    13,    14,    79,
     281,    81,   124,   319,    15,    16,   128,   129,   251,   169,
     131,   338,   124,   340,    80,    17,   128,   129,   290,   271,
     131,   116,   117,   118,   119,   120,   121,   122,   123,   158,
     352,   252,   159,   314,    82,   346,   315,   348,   125,   126,
     127,    83,    87,    84,    92,   281,    88,    91,    93,   356,
     116,   117,   118,   119,   120,   121,   122,   123,   124,    95,
      97,    98,   128,   129,   318,   101,   131,   125,   126,   127,
      42,   104,   112,    43,   111,    44,    45,    46,    47,    48,
      85,   113,   137,   138,   139,    44,    45,    46,    47,    48,
     124,   141,   142,   150,   128,   129,   345,   124,   131,   151,
     154,   128,   129,   355,   155,   131,   144,   145,   146,   147,
     148,    44,    45,    46,    47,    48,   162,   164,   115,   168,
     182,   184,   190,   196,   194,   204,   197,   199,   200,   217,
     201,   205,   219,   222,   223,   220,   226,   224,   230,   231,
     232,   228,   237,   241,   245,   244,   246,   248,   254,   247,
     250,   256,   257,   270,   265,   266,   269,   272,   274,   280,
     283,   288,   285,   284,   296,   301,   293,   307,   289,   302,
     298,   330,   295,   309,   332,   287,   333,   334,   347,   351,
     304,   311,   357,   312,   313,   320,   221,   183,   203,   321,
     335,   238,   180,   218,   305,   353,   176,   263,     0,   317,
     337,     0,   339,   316,   341,   354,   358
};

static const yytype_int16 yycheck[] =
{
     113,    68,    43,   130,   108,   132,   136,   134,   138,   213,
     214,   154,   274,   149,   292,    17,    11,   310,    11,    17,
      11,    10,   312,   313,   236,   138,   295,    19,   240,    64,
       6,   324,    19,     9,     3,    30,   140,    30,    30,    33,
     170,    18,   311,    48,    85,   175,    87,   337,    18,    64,
     343,    49,    67,    30,    17,    60,    61,   170,   171,    57,
      30,    56,    64,    56,   342,    56,   335,    54,   109,    58,
     111,    31,   334,    62,    63,    17,    64,    66,   205,    42,
      43,    44,    45,    46,    47,    48,    49,    17,   201,     7,
       8,    40,    18,   206,    17,    58,    59,    60,    61,    62,
      63,    64,   245,    66,    30,    49,     3,    19,    19,   239,
     151,   247,   316,    57,    35,    36,    58,   230,    30,    30,
      62,    63,    64,     3,    66,   252,   239,    19,    58,     6,
       3,     8,    62,    63,    64,    58,    66,    64,    30,    62,
      63,    58,     3,    66,     3,    62,    63,    64,     8,    66,
      64,   192,    19,   194,    64,   172,   173,    67,   199,   200,
     273,   274,   289,   267,    58,    64,     0,    17,    62,    63,
       4,     5,    66,     3,     3,    33,    10,    11,    12,    13,
      14,    15,    16,    64,    33,    64,    20,    21,    22,    37,
     257,    17,    58,   306,    28,    29,    62,    63,    64,    30,
      66,   328,    58,   330,    41,    39,    62,    63,    64,   250,
      66,    42,    43,    44,    45,    46,    47,    48,    49,    64,
     347,   334,    67,    64,    64,   338,    67,   340,    59,    60,
      61,     3,    64,     3,    32,   302,    64,    64,    34,   352,
      42,    43,    44,    45,    46,    47,    48,    49,    58,    64,
      66,    64,    62,    63,    64,    38,    66,    59,    60,    61,
      64,    17,     3,    67,    18,    69,    70,    71,    72,    73,
      64,    17,     3,    42,    19,    69,    70,    71,    72,    73,
      58,    31,    19,    64,    62,    63,    64,    58,    66,    64,
      64,    62,    63,    64,    55,    66,    23,    24,    25,    26,
      27,    69,    70,    71,    72,    73,    19,    10,    17,    49,
       6,    18,    17,    64,    18,     3,    52,    18,    18,    33,
      19,    64,     3,     3,    58,    64,    64,    62,    38,    53,
      51,    64,    18,    30,    64,    30,     3,    19,    64,    18,
      18,    53,    50,    18,    64,    64,    64,    30,    35,    64,
       3,     3,    18,    30,    19,    17,    54,    30,    64,    35,
      19,    18,    64,    55,    18,   269,    30,    38,    18,    30,
      64,    64,    30,    64,    64,    64,   183,   142,   162,    64,
      64,   203,   139,   180,   286,   349,   135,   239,    -1,   305,
      64,    -1,    64,   302,    64,    64,    64
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    75,     0,     4,     5,    10,    11,    12,    13,    14,
      15,    16,    20,    21,    22,    28,    29,    39,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      90,    91,    99,   104,   105,   108,   131,     6,     9,    88,
       6,     8,    64,    67,    69,    70,    71,    72,    73,   109,
     110,    64,     7,     8,     3,    31,    33,    64,     3,     3,
       3,     3,     3,    40,    64,     8,    64,    64,    19,    30,
     111,   111,    33,    17,     3,     3,    33,    64,    64,    37,
      41,    17,    64,     3,     3,    64,   110,    64,    64,    64,
      67,    64,    32,    34,   118,    64,   106,    66,    64,    93,
      98,    38,    30,   111,    17,   111,    19,    54,   114,    18,
      30,    18,     3,    17,   100,    17,    42,    43,    44,    45,
      46,    47,    48,    49,    58,    59,    60,    61,    62,    63,
      64,    66,   103,   120,   121,   123,   130,     3,    42,    19,
     107,    31,    19,    92,    23,    24,    25,    26,    27,    95,
      64,    64,    64,    67,    64,    55,   118,   111,    64,    67,
     111,   103,    19,   101,    10,    48,    60,    61,    49,    30,
     130,   130,    35,    36,   119,   130,   109,   121,   103,   121,
     106,   118,     6,    93,    18,    49,    57,    97,    17,    97,
      17,   111,    18,    30,    18,   114,    64,    52,   127,    18,
      18,    19,   102,   100,     3,    64,    17,    64,   103,   121,
     122,    64,   103,   120,   120,    64,   121,    33,   107,     3,
      64,    92,     3,    58,    62,    94,    64,   111,    64,   111,
      38,    53,    51,   112,   111,   111,   103,    18,   101,   130,
     103,    30,   119,   119,    30,    64,     3,    18,    19,    89,
      18,    64,   103,   117,    64,   128,    53,    50,   124,   102,
      64,   103,   121,   122,   102,    64,    64,   114,    97,    64,
      18,   111,    30,   130,    35,   116,    11,    30,    56,    96,
      64,   110,   126,     3,    30,    18,   118,    89,     3,    64,
      64,   103,   117,    54,   115,    64,    19,   129,    19,    30,
     113,    17,    35,   125,    64,   112,   130,    30,   116,    55,
      96,    64,    64,    64,    64,    67,   126,   124,    64,   103,
      64,    64,   129,    30,    96,    30,   113,   113,    18,    30,
      18,   119,    18,    30,    38,    64,   129,    64,   130,    64,
     130,    64,   117,    96,   113,    64,   103,    18,   103,   116,
     129,    30,   130,   115,    64,    64,   103,    30,    64
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    74,    75,    75,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    88,    89,    89,    90,    91,    92,
      92,    93,    93,    93,    94,    95,    95,    95,    95,    96,
      96,    96,    97,    97,    97,    98,    99,   100,   101,   101,
     102,   102,   103,   103,   103,   103,   104,   105,   106,   106,
     107,   107,   108,   109,   109,   109,   109,   109,   109,   109,
     110,   110,   110,   110,   110,   111,   111,   111,   111,   111,
     111,   112,   112,   112,   113,   113,   113,   114,   114,   114,
     115,   115,   116,   116,   117,   117,   117,   117,   118,   118,
     119,   119,   119,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   121,   122,
     123,   124,   124,   125,   125,   126,   126,   126,   126,   126,
     126,   126,   127,   127,   128,   128,   129,   129,   129,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   131
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
       0,     3,    10,     2,     2,     4,     5,     7,     5,     7,
       1,     1,     1,     1,     1,     0,     3,     5,     6,     6,
       8,     0,     4,     6,     0,     3,     5,     0,     3,     7,
       0,     7,     0,     3,     3,     5,     5,     7,     0,     3,
       0,     3,     3,     3,     3,     3,     3,     5,     5,     7,
       3,     5,     3,     5,     3,     3,     5,     2,     9,     4,
       2,     0,     3,     0,     3,     6,     6,     8,     6,     8,
       8,    10,     0,     3,     3,     5,     0,     4,     6,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     1,
       2,     2,     1,     8
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
#line 249 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1770 "yacc_sql.tab.c"
    break;

  case 23:
#line 254 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1778 "yacc_sql.tab.c"
    break;

  case 24:
#line 259 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1786 "yacc_sql.tab.c"
    break;

  case 25:
#line 265 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1794 "yacc_sql.tab.c"
    break;

  case 26:
#line 271 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1802 "yacc_sql.tab.c"
    break;

  case 27:
#line 277 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1810 "yacc_sql.tab.c"
    break;

  case 28:
#line 283 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1819 "yacc_sql.tab.c"
    break;

  case 29:
#line 289 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1827 "yacc_sql.tab.c"
    break;

  case 30:
#line 295 "yacc_sql.y"
                                 {
      CONTEXT->ssql->flag = SCF_SHOW_INDEX;
	  show_index_init(&CONTEXT->ssql->sstr.show_index, (yyvsp[-1].string));
    }
#line 1836 "yacc_sql.tab.c"
    break;

  case 31:
#line 302 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1845 "yacc_sql.tab.c"
    break;

  case 32:
#line 310 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, (yyvsp[-3].string));
		}
#line 1855 "yacc_sql.tab.c"
    break;

  case 33:
#line 319 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 0);
		}
#line 1863 "yacc_sql.tab.c"
    break;

  case 34:
#line 323 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 1);
		}
#line 1871 "yacc_sql.tab.c"
    break;

  case 36:
#line 329 "yacc_sql.y"
                {
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, (yyvsp[-1].string));
		}
#line 1879 "yacc_sql.tab.c"
    break;

  case 37:
#line 335 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1888 "yacc_sql.tab.c"
    break;

  case 38:
#line 342 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1900 "yacc_sql.tab.c"
    break;

  case 40:
#line 352 "yacc_sql.y"
                                   {    }
#line 1906 "yacc_sql.tab.c"
    break;

  case 41:
#line 357 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1917 "yacc_sql.tab.c"
    break;

  case 42:
#line 364 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1928 "yacc_sql.tab.c"
    break;

  case 43:
#line 371 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, CHARS, 4096, (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1939 "yacc_sql.tab.c"
    break;

  case 44:
#line 379 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1945 "yacc_sql.tab.c"
    break;

  case 45:
#line 382 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1951 "yacc_sql.tab.c"
    break;

  case 46:
#line 383 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1957 "yacc_sql.tab.c"
    break;

  case 47:
#line 384 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1963 "yacc_sql.tab.c"
    break;

  case 48:
#line 385 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 1969 "yacc_sql.tab.c"
    break;

  case 49:
#line 388 "yacc_sql.y"
                { (yyval.number)=1; }
#line 1975 "yacc_sql.tab.c"
    break;

  case 50:
#line 389 "yacc_sql.y"
                  { (yyval.number)=1; }
#line 1981 "yacc_sql.tab.c"
    break;

  case 51:
#line 390 "yacc_sql.y"
              { (yyval.number)=0; }
#line 1987 "yacc_sql.tab.c"
    break;

  case 52:
#line 394 "yacc_sql.y"
                    { (yyval.number)=0; }
#line 1993 "yacc_sql.tab.c"
    break;

  case 53:
#line 395 "yacc_sql.y"
                          { (yyval.number)=0; }
#line 1999 "yacc_sql.tab.c"
    break;

  case 54:
#line 396 "yacc_sql.y"
                   { (yyval.number)=1; }
#line 2005 "yacc_sql.tab.c"
    break;

  case 55:
#line 400 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 2014 "yacc_sql.tab.c"
    break;

  case 56:
#line 409 "yacc_sql.y"
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
#line 2033 "yacc_sql.tab.c"
    break;

  case 57:
#line 425 "yacc_sql.y"
                                       {
		inserts_create_tuple(&CONTEXT->ssql->sstr.insertion, CONTEXT->values, CONTEXT->value_length);
		CONTEXT->value_length = 0;
	}
#line 2042 "yacc_sql.tab.c"
    break;

  case 59:
#line 432 "yacc_sql.y"
                                               {}
#line 2048 "yacc_sql.tab.c"
    break;

  case 61:
#line 436 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 2056 "yacc_sql.tab.c"
    break;

  case 62:
#line 441 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 2064 "yacc_sql.tab.c"
    break;

  case 63:
#line 444 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 2072 "yacc_sql.tab.c"
    break;

  case 64:
#line 447 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 2081 "yacc_sql.tab.c"
    break;

  case 65:
#line 451 "yacc_sql.y"
               {
		value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
	}
#line 2089 "yacc_sql.tab.c"
    break;

  case 66:
#line 458 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 2101 "yacc_sql.tab.c"
    break;

  case 67:
#line 468 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-5].string), CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 2111 "yacc_sql.tab.c"
    break;

  case 68:
#line 475 "yacc_sql.y"
                    {
		updates_attr_init(&CONTEXT->ssql->sstr.update, (yyvsp[-2].string), &CONTEXT->values[0]);
		CONTEXT->value_length = 0;
	}
#line 2120 "yacc_sql.tab.c"
    break;

  case 69:
#line 479 "yacc_sql.y"
                      {
		Query *query = (yyvsp[0].query);
		updates_attr_init_with_subquery(&CONTEXT->ssql->sstr.update, (yyvsp[-2].string), &query->sstr.selection);
	}
#line 2129 "yacc_sql.tab.c"
    break;

  case 71:
#line 485 "yacc_sql.y"
                                                         {}
#line 2135 "yacc_sql.tab.c"
    break;

  case 72:
#line 488 "yacc_sql.y"
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-6].string));

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
#line 2158 "yacc_sql.tab.c"
    break;

  case 73:
#line 509 "yacc_sql.y"
                   {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2168 "yacc_sql.tab.c"
    break;

  case 74:
#line 514 "yacc_sql.y"
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2178 "yacc_sql.tab.c"
    break;

  case 75:
#line 519 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2188 "yacc_sql.tab.c"
    break;

  case 76:
#line 524 "yacc_sql.y"
                                                 {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2200 "yacc_sql.tab.c"
    break;

  case 77:
#line 531 "yacc_sql.y"
                                                        {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), "*");
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2212 "yacc_sql.tab.c"
    break;

  case 78:
#line 538 "yacc_sql.y"
                                               {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2224 "yacc_sql.tab.c"
    break;

  case 79:
#line 545 "yacc_sql.y"
                                                      {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2236 "yacc_sql.tab.c"
    break;

  case 80:
#line 554 "yacc_sql.y"
               {(yyval.number) = MIN;}
#line 2242 "yacc_sql.tab.c"
    break;

  case 81:
#line 555 "yacc_sql.y"
                   {(yyval.number) = MAX;}
#line 2248 "yacc_sql.tab.c"
    break;

  case 82:
#line 556 "yacc_sql.y"
                     {(yyval.number) = COUNT;}
#line 2254 "yacc_sql.tab.c"
    break;

  case 83:
#line 557 "yacc_sql.y"
                   {(yyval.number) = SUM;}
#line 2260 "yacc_sql.tab.c"
    break;

  case 84:
#line 558 "yacc_sql.y"
                   {(yyval.number) = AVG;}
#line 2266 "yacc_sql.tab.c"
    break;

  case 86:
#line 562 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2278 "yacc_sql.tab.c"
    break;

  case 87:
#line 569 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2290 "yacc_sql.tab.c"
    break;

  case 88:
#line 576 "yacc_sql.y"
                                                       {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2304 "yacc_sql.tab.c"
    break;

  case 89:
#line 585 "yacc_sql.y"
                                                     {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2318 "yacc_sql.tab.c"
    break;

  case 90:
#line 594 "yacc_sql.y"
                                                            {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2332 "yacc_sql.tab.c"
    break;

  case 92:
#line 605 "yacc_sql.y"
                                 {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2342 "yacc_sql.tab.c"
    break;

  case 93:
#line 610 "yacc_sql.y"
                                            {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2352 "yacc_sql.tab.c"
    break;

  case 95:
#line 618 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2364 "yacc_sql.tab.c"
    break;

  case 96:
#line 625 "yacc_sql.y"
                                     {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2376 "yacc_sql.tab.c"
    break;

  case 98:
#line 635 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2384 "yacc_sql.tab.c"
    break;

  case 99:
#line 638 "yacc_sql.y"
                                                          {	
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
	}
#line 2392 "yacc_sql.tab.c"
    break;

  case 101:
#line 644 "yacc_sql.y"
                                                          {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
		  }
#line 2400 "yacc_sql.tab.c"
    break;

  case 103:
#line 650 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
		  }
#line 2408 "yacc_sql.tab.c"
    break;

  case 104:
#line 656 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 2421 "yacc_sql.tab.c"
    break;

  case 105:
#line 665 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    	}
#line 2435 "yacc_sql.tab.c"
    break;

  case 106:
#line 675 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;		
    	}
#line 2450 "yacc_sql.tab.c"
    break;

  case 107:
#line 686 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			// 判断是否同一表中的两个属性。若否，条件加入join中
			if (strcmp((yyvsp[-6].string), (yyvsp[-2].string)) != 0){
				Condition condition;
				condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
				selects_append_joincond(&CONTEXT->ssql->sstr.selection, condition);
			} else {
				Condition condition;
				condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
				CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			}
    	}
#line 2472 "yacc_sql.tab.c"
    break;

  case 109:
#line 706 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2480 "yacc_sql.tab.c"
    break;

  case 111:
#line 712 "yacc_sql.y"
                                   {
				CONTEXT->ssql->sstr.selection.condition_mode = AND_MODE;
			}
#line 2488 "yacc_sql.tab.c"
    break;

  case 112:
#line 715 "yacc_sql.y"
                                      {
				CONTEXT->ssql->sstr.selection.condition_mode = OR_MODE;
			}
#line 2496 "yacc_sql.tab.c"
    break;

  case 113:
#line 721 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

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
#line 2521 "yacc_sql.tab.c"
    break;

  case 114:
#line 742 "yacc_sql.y"
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
#line 2545 "yacc_sql.tab.c"
    break;

  case 115:
#line 762 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

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
#line 2569 "yacc_sql.tab.c"
    break;

  case 116:
#line 782 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

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
#line 2595 "yacc_sql.tab.c"
    break;

  case 117:
#line 804 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
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
#line 2620 "yacc_sql.tab.c"
    break;

  case 118:
#line 825 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

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
#line 2645 "yacc_sql.tab.c"
    break;

  case 119:
#line 846 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			// 判断是否同一表中的两个属性。若否，条件加入join中
			// if (strcmp($1, $5) != 0){
			// 	Condition condition;
			// 	condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			// 	selects_append_joincond(&CONTEXT->ssql->sstr.selection, condition);
			// } else {
				Condition condition;
				condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
				CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// }
    	}
#line 2667 "yacc_sql.tab.c"
    break;

  case 120:
#line 864 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));
		Query *right_subquery = (yyvsp[0].query);

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
#line 2682 "yacc_sql.tab.c"
    break;

  case 121:
#line 875 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
		Query *right_subquery = (yyvsp[0].query);

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
#line 2697 "yacc_sql.tab.c"
    break;

  case 122:
#line 886 "yacc_sql.y"
        {
		RelAttr right_attr;
		relation_attr_init(&right_attr, NULL, (yyvsp[0].string));
		Query *left_subquery = (yyvsp[-2].query);

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, SUB_QUERY, NULL, NULL, &left_subquery->sstr.selection, ATTR, &right_attr, NULL, NULL);
		printf("1condition_length=%d\n", CONTEXT->condition_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(left_subquery);
		printf("2condition_length=%d\n", CONTEXT->condition_length);
	}
#line 2714 "yacc_sql.tab.c"
    break;

  case 123:
#line 899 "yacc_sql.y"
        {
		RelAttr right_attr;
		relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));
		Query *left_subquery = (yyvsp[-4].query);
		
		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, SUB_QUERY, NULL, NULL, &left_subquery->sstr.selection, ATTR, &right_attr, NULL, NULL);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(left_subquery);
		printf("s op t.a condition_length=%d\n", CONTEXT->condition_length);
	}
#line 2730 "yacc_sql.tab.c"
    break;

  case 124:
#line 911 "yacc_sql.y"
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
#line 2746 "yacc_sql.tab.c"
    break;

  case 125:
#line 922 "yacc_sql.y"
                                      {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

		Condition condition;
		condition_init_with_value_list(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, CONTEXT->value_list, CONTEXT->value_list_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		CONTEXT->value_list_length = 0;
	}
#line 2760 "yacc_sql.tab.c"
    break;

  case 126:
#line 931 "yacc_sql.y"
                                             {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));

		Condition condition;
		condition_init_with_value_list(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, CONTEXT->value_list, CONTEXT->value_list_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		CONTEXT->value_list_length = 0;
	}
#line 2774 "yacc_sql.tab.c"
    break;

  case 127:
#line 940 "yacc_sql.y"
                         {
		Query *right_subquery = (yyvsp[0].query);
		Condition condition;

		condition_init_with_subquery(&condition, CONTEXT->comp, NO_EXPR, NULL, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
#line 2787 "yacc_sql.tab.c"
    break;

  case 128:
#line 950 "yacc_sql.y"
                                                                             {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-5].string));

		selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

		CONTEXT->ssql->flag=SCF_SELECT;//"select";
		CONTEXT->ssql->sstr.selection.is_subquery=1;
		// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;
		(yyval.query) = CONTEXT->ssql;
        
		query_stack_pop(CONTEXT);
	}
#line 2804 "yacc_sql.tab.c"
    break;

  case 129:
#line 964 "yacc_sql.y"
                                       {
		subquery_create_value_list(CONTEXT->value_list, CONTEXT->values, CONTEXT->value_length);
		CONTEXT->value_list_length = CONTEXT->value_length;
		CONTEXT->value_length = 0;
	}
#line 2814 "yacc_sql.tab.c"
    break;

  case 130:
#line 971 "yacc_sql.y"
                 {
	// 将当前状态入栈
	query_stack_push(CONTEXT);
   }
#line 2823 "yacc_sql.tab.c"
    break;

  case 132:
#line 977 "yacc_sql.y"
                                                        {

	}
#line 2831 "yacc_sql.tab.c"
    break;

  case 134:
#line 983 "yacc_sql.y"
                                          {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2839 "yacc_sql.tab.c"
    break;

  case 135:
#line 989 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, "*");
		left_attr.aggr_type = (yyvsp[-5].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2855 "yacc_sql.tab.c"
    break;

  case 136:
#line 1001 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-3].string));
		left_attr.aggr_type = (yyvsp[-5].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2871 "yacc_sql.tab.c"
    break;

  case 137:
#line 1013 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string));
		left_attr.aggr_type = (yyvsp[-7].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2887 "yacc_sql.tab.c"
    break;

  case 138:
#line 1025 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-3].string));
			left_attr.aggr_type = (yyvsp[-5].number);

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 2904 "yacc_sql.tab.c"
    break;

  case 139:
#line 1038 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-5].string));
			left_attr.aggr_type = (yyvsp[-7].number);

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 2921 "yacc_sql.tab.c"
    break;

  case 140:
#line 1051 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string));
			left_attr.aggr_type = (yyvsp[-7].number);

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;						
    	}
#line 2938 "yacc_sql.tab.c"
    break;

  case 141:
#line 1064 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-7].string), (yyvsp[-5].string));
			left_attr.aggr_type = (yyvsp[-9].number);

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    	}
#line 2955 "yacc_sql.tab.c"
    break;

  case 143:
#line 1079 "yacc_sql.y"
                          {	
		
			}
#line 2963 "yacc_sql.tab.c"
    break;

  case 144:
#line 1084 "yacc_sql.y"
                                {
			OrderAttr attr;
			order_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2973 "yacc_sql.tab.c"
    break;

  case 145:
#line 1089 "yacc_sql.y"
                                          {
			OrderAttr attr;
			order_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2983 "yacc_sql.tab.c"
    break;

  case 147:
#line 1097 "yacc_sql.y"
                                    {
			OrderAttr attr;
			order_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
      }
#line 2993 "yacc_sql.tab.c"
    break;

  case 148:
#line 1102 "yacc_sql.y"
                                           {
			OrderAttr attr;
			order_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
  	  }
#line 3003 "yacc_sql.tab.c"
    break;

  case 149:
#line 1110 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 3009 "yacc_sql.tab.c"
    break;

  case 150:
#line 1111 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 3015 "yacc_sql.tab.c"
    break;

  case 151:
#line 1112 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 3021 "yacc_sql.tab.c"
    break;

  case 152:
#line 1113 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 3027 "yacc_sql.tab.c"
    break;

  case 153:
#line 1114 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 3033 "yacc_sql.tab.c"
    break;

  case 154:
#line 1115 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 3039 "yacc_sql.tab.c"
    break;

  case 155:
#line 1116 "yacc_sql.y"
                 { CONTEXT->comp = LIKE; }
#line 3045 "yacc_sql.tab.c"
    break;

  case 156:
#line 1117 "yacc_sql.y"
                           { CONTEXT->comp = NOT_LIKE; }
#line 3051 "yacc_sql.tab.c"
    break;

  case 157:
#line 1119 "yacc_sql.y"
         { CONTEXT->comp = IS_NULL; }
#line 3057 "yacc_sql.tab.c"
    break;

  case 158:
#line 1120 "yacc_sql.y"
                   { CONTEXT->comp = IS_NOT_NULL; }
#line 3063 "yacc_sql.tab.c"
    break;

  case 159:
#line 1121 "yacc_sql.y"
                   {CONTEXT->comp = IN;}
#line 3069 "yacc_sql.tab.c"
    break;

  case 160:
#line 1122 "yacc_sql.y"
                             {CONTEXT->comp = NOT_IN;}
#line 3075 "yacc_sql.tab.c"
    break;

  case 161:
#line 1123 "yacc_sql.y"
                                 {CONTEXT->comp = NOT_EXISTS;}
#line 3081 "yacc_sql.tab.c"
    break;

  case 162:
#line 1124 "yacc_sql.y"
                       {CONTEXT->comp = EXISTS;}
#line 3087 "yacc_sql.tab.c"
    break;

  case 163:
#line 1129 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 3096 "yacc_sql.tab.c"
    break;


#line 3100 "yacc_sql.tab.c"

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
#line 1134 "yacc_sql.y"

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
