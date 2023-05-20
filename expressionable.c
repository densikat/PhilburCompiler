//
// Created by David Ensikat on 19/5/2023.
//

#include "compiler.h"
#include <stdlib.h>

/**
* Format: {operator1, operator2, operator3, NULL}
*/

struct expressionable_op_precedence_group op_precedence[TOTAL_OPERATOR_GROUPS] = {
	{.operators={"++", "--", "()", "[]", "(", "[", ".", "->", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT},
	{.operators={"*", "/", "%", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT},
	{.operators={"+", "-", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT},
	{.operators={"<<", ">>", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT},
	{.operators={"<", "<=", ">", ">=", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT},
	{.operators={"==", "!=", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT},
	{.operators={"&", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT},
	{.operators={"^", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT},
	{.operators={"|", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT},
	{.operators={"&&", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT},
	{.operators={"||", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT},
	{.operators={"?", ":", NULL}, .associativity=ASSOCIATIVITY_RIGHT_TO_LEFT},
	{.operators={"=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=",
				 "^=", "|=", NULL}, .associativity=ASSOCIATIVITY_RIGHT_TO_LEFT},
	{.operators={",", NULL}, .associativity=ASSOCIATIVITY_LEFT_TO_RIGHT}
};