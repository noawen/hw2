#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "visitor_room.h"

/* IMPLEMENT HERE ALL WHAT IS NEEDED */
Result init_challenge_activity(ChallengeActivity *activity, Challenge *challenge){
	if (activity == NULL || challenge == NULL)
			return NULL_PARAMETER;
	activity->challenge = challenge;
	activity->visitor = NULL;
	activity->start_time = 0;
	return OK;
}

Result reset_challenge_activity(ChallengeActivity *activity){
	if (activity == NULL)
			return NULL_PARAMETER;
	activity->challenge = NULL;
	activity->visitor = NULL;
	activity->start_time = 0;
	return OK;
}

Result init_visitor(Visitor *visitor, char *name, int id){
	if (visitor == NULL || name == NULL)
			return NULL_PARAMETER;

	visitor->visitor_name = malloc(strlen(name)+1);
	if (visitor->visitor_name == NULL)
		return MEMORY_PROBLEM;
	visitor->visitor_name = strcpy(visitor->visitor_name ,name);
	visitor->visitor_id = id;
	visitor->room_name = NULL;
	visitor->current_challenge = NULL;
	return OK;
}

Result reset_visitor(Visitor *visitor){
	if (visitor == NULL)
		return NULL_PARAMETER;
	free(visitor->visitor_name);
	visitor->visitor_name = NULL;

	visitor->visitor_id = 0;
	visitor->room_name = NULL;
	visitor->current_challenge = NULL;
	return OK;
}

Result init_room(ChallengeRoom *room, char *name, int num_challenges){
	if (room == NULL || name == NULL)
		return NULL_PARAMETER;
	if ( num_challenges < 1)
		return ILLEGAL_PARAMETER;
	room->name =(malloc(strlen(name)+1));
	if (room->name == NULL)
		return MEMORY_PROBLEM;
	room->name = strcpy(room->name ,name);
	room->challenges = (malloc(num_challenges*
			sizeof(ChallengeActivity)));
	if (room->challenges == NULL){
		free(room->name);
		room->name = NULL;
		return MEMORY_PROBLEM;
	}
	return OK;
}

Result reset_room(ChallengeRoom *room){
	if (room == NULL)
		return NULL_PARAMETER;
	free(room->name);
	room->name = NULL;
	room->num_of_challenges = 0;
	free(room->challenges);
	room->challenges = NULL;
	return OK;
}

Result num_of_free_places_for_level(ChallengeRoom *room, Level level, int *places){
	*places = 0;

    if (room == NULL || places == NULL)
		return NULL_PARAMETER;
	for (int i = 0; i < room->num_of_challenges; i++){
		if(level == room->challenges[i].challenge->level || level == All_Levels) {
        }
            if (room->challenges[i].visitor == NULL)
				(*places)++;
	}
	return OK;
}

Result change_room_name(ChallengeRoom *room, char *new_name){
	if (room == NULL || new_name == NULL)
		return NULL_PARAMETER;
	room->name = (char *)(malloc(strlen(new_name)+1));
	if (room->name == NULL)
		return MEMORY_PROBLEM;
	room->name = strcpy(room->name ,new_name);
	return OK;
}

Result room_of_visitor(Visitor *visitor, char **room_name){
	if (visitor == NULL || *room_name == NULL)
		return NULL_PARAMETER;
	*(room_name) =(malloc(strlen(*visitor->room_name)+1));
	if (*room_name == NULL)
		return MEMORY_PROBLEM;
	*(room_name) = strcpy(*room_name, *visitor->room_name);
	return OK;
}


Result visitor_enter_room(ChallengeRoom *room, Visitor *visitor, Level level, int start_time){
/* the challenge to be chosen is the lexicographically named smaller one that has
   the required level. assume all names are different. */
	int min_index = -1;
	int is_first_good_index = 1;

	if (visitor == NULL || room == NULL)
		return NULL_PARAMETER;

	if(visitor->room_name != NULL)
		return ALREADY_IN_ROOM;

	for (int i = 0; i < room->num_of_challenges; i++){
		if(level == room->challenges[i].challenge->level || level == All_Levels){
			if (room->challenges[i].visitor == NULL) {
				if (is_first_good_index){
					min_index=i;
					is_first_good_index = 0;
				}
				else
					if (strcmp(room->challenges[i].challenge->name,
							room->challenges[min_index].challenge->name) < 0){
						min_index = i;
					}
			}
		}
	}
	if (min_index == -1)
		return NO_AVAILABLE_CHALLENGES;
	room->challenges[min_index].visitor = visitor;
	room->challenges[min_index].visitor->room_name = &(room->name);
	room->challenges[min_index].visitor->current_challenge =
			&(room->challenges[min_index]);
	room->challenges[min_index].start_time = start_time;
	Result result = inc_num_visits(room->challenges[min_index].challenge);
	if (result != OK)
		return result;
	return OK;
}

Result visitor_quit_room(Visitor *visitor, int quit_time){
	int time_in_room = 0, start_time = 0;
	if (visitor == NULL)
		return NULL_PARAMETER;
	if(visitor->room_name == NULL)
			return NOT_IN_ROOM;
    if (visitor->current_challenge == NULL) {
        return NULL_PARAMETER;
    }
	start_time = visitor->current_challenge->start_time;
	time_in_room = quit_time - start_time;
	if ((visitor->current_challenge->challenge->best_time == 0)
			|| (time_in_room < visitor->current_challenge->challenge->best_time))
		set_best_time_of_challenge(visitor->current_challenge->challenge,
				time_in_room);

	visitor->room_name = NULL;
	visitor->current_challenge->start_time = 0;
	visitor->current_challenge = NULL;
	return OK;
}
