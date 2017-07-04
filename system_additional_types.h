/*
 * system_additional_types.h
 *
 *  Created on: 14 במאי 2017
 *      Author: Dell
 */

#ifndef SYSTEM_ADDITIONAL_TYPES_H_
#define SYSTEM_ADDITIONAL_TYPES_H_

#include "visitor_room.h"

typedef struct SVisitorList
{
	Visitor* visitor;
	struct SVisitorList *next_visitor;

} *VisitorList;

#endif /* SYSTEM_ADDITIONAL_TYPES_H_ */
