
%{

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

%}

%define api.pure full
%lex-param { yyscan_t scanner }
%parse-param { void *scanner }

//标识tokens
%token  SEMICOLON
        CREATE
        DROP
        TABLE
        TABLES
        INDEX
		UNIQUE
        SELECT
        DESC
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        RBRACE
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
		TEXT_T
        FLOAT_T
		DATE_T
        HELP
        EXIT
        DOT //QUOTE_type
        INTO
        VALUES
        FROM
        WHERE
        AND
		OR
        SET
        ON
        LOAD
        DATA
        INFILE
        EQ
        LT
        GT
        LE
        GE
        NE
		LIKE_TOKEN
		NOT_TOKEN
        HAVING
		GROUP
		ORDER
		BY		
		INNER
		JOIN
		ASC
		NULLABLE
		NULL_
		IS
		IN_TOKEN
		EXISTS_TOKEN
		AS

%union {
  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
  char *position;
  Query*query;
}

%token <number> NUMBER
%token <floats> FLOAT 
%token <string> ID
%token <string> PATH
%token <string> SSS
%token <string> STAR
%token <string> STRING_V
%token <string> EXPRESSION
%token <number> MIN_AGGR
%token <number> MAX_AGGR
%token <number> COUNT_AGGR
%token <number> SUM_AGGR
%token <number> AVG_AGGR
//非终结符

%type <number> type;
%type <condition1> condition;
%type <value1> value;
%type <number> number;
%type <number> aggregate;
%type <number> order_type;
%type <number> nullable;
%type <query> sub_query

%%

commands:		//commands or sqls. parser starts here.
    /* empty */
    | commands command
    ;

command:
	  select  
	| insert
	| update
	| delete
	| create_table
	| drop_table
	| show_tables
	| show_index
	| desc_table
	| create_index	
	| drop_index
	| sync
	| begin
	| commit
	| rollback
	| load_data
	| help
	| exit
    ;

exit:			
    EXIT SEMICOLON {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    };

help:
    HELP SEMICOLON {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    };

sync:
    SYNC SEMICOLON {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
    ;

begin:
    TRX_BEGIN SEMICOLON {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
    ;

commit:
    TRX_COMMIT SEMICOLON {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
    ;

rollback:
    TRX_ROLLBACK SEMICOLON {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
    ;

drop_table:		/*drop table 语句的语法解析树*/
    DROP TABLE ID SEMICOLON {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, $3);
    };

show_tables:
    SHOW TABLES SEMICOLON {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
    ;

show_index:
    SHOW INDEX FROM ID SEMICOLON {
      CONTEXT->ssql->flag = SCF_SHOW_INDEX;
	  show_index_init(&CONTEXT->ssql->sstr.show_index, $4);
    }
    ;

desc_table:
    DESC ID SEMICOLON {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, $2);
    }
    ;

create_index:		/*create index 语句的语法解析树*/
    CREATE optional_unique INDEX ID ON ID LBRACE ID create_index_attrs RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, $4, $6);
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, $8);
		}
    ;

optional_unique:
	/*empty*/
		{
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 0);
		}
	| UNIQUE 
		{
			create_index_set_unique(&CONTEXT->ssql->sstr.create_index, 1);
		}

create_index_attrs:
	/*empty*/ | COMMA ID create_index_attrs 
		{
			create_index_add_attr(&CONTEXT->ssql->sstr.create_index, $2);
		}; 

drop_index:			/*drop index 语句的语法解析树*/
    DROP INDEX ID  SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, $3);
		}
    ;
create_table:		/*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, $3);
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
    ;
attr_def_list:
    /* empty */
    | COMMA attr_def attr_def_list {    }
    ;
    
attr_def:
    ID_get type LBRACE number RBRACE nullable
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $4, $6);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
    |ID_get type nullable
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, 4, $3);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
    |ID_get TEXT_T nullable // dirty hack, just to make the length 4096
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, CHARS, 4096, $3);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
    ;
number:
		NUMBER {$$ = $1;}
		;
type:
	INT_T { $$=INTS; }
       | STRING_T { $$=CHARS; }
       | FLOAT_T { $$=FLOATS; }
	   | DATE_T { $$=DATES; }
       ;
order_type:
    /* empty */ { $$=1; }
	   | ASC  { $$=1; }
       | DESC { $$=0; }
       ;

nullable:
	/* empty */ { $$=0; }
	| NOT_TOKEN NULL_ { $$=0; }
	| NULLABLE { $$=1; }

ID_get:
	ID 
	{
		char *temp=$1; 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
	;

	
insert:				/*insert   语句的语法解析树*/
    INSERT INTO ID VALUES insert_tuple insert_tuple_list SEMICOLON 
		{
			// CONTEXT->values[CONTEXT->value_length++] = *$6;

			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			// CONTEXT->ssql->sstr.insertion.relation_name = $3;
			// CONTEXT->ssql->sstr.insertion.value_num = CONTEXT->value_length;
			// for(i = 0; i < CONTEXT->value_length; i++){
			// 	CONTEXT->ssql->sstr.insertion.values[i] = CONTEXT->values[i];
      // }
			inserts_init(&CONTEXT->ssql->sstr.insertion, $3);

      //临时变量清零
      CONTEXT->value_length=0;
    }

insert_tuple:
	LBRACE value value_list RBRACE {
		inserts_create_tuple(&CONTEXT->ssql->sstr.insertion, CONTEXT->values, CONTEXT->value_length);
		CONTEXT->value_length = 0;
	}

insert_tuple_list:
	/* empty */
	| COMMA insert_tuple insert_tuple_list {}

value_list:
    /* empty */
    | COMMA value value_list  { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
    ;
value:
    NUMBER{	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    |FLOAT{
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    |SSS {
			$1 = substr($1,1,strlen($1)-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
	|NULL_ {
		value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
	}
    ;
    
delete:		/*  delete 语句的语法解析树*/
    DELETE FROM ID where SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, $3);
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
    ;
update:			/*  update 语句的语法解析树*/
    UPDATE ID SET update_attr update_attr_list where SEMICOLON
		{
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, $2, CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
    ;
update_attr:
	ID EQ value {
		updates_attr_init(&CONTEXT->ssql->sstr.update, $1, &CONTEXT->values[0]);
		CONTEXT->value_length = 0;
	}
    | ID EQ sub_query {
		Query *query = $3;
		updates_attr_init_with_subquery(&CONTEXT->ssql->sstr.update, $1, &query->sstr.selection);
	}
	;
update_attr_list:
	/* empty */ | COMMA update_attr update_attr_list {}
select:				/*  select 语句的语法解析树*/
    SELECT select_attr FROM ID rel_list where order group_by having SEMICOLON
		{
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, $4);

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
	;

select_attr:
    STAR attr_list {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
    | ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $1);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
  	| ID DOT ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, $1, $3);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| aggregate LBRACE STAR RBRACE attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			attr.aggr_type = $1;
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
	| aggregate LBRACE ID DOT STAR RBRACE attr_list {
			RelAttr attr;
			relation_attr_init(&attr, $3, "*");
			attr.aggr_type = $1;
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
	| aggregate LBRACE ID RBRACE attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $3);
			attr.aggr_type = $1;
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
	| aggregate LBRACE ID DOT ID RBRACE attr_list {
			RelAttr attr;
			relation_attr_init(&attr, $3, $5);
			attr.aggr_type = $1;
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
	}
	| EXPRESSION attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $1);
			attr.is_complex = 1;
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
    ;
aggregate:
      MIN_AGGR {$$ = MIN;}
	| MAX_AGGR {$$ = MAX;}
	| COUNT_AGGR {$$ = COUNT;}
	| SUM_AGGR {$$ = SUM;}
	| AVG_AGGR {$$ = AVG;}
	;
attr_list:
    /* empty */
    | COMMA ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $2);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
    | COMMA ID DOT ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, $2, $4);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
	| COMMA aggregate LBRACE STAR RBRACE attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			attr.aggr_type = $2;
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
	| COMMA aggregate LBRACE ID RBRACE attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $4);
			attr.aggr_type = $2;
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
	| COMMA aggregate LBRACE ID DOT ID RBRACE attr_list {
			RelAttr attr;
			relation_attr_init(&attr, $4, $6);
			attr.aggr_type = $2;
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			CONTEXT->ssql->sstr.selection.is_aggr = 1;
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
	| COMMA EXPRESSION attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $2);
			attr.is_complex = 1;
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
      }
  	;
group_by:
    | GROUP BY ID group_key_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $3);
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| GROUP BY ID DOT ID group_key_list {
			RelAttr attr;
			relation_attr_init(&attr, $3, $5);
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
	}
	;
group_key_list:
    /* empty */
    | COMMA ID group_key_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $2);
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
    | COMMA ID DOT ID group_key_list {
			RelAttr attr;
			relation_attr_init(&attr, $2, $4);
			selects_append_groupkey(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
	;
rel_list:
    /* empty */
    | COMMA ID rel_list {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, $2);
		  }
    | INNER JOIN ID ON join_cond join_cond_list join_list {	
		selects_append_relation(&CONTEXT->ssql->sstr.selection, $3);
	}
    ;
join_list:
    /* empty */
    | INNER JOIN ID ON join_cond join_cond_list join_list {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, $3);
		  }
    ;
join_cond_list:
    /* empty */
    | AND join_cond join_cond_list {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
		  }
    ;
join_cond:
    value comOp value 
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
    |ID DOT ID comOp value
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3);
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    	}
    |value comOp ID DOT ID
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, $3, $5);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;		
    	}
    |ID DOT ID comOp ID DOT ID
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3);
			RelAttr right_attr;
			relation_attr_init(&right_attr, $5, $7);

			// 判断是否同一表中的两个属性。若否，条件加入join中
			if (strcmp($1, $5) != 0){
				Condition condition;
				condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
				selects_append_joincond(&CONTEXT->ssql->sstr.selection, condition);
			} else {
				Condition condition;
				condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
				CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			}
    	}
    ;
where:
    /* empty */ 
    | WHERE condition condition_list {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
    ;
condition_list:
    /* empty */
    | AND condition condition_list {
				CONTEXT->ssql->sstr.selection.condition_mode = AND_MODE;
			}
	| OR condition condition_list {
				CONTEXT->ssql->sstr.selection.condition_mode = OR_MODE;
			}
    ;
condition:
    ID comOp value 
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);

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
		|value comOp value 
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
		|ID comOp ID 
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3);

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
    |value comOp ID
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3);

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
    |ID DOT ID comOp value
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3);
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
    |value comOp ID DOT ID
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, $3, $5);

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
    |ID DOT ID comOp ID DOT ID
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3);
			RelAttr right_attr;
			relation_attr_init(&right_attr, $5, $7);

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
	| ID comOp sub_query
	{
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, $1);
		Query *right_subquery = $3;

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
	| ID DOT ID comOp sub_query
	{
		RelAttr left_attr;
		relation_attr_init(&left_attr, $1, $3);
		Query *right_subquery = $5;

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
	| sub_query comOp ID
	{
		RelAttr right_attr;
		relation_attr_init(&right_attr, NULL, $3);
		Query *left_subquery = $1;

		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, SUB_QUERY, NULL, NULL, &left_subquery->sstr.selection, ATTR, &right_attr, NULL, NULL);
		printf("1condition_length=%d\n", CONTEXT->condition_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(left_subquery);
		printf("2condition_length=%d\n", CONTEXT->condition_length);
	}
	| sub_query comOp ID DOT ID
	{
		RelAttr right_attr;
		relation_attr_init(&right_attr, $3, $5);
		Query *left_subquery = $1;
		
		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, SUB_QUERY, NULL, NULL, &left_subquery->sstr.selection, ATTR, &right_attr, NULL, NULL);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(left_subquery);
		printf("s op t.a condition_length=%d\n", CONTEXT->condition_length);
	}
	| sub_query comOp sub_query
	{
		Query *left_subquery = $1;
		Query *right_subquery = $3;
		
		Condition condition;
		condition_init_with_subquery(&condition, CONTEXT->comp, SUB_QUERY, NULL, NULL, &left_subquery->sstr.selection, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(left_subquery);
		free(right_subquery);
		printf("s op s condition_length=%d\n", CONTEXT->condition_length);
	}
	| ID comOp subquery_value_list{
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, $1);

		Condition condition;
		condition_init_with_value_list(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, CONTEXT->value_list, CONTEXT->value_list_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		CONTEXT->value_list_length = 0;
	}
	| ID DOT ID comOp subquery_value_list{
		RelAttr left_attr;
		relation_attr_init(&left_attr, $1, $3);

		Condition condition;
		condition_init_with_value_list(&condition, CONTEXT->comp, ATTR, &left_attr, NULL, CONTEXT->value_list, CONTEXT->value_list_length);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		CONTEXT->value_list_length = 0;
	}
	| comOp sub_query{
		Query *right_subquery = $2;
		Condition condition;

		condition_init_with_subquery(&condition, CONTEXT->comp, NO_EXPR, NULL, NULL, NULL, SUB_QUERY, NULL, NULL, &right_subquery->sstr.selection);
		CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		free(right_subquery);
	}
	|value comOp EXPRESSION 
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3);
			right_attr.is_complex = 1;

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);
		}
    |EXPRESSION comOp value 
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);
			left_attr.is_complex = 1;

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);

		}
	|ID comOp EXPRESSION 
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3);
			right_attr.is_complex = 1;

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);

		}
    |EXPRESSION comOp ID
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);
			left_attr.is_complex = 1;

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);
		}
    |ID DOT ID comOp EXPRESSION
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3);

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $5);
			right_attr.is_complex = 1;

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);
    	}
    |EXPRESSION comOp ID DOT ID
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);
			left_attr.is_complex = 1;

			RelAttr right_attr;
			relation_attr_init(&right_attr, $3, $5);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);			
    	}
    |EXPRESSION comOp EXPRESSION
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);
			left_attr.is_complex = 1;

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3);
			right_attr.is_complex = 1;

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			selects_append_exprcond(&CONTEXT->ssql->sstr.selection, condition);
    	}

    ;
sub_query:
    sub_query_init select_attr FROM ID rel_list where group_by having RBRACE {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, $4);

		selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

		CONTEXT->ssql->flag=SCF_SELECT;//"select";
		CONTEXT->ssql->sstr.selection.is_subquery=1;
		// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;
		$$ = CONTEXT->ssql;
        
		query_stack_pop(CONTEXT);
	}
	;
subquery_value_list:
	LBRACE value value_list RBRACE {
		subquery_create_value_list(CONTEXT->value_list, CONTEXT->values, CONTEXT->value_length);
		CONTEXT->value_list_length = CONTEXT->value_length;
		CONTEXT->value_length = 0;
	}
	;
sub_query_init:
   LBRACE SELECT {
	// 将当前状态入栈
	query_stack_push(CONTEXT);
   }
   ;
having:
	| HAVING having_condition having_condition_list {

	}
	;
having_condition_list:
    /* empty */
    | AND having_condition condition_list {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
    ;
having_condition:
    aggregate LBRACE STAR RBRACE comOp value 
	{
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, "*");
		left_attr.aggr_type = $1;

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
	| aggregate LBRACE ID RBRACE comOp value 
	{
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, $3);
		left_attr.aggr_type = $1;

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
	| aggregate LBRACE ID DOT ID RBRACE comOp value 
	{
		RelAttr left_attr;
		relation_attr_init(&left_attr, $3, $5);
		left_attr.aggr_type = $1;

		Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

		Condition having_condition;
		condition_init(&having_condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
		CONTEXT->having_conditions[CONTEXT->having_condition_length++] = having_condition;
	}
    | aggregate LBRACE ID RBRACE comOp ID
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $3);
			left_attr.aggr_type = $1;

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $6);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
	| aggregate LBRACE ID RBRACE comOp ID DOT ID
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $3);
			left_attr.aggr_type = $1;

			RelAttr right_attr;
			relation_attr_init(&right_attr, $6, $8);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
    | aggregate LBRACE ID DOT ID RBRACE comOp ID
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $3, $5);
			left_attr.aggr_type = $1;

			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $8);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;						
    	}
    | aggregate LBRACE ID DOT ID RBRACE comOp ID DOT ID
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $3, $5);
			left_attr.aggr_type = $1;

			RelAttr right_attr;
			relation_attr_init(&right_attr, $8, $10);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
    	}
    ;
order:
    /* empty */ 
    | ORDER BY order_attr {	
		
			}
    ;
order_attr:
	ID order_type order_list{
			OrderAttr attr;
			order_attr_init(&attr, NULL, $1, $2);
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
		}
  	| ID DOT ID order_type order_list {
			OrderAttr attr;
			order_attr_init(&attr, $1, $3, $4);
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
		}
    ;
order_list:
    /* empty */
    | COMMA ID order_type order_list{
			OrderAttr attr;
			order_attr_init(&attr, NULL, $2, $3);
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
      }
    | COMMA ID DOT ID order_type order_list{
			OrderAttr attr;
			order_attr_init(&attr, $2, $4, $5);
			selects_append_orders(&CONTEXT->ssql->sstr.selection, &attr);
  	  }
  	;

comOp:
  	  EQ { CONTEXT->comp = EQUAL_TO; }
    | LT { CONTEXT->comp = LESS_THAN; }
    | GT { CONTEXT->comp = GREAT_THAN; }
    | LE { CONTEXT->comp = LESS_EQUAL; }
    | GE { CONTEXT->comp = GREAT_EQUAL; }
    | NE { CONTEXT->comp = NOT_EQUAL; }
    | LIKE_TOKEN { CONTEXT->comp = LIKE; }
    | NOT_TOKEN LIKE_TOKEN { CONTEXT->comp = NOT_LIKE; }
	// hack for IS NULL and IS NOT NULL, dirty, but works
    | IS { CONTEXT->comp = IS_NULL; }
    | IS NOT_TOKEN { CONTEXT->comp = IS_NOT_NULL; }
	| IN_TOKEN {CONTEXT->comp = IN;}
	| NOT_TOKEN IN_TOKEN {CONTEXT->comp = NOT_IN;}
	| NOT_TOKEN EXISTS_TOKEN {CONTEXT->comp = NOT_EXISTS;}
	| EXISTS_TOKEN {CONTEXT->comp = EXISTS;}
    ;

load_data:
		LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON
		{
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, $7, $4);
		}
		;
%%
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
