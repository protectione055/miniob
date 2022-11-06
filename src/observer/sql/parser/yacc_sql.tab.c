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
    SET = 291,
    ON = 292,
    LOAD = 293,
    DATA = 294,
    INFILE = 295,
    EQ = 296,
    LT = 297,
    GT = 298,
    LE = 299,
    GE = 300,
    NE = 301,
    LIKE_TOKEN = 302,
    NOT_TOKEN = 303,
    HAVING = 304,
    GROUP = 305,
    ORDER = 306,
    BY = 307,
    INNER = 308,
    JOIN = 309,
    ASC = 310,
    NULLABLE = 311,
    NULL_ = 312,
    IS = 313,
    IN_TOKEN = 314,
    EXISTS_TOKEN = 315,
    NUMBER = 316,
    FLOAT = 317,
    ID = 318,
    PATH = 319,
    SSS = 320,
    STAR = 321,
    STRING_V = 322,
    MIN_AGGR = 323,
    MAX_AGGR = 324,
    COUNT_AGGR = 325,
    SUM_AGGR = 326,
    AVG_AGGR = 327
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 185 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
  char *position;
  Query*query;

#line 325 "yacc_sql.tab.c"

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
#define YYLAST   401

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  58
/* YYNRULES -- Number of rules.  */
#define YYNRULES  162
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  356

#define YYUNDEFTOK  2
#define YYMAXUTOK   327


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
      65,    66,    67,    68,    69,    70,    71,    72
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   221,   221,   223,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   248,   253,   258,   264,   270,   276,   282,   288,
     294,   301,   308,   318,   321,   326,   327,   333,   340,   349,
     351,   355,   362,   369,   378,   381,   382,   383,   384,   387,
     388,   389,   393,   394,   395,   398,   407,   424,   429,   431,
     433,   435,   440,   443,   446,   450,   456,   466,   474,   478,
     483,   484,   486,   508,   513,   518,   523,   530,   537,   544,
     553,   554,   555,   556,   557,   559,   561,   568,   575,   584,
     593,   603,   604,   609,   615,   617,   624,   632,   634,   637,
     641,   643,   647,   649,   654,   663,   673,   684,   703,   705,
     709,   711,   716,   737,   757,   777,   799,   820,   841,   859,
     870,   881,   894,   906,   918,   927,   936,   946,   960,   967,
     972,   973,   977,   979,   984,   996,  1008,  1020,  1033,  1046,
    1059,  1073,  1075,  1080,  1085,  1091,  1093,  1098,  1106,  1107,
    1108,  1109,  1110,  1111,  1112,  1113,  1115,  1116,  1117,  1118,
    1119,  1120,  1124
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
  "AND", "SET", "ON", "LOAD", "DATA", "INFILE", "EQ", "LT", "GT", "LE",
  "GE", "NE", "LIKE_TOKEN", "NOT_TOKEN", "HAVING", "GROUP", "ORDER", "BY",
  "INNER", "JOIN", "ASC", "NULLABLE", "NULL_", "IS", "IN_TOKEN",
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
     325,   326,   327
};
# endif

#define YYPACT_NINF (-290)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -290,   191,  -290,    96,    48,   155,   -30,    21,    31,    24,
      59,    51,   135,   140,   175,   179,   185,   137,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,   129,  -290,    88,
     131,   145,    63,   170,  -290,  -290,  -290,  -290,  -290,   167,
     181,   212,   219,   200,  -290,   174,   176,   204,  -290,  -290,
    -290,  -290,  -290,   208,   226,   187,   248,   249,    94,   192,
    -290,  -290,   199,    79,  -290,  -290,   206,   222,   229,   207,
     211,   214,   243,  -290,  -290,    70,   264,   170,     0,     9,
     270,   286,   273,    89,   288,   251,   274,   263,  -290,   276,
      93,   233,   234,  -290,   127,  -290,   235,   245,   229,   170,
     151,   170,  -290,   217,   281,   291,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,   -22,  -290,   254,  -290,  -290,  -290,  -290,
     126,  -290,    81,   268,    81,   155,   287,  -290,    33,   207,
     229,   299,   214,   289,  -290,  -290,    53,  -290,  -290,    35,
     292,   170,    13,   290,     0,   247,   255,  -290,   293,   294,
    -290,   295,   273,   310,  -290,  -290,  -290,  -290,  -290,   252,
       7,   118,    89,  -290,    -4,   283,  -290,  -290,  -290,   274,
     314,   256,   276,   315,   265,  -290,  -290,   259,  -290,   258,
    -290,   170,   260,   170,  -290,   296,   272,   275,   170,   170,
     217,   308,   281,  -290,    81,     1,  -290,  -290,  -290,  -290,
     297,  -290,   268,   298,  -290,   266,  -290,  -290,   327,  -290,
    -290,  -290,  -290,   313,   316,  -290,   318,  -290,   173,   269,
     282,   300,  -290,  -290,   295,  -290,  -290,    23,   295,   277,
    -290,   278,     0,  -290,    53,   279,   319,   170,   309,    81,
     303,     6,  -290,   280,   215,   341,  -290,   317,  -290,  -290,
    -290,   328,  -290,  -290,   229,  -290,   316,   342,  -290,   285,
     184,   173,   301,  -290,   302,  -290,   331,    80,   334,   320,
    -290,   304,  -290,   275,  -290,  -290,    81,   322,  -290,   303,
     305,  -290,     5,   306,  -290,   307,   311,  -290,   165,   215,
    -290,  -290,   300,   196,   312,  -290,   321,   331,    12,    85,
     337,    57,   335,   268,   339,   330,  -290,  -290,   324,  -290,
     323,   331,   325,  -290,  -290,    81,   326,    81,  -290,  -290,
     329,   173,     5,  -290,   337,   203,   340,   217,  -290,   303,
     331,  -290,   332,  -290,    81,  -290,   301,  -290,   333,   210,
    -290,  -290,   336,  -290,   338,  -290
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
       0,    85,    30,     0,    58,     0,   148,   149,   150,   151,
     152,   153,   154,     0,    65,   156,   158,   161,    62,    63,
       0,    64,     0,   110,     0,     0,     0,    66,     0,     0,
     108,     0,     0,     0,    45,    46,    52,    47,    48,    52,
       0,    85,     0,     0,    97,     0,   141,    78,     0,     0,
      76,    60,     0,     0,   129,   155,   159,   160,   157,     0,
       0,     0,     0,   109,     0,     0,   126,    68,    69,    70,
       0,     0,    39,     0,     0,    54,    43,     0,    42,     0,
      87,    85,     0,    85,    98,     0,     0,    91,    85,    85,
       0,     0,    58,    56,     0,     0,   114,   112,   119,   124,
     115,   113,   110,   121,   123,     0,    71,    67,     0,    40,
      38,    53,    44,     0,    35,    89,     0,    88,     0,     0,
       0,   130,    79,    77,    60,    57,    59,     0,    60,     0,
     111,     0,    97,   162,    52,     0,     0,    85,     0,     0,
     102,    49,   142,     0,     0,     0,    61,     0,   116,   120,
     125,     0,   117,   122,   108,    41,    35,     0,    90,     0,
       0,     0,   100,    51,     0,    50,   145,    94,     0,   132,
      72,     0,   128,    91,    36,    32,     0,     0,   104,   102,
       0,    99,    49,     0,   143,     0,     0,    92,     0,     0,
     131,   118,   130,     0,     0,   103,     0,   145,    49,    94,
      94,     0,     0,   110,     0,     0,   105,   106,     0,   144,
       0,   145,     0,    95,    93,     0,     0,     0,   133,   127,
       0,     0,    49,   146,    94,     0,     0,     0,   107,   102,
     145,    96,   137,   135,     0,   134,   100,   147,     0,     0,
     101,   138,   139,   136,     0,   140
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,    97,  -290,  -290,   182,   230,
    -290,  -290,  -251,  -141,  -290,  -290,   209,   166,  -208,   -93,
    -290,  -290,   237,   194,  -290,   242,   -67,   -38,    95,  -288,
    -145,    34,  -274,  -257,  -105,  -205,   213,  -126,   142,  -290,
      91,  -290,    82,  -290,  -290,  -289,  -128,  -290
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    39,   246,    30,    31,   143,    99,
     223,   149,   276,   186,   100,    32,   114,   163,   201,   249,
      33,    34,    96,   140,    35,    49,    50,    70,   231,   297,
     108,   291,   272,   250,    94,   173,   133,   134,   209,   135,
     255,   300,   279,   197,   252,   294,   136,    36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     132,    86,   170,   156,   171,    71,   174,   240,   188,   194,
     176,   164,   178,   115,   289,   305,   273,   273,   319,   106,
     161,   323,   324,   273,   205,   165,   256,   109,    52,    53,
     261,   191,   333,    51,    54,   180,   274,   166,   167,   110,
     205,   307,   320,   192,   208,   177,   341,   103,   214,   105,
     115,   347,   187,   107,    40,    55,    41,   321,   124,   213,
     275,   275,   128,   129,   124,   346,   131,   275,   128,   129,
     206,   157,   131,   160,   339,   325,   237,   207,   211,   132,
     124,   340,    68,   184,   128,   129,   257,   326,   131,    68,
     124,   185,    56,    69,   128,   129,    65,   264,   131,   295,
     102,   184,    37,   265,   295,    38,   115,   234,   328,   185,
     296,   259,   238,   190,    57,   322,   144,   145,   146,   147,
     148,   270,   116,   117,   118,   119,   120,   121,   122,   123,
     116,   117,   118,   119,   120,   121,   122,   123,    58,   125,
     126,   127,    89,    59,   258,    90,   124,   125,   126,   127,
     128,   129,   130,   225,   131,   227,   169,    85,   303,   283,
     232,   233,    44,    45,    46,    47,    48,   116,   117,   118,
     119,   120,   121,   122,   123,   124,    63,   288,    60,   128,
     129,   210,    61,   131,   125,   126,   127,   278,    62,    68,
     152,     2,    64,   153,    66,     3,     4,   335,    73,   337,
      72,     5,     6,     7,     8,     9,    10,    11,    67,   268,
     316,    12,    13,    14,   158,    74,   349,   159,    42,    15,
      16,    43,    75,    44,    45,    46,    47,    48,   311,    17,
     124,   312,   278,    76,   128,   129,   248,    77,   131,    78,
      79,   124,   343,    81,   345,   128,   129,   287,    80,   131,
      82,    83,    84,   124,    92,    87,   353,   128,   129,   315,
     124,   131,    88,    93,   128,   129,   342,   124,   131,    91,
      95,   128,   129,   352,   124,   131,    97,    98,   128,   129,
     101,   104,   131,    44,    45,    46,    47,    48,   111,   112,
     113,   137,   138,   139,   141,   142,   150,   151,   154,   155,
     162,   164,   168,   172,   115,   181,   196,   183,   193,   189,
     195,   198,   199,   203,   200,   204,   215,   217,   220,   218,
     222,   224,   221,   226,   229,   230,   235,   239,   241,   242,
     243,   244,   251,   228,   253,   245,   247,   267,   271,   269,
     262,   263,   266,   277,   280,   285,   282,   281,   286,   254,
     293,   298,   304,   327,   290,   299,   295,   329,   344,   306,
     330,   331,   348,   284,   219,   292,   354,   301,   236,   308,
     309,   202,   182,   216,   310,   317,   179,   175,   302,   260,
     350,   313,     0,     0,   318,   212,   332,     0,   334,   336,
       0,     0,   338,   314,     0,     0,   351,     0,     0,     0,
       0,   355
};

static const yytype_int16 yycheck[] =
{
      93,    68,   130,   108,   132,    43,   134,   212,   149,   154,
     136,    10,   138,    17,   271,   289,    11,    11,   307,    19,
     113,   309,   310,    11,    17,    47,   234,    18,     7,     8,
     238,    18,   321,    63,     3,   140,    30,    59,    60,    30,
      17,   292,    30,    30,   170,   138,   334,    85,   174,    87,
      17,   340,    17,    53,     6,    31,     8,   308,    57,    63,
      55,    55,    61,    62,    57,   339,    65,    55,    61,    62,
      63,   109,    65,   111,   331,    18,   204,   170,   171,   172,
      57,   332,    19,    48,    61,    62,    63,    30,    65,    19,
      57,    56,    33,    30,    61,    62,     8,   242,    65,    19,
      30,    48,     6,   244,    19,     9,    17,   200,   313,    56,
      30,   237,   205,   151,    63,    30,    23,    24,    25,    26,
      27,   249,    41,    42,    43,    44,    45,    46,    47,    48,
      41,    42,    43,    44,    45,    46,    47,    48,     3,    58,
      59,    60,    63,     3,   237,    66,    57,    58,    59,    60,
      61,    62,    63,   191,    65,   193,    30,    63,   286,   264,
     198,   199,    68,    69,    70,    71,    72,    41,    42,    43,
      44,    45,    46,    47,    48,    57,    39,   270,     3,    61,
      62,    63,     3,    65,    58,    59,    60,   254,     3,    19,
      63,     0,    63,    66,    63,     4,     5,   325,    17,   327,
      33,    10,    11,    12,    13,    14,    15,    16,    63,   247,
     303,    20,    21,    22,    63,     3,   344,    66,    63,    28,
      29,    66,     3,    68,    69,    70,    71,    72,    63,    38,
      57,    66,   299,    33,    61,    62,    63,    63,    65,    63,
      36,    57,   335,    17,   337,    61,    62,    63,    40,    65,
      63,     3,     3,    57,    32,    63,   349,    61,    62,    63,
      57,    65,    63,    34,    61,    62,    63,    57,    65,    63,
      63,    61,    62,    63,    57,    65,    65,    63,    61,    62,
      37,    17,    65,    68,    69,    70,    71,    72,    18,     3,
      17,     3,    41,    19,    31,    19,    63,    63,    63,    54,
      19,    10,    48,    35,    17,     6,    51,    18,    18,    17,
      63,    18,    18,     3,    19,    63,    33,     3,     3,    63,
      61,    63,    57,    63,    52,    50,    18,    30,    30,    63,
       3,    18,    63,    37,    52,    19,    18,    18,    35,    30,
      63,    63,    63,    63,     3,     3,    18,    30,    63,    49,
      19,    17,    30,    18,    53,    35,    19,    18,    18,    54,
      30,    37,    30,   266,   182,    63,    30,    63,   202,    63,
      63,   162,   142,   179,    63,    63,   139,   135,   283,   237,
     346,   299,    -1,    -1,    63,   172,    63,    -1,    63,    63,
      -1,    -1,    63,   302,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    63
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    74,     0,     4,     5,    10,    11,    12,    13,    14,
      15,    16,    20,    21,    22,    28,    29,    38,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      89,    90,    98,   103,   104,   107,   130,     6,     9,    87,
       6,     8,    63,    66,    68,    69,    70,    71,    72,   108,
     109,    63,     7,     8,     3,    31,    33,    63,     3,     3,
       3,     3,     3,    39,    63,     8,    63,    63,    19,    30,
     110,   110,    33,    17,     3,     3,    33,    63,    63,    36,
      40,    17,    63,     3,     3,    63,   109,    63,    63,    63,
      66,    63,    32,    34,   117,    63,   105,    65,    63,    92,
      97,    37,    30,   110,    17,   110,    19,    53,   113,    18,
      30,    18,     3,    17,    99,    17,    41,    42,    43,    44,
      45,    46,    47,    48,    57,    58,    59,    60,    61,    62,
      63,    65,   102,   119,   120,   122,   129,     3,    41,    19,
     106,    31,    19,    91,    23,    24,    25,    26,    27,    94,
      63,    63,    63,    66,    63,    54,   117,   110,    63,    66,
     110,   102,    19,   100,    10,    47,    59,    60,    48,    30,
     129,   129,    35,   118,   129,   108,   120,   102,   120,   105,
     117,     6,    92,    18,    48,    56,    96,    17,    96,    17,
     110,    18,    30,    18,   113,    63,    51,   126,    18,    18,
      19,   101,    99,     3,    63,    17,    63,   102,   120,   121,
      63,   102,   119,    63,   120,    33,   106,     3,    63,    91,
       3,    57,    61,    93,    63,   110,    63,   110,    37,    52,
      50,   111,   110,   110,   102,    18,   100,   129,   102,    30,
     118,    30,    63,     3,    18,    19,    88,    18,    63,   102,
     116,    63,   127,    52,    49,   123,   101,    63,   102,   120,
     121,   101,    63,    63,   113,    96,    63,    18,   110,    30,
     129,    35,   115,    11,    30,    55,    95,    63,   109,   125,
       3,    30,    18,   117,    88,     3,    63,    63,   102,   116,
      53,   114,    63,    19,   128,    19,    30,   112,    17,    35,
     124,    63,   111,   129,    30,   115,    54,    95,    63,    63,
      63,    63,    66,   125,   123,    63,   102,    63,    63,   128,
      30,    95,    30,   112,   112,    18,    30,    18,   118,    18,
      30,    37,    63,   128,    63,   129,    63,   129,    63,   116,
      95,   112,    63,   102,    18,   102,   115,   128,    30,   129,
     114,    63,    63,   102,    30,    63
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    73,    74,    74,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    87,    88,    88,    89,    90,    91,
      91,    92,    92,    92,    93,    94,    94,    94,    94,    95,
      95,    95,    96,    96,    96,    97,    98,    99,   100,   100,
     101,   101,   102,   102,   102,   102,   103,   104,   105,   105,
     106,   106,   107,   108,   108,   108,   108,   108,   108,   108,
     109,   109,   109,   109,   109,   110,   110,   110,   110,   110,
     110,   111,   111,   111,   112,   112,   112,   113,   113,   113,
     114,   114,   115,   115,   116,   116,   116,   116,   117,   117,
     118,   118,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   120,   121,   122,
     123,   123,   124,   124,   125,   125,   125,   125,   125,   125,
     125,   126,   126,   127,   127,   128,   128,   128,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   130
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
       0,     3,     3,     3,     3,     3,     5,     5,     7,     3,
       5,     3,     5,     3,     3,     5,     2,     9,     4,     2,
       0,     3,     0,     3,     6,     6,     8,     6,     8,     8,
      10,     0,     3,     3,     5,     0,     4,     6,     1,     1,
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
#line 248 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1771 "yacc_sql.tab.c"
    break;

  case 23:
#line 253 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1779 "yacc_sql.tab.c"
    break;

  case 24:
#line 258 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1787 "yacc_sql.tab.c"
    break;

  case 25:
#line 264 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1795 "yacc_sql.tab.c"
    break;

  case 26:
#line 270 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1803 "yacc_sql.tab.c"
    break;

  case 27:
#line 276 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1811 "yacc_sql.tab.c"
    break;

  case 28:
#line 282 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1820 "yacc_sql.tab.c"
    break;

  case 29:
#line 288 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1828 "yacc_sql.tab.c"
    break;

  case 30:
#line 294 "yacc_sql.y"
                                 {
      CONTEXT->ssql->flag = SCF_SHOW_INDEX;
	  show_index_init(&CONTEXT->ssql->sstr.show_index, (yyvsp[-1].string));
    }
#line 1837 "yacc_sql.tab.c"
    break;

  case 31:
#line 301 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1846 "yacc_sql.tab.c"
    break;

  case 32:
#line 309 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, (yyvsp[-3].string));
		}
#line 1856 "yacc_sql.tab.c"
    break;

  case 33:
#line 318 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 0);
		}
#line 1864 "yacc_sql.tab.c"
    break;

  case 34:
#line 322 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 1);
		}
#line 1872 "yacc_sql.tab.c"
    break;

  case 36:
#line 328 "yacc_sql.y"
                {
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, (yyvsp[-1].string));
		}
#line 1880 "yacc_sql.tab.c"
    break;

  case 37:
#line 334 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1889 "yacc_sql.tab.c"
    break;

  case 38:
#line 341 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1901 "yacc_sql.tab.c"
    break;

  case 40:
#line 351 "yacc_sql.y"
                                   {    }
#line 1907 "yacc_sql.tab.c"
    break;

  case 41:
#line 356 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1918 "yacc_sql.tab.c"
    break;

  case 42:
#line 363 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1929 "yacc_sql.tab.c"
    break;

  case 43:
#line 370 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, CHARS, 4096, (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1940 "yacc_sql.tab.c"
    break;

  case 44:
#line 378 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1946 "yacc_sql.tab.c"
    break;

  case 45:
#line 381 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1952 "yacc_sql.tab.c"
    break;

  case 46:
#line 382 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1958 "yacc_sql.tab.c"
    break;

  case 47:
#line 383 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1964 "yacc_sql.tab.c"
    break;

  case 48:
#line 384 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 1970 "yacc_sql.tab.c"
    break;

  case 49:
#line 387 "yacc_sql.y"
                { (yyval.number)=1; }
#line 1976 "yacc_sql.tab.c"
    break;

  case 50:
#line 388 "yacc_sql.y"
                  { (yyval.number)=1; }
#line 1982 "yacc_sql.tab.c"
    break;

  case 51:
#line 389 "yacc_sql.y"
              { (yyval.number)=0; }
#line 1988 "yacc_sql.tab.c"
    break;

  case 52:
#line 393 "yacc_sql.y"
                    { (yyval.number)=0; }
#line 1994 "yacc_sql.tab.c"
    break;

  case 53:
#line 394 "yacc_sql.y"
                          { (yyval.number)=0; }
#line 2000 "yacc_sql.tab.c"
    break;

  case 54:
#line 395 "yacc_sql.y"
                   { (yyval.number)=1; }
#line 2006 "yacc_sql.tab.c"
    break;

  case 55:
#line 399 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 2015 "yacc_sql.tab.c"
    break;

  case 56:
#line 408 "yacc_sql.y"
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
#line 2034 "yacc_sql.tab.c"
    break;

  case 57:
#line 424 "yacc_sql.y"
                                       {
		inserts_create_tuple(&CONTEXT->ssql->sstr.insertion, CONTEXT->values, CONTEXT->value_length);
		CONTEXT->value_length = 0;
	}
#line 2043 "yacc_sql.tab.c"
    break;

  case 59:
#line 431 "yacc_sql.y"
                                               {}
#line 2049 "yacc_sql.tab.c"
    break;

  case 61:
#line 435 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 2057 "yacc_sql.tab.c"
    break;

  case 62:
#line 440 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 2065 "yacc_sql.tab.c"
    break;

  case 63:
#line 443 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 2073 "yacc_sql.tab.c"
    break;

  case 64:
#line 446 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 2082 "yacc_sql.tab.c"
    break;

  case 65:
#line 450 "yacc_sql.y"
               {
		value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
	}
#line 2090 "yacc_sql.tab.c"
    break;

  case 66:
#line 457 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 2102 "yacc_sql.tab.c"
    break;

  case 67:
#line 467 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-5].string), CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 2112 "yacc_sql.tab.c"
    break;

  case 68:
#line 474 "yacc_sql.y"
                    {
		updates_attr_init(&CONTEXT->ssql->sstr.update, (yyvsp[-2].string), &CONTEXT->values[0]);
		CONTEXT->value_length = 0;
	}
#line 2121 "yacc_sql.tab.c"
    break;

  case 69:
#line 478 "yacc_sql.y"
                      {
		Query *query = (yyvsp[0].query);
		updates_attr_init_with_subquery(&CONTEXT->ssql->sstr.update, (yyvsp[-2].string), &query->sstr.selection);
	}
#line 2130 "yacc_sql.tab.c"
    break;

  case 71:
#line 484 "yacc_sql.y"
                                                         {}
#line 2136 "yacc_sql.tab.c"
    break;

  case 72:
#line 487 "yacc_sql.y"
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
#line 2159 "yacc_sql.tab.c"
    break;

  case 73:
#line 508 "yacc_sql.y"
                   {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2169 "yacc_sql.tab.c"
    break;

  case 74:
#line 513 "yacc_sql.y"
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2179 "yacc_sql.tab.c"
    break;

  case 75:
#line 518 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2189 "yacc_sql.tab.c"
    break;

  case 76:
#line 523 "yacc_sql.y"
                                                 {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2201 "yacc_sql.tab.c"
    break;

  case 77:
#line 530 "yacc_sql.y"
                                                        {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), "*");
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2213 "yacc_sql.tab.c"
    break;

  case 78:
#line 537 "yacc_sql.y"
                                               {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2225 "yacc_sql.tab.c"
    break;

  case 79:
#line 544 "yacc_sql.y"
                                                      {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2237 "yacc_sql.tab.c"
    break;

  case 80:
#line 553 "yacc_sql.y"
               {(yyval.number) = MIN;}
#line 2243 "yacc_sql.tab.c"
    break;

  case 81:
#line 554 "yacc_sql.y"
                   {(yyval.number) = MAX;}
#line 2249 "yacc_sql.tab.c"
    break;

  case 82:
#line 555 "yacc_sql.y"
                     {(yyval.number) = COUNT;}
#line 2255 "yacc_sql.tab.c"
    break;

  case 83:
#line 556 "yacc_sql.y"
                   {(yyval.number) = SUM;}
#line 2261 "yacc_sql.tab.c"
    break;

  case 84:
#line 557 "yacc_sql.y"
                   {(yyval.number) = AVG;}
#line 2267 "yacc_sql.tab.c"
    break;

  case 86:
#line 561 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2279 "yacc_sql.tab.c"
    break;

  case 87:
#line 568 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2291 "yacc_sql.tab.c"
    break;

  case 88:
#line 575 "yacc_sql.y"
                                                       {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2305 "yacc_sql.tab.c"
    break;

  case 89:
#line 584 "yacc_sql.y"
                                                     {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2319 "yacc_sql.tab.c"
    break;

  case 90:
#line 593 "yacc_sql.y"
                                                            {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2333 "yacc_sql.tab.c"
    break;

  case 92:
#line 604 "yacc_sql.y"
                                 {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2343 "yacc_sql.tab.c"
    break;

  case 93:
#line 609 "yacc_sql.y"
                                            {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2353 "yacc_sql.tab.c"
    break;

  case 95:
#line 617 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2365 "yacc_sql.tab.c"
    break;

  case 96:
#line 624 "yacc_sql.y"
                                     {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2377 "yacc_sql.tab.c"
    break;

  case 98:
#line 634 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2385 "yacc_sql.tab.c"
    break;

  case 99:
#line 637 "yacc_sql.y"
                                                          {	
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
	}
#line 2393 "yacc_sql.tab.c"
    break;

  case 101:
#line 643 "yacc_sql.y"
                                                          {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
		  }
#line 2401 "yacc_sql.tab.c"
    break;

  case 103:
#line 649 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
		  }
#line 2409 "yacc_sql.tab.c"
    break;

  case 104:
#line 655 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 2422 "yacc_sql.tab.c"
    break;

  case 105:
#line 664 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    	}
#line 2436 "yacc_sql.tab.c"
    break;

  case 106:
#line 674 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;		
    	}
#line 2451 "yacc_sql.tab.c"
    break;

  case 107:
#line 685 "yacc_sql.y"
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
#line 2473 "yacc_sql.tab.c"
    break;

  case 109:
#line 705 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2481 "yacc_sql.tab.c"
    break;

  case 111:
#line 711 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2489 "yacc_sql.tab.c"
    break;

  case 112:
#line 717 "yacc_sql.y"
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
#line 2514 "yacc_sql.tab.c"
    break;

  case 113:
#line 738 "yacc_sql.y"
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
#line 2538 "yacc_sql.tab.c"
    break;

  case 114:
#line 758 "yacc_sql.y"
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
#line 2562 "yacc_sql.tab.c"
    break;

  case 115:
#line 778 "yacc_sql.y"
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
#line 2588 "yacc_sql.tab.c"
    break;

  case 116:
#line 800 "yacc_sql.y"
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
#line 2613 "yacc_sql.tab.c"
    break;

  case 117:
#line 821 "yacc_sql.y"
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
#line 2638 "yacc_sql.tab.c"
    break;

  case 118:
#line 842 "yacc_sql.y"
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
#line 2660 "yacc_sql.tab.c"
    break;

  case 119:
#line 860 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));
		Query *right_subquery = (yyvsp[0].query);

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
#line 2675 "yacc_sql.tab.c"
    break;

  case 120:
#line 871 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
		Query *right_subquery = (yyvsp[0].query);

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
#line 2690 "yacc_sql.tab.c"
    break;

  case 121:
#line 882 "yacc_sql.y"
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
#line 2707 "yacc_sql.tab.c"
    break;

  case 122:
#line 895 "yacc_sql.y"
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
#line 2723 "yacc_sql.tab.c"
    break;

  case 123:
#line 907 "yacc_sql.y"
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
#line 2739 "yacc_sql.tab.c"
    break;

  case 124:
#line 918 "yacc_sql.y"
                                      {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

		Condition condition;
		condition_init_with_value_list(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, CONTEXT->value_list, CONTEXT->value_list_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		CONTEXT->value_list_length = 0;
	}
#line 2753 "yacc_sql.tab.c"
    break;

  case 125:
#line 927 "yacc_sql.y"
                                             {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));

		Condition condition;
		condition_init_with_value_list(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, CONTEXT->value_list, CONTEXT->value_list_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		CONTEXT->value_list_length = 0;
	}
#line 2767 "yacc_sql.tab.c"
    break;

  case 126:
#line 936 "yacc_sql.y"
                         {
		Query *right_subquery = (yyvsp[0].query);
		Condition condition;

		condition_init_with_subquery(&condition, CONTEXT->comp, NO_EXPR, NULL, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
#line 2780 "yacc_sql.tab.c"
    break;

  case 127:
#line 946 "yacc_sql.y"
                                                                             {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-5].string));

		selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

		CONTEXT->ssql->flag=SCF_SELECT;//"select";
		CONTEXT->ssql->sstr.selection.is_subquery=1;
		// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;
		(yyval.query) = CONTEXT->ssql;
        
		query_stack_pop(CONTEXT);
	}
#line 2797 "yacc_sql.tab.c"
    break;

  case 128:
#line 960 "yacc_sql.y"
                                       {
		subquery_create_value_list(CONTEXT->value_list, CONTEXT->values, CONTEXT->value_length);
		CONTEXT->value_list_length = CONTEXT->value_length;
		CONTEXT->value_length = 0;
	}
#line 2807 "yacc_sql.tab.c"
    break;

  case 129:
#line 967 "yacc_sql.y"
                 {
	// 将当前状态入栈
	query_stack_push(CONTEXT);
   }
#line 2816 "yacc_sql.tab.c"
    break;

  case 131:
#line 973 "yacc_sql.y"
                                                        {

	}
#line 2824 "yacc_sql.tab.c"
    break;

  case 133:
#line 979 "yacc_sql.y"
                                          {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2832 "yacc_sql.tab.c"
    break;

  case 134:
#line 985 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, "*");
		left_attr.aggr_type = (yyvsp[-5].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2848 "yacc_sql.tab.c"
    break;

  case 135:
#line 997 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-3].string));
		left_attr.aggr_type = (yyvsp[-5].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2864 "yacc_sql.tab.c"
    break;

  case 136:
#line 1009 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string));
		left_attr.aggr_type = (yyvsp[-7].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2880 "yacc_sql.tab.c"
    break;

  case 137:
#line 1021 "yacc_sql.y"
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
#line 2897 "yacc_sql.tab.c"
    break;

  case 138:
#line 1034 "yacc_sql.y"
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
#line 2914 "yacc_sql.tab.c"
    break;

  case 139:
#line 1047 "yacc_sql.y"
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
#line 2931 "yacc_sql.tab.c"
    break;

  case 140:
#line 1060 "yacc_sql.y"
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
#line 2948 "yacc_sql.tab.c"
    break;

  case 142:
#line 1075 "yacc_sql.y"
                          {	
		
			}
#line 2956 "yacc_sql.tab.c"
    break;

  case 143:
#line 1080 "yacc_sql.y"
                                {
			OrderAttr attr;
			order_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2966 "yacc_sql.tab.c"
    break;

  case 144:
#line 1085 "yacc_sql.y"
                                          {
			OrderAttr attr;
			order_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2976 "yacc_sql.tab.c"
    break;

  case 146:
#line 1093 "yacc_sql.y"
                                    {
			OrderAttr attr;
			order_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
      }
#line 2986 "yacc_sql.tab.c"
    break;

  case 147:
#line 1098 "yacc_sql.y"
                                           {
			OrderAttr attr;
			order_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
  	  }
#line 2996 "yacc_sql.tab.c"
    break;

  case 148:
#line 1106 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 3002 "yacc_sql.tab.c"
    break;

  case 149:
#line 1107 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 3008 "yacc_sql.tab.c"
    break;

  case 150:
#line 1108 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 3014 "yacc_sql.tab.c"
    break;

  case 151:
#line 1109 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 3020 "yacc_sql.tab.c"
    break;

  case 152:
#line 1110 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 3026 "yacc_sql.tab.c"
    break;

  case 153:
#line 1111 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 3032 "yacc_sql.tab.c"
    break;

  case 154:
#line 1112 "yacc_sql.y"
                 { CONTEXT->comp = LIKE; }
#line 3038 "yacc_sql.tab.c"
    break;

  case 155:
#line 1113 "yacc_sql.y"
                           { CONTEXT->comp = NOT_LIKE; }
#line 3044 "yacc_sql.tab.c"
    break;

  case 156:
#line 1115 "yacc_sql.y"
         { CONTEXT->comp = IS_NULL; }
#line 3050 "yacc_sql.tab.c"
    break;

  case 157:
#line 1116 "yacc_sql.y"
                   { CONTEXT->comp = IS_NOT_NULL; }
#line 3056 "yacc_sql.tab.c"
    break;

  case 158:
#line 1117 "yacc_sql.y"
                   {CONTEXT->comp = IN;}
#line 3062 "yacc_sql.tab.c"
    break;

  case 159:
#line 1118 "yacc_sql.y"
                             {CONTEXT->comp = NOT_IN;}
#line 3068 "yacc_sql.tab.c"
    break;

  case 160:
#line 1119 "yacc_sql.y"
                                 {CONTEXT->comp = NOT_EXISTS;}
#line 3074 "yacc_sql.tab.c"
    break;

  case 161:
#line 1120 "yacc_sql.y"
                       {CONTEXT->comp = EXISTS;}
#line 3080 "yacc_sql.tab.c"
    break;

  case 162:
#line 1125 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 3089 "yacc_sql.tab.c"
    break;


#line 3093 "yacc_sql.tab.c"

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
#line 1130 "yacc_sql.y"

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
