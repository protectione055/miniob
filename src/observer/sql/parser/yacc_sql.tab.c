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
} ParserContext;

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
  printf("parse sql failed. error=%s", str);
}

ParserContext *get_context(yyscan_t scanner)
{
  return (ParserContext *)yyget_extra(scanner);
}

#define CONTEXT get_context(scanner)


#line 132 "yacc_sql.tab.c"

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
    BY = 306,
    NUMBER = 307,
    FLOAT = 308,
    ID = 309,
    PATH = 310,
    SSS = 311,
    STAR = 312,
    STRING_V = 313,
    MIN_AGGR = 314,
    MAX_AGGR = 315,
    COUNT_AGGR = 316,
    SUM_AGGR = 317,
    AVG_AGGR = 318
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
<<<<<<< HEAD
#line 118 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
  char *position;

#line 258 "yacc_sql.tab.c"

=======
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_CREATE = 4,                     /* CREATE  */
  YYSYMBOL_DROP = 5,                       /* DROP  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_TABLES = 7,                     /* TABLES  */
  YYSYMBOL_INDEX = 8,                      /* INDEX  */
  YYSYMBOL_UNIQUE = 9,                     /* UNIQUE  */
  YYSYMBOL_SELECT = 10,                    /* SELECT  */
  YYSYMBOL_DESC = 11,                      /* DESC  */
  YYSYMBOL_SHOW = 12,                      /* SHOW  */
  YYSYMBOL_SYNC = 13,                      /* SYNC  */
  YYSYMBOL_INSERT = 14,                    /* INSERT  */
  YYSYMBOL_DELETE = 15,                    /* DELETE  */
  YYSYMBOL_UPDATE = 16,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 17,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 18,                    /* RBRACE  */
  YYSYMBOL_COMMA = 19,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 20,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 21,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 22,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 23,                     /* INT_T  */
  YYSYMBOL_STRING_T = 24,                  /* STRING_T  */
  YYSYMBOL_TEXT_T = 25,                    /* TEXT_T  */
  YYSYMBOL_FLOAT_T = 26,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 27,                    /* DATE_T  */
  YYSYMBOL_HELP = 28,                      /* HELP  */
  YYSYMBOL_EXIT = 29,                      /* EXIT  */
  YYSYMBOL_DOT = 30,                       /* DOT  */
  YYSYMBOL_INTO = 31,                      /* INTO  */
  YYSYMBOL_VALUES = 32,                    /* VALUES  */
  YYSYMBOL_FROM = 33,                      /* FROM  */
  YYSYMBOL_WHERE = 34,                     /* WHERE  */
  YYSYMBOL_AND = 35,                       /* AND  */
  YYSYMBOL_SET = 36,                       /* SET  */
  YYSYMBOL_ON = 37,                        /* ON  */
  YYSYMBOL_LOAD = 38,                      /* LOAD  */
  YYSYMBOL_DATA = 39,                      /* DATA  */
  YYSYMBOL_INFILE = 40,                    /* INFILE  */
  YYSYMBOL_EQ = 41,                        /* EQ  */
  YYSYMBOL_LT = 42,                        /* LT  */
  YYSYMBOL_GT = 43,                        /* GT  */
  YYSYMBOL_LE = 44,                        /* LE  */
  YYSYMBOL_GE = 45,                        /* GE  */
  YYSYMBOL_NE = 46,                        /* NE  */
  YYSYMBOL_LIKE_TOKEN = 47,                /* LIKE_TOKEN  */
  YYSYMBOL_NOT_TOKEN = 48,                 /* NOT_TOKEN  */
  YYSYMBOL_AGGR = 49,                      /* AGGR  */
  YYSYMBOL_INNER = 50,                     /* INNER  */
  YYSYMBOL_JOIN = 51,                      /* JOIN  */
  YYSYMBOL_NUMBER = 52,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 53,                     /* FLOAT  */
  YYSYMBOL_ID = 54,                        /* ID  */
  YYSYMBOL_PATH = 55,                      /* PATH  */
  YYSYMBOL_SSS = 56,                       /* SSS  */
  YYSYMBOL_STAR = 57,                      /* STAR  */
  YYSYMBOL_STRING_V = 58,                  /* STRING_V  */
  YYSYMBOL_YYACCEPT = 59,                  /* $accept  */
  YYSYMBOL_commands = 60,                  /* commands  */
  YYSYMBOL_command = 61,                   /* command  */
  YYSYMBOL_exit = 62,                      /* exit  */
  YYSYMBOL_help = 63,                      /* help  */
  YYSYMBOL_sync = 64,                      /* sync  */
  YYSYMBOL_begin = 65,                     /* begin  */
  YYSYMBOL_commit = 66,                    /* commit  */
  YYSYMBOL_rollback = 67,                  /* rollback  */
  YYSYMBOL_drop_table = 68,                /* drop_table  */
  YYSYMBOL_show_tables = 69,               /* show_tables  */
  YYSYMBOL_show_index = 70,                /* show_index  */
  YYSYMBOL_desc_table = 71,                /* desc_table  */
  YYSYMBOL_create_index = 72,              /* create_index  */
  YYSYMBOL_optional_unique = 73,           /* optional_unique  */
  YYSYMBOL_create_index_attrs = 74,        /* create_index_attrs  */
  YYSYMBOL_drop_index = 75,                /* drop_index  */
  YYSYMBOL_create_table = 76,              /* create_table  */
  YYSYMBOL_attr_def_list = 77,             /* attr_def_list  */
  YYSYMBOL_attr_def = 78,                  /* attr_def  */
  YYSYMBOL_number = 79,                    /* number  */
  YYSYMBOL_type = 80,                      /* type  */
  YYSYMBOL_ID_get = 81,                    /* ID_get  */
  YYSYMBOL_insert = 82,                    /* insert  */
  YYSYMBOL_value_list = 83,                /* value_list  */
  YYSYMBOL_value = 84,                     /* value  */
  YYSYMBOL_delete = 85,                    /* delete  */
  YYSYMBOL_update = 86,                    /* update  */
  YYSYMBOL_select = 87,                    /* select  */
  YYSYMBOL_select_attr = 88,               /* select_attr  */
  YYSYMBOL_attr_list = 89,                 /* attr_list  */
  YYSYMBOL_rel_list = 90,                  /* rel_list  */
  YYSYMBOL_join_list = 91,                 /* join_list  */
  YYSYMBOL_join_cond_list = 92,            /* join_cond_list  */
  YYSYMBOL_join_cond = 93,                 /* join_cond  */
  YYSYMBOL_where = 94,                     /* where  */
  YYSYMBOL_condition_list = 95,            /* condition_list  */
  YYSYMBOL_condition = 96,                 /* condition  */
  YYSYMBOL_comOp = 97,                     /* comOp  */
  YYSYMBOL_load_data = 98                  /* load_data  */
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
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
<<<<<<< HEAD
#define YYLAST   255

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  64
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  47
/* YYNRULES -- Number of rules.  */
#define YYNRULES  121
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  263

#define YYUNDEFTOK  2
#define YYMAXUTOK   318
=======
#define YYLAST   202

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  59
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  95
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  210

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   313
>>>>>>> 16ea068 (implement join metadata from parse to do_select)


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
<<<<<<< HEAD
      55,    56,    57,    58,    59,    60,    61,    62,    63
=======
      55,    56,    57,    58
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
<<<<<<< HEAD
       0,   150,   150,   152,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   177,   182,   187,   193,   199,   205,   211,   217,
     223,   230,   237,   247,   250,   255,   256,   262,   269,   278,
     280,   284,   295,   306,   319,   322,   323,   324,   325,   328,
     337,   354,   359,   361,   363,   365,   370,   373,   376,   383,
     393,   401,   406,   407,   409,   428,   433,   438,   443,   450,
     457,   464,   473,   474,   475,   476,   477,   479,   481,   488,
     495,   504,   513,   523,   524,   529,   535,   537,   544,   552,
     554,   558,   560,   564,   566,   571,   592,   612,   632,   654,
     675,   696,   716,   717,   721,   723,   728,   740,   752,   764,
     777,   790,   803,   819,   820,   821,   822,   823,   824,   825,
     826,   830
=======
       0,   140,   140,   142,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   167,   172,   177,   183,   189,   195,   201,   207,
     213,   220,   227,   237,   240,   245,   246,   252,   259,   268,
     270,   274,   285,   296,   309,   312,   313,   314,   315,   318,
     327,   343,   345,   350,   353,   356,   363,   373,   383,   402,
     407,   412,   418,   420,   427,   436,   438,   441,   445,   447,
     451,   453,   458,   467,   477,   488,   500,   502,   506,   508,
     513,   534,   554,   574,   596,   617,   638,   660,   661,   662,
     663,   664,   665,   666,   667,   671
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
<<<<<<< HEAD
  "$end", "error", "$undefined", "SEMICOLON", "CREATE", "DROP", "TABLE",
  "TABLES", "INDEX", "UNIQUE", "SELECT", "DESC", "SHOW", "SYNC", "INSERT",
  "DELETE", "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN",
  "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T", "TEXT_T", "FLOAT_T",
  "DATE_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM", "WHERE",
  "AND", "SET", "ON", "LOAD", "DATA", "INFILE", "EQ", "LT", "GT", "LE",
  "GE", "NE", "LIKE_TOKEN", "NOT_TOKEN", "HAVING", "GROUP", "BY", "NUMBER",
  "FLOAT", "ID", "PATH", "SSS", "STAR", "STRING_V", "MIN_AGGR", "MAX_AGGR",
  "COUNT_AGGR", "SUM_AGGR", "AVG_AGGR", "$accept", "commands", "command",
  "exit", "help", "sync", "begin", "commit", "rollback", "drop_table",
  "show_tables", "show_index", "desc_table", "create_index",
  "optional_unique", "create_index_attrs", "drop_index", "create_table",
  "attr_def_list", "attr_def", "number", "type", "ID_get", "insert",
  "insert_tuple", "insert_tuple_list", "value_list", "value", "delete",
  "update", "update_attr", "update_attr_list", "select", "select_attr",
  "aggregate", "attr_list", "group_by", "group_key_list", "rel_list",
  "where", "condition_list", "condition", "having",
  "having_condition_list", "having_condition", "comOp", "load_data", YY_NULLPTR
=======
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "CREATE",
  "DROP", "TABLE", "TABLES", "INDEX", "UNIQUE", "SELECT", "DESC", "SHOW",
  "SYNC", "INSERT", "DELETE", "UPDATE", "LBRACE", "RBRACE", "COMMA",
  "TRX_BEGIN", "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T", "TEXT_T",
  "FLOAT_T", "DATE_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM",
  "WHERE", "AND", "SET", "ON", "LOAD", "DATA", "INFILE", "EQ", "LT", "GT",
  "LE", "GE", "NE", "LIKE_TOKEN", "NOT_TOKEN", "AGGR", "INNER", "JOIN",
  "NUMBER", "FLOAT", "ID", "PATH", "SSS", "STAR", "STRING_V", "$accept",
  "commands", "command", "exit", "help", "sync", "begin", "commit",
  "rollback", "drop_table", "show_tables", "show_index", "desc_table",
  "create_index", "optional_unique", "create_index_attrs", "drop_index",
  "create_table", "attr_def_list", "attr_def", "number", "type", "ID_get",
  "insert", "value_list", "value", "delete", "update", "select",
  "select_attr", "attr_list", "rel_list", "join_list", "join_cond_list",
  "join_cond", "where", "condition_list", "condition", "comOp",
  "load_data", YY_NULLPTR
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
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
<<<<<<< HEAD
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318
=======
     305,   306,   307,   308,   309,   310,   311,   312,   313
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
};
# endif

<<<<<<< HEAD
#define YYPACT_NINF (-220)
=======
#define YYPACT_NINF (-184)
>>>>>>> 16ea068 (implement join metadata from parse to do_select)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
<<<<<<< HEAD
    -220,     9,  -220,    -2,    27,    23,   -46,    81,    14,   -19,
      -6,   -20,    37,    59,    90,    91,    92,    25,  -220,  -220,
    -220,  -220,  -220,  -220,  -220,  -220,  -220,  -220,  -220,  -220,
    -220,  -220,  -220,  -220,  -220,  -220,  -220,    69,  -220,    71,
      75,    80,    13,   116,  -220,  -220,  -220,  -220,  -220,   103,
     122,   141,   143,   118,  -220,   102,   107,   130,  -220,  -220,
    -220,  -220,  -220,   128,   152,   117,   169,   170,    38,   120,
    -220,  -220,   121,     2,  -220,  -220,   123,   144,   145,   124,
     125,   126,   146,  -220,  -220,    22,   165,   116,   166,   -12,
     168,   181,   171,    68,   184,   148,   172,   159,  -220,   173,
      87,   139,   140,  -220,    24,  -220,   142,   145,   116,    33,
     116,  -220,     1,   176,  -220,  -220,    28,  -220,    61,   162,
    -220,     1,   124,   145,   192,   126,   182,  -220,  -220,  -220,
    -220,  -220,   185,   186,   116,    18,   183,   166,   149,  -220,
     187,   188,  -220,   189,   171,   201,   153,  -220,  -220,  -220,
    -220,  -220,  -220,  -220,   163,    89,    96,    68,  -220,  -220,
     172,   206,   157,   173,   209,   161,   160,  -220,   116,   164,
     116,  -220,   174,   167,   116,   116,     1,   197,   176,  -220,
      61,  -220,  -220,  -220,   190,  -220,   162,  -220,  -220,   214,
    -220,  -220,  -220,   203,   200,  -220,   204,  -220,   175,    56,
     220,  -220,  -220,   189,  -220,  -220,   101,   177,  -220,  -220,
    -220,   178,   208,   116,    30,   207,   193,  -220,  -220,   205,
    -220,  -220,   200,   224,  -220,   179,   180,  -220,    76,    56,
    -220,   191,  -220,  -220,    31,   211,    21,   218,   162,  -220,
     194,  -220,  -220,    61,   195,    61,  -220,   211,   106,   219,
       1,  -220,   210,  -220,    61,  -220,   196,   111,  -220,   212,
    -220,   198,  -220
=======
    -184,     6,  -184,    90,    23,    55,   -42,     0,    30,   -18,
     -19,   -15,    43,    44,    56,    57,    58,    33,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,  -184,  -184,    24,  -184,    71,
      26,    37,    11,    79,    78,   101,   114,    89,  -184,    73,
      75,    94,  -184,  -184,  -184,  -184,  -184,    86,   115,    77,
     130,   131,    81,    82,  -184,  -184,    83,  -184,  -184,    84,
     107,   106,    87,    88,    91,   105,  -184,  -184,    13,    79,
     -14,   140,   129,    21,   144,   108,   117,  -184,   132,    59,
      96,    98,  -184,  -184,    99,   103,   106,  -184,    72,  -184,
    -184,    10,  -184,    22,   120,  -184,    72,   150,    91,   139,
    -184,  -184,  -184,  -184,  -184,   141,   142,    79,   -14,   109,
     157,   143,   110,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
     118,    36,    41,    21,  -184,   106,   112,   132,   158,   116,
     113,  -184,  -184,   133,  -184,    72,   151,    22,  -184,  -184,
    -184,   145,  -184,   120,   168,   169,  -184,  -184,  -184,   155,
     159,    49,   143,   171,    54,   122,  -184,  -184,  -184,  -184,
     123,   161,   152,    22,   146,  -184,  -184,   153,  -184,  -184,
     159,   177,   134,    62,    49,   135,   136,  -184,  -184,    22,
     154,  -184,   146,   138,  -184,  -184,    67,   137,  -184,   147,
     156,  -184,  -184,   160,   148,    49,  -184,   146,   135,  -184
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    21,
      20,    15,    16,    17,    18,     9,    10,    11,    12,    13,
      14,     8,     5,     7,     6,     4,    19,     0,    34,     0,
<<<<<<< HEAD
       0,     0,    77,    77,    72,    73,    74,    75,    76,     0,
       0,     0,     0,     0,    24,     0,     0,     0,    25,    26,
      27,    23,    22,     0,     0,     0,     0,     0,     0,     0,
      66,    65,     0,     0,    31,    29,     0,     0,    91,     0,
       0,     0,     0,    28,    37,    77,     0,    77,    89,     0,
       0,     0,     0,     0,     0,     0,    62,     0,    49,    39,
       0,     0,     0,    78,     0,    67,     0,    91,    77,     0,
      77,    30,     0,    52,    56,    57,     0,    58,     0,    93,
      59,     0,     0,    91,     0,     0,     0,    45,    46,    43,
      47,    48,    42,     0,    77,     0,     0,    89,    83,    70,
       0,     0,    68,    54,     0,     0,     0,   113,   114,   115,
     116,   117,   118,   119,     0,     0,     0,     0,    92,    61,
      62,     0,     0,    39,     0,     0,     0,    79,    77,     0,
      77,    90,     0,   102,    77,    77,     0,     0,    52,    50,
       0,   120,    97,    95,    98,    96,    93,    63,    60,     0,
      40,    38,    44,     0,    35,    81,     0,    80,     0,     0,
       0,    71,    69,    54,    51,    53,     0,     0,    94,   121,
      41,     0,     0,    77,    86,     0,   104,    64,    55,     0,
      99,   100,    35,     0,    82,     0,     0,    84,     0,     0,
     103,     0,    36,    32,    86,    86,     0,     0,    93,   101,
       0,    87,    85,     0,     0,     0,   105,    86,     0,     0,
       0,    88,   109,   107,     0,   106,     0,     0,   110,   111,
     108,     0,   112
=======
       0,     0,    62,    62,     0,     0,     0,     0,    24,     0,
       0,     0,    25,    26,    27,    23,    22,     0,     0,     0,
       0,     0,     0,     0,    60,    59,     0,    31,    29,     0,
       0,    76,     0,     0,     0,     0,    28,    37,    62,    62,
      65,     0,     0,     0,     0,     0,     0,    49,    39,     0,
       0,     0,    63,    61,     0,     0,    76,    30,     0,    53,
      54,     0,    55,     0,    78,    56,     0,     0,     0,     0,
      45,    46,    43,    47,    48,    42,     0,    62,    65,     0,
       0,    51,     0,    87,    88,    89,    90,    91,    92,    93,
       0,     0,     0,     0,    77,    76,     0,    39,     0,     0,
       0,    64,    66,     0,    58,     0,     0,     0,    94,    82,
      80,    83,    81,    78,     0,     0,    40,    38,    44,     0,
      35,     0,    51,     0,     0,     0,    79,    57,    95,    41,
       0,     0,     0,     0,    70,    52,    50,     0,    84,    85,
      35,     0,     0,     0,     0,    68,     0,    36,    32,     0,
       0,    72,    70,     0,    67,    86,     0,     0,    71,     0,
       0,    73,    74,     0,     0,     0,    75,    70,    68,    69
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
<<<<<<< HEAD
    -220,  -220,  -220,  -220,  -220,  -220,  -220,  -220,  -220,  -220,
    -220,  -220,  -220,  -220,  -220,    16,  -220,  -220,    78,   114,
    -220,  -220,  -220,  -220,    99,    66,    43,  -110,  -220,  -220,
     129,    93,  -220,  -220,     0,   -43,  -220,  -219,   110,   -97,
    -183,    97,  -220,  -220,    26,  -117,  -220
=======
    -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,  -184,
    -184,  -184,  -184,  -184,  -184,     7,  -184,  -184,    61,    85,
    -184,  -184,  -184,  -184,    32,   -83,  -184,  -184,  -184,  -184,
     -41,    74,   -13,  -183,  -180,   -93,    46,    63,  -102,  -184
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
<<<<<<< HEAD
      26,    27,    28,    29,    39,   212,    30,    31,   126,    99,
     193,   132,   100,    32,   113,   145,   177,   118,    33,    34,
      96,   123,    35,    49,   215,    70,   173,   227,   107,    94,
     158,   119,   200,   230,   216,   155,    36
=======
      26,    27,    28,    29,    39,   171,    30,    31,   109,    88,
     159,   115,    89,    32,   146,   173,    33,    34,    35,    44,
      64,    96,   194,   185,   174,    84,   134,   104,   131,    36
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
<<<<<<< HEAD
      71,   156,   143,   208,    37,    50,   108,    38,    51,     2,
     138,   159,    55,     3,     4,   241,   242,    54,   109,     5,
       6,     7,     8,     9,    10,    11,   161,    56,   251,    12,
      13,    14,    68,    40,    57,    41,   168,    15,    16,   243,
      58,    68,   103,    69,   105,   183,   185,    17,   169,   225,
     225,   244,   102,   114,   115,   246,    89,   117,   146,    90,
     226,   240,    59,   206,    63,   139,   203,   142,    86,   147,
     148,   149,   150,   151,   152,   153,   154,    42,   135,    65,
      43,   136,    44,    45,    46,    47,    48,   140,    52,    53,
     141,   167,    85,    60,    61,    62,   220,    44,    45,    46,
      47,    48,   147,   148,   149,   150,   151,   152,   153,   154,
     127,   128,   129,   130,   131,    44,    45,    46,    47,    48,
     114,   115,   116,    64,   117,   195,   248,   197,   250,    66,
     236,   201,   202,   237,    67,    68,    72,   257,   253,    73,
     255,   114,   115,   182,    74,   117,    75,   260,   114,   115,
     184,    76,   117,   114,   115,   219,    77,   117,   114,   115,
     252,    78,   117,   114,   115,   259,    79,   117,    80,    81,
     224,    82,    83,    84,    87,    88,    92,    91,    95,    93,
      98,    97,   104,   101,   111,   106,   110,   120,   112,   121,
     124,   122,   125,   133,   134,   144,   137,   157,   162,   172,
     164,   170,   165,   166,   179,   174,   175,   180,   176,   188,
     181,   189,   191,   192,   194,   204,   199,   209,   196,   211,
     207,   210,   213,   217,   228,   198,   223,   233,   229,   214,
     225,   221,   222,   234,   235,   231,   245,   254,   232,   163,
     256,   190,   261,   178,   205,   239,   218,   171,   247,   249,
     258,   160,   262,   187,   186,   238
=======
     103,   132,    65,   120,   192,    94,     2,    46,    47,   198,
       3,     4,    45,    49,    50,   121,     5,     6,     7,     8,
       9,    10,    11,   135,   208,   207,    12,    13,    14,    40,
      62,    41,    62,    48,    15,    16,    95,    92,    93,    51,
     122,    63,   154,    91,    17,   164,    52,    53,   150,   152,
     103,   123,   124,   125,   126,   127,   128,   129,   130,    54,
      55,    56,   162,   123,   124,   125,   126,   127,   128,   129,
     130,   183,    57,    99,   100,   101,   141,   102,    58,    59,
      60,   178,   110,   111,   112,   113,   114,   196,    99,   100,
     149,    61,   102,    99,   100,   151,    37,   102,    62,    38,
     191,    99,   100,   172,    67,   102,    99,   100,   177,    42,
     102,    66,    43,   201,    99,   100,   190,    68,   102,    99,
     100,   200,    69,   102,    99,   100,    73,    70,   102,    71,
      72,    75,    74,    76,    77,    78,    79,    80,    81,    82,
      83,    85,    90,    97,    86,    87,    98,   105,   107,   106,
     116,   108,   117,   118,   119,   133,   136,   138,   139,   140,
     144,   157,   145,   143,   147,   148,   155,   160,   158,   163,
     161,   167,   168,   169,   176,   165,   179,   180,   170,   181,
     188,   184,   182,   186,   197,   193,   204,   187,   189,   199,
     195,   202,   142,   137,   175,   209,   153,   205,   156,   166,
       0,   203,   206
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
};

static const yytype_int16 yycheck[] =
{
<<<<<<< HEAD
      43,   118,   112,   186,     6,     5,    18,     9,    54,     0,
     107,   121,    31,     4,     5,   234,   235,     3,    30,    10,
      11,    12,    13,    14,    15,    16,   123,    33,   247,    20,
      21,    22,    19,     6,    54,     8,    18,    28,    29,    18,
       3,    19,    85,    30,    87,   155,   156,    38,    30,    19,
      19,    30,    30,    52,    53,   238,    54,    56,    30,    57,
      30,    30,     3,   180,    39,   108,   176,   110,    68,    41,
      42,    43,    44,    45,    46,    47,    48,    54,    54,     8,
      57,    57,    59,    60,    61,    62,    63,    54,     7,     8,
      57,   134,    54,     3,     3,     3,   206,    59,    60,    61,
      62,    63,    41,    42,    43,    44,    45,    46,    47,    48,
      23,    24,    25,    26,    27,    59,    60,    61,    62,    63,
      52,    53,    54,    54,    56,   168,   243,   170,   245,    54,
      54,   174,   175,    57,    54,    19,    33,   254,   248,    17,
     250,    52,    53,    54,     3,    56,     3,   257,    52,    53,
      54,    33,    56,    52,    53,    54,    54,    56,    52,    53,
      54,    54,    56,    52,    53,    54,    36,    56,    40,    17,
     213,    54,     3,     3,    54,    54,    32,    54,    54,    34,
      54,    56,    17,    37,     3,    19,    18,     3,    17,    41,
      31,    19,    19,    54,    54,    19,    54,    35,     6,    50,
      18,    18,    17,    17,     3,    18,    18,    54,    19,     3,
      47,    54,     3,    52,    54,    18,    49,     3,    54,    19,
      30,    18,    18,     3,    17,    51,    18,     3,    35,    54,
      19,    54,    54,    54,    54,    30,    18,    18,   222,   125,
      30,   163,    30,   144,   178,    54,   203,   137,    54,    54,
      54,   122,    54,   160,   157,   229
=======
      83,   103,    43,    96,   184,    19,     0,     7,     8,   192,
       4,     5,    54,    31,    33,    98,    10,    11,    12,    13,
      14,    15,    16,   106,   207,   205,    20,    21,    22,     6,
      19,     8,    19,     3,    28,    29,    50,    78,    79,    54,
      30,    30,   135,    30,    38,   147,     3,     3,   131,   132,
     133,    41,    42,    43,    44,    45,    46,    47,    48,     3,
       3,     3,   145,    41,    42,    43,    44,    45,    46,    47,
      48,   173,    39,    52,    53,    54,   117,    56,    54,     8,
      54,   164,    23,    24,    25,    26,    27,   189,    52,    53,
      54,    54,    56,    52,    53,    54,     6,    56,    19,     9,
     183,    52,    53,    54,     3,    56,    52,    53,    54,    54,
      56,    33,    57,   196,    52,    53,    54,     3,    56,    52,
      53,    54,    33,    56,    52,    53,    40,    54,    56,    54,
      36,    54,    17,     3,     3,    54,    54,    54,    54,    32,
      34,    54,    37,     3,    56,    54,    17,     3,    31,    41,
      54,    19,    54,    54,    51,    35,     6,    18,    17,    17,
       3,     3,    19,    54,    54,    47,    54,    54,    52,    18,
      37,     3,     3,    18,     3,    30,    54,    54,    19,    18,
       3,    35,    30,    30,    30,    50,    30,   180,    54,    51,
      54,    54,   118,   108,   162,   208,   133,    37,   137,   153,
      -1,    54,    54
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
<<<<<<< HEAD
       0,    65,     0,     4,     5,    10,    11,    12,    13,    14,
      15,    16,    20,    21,    22,    28,    29,    38,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      80,    81,    87,    92,    93,    96,   110,     6,     9,    78,
       6,     8,    54,    57,    59,    60,    61,    62,    63,    97,
      98,    54,     7,     8,     3,    31,    33,    54,     3,     3,
       3,     3,     3,    39,    54,     8,    54,    54,    19,    30,
      99,    99,    33,    17,     3,     3,    33,    54,    54,    36,
      40,    17,    54,     3,     3,    54,    98,    54,    54,    54,
      57,    54,    32,    34,   103,    54,    94,    56,    54,    83,
      86,    37,    30,    99,    17,    99,    19,   102,    18,    30,
      18,     3,    17,    88,    52,    53,    54,    56,    91,   105,
       3,    41,    19,    95,    31,    19,    82,    23,    24,    25,
      26,    27,    85,    54,    54,    54,    57,    54,   103,    99,
      54,    57,    99,    91,    19,    89,    30,    41,    42,    43,
      44,    45,    46,    47,    48,   109,   109,    35,   104,    91,
      94,   103,     6,    83,    18,    17,    17,    99,    18,    30,
      18,   102,    50,   100,    18,    18,    19,    90,    88,     3,
      54,    47,    54,    91,    54,    91,   105,    95,     3,    54,
      82,     3,    52,    84,    54,    99,    54,    99,    51,    49,
     106,    99,    99,    91,    18,    89,   109,    30,   104,     3,
      18,    19,    79,    18,    54,    98,   108,     3,    90,    54,
      91,    54,    54,    18,    99,    19,    30,   101,    17,    35,
     107,    30,    79,     3,    54,    54,    54,    57,   108,    54,
      30,   101,   101,    18,    30,    18,   104,    54,   109,    54,
     109,   101,    54,    91,    18,    91,    30,   109,    54,    54,
      91,    30,    54
=======
       0,    60,     0,     4,     5,    10,    11,    12,    13,    14,
      15,    16,    20,    21,    22,    28,    29,    38,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      75,    76,    82,    85,    86,    87,    98,     6,     9,    73,
       6,     8,    54,    57,    88,    54,     7,     8,     3,    31,
      33,    54,     3,     3,     3,     3,     3,    39,    54,     8,
      54,    54,    19,    30,    89,    89,    33,     3,     3,    33,
      54,    54,    36,    40,    17,    54,     3,     3,    54,    54,
      54,    54,    32,    34,    94,    54,    56,    54,    78,    81,
      37,    30,    89,    89,    19,    50,    90,     3,    17,    52,
      53,    54,    56,    84,    96,     3,    41,    31,    19,    77,
      23,    24,    25,    26,    27,    80,    54,    54,    54,    51,
      94,    84,    30,    41,    42,    43,    44,    45,    46,    47,
      48,    97,    97,    35,    95,    84,     6,    78,    18,    17,
      17,    89,    90,    54,     3,    19,    83,    54,    47,    54,
      84,    54,    84,    96,    94,    54,    77,     3,    52,    79,
      54,    37,    84,    18,    97,    30,    95,     3,     3,    18,
      19,    74,    54,    84,    93,    83,     3,    54,    84,    54,
      54,    18,    30,    97,    35,    92,    30,    74,     3,    54,
      54,    84,    93,    50,    91,    54,    97,    30,    92,    51,
      54,    84,    54,    54,    30,    37,    54,    93,    92,    91
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
<<<<<<< HEAD
       0,    64,    65,    65,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    78,    79,    79,    80,    81,    82,
      82,    83,    83,    83,    84,    85,    85,    85,    85,    86,
      87,    88,    89,    89,    90,    90,    91,    91,    91,    92,
      93,    94,    95,    95,    96,    97,    97,    97,    97,    97,
      97,    97,    98,    98,    98,    98,    98,    99,    99,    99,
      99,    99,    99,   100,   100,   100,   101,   101,   101,   102,
     102,   103,   103,   104,   104,   105,   105,   105,   105,   105,
     105,   105,   106,   106,   107,   107,   108,   108,   108,   108,
     108,   108,   108,   109,   109,   109,   109,   109,   109,   109,
     109,   110
=======
       0,    59,    60,    60,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    73,    74,    74,    75,    76,    77,
      77,    78,    78,    78,    79,    80,    80,    80,    80,    81,
      82,    83,    83,    84,    84,    84,    85,    86,    87,    88,
      88,    88,    89,    89,    89,    90,    90,    90,    91,    91,
      92,    92,    93,    93,    93,    93,    94,    94,    95,    95,
      96,    96,    96,    96,    96,    96,    96,    97,    97,    97,
      97,    97,    97,    97,    97,    98
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     4,     3,
       5,     3,    11,     0,     1,     0,     3,     4,     8,     0,
       3,     5,     2,     2,     1,     1,     1,     1,     1,     1,
<<<<<<< HEAD
       7,     4,     0,     3,     0,     3,     1,     1,     1,     5,
       7,     3,     0,     3,     9,     2,     2,     4,     5,     7,
       5,     7,     1,     1,     1,     1,     1,     0,     3,     5,
       6,     6,     8,     0,     4,     6,     0,     3,     5,     0,
       3,     0,     3,     0,     3,     3,     3,     3,     3,     5,
       5,     7,     0,     3,     0,     3,     6,     6,     8,     6,
       8,     8,    10,     1,     1,     1,     1,     1,     1,     1,
       2,     8
=======
       9,     0,     3,     1,     1,     1,     5,     8,     7,     2,
       2,     4,     0,     3,     5,     0,     3,     7,     0,     7,
       0,     3,     3,     5,     5,     7,     0,     3,     0,     3,
       3,     3,     3,     3,     5,     5,     7,     1,     1,     1,
       1,     1,     1,     1,     2,     8
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
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
<<<<<<< HEAD
  case 22:
#line 177 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1627 "yacc_sql.tab.c"
    break;

  case 23:
#line 182 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1635 "yacc_sql.tab.c"
    break;

  case 24:
#line 187 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1643 "yacc_sql.tab.c"
    break;

  case 25:
#line 193 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1651 "yacc_sql.tab.c"
    break;

  case 26:
#line 199 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1659 "yacc_sql.tab.c"
    break;

  case 27:
#line 205 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1667 "yacc_sql.tab.c"
    break;

  case 28:
#line 211 "yacc_sql.y"
=======
  case 22: /* exit: EXIT SEMICOLON  */
#line 167 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1362 "yacc_sql.tab.c"
    break;

  case 23: /* help: HELP SEMICOLON  */
#line 172 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1370 "yacc_sql.tab.c"
    break;

  case 24: /* sync: SYNC SEMICOLON  */
#line 177 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1378 "yacc_sql.tab.c"
    break;

  case 25: /* begin: TRX_BEGIN SEMICOLON  */
#line 183 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1386 "yacc_sql.tab.c"
    break;

  case 26: /* commit: TRX_COMMIT SEMICOLON  */
#line 189 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1394 "yacc_sql.tab.c"
    break;

  case 27: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 195 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1402 "yacc_sql.tab.c"
    break;

  case 28: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 201 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
<<<<<<< HEAD
#line 1676 "yacc_sql.tab.c"
    break;

  case 29:
#line 217 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1684 "yacc_sql.tab.c"
    break;

  case 30:
#line 223 "yacc_sql.y"
=======
#line 1411 "yacc_sql.tab.c"
    break;

  case 29: /* show_tables: SHOW TABLES SEMICOLON  */
#line 207 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1419 "yacc_sql.tab.c"
    break;

  case 30: /* show_index: SHOW INDEX FROM ID SEMICOLON  */
#line 213 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                                 {
      CONTEXT->ssql->flag = SCF_SHOW_INDEX;
	  show_index_init(&CONTEXT->ssql->sstr.show_index, (yyvsp[-1].string));
    }
<<<<<<< HEAD
#line 1693 "yacc_sql.tab.c"
    break;

  case 31:
#line 230 "yacc_sql.y"
=======
#line 1428 "yacc_sql.tab.c"
    break;

  case 31: /* desc_table: DESC ID SEMICOLON  */
#line 220 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
<<<<<<< HEAD
#line 1702 "yacc_sql.tab.c"
    break;

  case 32:
#line 238 "yacc_sql.y"
=======
#line 1437 "yacc_sql.tab.c"
    break;

  case 32: /* create_index: CREATE optional_unique INDEX ID ON ID LBRACE ID create_index_attrs RBRACE SEMICOLON  */
#line 228 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, (yyvsp[-3].string));
		}
<<<<<<< HEAD
#line 1712 "yacc_sql.tab.c"
    break;

  case 33:
#line 247 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 0);
		}
#line 1720 "yacc_sql.tab.c"
    break;

  case 34:
#line 251 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 1);
		}
#line 1728 "yacc_sql.tab.c"
    break;

  case 36:
#line 257 "yacc_sql.y"
                {
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, (yyvsp[-1].string));
		}
#line 1736 "yacc_sql.tab.c"
    break;

  case 37:
#line 263 "yacc_sql.y"
=======
#line 1447 "yacc_sql.tab.c"
    break;

  case 33: /* optional_unique: %empty  */
#line 237 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 0);
		}
#line 1455 "yacc_sql.tab.c"
    break;

  case 34: /* optional_unique: UNIQUE  */
#line 241 "yacc_sql.y"
                {
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 1);
		}
#line 1463 "yacc_sql.tab.c"
    break;

  case 36: /* create_index_attrs: COMMA ID create_index_attrs  */
#line 247 "yacc_sql.y"
                {
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, (yyvsp[-1].string));
		}
#line 1471 "yacc_sql.tab.c"
    break;

  case 37: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 253 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
<<<<<<< HEAD
#line 1745 "yacc_sql.tab.c"
    break;

  case 38:
#line 270 "yacc_sql.y"
=======
#line 1480 "yacc_sql.tab.c"
    break;

  case 38: /* create_table: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON  */
#line 260 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
<<<<<<< HEAD
#line 1757 "yacc_sql.tab.c"
    break;

  case 40:
#line 280 "yacc_sql.y"
                                   {    }
#line 1763 "yacc_sql.tab.c"
    break;

  case 41:
#line 285 "yacc_sql.y"
=======
#line 1492 "yacc_sql.tab.c"
    break;

  case 40: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 270 "yacc_sql.y"
                                   {    }
#line 1498 "yacc_sql.tab.c"
    break;

  case 41: /* attr_def: ID_get type LBRACE number RBRACE  */
#line 275 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-3].number), (yyvsp[-1].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
<<<<<<< HEAD
#line 1778 "yacc_sql.tab.c"
    break;

  case 42:
#line 296 "yacc_sql.y"
=======
#line 1513 "yacc_sql.tab.c"
    break;

  case 42: /* attr_def: ID_get type  */
#line 286 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[0].number), 4);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
<<<<<<< HEAD
#line 1793 "yacc_sql.tab.c"
    break;

  case 43:
#line 307 "yacc_sql.y"
=======
#line 1528 "yacc_sql.tab.c"
    break;

  case 43: /* attr_def: ID_get TEXT_T  */
#line 297 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, CHARS, 4096);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
<<<<<<< HEAD
#line 1808 "yacc_sql.tab.c"
    break;

  case 44:
#line 319 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1814 "yacc_sql.tab.c"
    break;

  case 45:
#line 322 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1820 "yacc_sql.tab.c"
    break;

  case 46:
#line 323 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1826 "yacc_sql.tab.c"
    break;

  case 47:
#line 324 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1832 "yacc_sql.tab.c"
    break;

  case 48:
#line 325 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 1838 "yacc_sql.tab.c"
    break;

  case 49:
#line 329 "yacc_sql.y"
=======
#line 1543 "yacc_sql.tab.c"
    break;

  case 44: /* number: NUMBER  */
#line 309 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1549 "yacc_sql.tab.c"
    break;

  case 45: /* type: INT_T  */
#line 312 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1555 "yacc_sql.tab.c"
    break;

  case 46: /* type: STRING_T  */
#line 313 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1561 "yacc_sql.tab.c"
    break;

  case 47: /* type: FLOAT_T  */
#line 314 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1567 "yacc_sql.tab.c"
    break;

  case 48: /* type: DATE_T  */
#line 315 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 1573 "yacc_sql.tab.c"
    break;

  case 49: /* ID_get: ID  */
#line 319 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
<<<<<<< HEAD
#line 1847 "yacc_sql.tab.c"
    break;

  case 50:
#line 338 "yacc_sql.y"
=======
#line 1582 "yacc_sql.tab.c"
    break;

  case 50: /* insert: INSERT INTO ID VALUES LBRACE value value_list RBRACE SEMICOLON  */
#line 328 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
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
<<<<<<< HEAD
#line 1866 "yacc_sql.tab.c"
    break;

  case 51:
#line 354 "yacc_sql.y"
                                       {
		inserts_create_tuple(&CONTEXT->ssql->sstr.insertion, CONTEXT->values, CONTEXT->value_length);
		CONTEXT->value_length = 0;
	}
#line 1875 "yacc_sql.tab.c"
    break;

  case 53:
#line 361 "yacc_sql.y"
                                               {}
#line 1881 "yacc_sql.tab.c"
    break;

  case 55:
#line 365 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 1889 "yacc_sql.tab.c"
    break;

  case 56:
#line 370 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 1897 "yacc_sql.tab.c"
    break;

  case 57:
#line 373 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 1905 "yacc_sql.tab.c"
    break;

  case 58:
#line 376 "yacc_sql.y"
=======
#line 1601 "yacc_sql.tab.c"
    break;

  case 52: /* value_list: COMMA value value_list  */
#line 345 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 1609 "yacc_sql.tab.c"
    break;

  case 53: /* value: NUMBER  */
#line 350 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 1617 "yacc_sql.tab.c"
    break;

  case 54: /* value: FLOAT  */
#line 353 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 1625 "yacc_sql.tab.c"
    break;

  case 55: /* value: SSS  */
#line 356 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
<<<<<<< HEAD
#line 1914 "yacc_sql.tab.c"
    break;

  case 59:
#line 384 "yacc_sql.y"
=======
#line 1634 "yacc_sql.tab.c"
    break;

  case 56: /* delete: DELETE FROM ID where SEMICOLON  */
#line 364 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
<<<<<<< HEAD
#line 1926 "yacc_sql.tab.c"
    break;

  case 60:
#line 394 "yacc_sql.y"
=======
#line 1646 "yacc_sql.tab.c"
    break;

  case 57: /* update: UPDATE ID SET ID EQ value where SEMICOLON  */
#line 374 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-5].string), CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
<<<<<<< HEAD
#line 1936 "yacc_sql.tab.c"
    break;

  case 61:
#line 401 "yacc_sql.y"
                    {
		updates_attr_init(&CONTEXT->ssql->sstr.update, (yyvsp[-2].string), &CONTEXT->values[0]);
		CONTEXT->value_length = 0;
	}
#line 1945 "yacc_sql.tab.c"
    break;

  case 63:
#line 407 "yacc_sql.y"
                                                         {}
#line 1951 "yacc_sql.tab.c"
    break;

  case 64:
#line 410 "yacc_sql.y"
=======
#line 1658 "yacc_sql.tab.c"
    break;

  case 58: /* select: SELECT select_attr FROM ID rel_list where SEMICOLON  */
#line 384 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-5].string));

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
<<<<<<< HEAD
#line 1971 "yacc_sql.tab.c"
    break;

  case 65:
#line 428 "yacc_sql.y"
=======
#line 1678 "yacc_sql.tab.c"
    break;

  case 59: /* select_attr: STAR attr_list  */
#line 402 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                   {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
<<<<<<< HEAD
#line 1981 "yacc_sql.tab.c"
    break;

  case 66:
#line 433 "yacc_sql.y"
=======
#line 1688 "yacc_sql.tab.c"
    break;

  case 60: /* select_attr: ID attr_list  */
#line 407 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
<<<<<<< HEAD
#line 1991 "yacc_sql.tab.c"
    break;

  case 67:
#line 438 "yacc_sql.y"
=======
#line 1698 "yacc_sql.tab.c"
    break;

  case 61: /* select_attr: ID DOT ID attr_list  */
#line 412 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
<<<<<<< HEAD
#line 2001 "yacc_sql.tab.c"
    break;

  case 68:
#line 443 "yacc_sql.y"
                                                 {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2013 "yacc_sql.tab.c"
    break;

  case 69:
#line 450 "yacc_sql.y"
                                                        {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), "*");
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2025 "yacc_sql.tab.c"
    break;

  case 70:
#line 457 "yacc_sql.y"
                                               {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2037 "yacc_sql.tab.c"
    break;

  case 71:
#line 464 "yacc_sql.y"
                                                      {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
#line 2049 "yacc_sql.tab.c"
    break;

  case 72:
#line 473 "yacc_sql.y"
               {(yyval.number) = MIN;}
#line 2055 "yacc_sql.tab.c"
    break;

  case 73:
#line 474 "yacc_sql.y"
                   {(yyval.number) = MAX;}
#line 2061 "yacc_sql.tab.c"
    break;

  case 74:
#line 475 "yacc_sql.y"
                     {(yyval.number) = COUNT;}
#line 2067 "yacc_sql.tab.c"
    break;

  case 75:
#line 476 "yacc_sql.y"
                   {(yyval.number) = SUM;}
#line 2073 "yacc_sql.tab.c"
    break;

  case 76:
#line 477 "yacc_sql.y"
                   {(yyval.number) = AVG;}
#line 2079 "yacc_sql.tab.c"
    break;

  case 78:
#line 481 "yacc_sql.y"
=======
#line 1708 "yacc_sql.tab.c"
    break;

  case 63: /* attr_list: COMMA ID attr_list  */
#line 420 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
<<<<<<< HEAD
#line 2091 "yacc_sql.tab.c"
    break;

  case 79:
#line 488 "yacc_sql.y"
=======
#line 1720 "yacc_sql.tab.c"
    break;

  case 64: /* attr_list: COMMA ID DOT ID attr_list  */
#line 427 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
<<<<<<< HEAD
#line 2103 "yacc_sql.tab.c"
    break;

  case 80:
#line 495 "yacc_sql.y"
                                                       {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2117 "yacc_sql.tab.c"
    break;

  case 81:
#line 504 "yacc_sql.y"
                                                     {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-4].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2131 "yacc_sql.tab.c"
    break;

  case 82:
#line 513 "yacc_sql.y"
                                                            {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			attr.aggr_type = (yyvsp[-6].number);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2145 "yacc_sql.tab.c"
    break;

  case 84:
#line 524 "yacc_sql.y"
                                 {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2155 "yacc_sql.tab.c"
    break;

  case 85:
#line 529 "yacc_sql.y"
                                            {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2165 "yacc_sql.tab.c"
    break;

  case 87:
#line 537 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2177 "yacc_sql.tab.c"
    break;

  case 88:
#line 544 "yacc_sql.y"
                                     {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2189 "yacc_sql.tab.c"
    break;

  case 90:
#line 554 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2197 "yacc_sql.tab.c"
    break;

  case 92:
#line 560 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2205 "yacc_sql.tab.c"
    break;

  case 94:
#line 566 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2213 "yacc_sql.tab.c"
    break;

  case 95:
#line 572 "yacc_sql.y"
=======
#line 1732 "yacc_sql.tab.c"
    break;

  case 66: /* rel_list: COMMA ID rel_list  */
#line 438 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 1740 "yacc_sql.tab.c"
    break;

  case 67: /* rel_list: INNER JOIN ID ON join_cond join_cond_list join_list  */
#line 441 "yacc_sql.y"
                                                          {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
		  }
#line 1748 "yacc_sql.tab.c"
    break;

  case 69: /* join_list: INNER JOIN ID ON join_cond join_cond_list join_list  */
#line 447 "yacc_sql.y"
                                                          {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
		  }
#line 1756 "yacc_sql.tab.c"
    break;

  case 71: /* join_cond_list: AND join_cond join_cond_list  */
#line 453 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
		  }
#line 1764 "yacc_sql.tab.c"
    break;

  case 72: /* join_cond: value comOp value  */
#line 459 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 1777 "yacc_sql.tab.c"
    break;

  case 73: /* join_cond: ID DOT ID comOp value  */
#line 468 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    	}
#line 1791 "yacc_sql.tab.c"
    break;

  case 74: /* join_cond: value comOp ID DOT ID  */
#line 478 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;		
    	}
#line 1806 "yacc_sql.tab.c"
    break;

  case 75: /* join_cond: ID DOT ID comOp ID DOT ID  */
#line 489 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			selects_append_joincond(&CONTEXT->ssql->sstr.selection, condition);
    	}
#line 1821 "yacc_sql.tab.c"
    break;

  case 77: /* where: WHERE condition condition_list  */
#line 502 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 1829 "yacc_sql.tab.c"
    break;

  case 79: /* condition_list: AND condition condition_list  */
#line 508 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 1837 "yacc_sql.tab.c"
    break;

  case 80: /* condition: ID comOp value  */
#line 514 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
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
<<<<<<< HEAD
#line 2238 "yacc_sql.tab.c"
    break;

  case 96:
#line 593 "yacc_sql.y"
=======
#line 1862 "yacc_sql.tab.c"
    break;

  case 81: /* condition: value comOp value  */
#line 535 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
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
<<<<<<< HEAD
#line 2262 "yacc_sql.tab.c"
    break;

  case 97:
#line 613 "yacc_sql.y"
=======
#line 1886 "yacc_sql.tab.c"
    break;

  case 82: /* condition: ID comOp ID  */
#line 555 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
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
<<<<<<< HEAD
#line 2286 "yacc_sql.tab.c"
    break;

  case 98:
#line 633 "yacc_sql.y"
=======
#line 1910 "yacc_sql.tab.c"
    break;

  case 83: /* condition: value comOp ID  */
#line 575 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
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
<<<<<<< HEAD
#line 2312 "yacc_sql.tab.c"
    break;

  case 99:
#line 655 "yacc_sql.y"
=======
#line 1936 "yacc_sql.tab.c"
    break;

  case 84: /* condition: ID DOT ID comOp value  */
#line 597 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
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
<<<<<<< HEAD
#line 2337 "yacc_sql.tab.c"
    break;

  case 100:
#line 676 "yacc_sql.y"
=======
#line 1961 "yacc_sql.tab.c"
    break;

  case 85: /* condition: value comOp ID DOT ID  */
#line 618 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
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
<<<<<<< HEAD
#line 2362 "yacc_sql.tab.c"
    break;

  case 101:
#line 697 "yacc_sql.y"
=======
#line 1986 "yacc_sql.tab.c"
    break;

  case 86: /* condition: ID DOT ID comOp ID DOT ID  */
#line 639 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;		//属性
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;		//属性
			// $$->right_attr.relation_name=$5;
			// $$->right_attr.attribute_name=$7;
    	}
<<<<<<< HEAD
#line 2385 "yacc_sql.tab.c"
    break;

  case 103:
#line 717 "yacc_sql.y"
                                                        {

	}
#line 2393 "yacc_sql.tab.c"
    break;

  case 105:
#line 723 "yacc_sql.y"
                                          {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2401 "yacc_sql.tab.c"
    break;

  case 106:
#line 729 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, "COUNT(*)");
		left_attr.aggr_type = COUNT;

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2417 "yacc_sql.tab.c"
    break;

  case 107:
#line 741 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-3].string));
		left_attr.aggr_type = (yyvsp[-5].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2433 "yacc_sql.tab.c"
    break;

  case 108:
#line 753 "yacc_sql.y"
        {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string));
		left_attr.aggr_type = (yyvsp[-7].number);

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
#line 2449 "yacc_sql.tab.c"
    break;

  case 109:
#line 765 "yacc_sql.y"
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
#line 2466 "yacc_sql.tab.c"
    break;

  case 110:
#line 778 "yacc_sql.y"
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
#line 2483 "yacc_sql.tab.c"
    break;

  case 111:
#line 791 "yacc_sql.y"
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
#line 2500 "yacc_sql.tab.c"
    break;

  case 112:
#line 804 "yacc_sql.y"
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
#line 2517 "yacc_sql.tab.c"
    break;

  case 113:
#line 819 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2523 "yacc_sql.tab.c"
    break;

  case 114:
#line 820 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2529 "yacc_sql.tab.c"
    break;

  case 115:
#line 821 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2535 "yacc_sql.tab.c"
    break;

  case 116:
#line 822 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2541 "yacc_sql.tab.c"
    break;

  case 117:
#line 823 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2547 "yacc_sql.tab.c"
    break;

  case 118:
#line 824 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2553 "yacc_sql.tab.c"
    break;

  case 119:
#line 825 "yacc_sql.y"
                 { CONTEXT->comp = LIKE; }
#line 2559 "yacc_sql.tab.c"
    break;

  case 120:
#line 826 "yacc_sql.y"
                           { CONTEXT->comp = NOT_LIKE; }
#line 2565 "yacc_sql.tab.c"
    break;

  case 121:
#line 831 "yacc_sql.y"
=======
#line 2009 "yacc_sql.tab.c"
    break;

  case 87: /* comOp: EQ  */
#line 660 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2015 "yacc_sql.tab.c"
    break;

  case 88: /* comOp: LT  */
#line 661 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2021 "yacc_sql.tab.c"
    break;

  case 89: /* comOp: GT  */
#line 662 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2027 "yacc_sql.tab.c"
    break;

  case 90: /* comOp: LE  */
#line 663 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2033 "yacc_sql.tab.c"
    break;

  case 91: /* comOp: GE  */
#line 664 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2039 "yacc_sql.tab.c"
    break;

  case 92: /* comOp: NE  */
#line 665 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2045 "yacc_sql.tab.c"
    break;

  case 93: /* comOp: LIKE_TOKEN  */
#line 666 "yacc_sql.y"
                 { CONTEXT->comp = LIKE; }
#line 2051 "yacc_sql.tab.c"
    break;

  case 94: /* comOp: NOT_TOKEN LIKE_TOKEN  */
#line 667 "yacc_sql.y"
                           { CONTEXT->comp = NOT_LIKE; }
#line 2057 "yacc_sql.tab.c"
    break;

  case 95: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 672 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
<<<<<<< HEAD
#line 2574 "yacc_sql.tab.c"
    break;


#line 2578 "yacc_sql.tab.c"
=======
#line 2066 "yacc_sql.tab.c"
    break;


#line 2070 "yacc_sql.tab.c"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)

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
<<<<<<< HEAD
#line 836 "yacc_sql.y"
=======

#line 677 "yacc_sql.y"
>>>>>>> 16ea068 (implement join metadata from parse to do_select)

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
