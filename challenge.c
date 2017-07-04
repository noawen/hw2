#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "challenge.h"

/* IMPLEMENT HERE ALL WHAT IS NEEDED */

Result init_challenge(Challenge *challenge, int id, char *name, Level level){
	if (name == NULL || challenge == NULL)
		return NULL_PARAMETER;
	challenge->name = (malloc(strlen(name)+1));
	if (challenge->name == NULL)
		return MEMORY_PROBLEM;
	challenge->name=strcpy(challenge->name,name);
	challenge->id = id;
	challenge->level = level;
	challenge->best_time = 0;
	challenge->num_visits = 0;
	return OK;
}

Result reset_challenge(Challenge *challenge){
	if(challenge == NULL)
		return NULL_PARAMETER;
	free(challenge->name);
	challenge->name = NULL;
	challenge-> id = 0;
	challenge->level = 0;
	challenge->best_time = 0;
	challenge->num_visits = 0;
	return OK;
}

Result change_name(Challenge *challenge, char *name){
	if (name == NULL || challenge == NULL)
		return NULL_PARAMETER;
	challenge->name = (malloc(strlen(name)+1));
	if (challenge->name == NULL)
		return MEMORY_PROBLEM;
	challenge->name = strcpy(challenge->name,name);
	return OK;
}

Result set_best_time_of_challenge(Challenge *challenge, int time){
	if (challenge == NULL)
			return NULL_PARAMETER;
	if (time < 0)
		return ILLEGAL_PARAMETER;
	challenge->best_time = time;
	return OK;
}

Result best_time_of_challenge(Challenge *challenge, int *time){
	if (challenge == NULL)
		return NULL_PARAMETER;
	*time = (challenge->best_time);
	return OK;
}

Result inc_num_visits(Challenge *challenge){
	if (challenge == NULL)
		return NULL_PARAMETER;
	challenge->num_visits++;
	return OK;
}

Result num_visits(Challenge *challenge, int *visits){
	if (challenge == NULL)
		return NULL_PARAMETER;
	*visits = (challenge->num_visits);
	return OK;
}

