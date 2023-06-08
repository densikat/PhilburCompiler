#ifndef PHILBURCOMPILER_H
#define PHILBURCOMPILER_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define S_EQ(str, str2) (str && str2 && (strcmp(str, str2) == 0))

// Forward declarations
struct datatype;

struct pos {
  int line;
  int col;
  const char *filename;
};

#define NUMERIC_CASE                                                           \
  case '0':                                                                    \
  case '1':                                                                    \
  case '2':                                                                    \
  case '3':                                                                    \
  case '4':                                                                    \
  case '5':                                                                    \
  case '6':                                                                    \
  case '7':                                                                    \
  case '8':                                                                    \
  case '9'

#define OPERATOR_CASE_EXCLUDING_DIVISION                                       \
  case '+':                                                                    \
  case '-':                                                                    \
  case '*':                                                                    \
  case '>':                                                                    \
  case '<':                                                                    \
  case '^':                                                                    \
  case '%':                                                                    \
  case '!':                                                                    \
  case '=':                                                                    \
  case '~':                                                                    \
  case '|':                                                                    \
  case '&':                                                                    \
  case '(':                                                                    \
  case '[':                                                                    \
  case ',':                                                                    \
  case '.':                                                                    \
  case '?'

#define SYMBOL_CASE                                                            \
  case '{':                                                                    \
  case '}':                                                                    \
  case ':':                                                                    \
  case ';':                                                                    \
  case '#':                                                                    \
  case '\\':                                                                   \
  case ')':                                                                    \
  case ']'

enum { LEXICAL_ANALYSIS_ALL_OK, LEXICAL_ANALYSIS_INPUT_ERROR };

enum {
  TOKEN_TYPE_IDENTIFIER,
  TOKEN_TYPE_KEYWORD,
  TOKEN_TYPE_OPERATOR,
  TOKEN_TYPE_SYMBOL,
  TOKEN_TYPE_NUMBER,
  TOKEN_TYPE_STRING,
  TOKEN_TYPE_COMMENT,
  TOKEN_TYPE_NEWLINE
};

enum {
  NUMBER_TYPE_NORMAL,
  NUMBER_TYPE_LONG,
  NUMBER_TYPE_FLOAT,
  NUMBER_TYPE_DOUBLE
};

struct token {
  int type;
  int flags;
  struct pos pos;

  union {
	char cval;
	const char *sval;
	unsigned int inum;
	unsigned long lnum;
	unsigned long long llnum;
	void *any;
  };

  struct token_number {
	int type;
  };
  struct token_number num;

  // true if whitespace between tokens
  bool whitespace;

  const char *between_brackets;
};

struct lex_process;

typedef char (*LEX_PROCESS_NEXT_CHAR)(struct lex_process *process);
typedef char (*LEX_PROCESS_PEEK_CHAR)(struct lex_process *process);
typedef void (*LEX_PROCESS_PUSH_CHAR)(struct lex_process *process, char c);

struct lex_process_functions {
  LEX_PROCESS_NEXT_CHAR next_char;
  LEX_PROCESS_PEEK_CHAR peek_char;
  LEX_PROCESS_PUSH_CHAR push_char;
};

struct lex_process {
  struct pos pos;
  struct vector *token_vec;
  struct compile_process *compiler;

  int current_expression_count;
  struct buffer *parentheses_buffer;
  struct lex_process_functions *function;

  // Data passed by consumer of lexer
  void *private_data;
};

enum { COMPILER_FILE_COMPILED_OK, COMPILER_FAILED_WITH_ERRORS };

struct scope {
  int flags;

  // void*
  struct vector *entities;

  // Total number of bytes in scope. Aligned to 16 bytes
  size_t size;

  // NULL if no parent
  struct scope *parent;
};

enum {
  SYMBOL_TYPE_NODE,
  SYMBOL_TYPE_NATIVE_FUNCTION,
  SYMBOL_TYPE_NATIVE_UNKNOWN
};

struct symbol {
  const char *name;
  int type;
  void *data;
};

struct codegen_entry_point {
  int id;
};

struct codegen_exit_point {
  int id;
};

struct string_table_element {
  // The string itself
  const char *str;

  // Label that points to memory where string is found
  const char label[50];
};

struct stack_frame_data {
  struct datatype *dtype; // Data type that was pushed to Stack
};

struct stack_frame_element {
  int flags;
  int type;
  const char *name; // name of frame element, not variable name i.e. result_value
  int offset_from_bp; // offset from base pointer
  struct stack_frame_data data;
};

struct code_generator {
  struct vector *string_table; // contains string_table_elements
  struct vector *entry_points;
  struct vector *exit_points;
};

#define STACK_PUSH_SIZE 4 // 32-bit

enum {
  STACK_FRAME_ELEMENT_TYPE_LOCAL_VARIABLE,
  STACK_FRAME_ELEMENT_TYPE_SAVED_REGISTER,
  STACK_FRAME_ELEMENT_TYPE_SAVED_BP,
  STACK_FRAME_ELEMENT_TYPE_PUSHED_VALUE,
  STACK_FRAME_ELEMENT_TYPE_UNKNOWN
};

enum {
  STACK_FRAME_ELEMENT_FLAG_IS_PUSHED_ADDRESS = 0b00000001,
  STACK_FRAME_ELEMENT_FLAG_ELEMENT_NOT_FOUND = 0b00000010,
  STACK_FRAME_ELEMENT_FLAG_IS_NUMERICAL = 0b00000100,
  STACK_FRAME_ELEMENT_FLAG_HAS_DATATYPE = 0b00001000
};

struct compile_process {
  int flags; // Flags for how file should be compiled

  struct pos pos;
  struct compile_process_input_file {
	FILE *fp;
	const char *abs_path;
  };
  struct compile_process_input_file cfile;

  // A vector of tokens from lexical analysis
  struct vector *token_vec;

  struct vector *node_vec;
  struct vector *node_tree_vec;

  FILE *ofile;

  struct {
	struct scope *root;
	struct scope *current;
  } scope;

  struct {
	// Current active symbol table.
	struct vector *table;
	// struc vector* -> table vectors, multi symbol tables
	struct vector *tables;
  } symbols;

  struct code_generator *generator;
};

enum { PARSE_ALL_OK, PARSE_GENERAL_ERROR };

enum { CODEGEN_ALL_OK, CODEGEN_GENERAL_ERROR };

enum {
  NODE_TYPE_EXPRESSION,
  NODE_TYPE_EXPRESSION_PARENTHESES,
  NODE_TYPE_NUMBER,
  NODE_TYPE_IDENTIFIER,
  NODE_TYPE_STRING,
  NODE_TYPE_VARIABLE,
  NODE_TYPE_VARIABLE_LIST,
  NODE_TYPE_FUNCTION,
  NODE_TYPE_BODY,
  NODE_TYPE_STATEMENT_RETURN,
  NODE_TYPE_STATEMENT_IF,
  NODE_TYPE_STATEMENT_ELSE,
  NODE_TYPE_STATEMENT_WHILE,
  NODE_TYPE_STATEMENT_DO_WHILE,
  NODE_TYPE_STATEMENT_FOR,
  NODE_TYPE_STATEMENT_BREAK,
  NODE_TYPE_STATEMENT_CONTINUE,
  NODE_TYPE_STATEMENT_SWITCH,
  NODE_TYPE_STATEMENT_CASE,
  NODE_TYPE_STATEMENT_DEFAULT,
  NODE_TYPE_STATEMENT_GOTO,
  NODE_TYPE_UNARY,
  NODE_TYPE_TERNARY,
  NODE_TYPE_LABEL,
  NODE_TYPE_STRUCT,
  NODE_TYPE_UNION,
  NODE_TYPE_BRACKET,
  NODE_TYPE_CAST,
  NODE_TYPE_BLANK
};

enum {
  NODE_FLAG_INSIDE_EXPRESSION = 0b00000001,
  NODE_FLAG_IS_FORWARD_DECLARATION = 0b00000010,
  NODE_FLAG_HAS_VARIABLE_COMBINED = 0b00000100
};

struct array_brackets {
  // vector of struct nodes pointers
  struct vector *n_brackets;
};

struct node;

struct datatype {
  int flags;
  // i.e long, int, float etc
  int type;

  // i.e long int a; int being secondary
  struct datatype *secondary;

  const char *type_str;

  // sizeof datatype
  size_t size;

  int pointer_depth;

  union {
	struct node *struct_node;
	struct node *union_node;
  };

  struct array {
	struct array_brackets *brackets;
	/**
	 * Total array size: datatype_size * each index
	 */
	size_t size;
  } array;
};

struct parsed_switch_case {
  // index of the parsed case
  int index;
};

struct node {
  int type;
  int flags;

  struct pos pos;

  struct node_binded {
	struct node *owner;    // pointer to our body node
	struct node *function; // pointer to function we're in
  };

  struct node_binded binded;

  union {
	struct exp {
	  struct node *left;
	  struct node *right;
	  const char *op;
	} exp;

	struct parenthesis {
	  // The expression inside the parenthesis node
	  struct node *exp;
	} parenthesis;

	struct var {
	  struct datatype type;
	  int padding;
	  int aoffset; // offset from scope it's related to (aligned)
	  const char *name;
	  struct node *val;
	} var;

	struct node_ternary {
	  struct node *true_node;
	  struct node *false_node;
	} ternary;

	struct varlist {
	  // A list of struct node* variables
	  struct vector *list;
	} var_list;

	struct bracket {
	  struct node *inner;
	} bracket;

	struct _struct {
	  const char *name;
	  struct node *body_n;

	  // NULL if no variable attached to struct
	  struct node *var;
	} _struct;

	struct _union {
	  const char *name;
	  struct node *body_n;

	  // NULL if no variable attached to struct
	  struct node *var;
	} _union;

	struct body {
	  // struct node* vector of statements
	  struct vector *statements;

	  // size of all statements stored together
	  size_t size;

	  // True if we had to pad size
	  bool padded;

	  // Pointer to largest (size) variable node in statements vector
	  struct node *largest_var_node;
	} body;

	// Represents a function
	struct function {
	  int flags;

	  // Return type, void, int etc.
	  struct datatype rtype;

	  // i.e main in int main()
	  const char *name;

	  struct function_arguments {
		// Node pointers. NODE_TYPE_VARIABLE
		struct vector *vector;

		// How much to add to base pointer to find first argument
		size_t stack_addition;
	  } args;

	  // NULL if function prototype
	  struct node *body_n;

	  struct stack_frame {
		struct vector *elements; // a vector of stack frame elements
	  } frame;

	  // Stack size for all variables in function
	  size_t stack_size;
	} func;

	struct statement {
	  struct return_stmt {
		// expression of the return
		struct node *exp;
	  } return_stmt;

	  struct if_stmt {
		// if (COND)
		struct node *cond_node;

		// if (COND) { BODY };
		struct node *body_node;

		// else
		struct node *next;
	  } if_stmt;

	  struct else_stmt {
		struct node *body_node;
	  } else_stmt;

	  struct for_stmt {
		struct node *init_node;
		struct node *cond_node;
		struct node *loop_node;
		struct node *body_node;
	  } for_stmt;

	  struct while_stmt {
		struct node *exp_node;
		struct node *body_node;
	  } while_stmt;

	  struct do_while_stmt {
		struct node *exp_node;
		struct node *body_node;
	  } do_while_stmt;

	  struct switch_stmt {
		struct node *exp;
		struct node *body;
		struct vector *cases;
		bool has_default_case;
	  } switch_stmt;

	  struct _case_stmt {
		struct node *exp_node;
	  } _case;

	  struct _goto_stmt {
		struct node *label;
	  } _goto;

	} stmt;

	struct node_label {
	  struct node *name;
	} label;

	struct cast {
	  struct datatype dtype;
	  struct node *operand;
	} cast;
  };

  union {
	char cval;
	const char *sval;
	unsigned int inum;
	unsigned long lnum;
	unsigned long long llnum;
	void *any;
  };
};

enum {
  RESOLVER_ENTITY_FLAG_IS_STACK = 0b00000001,
  RESOLVER_ENTITY_FLAG_NO_MERGE_WITH_NEXT_ENTITY = 0b00000010,
  RESOLVER_ENTITY_FLAG_NO_MERGE_WITH_LEFT_ENTITY = 0b00000100,
  RESOLVER_ENTITY_FLAG_DO_INDIRECTION = 0b00001000,
  RESOLVER_ENTITY_FLAG_JUST_USE_OFFSET = 0b00010000,
  RESOLVER_ENTITY_FLAG_IS_POINTER_ARRAY_ENTITY = 0b00100000,
  RESOLVER_ENTITY_FLAG_WAS_CASTED = 0b01000000,
  RESOLVER_ENTITY_FLAG_USES_ARRAY_BRACKETS = 0b10000000
};

enum {
  RESOLVER_ENTITY_TYPE_VARIABLE,
  RESOLVER_ENTITY_TYPE_FUNCTION,
  RESOLVER_ENTITY_TYPE_STRUCTURE,
  RESOLVER_ENTITY_TYPE_FUNCTION_CALL,
  RESOLVER_ENTITY_TYPE_ARRAY_BRACKET,
  RESOLVER_ENTITY_TYPE_RULE,
  RESOLVER_ENTITY_TYPE_GENERAL,
  RESOLVER_ENTITY_TYPE_UNARY_GET_ADDRESS,
  RESOLVER_ENTITY_TYPE_UNARY_INDIRECTION,
  RESOLVER_ENTITY_TYPE_UNSUPPORTED,
  RESOLVER_ENTITY_TYPE_CAST
};

struct resolver_entity;
struct resolver_result;
struct resolver_scope;
struct resolver_process;

enum {
  RESOLVER_SCOPE_FLAG_IS_STACK = 0b00000001
};

typedef void *(*RESOLVER_NEW_ARRAY_BRACKET_ENTITY)(struct resolver_result *result, struct node *array_entity_node);
typedef void *(*RESOLVER_DELETE_SCOPE)(struct resolver_scope *scope);
typedef void *(*RESOLVER_DELETE_ENTITY)(struct resolver_entity *entity);
typedef struct resolver_entity
	*(*RESOLVER_MERGE_ENTITIES)
	(struct resolver_process *process,
	 struct resolver_result *result,
	 struct resolver_entity *left_entity,
	 struct resolver_entity *right_entity);
typedef void *(*RESOLVER_MAKE_PRIVATE)
	(struct resolver_entity *entity, struct node *node, int offset, struct resolver_scope *scope);
typedef void *(*RESOLVER_SET_RESULT_BASE)(struct resolver_result *result, struct resolver_entity *base_entity);

struct resolver_callbacks {
  RESOLVER_NEW_ARRAY_BRACKET_ENTITY new_array_entity;
  RESOLVER_DELETE_SCOPE delete_scope;
  RESOLVER_DELETE_ENTITY delete_entity;
  RESOLVER_MERGE_ENTITIES merge_entities;
  RESOLVER_MAKE_PRIVATE make_private;
  RESOLVER_SET_RESULT_BASE set_result_base;
};

struct resolver_process {
  struct resolver_scopes {
	struct resolver_scope *root;
	struct resolver_scope *current;
  } scope;
  struct compile_process *compiler;
  struct resolver_callbacks callbacks;
};

struct resolver_array_data {
  struct vector *array_entities; // nodes of type resolver_entity

};

enum {
  RESOLVER_RESULT_FLAG_FAILED = 0b00000001,
  RESOLVER_RESULT_FLAG_RUNTIME_NEEDED_TO_FINISH_PATH = 0b00000010,
  RESOLVER_RESULT_FLAG_PROCESSING_ARRAY_ENTITIES = 0b00000100,
  RESOLVER_RESULT_FLAG_HAS_POINTER_ARRAY_ACCESS = 0b00001000,
  RESOLVER_RESULT_FLAG_FIRST_ENTITY_LOAD_TO_EBX = 0b00010000,
  RESOLVER_RESULT_FLAG_FIRST_ENTITY_PUSH_VALUE = 0b00100000,
  RESOLVER_RESULT_FLAG_FINAL_INDIRECTION_REQUIRED_FOR_VALUE = 0b01000000,
  RESOLVER_RESULT_FLAG_DOES_GET_ADDRESS = 0b10000000
};

struct resolver_result {
  struct resolver_entity *first_entity_const;
  struct resolver_entity *identifier; // in a.b.d it would be a
  struct resolver_entity *last_struct_union_entity;
  struct resolver_array_data array_data;
  struct resolver_entity *entity; // root entity
  struct resolver_entity *last_entity;
  int flags;
  size_t count; // total number of entities

  struct resolver_result_base {
	char address[60];
	char base_address[60];
	int offset;
  } base;
};

struct resolver_scope {
  int flags;
  struct vector *entities;
  struct resolver_scope *next;
  struct resolver_scope *prev;
  void *private;
};

struct resolver_entity {
  int type;
  int flags;
  const char *name; // function name, variable name etc
  int offset; // offset from stack (EBP+offset)
  struct node *node; // node entity is relating to
  union {

	struct resolver_entity_var_data {
	  struct datatype dtype;
	  struct resolver_array_runtime {
		struct datatype dtype;
		struct node *node;
		int multiplier;
	  } array_runtime;
	} var_data;

	struct resolver_array {
	  struct datatype dtype;
	  struct node *array_index_node;
	  int index;
	} array;

	struct resolver_entity_function_call_data {
	  struct vector *arguments; // struct node* vector
	  size_t stack_size; // total bytes used by function call
	} func_call_data;

	struct resolver_entity_rule {
	  struct resolver_entity_rule_left {
		int flags;
	  } left;
	  struct resolver_entity_rule_right {
		int flags;
	  } right;
	} rule;

	struct resolver_indirection {
	  int depth; // e.g. **pointer == 2
	} indirection;
  };

  struct entity_last_resolve {
	struct node *referencing_node;
  } last_resolve;

  struct datatype dtype; // datatype of resolver entity

  struct resolver_scope *scope; // scope this resolver entity belongs to

  struct resolver_result *result; // the result of the resolution

  struct resolver_process *process;

  void *private;

  struct resolver_entity *next;
  struct resolver_entity *prev;
};

enum {
  DATATYPE_FLAG_IS_SIGNED = 0b00000001,
  DATATYPE_FLAG_IS_STATIC = 0b00000010,
  DATATYPE_FLAG_IS_CONST = 0b00000100,
  DATATYPE_FLAG_IS_POINTER = 0b00001000,
  DATATYPE_FLAG_IS_ARRAY = 0b00010000,
  DATATYPE_FLAG_IS_EXTERN = 0b00100000,
  DATATYPE_FLAG_IS_RESTRICT = 0b01000000,
  DATATYPE_FLAG_IS_IGNORE_TYPE_CHECKING = 0b10000000,
  DATATYPE_FLAG_IS_SECONDARY = 0b100000000,
  DATATYPE_FLAG_STRUCT_UNION_NO_NAME = 0b1000000000,
  DATATYPE_FLAG_IS_LITERAL = 0b10000000000
};

enum {
  DATA_TYPE_VOID,
  DATA_TYPE_CHAR,
  DATA_TYPE_SHORT,
  DATA_TYPE_INTEGER,
  DATA_TYPE_LONG,
  DATA_TYPE_FLOAT,
  DATA_TYPE_DOUBLE,
  DATA_TYPE_STRUCT,
  DATA_TYPE_UNION,
  DATA_TYPE_UNKNOWN
};

enum {
  DATA_TYPE_EXPECT_PRIMITIVE,
  DATA_TYPE_EXPECT_UNION,
  DATA_TYPE_EXPECT_STRUCT
};

enum {
  DATA_SIZE_ZERO = 0,
  DATA_SIZE_BYTE = 1,
  DATA_SIZE_WORD = 2,
  DATA_SIZE_DWORD = 4,
  DATA_SIZE_DDWORD = 8
};

enum {
  STRUCT_ACCESS_BACKWARDS = 0b00000001,
  STRUCT_STOP_AT_POINTER_ACCESS = 0b00000010
};

enum { FUNCTION_NODE_FLAG_IS_NATIVE = 0b00000001 };

int compile_file(const char *filename, const char *out_filename, int flags);
struct compile_process *compile_process_create(const char *filename,
											   const char *filename_out,
											   int flags);

char compile_process_next_char(struct lex_process *lex_process);
char compile_process_peek_char(struct lex_process *lex_process);
void compile_process_push_char(struct lex_process *lex_process, char c);

void compiler_error(struct compile_process *compiler, const char *msg, ...);
void compiler_warning(struct compile_process *compiler, const char *msg, ...);

struct array_brackets *array_brackets_new();
void array_brackets_free(struct array_brackets *brackets);
void array_brackets_add(struct array_brackets *brackets,
						struct node *bracket_node);
struct vector *array_brackets_node_vector(struct array_brackets *brackets);
size_t array_brackets_calculate_size_from_index(struct datatype *dtype,
												struct array_brackets *brackets,
												int index);
size_t array_brackets_calculate_size(struct datatype *dtype,
									 struct array_brackets *brackets);
int array_total_indexes(struct datatype *dtype);

struct lex_process *lex_process_create(struct compile_process *compiler,
									   struct lex_process_functions *functions,
									   void *private_data);
void lex_process_free(struct lex_process *process);
void *lex_process_private(struct lex_process *process);
struct vector *lex_process_tokens(struct lex_process *process);
int lex(struct lex_process *process);

bool token_is_identifier(struct token *token);
bool token_is_symbol(struct token *token, char c);
bool token_is_nl_or_comment_or_newline_seperator(struct token *token);
bool token_is_keyword(struct token *token, const char *value);

bool keyword_is_datatype(const char *str);
bool token_is_primitive_keyword(struct token *token);

bool token_is_operator(struct token *token, const char *val);

struct node *node_create(struct node *_node);
struct node *node_pop();
struct node *node_peek();
struct node *node_peek_or_null();
struct node *variable_struct_or_union_body_node(struct node *node);
struct node *variable_node(struct node *node);
struct node *variable_node_or_list(struct node *node);
bool node_is_struct_or_union(struct node *node);
int array_multiplier(struct datatype *dtype, int index, int index_value);
int array_offset(struct datatype *dtype, int index, int index_value);

struct node *variable_struct_or_union_largest_variable_node(struct node *var_node);
int struct_offset(struct compile_process *compile_proc, const char *struct_name, const char *var_name,
				  struct node **var_node_out, int last_pos, int flags);
bool variable_node_is_primitive(struct node *node);
bool node_is_struct_or_union_variable(struct node *node);
void node_push(struct node *node);
void node_set_vector(struct vector *vec, struct vector *root_vec);
bool node_is_expressionable(struct node *node);
bool node_is_expression_or_parentheses(struct node *node);
bool node_is_value_type(struct node *node);
size_t function_node_argument_stack_addition(struct node *node);
struct node *node_peek_expressionable_or_null();
void make_bracket_node(struct node *node);
void make_exp_node(struct node *left_node, struct node *right_node,
				   const char *op);
void make_exp_parentheses_node(struct node *node);
void make_body_node(struct vector *body_vec, size_t size, bool padded,
					struct node *largest_var_node);
void make_struct_node(const char *name, struct node *body_node);
void make_union_node(const char *name, struct node *body_node);
void make_function_node(struct datatype *ret_type, const char *name,
						struct vector *arguments, struct node *body_node);
void make_if_node(struct node *cond_node, struct node *body_node,
				  struct node *next_node);
void make_else_node(struct node *body_node);
void make_return_node(struct node *exp_node);
void make_for_node(struct node *init_node, struct node *cond_node,
				   struct node *loop_node, struct node *body_node);
void make_while_node(struct node *exp_node, struct node *body_node);
void make_do_while_node(struct node *exp_node, struct node *body_node);
void make_switch_node(struct node *exp_node, struct node *body_node,
					  struct vector *cases, bool has_default_case);
void make_continue_node();
void make_break_node();
void make_label_node(struct node *name_node);
void make_goto_node(struct node *label_node);
void make_case_node(struct node *exp_node);
void make_ternary_node(struct node *true_node, struct node *false_node);
void make_cast_node(struct datatype *dtype, struct node *operand_node);
struct node *node_from_sym(struct symbol *sym);
struct node *node_from_symbol(struct compile_process *current_process,
							  const char *name);
struct node *struct_node_for_name(struct compile_process *current_process,
								  const char *name);
struct node *union_node_for_name(struct compile_process *current_process,
								 const char *name);
bool node_is_expression(struct node *node, const char *op);
bool is_array_node(struct node *node);
bool is_node_assignment(struct node *node);

struct lex_process *tokens_build_for_string(struct compile_process *compiler,
											const char *str);
int parse(struct compile_process *process);

#define TOTAL_OPERATOR_GROUPS 14
#define MAX_OPERATORS_IN_GROUP 12

enum { ASSOCIATIVITY_LEFT_TO_RIGHT, ASSOCIATIVITY_RIGHT_TO_LEFT };

struct expressionable_op_precedence_group {
  char *operators[MAX_OPERATORS_IN_GROUP];
  int associativity;
};

bool datatype_is_struct_or_union(struct datatype *dtype);
bool datatype_is_struct_or_union_for_name(const char *name);
bool datatype_is_primitive(struct datatype *dtype);

size_t datatype_size(struct datatype *dtype);
size_t datatype_element_size(struct datatype *dtype);
size_t datatype_size_for_array_access(struct datatype *dtype);
size_t datatype_size_no_ptr(struct datatype *dtype);

// variable size from given node
size_t variable_size(struct node *var_node);

// sums size of all variables in node
size_t variable_size_for_list(struct node *var_list_node);

int padding(int val, int to);
int align_value(int val, int to);
int align_value_treat_positive(int val, int to);
int compute_sum_padding(struct vector *vec);

struct scope *scope_alloc();
void scope_dealloc(struct scope *scope);
struct scope *scope_create_root(struct compile_process *process);
void scope_free_root(struct compile_process *process);
struct scope *scope_new(struct compile_process *process, int flags);
void scope_iteration_start(struct scope *scope);
void *scope_iterate_back(struct scope *scope);
void *scope_last_entity_at_scope(struct scope *scope);
void *scope_last_entity_from_scope_stop_at(struct scope *scope,
										   struct scope *stop_scope);
void *scope_last_entity_stop_at(struct compile_process *process,
								struct scope *stop_scope);
void *scope_last_entity(struct compile_process *process);
void scope_push(struct compile_process *process, void *ptr, size_t elem_size);
void scope_finish(struct compile_process *process);
struct scope *scope_current(struct compile_process *process);

void symresolver_initialize(struct compile_process *process);
void symresolver_new_table(struct compile_process *process);
void symresolver_end_table(struct compile_process *process);
struct symbol *symresolver_get_symbol(struct compile_process *process,
									  const char *name);
void symresolver_build_for_node(struct compile_process *process,
								struct node *node);
struct symbol *
symresolver_get_symbol_for_native_function(struct compile_process *process,
										   const char *name);

struct fixup;

/**
 * Return true if fixup was successful
 */
typedef bool (*FIXUP_FIX)(struct fixup *fixup);

/**
 * Signifies fixup has been removed from memory
 */
typedef void (*FIXUP_END)(struct fixup *fixup);

struct fixup_config {
  FIXUP_FIX fix;
  FIXUP_END end;
  void *private;
};

struct fixup_system {
  struct vector *fixups;
};

enum { FIXUP_FLAG_RESOLVED = 0b00000001 };

struct fixup {
  int flags;
  struct fixup_system *system;
  struct fixup_config config;
};

struct fixup_system *fixup_sys_new();
struct fixup_config *fixup_config(struct fixup *fixup);
void fixup_free(struct fixup *fixup);
void fixup_start_iteration(struct fixup_system *system);
struct fixup *fixup_next(struct fixup_system *system);
void fixup_sys_fixups_free(struct fixup_system *system);
void fixup_sys_free(struct fixup_system *system);
int fixup_sys_unresolved_fixups_count(struct fixup_system *system);
struct fixup *fixup_register(struct fixup_system *system,
							 struct fixup_config *config);
bool fixup_resolve(struct fixup *fixup);
void *fixup_private(struct fixup *fixup);
bool fixups_resolve(struct fixup_system *system);

int codegen(struct compile_process *process);
struct code_generator *codegenerator_new(struct compile_process *process);

// STACK FRAME FUNCTIONS
void stackframe_pop(struct node *func_node);
struct stack_frame_element *stackframe_back(struct node *func_node);
struct stack_frame_element *stackframe_back_expect(struct node *func_node,
												   int expecting_type,
												   const char *expecting_name);
void stackframe_pop_expecting(struct node *func_node, int expecting_type, const char *expecting_name);
void stackframe_peek_start(struct node *func_node);
struct stack_frame_element *stackframe_peek(struct node *func_node);
void stackframe_push(struct node *func_node, struct stack_frame_element *element);
void stackframe_sub(struct node *func_node, int type, const char *name, size_t amount);
void stackframe_add(struct node *func_node, int type, const char *name, size_t amount);
void stackframe_assert_empty(struct node *func_node);

#endif
