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
    NUMBER = 315,
    FLOAT = 316,
    ID = 317,
    PATH = 318,
    SSS = 319,
    STAR = 320,
    STRING_V = 321,
    MIN_AGGR = 322,
    MAX_AGGR = 323,
    COUNT_AGGR = 324,
    SUM_AGGR = 325,
    AVG_AGGR = 326
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 183 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
  char *position;
  Query*query;

#line 324 "yacc_sql.tab.c"

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
#define YYLAST   383

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  58
/* YYNRULES -- Number of rules.  */
#define YYNRULES  158
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  351

#define YYUNDEFTOK  2
#define YYMAXUTOK   326


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
      65,    66,    67,    68,    69,    70,    71
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   219,   219,   221,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   246,   251,   256,   262,   268,   274,   280,   286,
     292,   299,   306,   316,   319,   324,   325,   331,   338,   347,
     349,   353,   360,   367,   376,   379,   380,   381,   382,   385,
     386,   387,   391,   392,   393,   396,   405,   422,   427,   429,
     431,   433,   438,   441,   444,   448,   454,   464,   472,   477,
     478,   480,   500,   505,   510,   515,   522,   529,   536,   545,
     546,   547,   548,   549,   551,   553,   560,   567,   576,   585,
     595,   596,   601,   607,   609,   616,   624,   626,   629,   633,
     635,   639,   641,   646,   655,   665,   676,   695,   697,   701,
     703,   708,   729,   749,   769,   791,   812,   833,   851,   862,
     873,   886,   898,   910,   919,   930,   944,   951,   956,   957,
     961,   963,   968,   980,   992,  1004,  1017,  1030,  1043,  1057,
    1059,  1064,  1069,  1075,  1077,  1082,  1090,  1091,  1092,  1093,
    1094,  1095,  1096,  1097,  1099,  1100,  1101,  1102,  1106
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
  "INNER", "JOIN", "ASC", "NULLABLE", "NULL_", "IS", "IN_TOKEN", "NUMBER",
  "FLOAT", "ID", "PATH", "SSS", "STAR", "STRING_V", "MIN_AGGR", "MAX_AGGR",
  "COUNT_AGGR", "SUM_AGGR", "AVG_AGGR", "$accept", "commands", "command",
  "exit", "help", "sync", "begin", "commit", "rollback", "drop_table",
  "show_tables", "show_index", "desc_table", "create_index",
  "optional_unique", "create_index_attrs", "drop_index", "create_table",
  "attr_def_list", "attr_def", "number", "type", "order_type", "nullable",
  "ID_get", "insert", "insert_tuple", "insert_tuple_list", "value_list",
  "value", "delete", "update", "update_attr", "update_attr_list", "select",
  "select_attr", "aggregate", "attr_list", "group_by", "group_key_list",
  "rel_list", "join_list", "join_cond_list", "join_cond", "where",
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
     325,   326
};
# endif

#define YYPACT_NINF (-287)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -287,   131,  -287,    40,    95,    96,   -31,   245,    24,    13,
      20,    18,    94,   102,   129,   146,   154,    54,  -287,  -287,
    -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,  -287,  -287,  -287,  -287,  -287,  -287,    65,  -287,   130,
      78,   108,    17,   164,  -287,  -287,  -287,  -287,  -287,   156,
     181,   201,   205,   191,  -287,   152,   166,   198,  -287,  -287,
    -287,  -287,  -287,   196,   221,   188,   251,   252,   125,   194,
    -287,  -287,   195,   106,  -287,  -287,   197,   226,   227,   200,
     199,   202,   223,  -287,  -287,    51,   248,   164,     2,     7,
     249,   263,   253,    22,   265,   228,   254,   240,  -287,   255,
     217,   210,   213,  -287,   135,  -287,   214,   224,   227,   164,
     137,   164,  -287,    73,   258,   269,  -287,  -287,  -287,    70,
    -287,   132,   246,   132,    96,  -287,    73,   200,   227,   274,
     202,   264,  -287,  -287,    19,  -287,  -287,     0,   266,   164,
       8,   267,     2,   222,   235,  -287,   270,   271,  -287,   268,
     253,   287,  -287,   229,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,   -18,   244,  -287,    28,    62,    22,  -287,    -5,   260,
    -287,   254,   291,   233,   255,   293,   241,  -287,  -287,   237,
    -287,   238,  -287,   164,   239,   164,  -287,   262,   250,   256,
     164,   164,    73,   285,   258,  -287,   132,  -287,  -287,  -287,
       1,  -287,  -287,  -287,  -287,   275,  -287,   246,   277,  -287,
     242,  -287,  -287,   305,  -287,  -287,  -287,  -287,   292,   290,
    -287,   294,  -287,   124,   257,   259,   272,  -287,  -287,   268,
    -287,  -287,    34,   268,   261,  -287,   273,     2,  -287,    19,
     276,   295,   164,   284,   132,   280,     4,  -287,   278,   178,
     313,  -287,   288,  -287,  -287,  -287,   299,  -287,  -287,   227,
    -287,   290,   317,  -287,   279,   149,   124,   281,  -287,   282,
    -287,   303,    90,   307,   296,  -287,   283,  -287,   256,  -287,
    -287,   132,   297,  -287,   280,   289,  -287,    11,   286,  -287,
     298,   300,  -287,   170,   178,  -287,  -287,   272,   155,   301,
    -287,   302,   303,     5,    91,   306,    12,   308,   246,   310,
     309,  -287,  -287,   312,  -287,   304,   303,   311,  -287,  -287,
     132,   314,   132,  -287,  -287,   315,   124,    11,  -287,   306,
     161,   318,    73,  -287,   280,   303,  -287,   316,  -287,   132,
    -287,   281,  -287,   319,   169,  -287,  -287,   320,  -287,   321,
    -287
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
       0,     0,    84,    84,    79,    80,    81,    82,    83,     0,
       0,     0,     0,     0,    24,     0,     0,     0,    25,    26,
      27,    23,    22,     0,     0,     0,     0,     0,     0,     0,
      73,    72,     0,     0,    31,    29,     0,     0,   107,     0,
       0,     0,     0,    28,    37,    84,     0,    84,    96,     0,
       0,     0,     0,     0,     0,     0,    69,     0,    55,    39,
       0,     0,     0,    85,     0,    74,     0,     0,   107,    84,
       0,    84,    30,     0,    58,     0,    65,    62,    63,     0,
      64,     0,   109,     0,     0,    66,     0,     0,   107,     0,
       0,     0,    45,    46,    52,    47,    48,    52,     0,    84,
       0,     0,    96,     0,   139,    77,     0,     0,    75,    60,
       0,     0,   127,     0,   146,   147,   148,   149,   150,   151,
     152,     0,   154,   156,     0,     0,     0,   108,     0,     0,
      68,    69,     0,     0,    39,     0,     0,    54,    43,     0,
      42,     0,    86,    84,     0,    84,    97,     0,     0,    90,
      84,    84,     0,     0,    58,    56,     0,   153,   157,   155,
       0,   113,   111,   118,   123,   114,   112,   109,   120,   122,
       0,    70,    67,     0,    40,    38,    53,    44,     0,    35,
      88,     0,    87,     0,     0,     0,   128,    78,    76,    60,
      57,    59,     0,    60,     0,   110,     0,    96,   158,    52,
       0,     0,    84,     0,     0,   101,    49,   140,     0,     0,
       0,    61,     0,   115,   119,   124,     0,   116,   121,   107,
      41,    35,     0,    89,     0,     0,     0,    99,    51,     0,
      50,   143,    93,     0,   130,    71,     0,   126,    90,    36,
      32,     0,     0,   103,   101,     0,    98,    49,     0,   141,
       0,     0,    91,     0,     0,   129,   117,   128,     0,     0,
     102,     0,   143,    49,    93,    93,     0,     0,   109,     0,
       0,   104,   105,     0,   142,     0,   143,     0,    94,    92,
       0,     0,     0,   131,   125,     0,     0,    49,   144,    93,
       0,     0,     0,   106,   101,   143,    95,   135,   133,     0,
     132,    99,   145,     0,     0,   100,   136,   137,   134,     0,
     138
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,  -287,
    -287,  -287,  -287,  -287,  -287,    68,  -287,  -287,   158,   203,
    -287,  -287,  -263,  -131,  -287,  -287,   180,   143,  -201,   -93,
    -287,  -287,   215,   176,  -287,   230,   -67,   -35,    74,  -286,
    -135,    10,  -271,  -257,  -105,  -202,   187,  -154,   123,  -287,
      59,  -287,    63,  -287,  -287,  -248,  -119,  -287
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    39,   241,    30,    31,   131,    99,
     218,   137,   271,   178,   100,    32,   114,   151,   193,   244,
      33,    34,    96,   128,    35,    49,    50,    70,   226,   292,
     108,   286,   267,   245,    94,   167,   122,   123,   204,   124,
     250,   295,   274,   189,   247,   289,   164,    36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     121,    86,   165,   144,   168,   235,   180,   186,    71,   284,
     203,   152,   115,   300,   209,   268,   268,   179,   318,   319,
     149,   106,   268,   172,   302,   109,   183,    54,   251,   197,
     320,    51,   256,   170,   269,   315,    68,   110,   184,   115,
     316,   198,   321,   336,    55,   200,    37,    69,   176,    38,
     103,   200,   105,    56,   314,   107,   177,   208,   116,   270,
     270,   117,   118,   341,   335,   120,   270,   176,   328,   334,
      68,   202,   206,   121,   145,   177,   148,   232,   254,   116,
      57,   102,   117,   118,   119,   116,   120,   342,   117,   118,
     201,   116,   120,    63,   117,   118,   252,    58,   120,   229,
     153,    40,   259,    41,   182,    59,   323,   233,   260,   290,
     290,   154,   155,   156,   157,   158,   159,   160,   161,   116,
     291,   317,   117,   118,   205,   265,   120,    64,   162,   163,
     116,     2,    60,   117,   118,     3,     4,   120,    65,   253,
      66,     5,     6,     7,     8,     9,    10,    11,   220,    61,
     222,    12,    13,    14,   278,   227,   228,    62,    42,    15,
      16,    43,   298,    44,    45,    46,    47,    48,    89,    17,
      67,    90,   283,   154,   155,   156,   157,   158,   159,   160,
     161,   116,   273,    68,   117,   118,   243,    85,   120,    72,
     162,   163,    44,    45,    46,    47,    48,   140,    73,   146,
     141,   330,   147,   332,    74,   311,   116,   263,    75,   117,
     118,   282,   116,   120,    77,   117,   118,   310,   116,   120,
     344,   117,   118,   337,    76,   120,   116,   273,    78,   117,
     118,   347,   306,   120,    79,   307,    80,   338,    81,   340,
     132,   133,   134,   135,   136,    44,    45,    46,    47,    48,
      82,   348,    52,    53,    83,    84,    87,    88,    92,    91,
     101,    93,    95,    97,    98,   104,   112,   111,   125,   126,
     113,   129,   138,   127,   130,   139,   142,   150,   143,   152,
     173,   166,   175,   181,   187,   185,   188,   192,   190,   191,
     195,   196,   199,   210,   212,   213,   215,   217,   216,   223,
     219,   221,   224,   230,   237,   234,   225,   236,   238,   240,
     239,   248,   242,   262,   264,   266,   275,   277,   276,   246,
     280,   249,   288,   257,   293,   290,   322,   299,   324,   279,
     194,   294,   214,   174,   285,   258,   339,   231,   261,   325,
     272,   281,   171,   301,   287,   296,   343,   211,   303,   326,
     349,   345,   297,   207,   169,   255,   309,   308,     0,     0,
     304,     0,   305,   312,   313,     0,   327,     0,     0,     0,
       0,     0,     0,   329,     0,     0,   331,   333,     0,     0,
       0,   346,     0,   350
};

static const yytype_int16 yycheck[] =
{
      93,    68,   121,   108,   123,   207,   137,   142,    43,   266,
     164,    10,    17,   284,   168,    11,    11,    17,   304,   305,
     113,    19,    11,   128,   287,    18,    18,     3,   229,    47,
      18,    62,   233,   126,    30,    30,    19,    30,    30,    17,
     303,    59,    30,   329,    31,    17,     6,    30,    48,     9,
      85,    17,    87,    33,   302,    53,    56,    62,    57,    55,
      55,    60,    61,   334,   327,    64,    55,    48,   316,   326,
      19,   164,   165,   166,   109,    56,   111,   196,   232,    57,
      62,    30,    60,    61,    62,    57,    64,   335,    60,    61,
      62,    57,    64,    39,    60,    61,    62,     3,    64,   192,
      30,     6,   237,     8,   139,     3,   308,   200,   239,    19,
      19,    41,    42,    43,    44,    45,    46,    47,    48,    57,
      30,    30,    60,    61,    62,   244,    64,    62,    58,    59,
      57,     0,     3,    60,    61,     4,     5,    64,     8,   232,
      62,    10,    11,    12,    13,    14,    15,    16,   183,     3,
     185,    20,    21,    22,   259,   190,   191,     3,    62,    28,
      29,    65,   281,    67,    68,    69,    70,    71,    62,    38,
      62,    65,   265,    41,    42,    43,    44,    45,    46,    47,
      48,    57,   249,    19,    60,    61,    62,    62,    64,    33,
      58,    59,    67,    68,    69,    70,    71,    62,    17,    62,
      65,   320,    65,   322,     3,   298,    57,   242,     3,    60,
      61,    62,    57,    64,    62,    60,    61,    62,    57,    64,
     339,    60,    61,    62,    33,    64,    57,   294,    62,    60,
      61,    62,    62,    64,    36,    65,    40,   330,    17,   332,
      23,    24,    25,    26,    27,    67,    68,    69,    70,    71,
      62,   344,     7,     8,     3,     3,    62,    62,    32,    62,
      37,    34,    62,    64,    62,    17,     3,    18,     3,    41,
      17,    31,    62,    19,    19,    62,    62,    19,    54,    10,
       6,    35,    18,    17,    62,    18,    51,    19,    18,    18,
       3,    62,    48,    33,     3,    62,     3,    60,    57,    37,
      62,    62,    52,    18,    62,    30,    50,    30,     3,    19,
      18,    52,    18,    18,    30,    35,     3,    18,    30,    62,
       3,    49,    19,    62,    17,    19,    18,    30,    18,   261,
     150,    35,   174,   130,    53,    62,    18,   194,    62,    30,
      62,    62,   127,    54,    62,    62,    30,   171,    62,    37,
      30,   341,   278,   166,   124,   232,   297,   294,    -1,    -1,
      62,    -1,    62,    62,    62,    -1,    62,    -1,    -1,    -1,
      -1,    -1,    -1,    62,    -1,    -1,    62,    62,    -1,    -1,
      -1,    62,    -1,    62
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    73,     0,     4,     5,    10,    11,    12,    13,    14,
      15,    16,    20,    21,    22,    28,    29,    38,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      88,    89,    97,   102,   103,   106,   129,     6,     9,    86,
       6,     8,    62,    65,    67,    68,    69,    70,    71,   107,
     108,    62,     7,     8,     3,    31,    33,    62,     3,     3,
       3,     3,     3,    39,    62,     8,    62,    62,    19,    30,
     109,   109,    33,    17,     3,     3,    33,    62,    62,    36,
      40,    17,    62,     3,     3,    62,   108,    62,    62,    62,
      65,    62,    32,    34,   116,    62,   104,    64,    62,    91,
      96,    37,    30,   109,    17,   109,    19,    53,   112,    18,
      30,    18,     3,    17,    98,    17,    57,    60,    61,    62,
      64,   101,   118,   119,   121,     3,    41,    19,   105,    31,
      19,    90,    23,    24,    25,    26,    27,    93,    62,    62,
      62,    65,    62,    54,   116,   109,    62,    65,   109,   101,
      19,    99,    10,    30,    41,    42,    43,    44,    45,    46,
      47,    48,    58,    59,   128,   128,    35,   117,   128,   107,
     101,   104,   116,     6,    91,    18,    48,    56,    95,    17,
      95,    17,   109,    18,    30,    18,   112,    62,    51,   125,
      18,    18,    19,   100,    98,     3,    62,    47,    59,    48,
      17,    62,   101,   119,   120,    62,   101,   118,    62,   119,
      33,   105,     3,    62,    90,     3,    57,    60,    92,    62,
     109,    62,   109,    37,    52,    50,   110,   109,   109,   101,
      18,    99,   128,   101,    30,   117,    30,    62,     3,    18,
      19,    87,    18,    62,   101,   115,    62,   126,    52,    49,
     122,   100,    62,   101,   119,   120,   100,    62,    62,   112,
      95,    62,    18,   109,    30,   128,    35,   114,    11,    30,
      55,    94,    62,   108,   124,     3,    30,    18,   116,    87,
       3,    62,    62,   101,   115,    53,   113,    62,    19,   127,
      19,    30,   111,    17,    35,   123,    62,   110,   128,    30,
     114,    54,    94,    62,    62,    62,    62,    65,   124,   122,
      62,   101,    62,    62,   127,    30,    94,    30,   111,   111,
      18,    30,    18,   117,    18,    30,    37,    62,   127,    62,
     128,    62,   128,    62,   115,    94,   111,    62,   101,    18,
     101,   114,   127,    30,   128,   113,    62,    62,   101,    30,
      62
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    72,    73,    73,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    86,    87,    87,    88,    89,    90,
      90,    91,    91,    91,    92,    93,    93,    93,    93,    94,
      94,    94,    95,    95,    95,    96,    97,    98,    99,    99,
     100,   100,   101,   101,   101,   101,   102,   103,   104,   105,
     105,   106,   107,   107,   107,   107,   107,   107,   107,   108,
     108,   108,   108,   108,   109,   109,   109,   109,   109,   109,
     110,   110,   110,   111,   111,   111,   112,   112,   112,   113,
     113,   114,   114,   115,   115,   115,   115,   116,   116,   117,
     117,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   119,   120,   121,   122,   122,
     123,   123,   124,   124,   124,   124,   124,   124,   124,   125,
     125,   126,   126,   127,   127,   127,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   129
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
       0,     3,     1,     1,     1,     1,     5,     7,     3,     0,
       3,    10,     2,     2,     4,     5,     7,     5,     7,     1,
       1,     1,     1,     1,     0,     3,     5,     6,     6,     8,
       0,     4,     6,     0,     3,     5,     0,     3,     7,     0,
       7,     0,     3,     3,     5,     5,     7,     0,     3,     0,
       3,     3,     3,     3,     3,     5,     5,     7,     3,     5,
       3,     5,     3,     3,     5,     9,     4,     2,     0,     3,
       0,     3,     6,     6,     8,     6,     8,     8,    10,     0,
       3,     3,     5,     0,     4,     6,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     2,     1,     2,     8
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
#line 246 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1762 "yacc_sql.tab.c"
    break;

  case 23:
#line 251 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1770 "yacc_sql.tab.c"
    break;

  case 24:
#line 256 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1778 "yacc_sql.tab.c"
    break;

  case 25:
#line 262 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1786 "yacc_sql.tab.c"
    break;

  case 26:
#line 268 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1794 "yacc_sql.tab.c"
    break;

  case 27:
#line 274 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1802 "yacc_sql.tab.c"
    break;

  case 28:
#line 280 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1811 "yacc_sql.tab.c"
    break;

  case 29:
#line 286 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1819 "yacc_sql.tab.c"
    break;

  case 30:
#line 292 "yacc_sql.y"
                                 {
      CONTEXT->ssql->flag = SCF_SHOW_INDEX;
	  show_index_init(&CONTEXT->ssql->sstr.show_index, (yyvsp[-1].string));
    }
#line 1828 "yacc_sql.tab.c"
    break;

  case 31:
#line 299 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1837 "yacc_sql.tab.c"
    break;

  case 32:
#line 307 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, (yyvsp[-3].string));
		}
#line 1847 "yacc_sql.tab.c"
    break;

  case 33:
#line 316 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 0);
		}
#line 1855 "yacc_sql.tab.c"
    break;

  case 34:
#line 320 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 1);
		}
#line 1863 "yacc_sql.tab.c"
    break;

  case 36:
#line 326 "yacc_sql.y"
                {
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, (yyvsp[-1].string));
		}
#line 1871 "yacc_sql.tab.c"
    break;

  case 37:
#line 332 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1880 "yacc_sql.tab.c"
    break;

  case 38:
#line 339 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1892 "yacc_sql.tab.c"
    break;

  case 40:
#line 349 "yacc_sql.y"
                                   {    }
#line 1898 "yacc_sql.tab.c"
    break;

  case 41:
#line 354 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1909 "yacc_sql.tab.c"
    break;

  case 42:
#line 361 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1920 "yacc_sql.tab.c"
    break;

  case 43:
#line 368 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, CHARS, 4096, (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1931 "yacc_sql.tab.c"
    break;

  case 44:
#line 376 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1937 "yacc_sql.tab.c"
    break;

  case 45:
#line 379 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1943 "yacc_sql.tab.c"
    break;

  case 46:
#line 380 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1949 "yacc_sql.tab.c"
    break;

  case 47:
#line 381 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1955 "yacc_sql.tab.c"
    break;

  case 48:
#line 382 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 1961 "yacc_sql.tab.c"
    break;

  case 49:
#line 385 "yacc_sql.y"
                { (yyval.number)=1; }
#line 1967 "yacc_sql.tab.c"
    break;

  case 50:
#line 386 "yacc_sql.y"
                  { (yyval.number)=1; }
#line 1973 "yacc_sql.tab.c"
    break;

  case 51:
#line 387 "yacc_sql.y"
              { (yyval.number)=0; }
#line 1979 "yacc_sql.tab.c"
    break;

  case 52:
#line 391 "yacc_sql.y"
                    { (yyval.number)=0; }
#line 1985 "yacc_sql.tab.c"
    break;

  case 53:
#line 392 "yacc_sql.y"
                          { (yyval.number)=0; }
#line 1991 "yacc_sql.tab.c"
    break;

  case 54:
#line 393 "yacc_sql.y"
                   { (yyval.number)=1; }
#line 1997 "yacc_sql.tab.c"
    break;

  case 55:
#line 397 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 2006 "yacc_sql.tab.c"
    break;

  case 56:
#line 406 "yacc_sql.y"
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
#line 2025 "yacc_sql.tab.c"
    break;

  case 57:
#line 422 "yacc_sql.y"
                                       {
		inserts_create_tuple(&CONTEXT->ssql->sstr.insertion, CONTEXT->values, CONTEXT->value_length);
		CONTEXT->value_length = 0;
	}
#line 2034 "yacc_sql.tab.c"
    break;

  case 59:
#line 429 "yacc_sql.y"
                                               {}
#line 2040 "yacc_sql.tab.c"
    break;

  case 61:
#line 433 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 2048 "yacc_sql.tab.c"
    break;

  case 62:
#line 438 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 2056 "yacc_sql.tab.c"
    break;

  case 63:
#line 441 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 2064 "yacc_sql.tab.c"
    break;

  case 64:
#line 444 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 2073 "yacc_sql.tab.c"
    break;

  case 65:
#line 448 "yacc_sql.y"
               {
		value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
	}
#line 2081 "yacc_sql.tab.c"
    break;

  case 66:
#line 455 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 2093 "yacc_sql.tab.c"
    break;

  case 67:
#line 465 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-5].string), CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 2103 "yacc_sql.tab.c"
    break;

  case 68:
#line 472 "yacc_sql.y"
                    {
		updates_attr_init(&CONTEXT->ssql->sstr.update, (yyvsp[-2].string), &CONTEXT->values[0]);
		CONTEXT->value_length = 0;
	}
#line 2112 "yacc_sql.tab.c"
    break;

  case 70:
#line 478 "yacc_sql.y"
                                                         {}
#line 2118 "yacc_sql.tab.c"
    break;

  case 71:
#line 481 "yacc_sql.y"
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-6].string));

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			CONTEXT->ssql->sstr.selection.is_subquery=0;
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
#line 2139 "yacc_sql.tab.c"
    break;

  case 72:
#line 500 "yacc_sql.y"
                   {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2149 "yacc_sql.tab.c"
    break;

  case 73:
#line 505 "yacc_sql.y"
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2159 "yacc_sql.tab.c"
    break;

  case 74:
#line 510 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2169 "yacc_sql.tab.c"
    break;

  case 75:
#line 515 "yacc_sql.y"
                                                 {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2181 "yacc_sql.tab.c"
    break;

  case 76:
#line 522 "yacc_sql.y"
                                                        {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), "*");
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2193 "yacc_sql.tab.c"
    break;

  case 77:
#line 529 "yacc_sql.y"
                                               {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2205 "yacc_sql.tab.c"
    break;

  case 78:
#line 536 "yacc_sql.y"
                                                      {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2217 "yacc_sql.tab.c"
    break;

  case 79:
#line 545 "yacc_sql.y"
               {(yyval.number) = MIN;}
#line 2223 "yacc_sql.tab.c"
    break;

  case 80:
#line 546 "yacc_sql.y"
                   {(yyval.number) = MAX;}
#line 2229 "yacc_sql.tab.c"
    break;

  case 81:
#line 547 "yacc_sql.y"
                     {(yyval.number) = COUNT;}
#line 2235 "yacc_sql.tab.c"
    break;

  case 82:
#line 548 "yacc_sql.y"
                   {(yyval.number) = SUM;}
#line 2241 "yacc_sql.tab.c"
    break;

  case 83:
#line 549 "yacc_sql.y"
                   {(yyval.number) = AVG;}
#line 2247 "yacc_sql.tab.c"
    break;

  case 85:
#line 553 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2259 "yacc_sql.tab.c"
    break;

  case 86:
#line 560 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2271 "yacc_sql.tab.c"
    break;

  case 87:
#line 567 "yacc_sql.y"
                                                       {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2285 "yacc_sql.tab.c"
    break;

  case 88:
#line 576 "yacc_sql.y"
                                                     {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2299 "yacc_sql.tab.c"
    break;

  case 89:
#line 585 "yacc_sql.y"
                                                            {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2313 "yacc_sql.tab.c"
    break;

  case 91:
#line 596 "yacc_sql.y"
                                 {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2323 "yacc_sql.tab.c"
    break;

  case 92:
#line 601 "yacc_sql.y"
                                            {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2333 "yacc_sql.tab.c"
    break;

  case 94:
#line 609 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2345 "yacc_sql.tab.c"
    break;

  case 95:
#line 616 "yacc_sql.y"
                                     {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2357 "yacc_sql.tab.c"
    break;

  case 97:
#line 626 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2365 "yacc_sql.tab.c"
    break;

  case 98:
#line 629 "yacc_sql.y"
                                                          {	
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
	}
#line 2373 "yacc_sql.tab.c"
    break;

  case 100:
#line 635 "yacc_sql.y"
                                                          {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
		  }
#line 2381 "yacc_sql.tab.c"
    break;

  case 102:
#line 641 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
		  }
#line 2389 "yacc_sql.tab.c"
    break;

  case 103:
#line 647 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 2402 "yacc_sql.tab.c"
    break;

  case 104:
#line 656 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    	}
#line 2416 "yacc_sql.tab.c"
    break;

  case 105:
#line 666 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;		
    	}
#line 2431 "yacc_sql.tab.c"
    break;

  case 106:
#line 677 "yacc_sql.y"
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
#line 2453 "yacc_sql.tab.c"
    break;

  case 108:
#line 697 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2461 "yacc_sql.tab.c"
    break;

  case 110:
#line 703 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2469 "yacc_sql.tab.c"
    break;

  case 111:
#line 709 "yacc_sql.y"
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
#line 2494 "yacc_sql.tab.c"
    break;

  case 112:
#line 730 "yacc_sql.y"
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
#line 2518 "yacc_sql.tab.c"
    break;

  case 113:
#line 750 "yacc_sql.y"
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
#line 2542 "yacc_sql.tab.c"
    break;

  case 114:
#line 770 "yacc_sql.y"
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
#line 2568 "yacc_sql.tab.c"
    break;

  case 115:
#line 792 "yacc_sql.y"
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
#line 2593 "yacc_sql.tab.c"
    break;

  case 116:
#line 813 "yacc_sql.y"
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
#line 2618 "yacc_sql.tab.c"
    break;

  case 117:
#line 834 "yacc_sql.y"
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
#line 2640 "yacc_sql.tab.c"
    break;

  case 118:
#line 852 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));
		Query *right_subquery = (yyvsp[0].query);

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
#line 2655 "yacc_sql.tab.c"
    break;

  case 119:
#line 863 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
		Query *right_subquery = (yyvsp[0].query);

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
#line 2670 "yacc_sql.tab.c"
    break;

  case 120:
#line 874 "yacc_sql.y"
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
#line 2687 "yacc_sql.tab.c"
    break;

  case 121:
#line 887 "yacc_sql.y"
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
#line 2703 "yacc_sql.tab.c"
    break;

  case 122:
#line 899 "yacc_sql.y"
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
#line 2719 "yacc_sql.tab.c"
    break;

  case 123:
#line 910 "yacc_sql.y"
                                      {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

		Condition condition;
		condition_init_with_value_list(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, CONTEXT->value_list, CONTEXT->value_list_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		CONTEXT->value_list_length = 0;
	}
#line 2733 "yacc_sql.tab.c"
    break;

  case 124:
#line 919 "yacc_sql.y"
                                             {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));

		Condition condition;
		condition_init_with_value_list(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, CONTEXT->value_list, CONTEXT->value_list_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		CONTEXT->value_list_length = 0;
	}
#line 2747 "yacc_sql.tab.c"
    break;

  case 125:
#line 930 "yacc_sql.y"
                                                                             {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-5].string));

		selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

		CONTEXT->ssql->flag=SCF_SELECT;//"select";
		CONTEXT->ssql->sstr.selection.is_subquery=1;
		// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;
		(yyval.query) = CONTEXT->ssql;
        
		query_stack_pop(CONTEXT);
	}
#line 2764 "yacc_sql.tab.c"
    break;

  case 126:
#line 944 "yacc_sql.y"
                                       {
		subquery_create_value_list(CONTEXT->value_list, CONTEXT->values, CONTEXT->value_length);
		CONTEXT->value_list_length = CONTEXT->value_length;
		CONTEXT->value_length = 0;
	}
#line 2774 "yacc_sql.tab.c"
    break;

  case 127:
#line 951 "yacc_sql.y"
                 {
	// 将当前状态入栈
	query_stack_push(CONTEXT);
   }
#line 2783 "yacc_sql.tab.c"
    break;

  case 129:
#line 957 "yacc_sql.y"
                                                        {

	}
#line 2791 "yacc_sql.tab.c"
    break;

  case 131:
#line 963 "yacc_sql.y"
                                          {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2799 "yacc_sql.tab.c"
    break;

  case 132:
#line 969 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, "COUNT(*)");
		left_attr.aggr_type = COUNT;

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2815 "yacc_sql.tab.c"
    break;

  case 133:
#line 981 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-3].string));
		left_attr.aggr_type = (yyvsp[-5].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2831 "yacc_sql.tab.c"
    break;

  case 134:
#line 993 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string));
		left_attr.aggr_type = (yyvsp[-7].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2847 "yacc_sql.tab.c"
    break;

  case 135:
#line 1005 "yacc_sql.y"
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
#line 2864 "yacc_sql.tab.c"
    break;

  case 136:
#line 1018 "yacc_sql.y"
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
#line 2881 "yacc_sql.tab.c"
    break;

  case 137:
#line 1031 "yacc_sql.y"
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
#line 2898 "yacc_sql.tab.c"
    break;

  case 138:
#line 1044 "yacc_sql.y"
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
#line 2915 "yacc_sql.tab.c"
    break;

  case 140:
#line 1059 "yacc_sql.y"
                          {	
		
			}
#line 2923 "yacc_sql.tab.c"
    break;

  case 141:
#line 1064 "yacc_sql.y"
                                {
			OrderAttr attr;
			order_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2933 "yacc_sql.tab.c"
    break;

  case 142:
#line 1069 "yacc_sql.y"
                                          {
			OrderAttr attr;
			order_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2943 "yacc_sql.tab.c"
    break;

  case 144:
#line 1077 "yacc_sql.y"
                                    {
			OrderAttr attr;
			order_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
      }
#line 2953 "yacc_sql.tab.c"
    break;

  case 145:
#line 1082 "yacc_sql.y"
                                           {
			OrderAttr attr;
			order_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].number));
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
  	  }
#line 2963 "yacc_sql.tab.c"
    break;

  case 146:
#line 1090 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2969 "yacc_sql.tab.c"
    break;

  case 147:
#line 1091 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2975 "yacc_sql.tab.c"
    break;

  case 148:
#line 1092 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2981 "yacc_sql.tab.c"
    break;

  case 149:
#line 1093 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2987 "yacc_sql.tab.c"
    break;

  case 150:
#line 1094 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2993 "yacc_sql.tab.c"
    break;

  case 151:
#line 1095 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2999 "yacc_sql.tab.c"
    break;

  case 152:
#line 1096 "yacc_sql.y"
                 { CONTEXT->comp = LIKE; }
#line 3005 "yacc_sql.tab.c"
    break;

  case 153:
#line 1097 "yacc_sql.y"
                           { CONTEXT->comp = NOT_LIKE; }
#line 3011 "yacc_sql.tab.c"
    break;

  case 154:
#line 1099 "yacc_sql.y"
         { CONTEXT->comp = IS_NULL; }
#line 3017 "yacc_sql.tab.c"
    break;

  case 155:
#line 1100 "yacc_sql.y"
                   { CONTEXT->comp = IS_NOT_NULL; }
#line 3023 "yacc_sql.tab.c"
    break;

  case 156:
#line 1101 "yacc_sql.y"
                   {CONTEXT->comp = IN;}
#line 3029 "yacc_sql.tab.c"
    break;

  case 157:
#line 1102 "yacc_sql.y"
                             {CONTEXT->comp = NOT_IN;}
#line 3035 "yacc_sql.tab.c"
    break;

  case 158:
#line 1107 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 3044 "yacc_sql.tab.c"
    break;


#line 3048 "yacc_sql.tab.c"

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
#line 1112 "yacc_sql.y"

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
