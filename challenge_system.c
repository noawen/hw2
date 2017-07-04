#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "system_additional_types.h"
#include "challenge_system.h"
#define MAX_WORD_LEN 51

/* IMPLEMENT HERE ALL WHAT IS NEEDED */
/*gets id and returns a pointer to the matching Challenge */
static Challenge* find_challenge_by_id(int id, ChallengeRoomSystem *sys) {
	for (int i = 0; i < sys->num_challenges; i++) {
		if (id == sys->all_challenges[i].id)
			return (&(sys->all_challenges[i]));
	}
	return NULL;
}

/*gets pointer to a challenge array and the number of challenges in it
 resets all the challenges in the array*/
static Result destroy_all_challenge(Challenge *all_challenges,
		int num_challenges) {
	Result result;
	for (int i = 0; i < num_challenges; i++) {
		result = reset_challenge(&(all_challenges[i]));
	}
	return result;
}

Result create_system(char *init_file, ChallengeRoomSystem **sys) {
	char name_challenge[MAX_WORD_LEN], name_room[MAX_WORD_LEN];

	int id;
	Level level;
	Result result;

	if (sys == NULL)
		return NULL_PARAMETER;

	*sys = malloc(sizeof(ChallengeRoomSystem));
	if (*sys == NULL)
		return MEMORY_PROBLEM;
	((*sys)->name) = malloc(sizeof(char) * 50);
	if (((*sys)->name) == NULL)
		return MEMORY_PROBLEM;

	//func create sys//
	(*sys)->visitor_list = (malloc(sizeof(VisitorList)));
	if ((*sys)->visitor_list == NULL)
		return MEMORY_PROBLEM;
	(*sys)->visitor_list->visitor = NULL;
	(*sys)->visitor_list->next_visitor = NULL;

	(*sys)->last_system_time = 0;

	FILE *file;
	file = fopen(init_file, "r");

	if (file == NULL)
		return MEMORY_PROBLEM;

	fscanf(file, "%s %d", ((*sys)->name), &((*sys)->num_challenges));

	(*sys)->all_challenges = (malloc(
			((*sys)->num_challenges) * sizeof(Challenge)));
	if ((*sys)->all_challenges == NULL) {
		free(*sys);
		sys = NULL;
		return MEMORY_PROBLEM;
	}

	for (int i = 0; i < (*sys)->num_challenges; i++) {
		int level_num;
		fscanf(file, "%s %d %d", name_challenge, &id, &level_num);
		level = level_num;
		result = init_challenge(&((*sys)->all_challenges[i]), id,
				name_challenge, level);
		if (result != OK)
			return result;
	}

	fscanf(file, "%d", &((*sys)->num_rooms));
	(*sys)->all_rooms = (malloc(
			(*sys)->num_rooms * sizeof(ChallengeRoom)));
	if ((*sys)->all_rooms == NULL) {
		destroy_all_challenge((*sys)->all_challenges, (*sys)->num_challenges);
		free((*sys)->all_challenges);
		(*sys)->all_challenges = NULL;
		free(*sys);
		(*sys) = NULL;
		return MEMORY_PROBLEM;
	}

	for (int i = 0; i < (*sys)->num_rooms; i++) {
		fscanf(file, "%s %d", name_room,
				&((*sys)->all_rooms[i].num_of_challenges));
		result = init_room(&(*sys)->all_rooms[i], name_room,
				((*sys)->all_rooms[i].num_of_challenges));

		if (result != OK)
			return result;

		for (int j = 0; j < (*sys)->all_rooms[i].num_of_challenges; j++) {
			fscanf(file, "%d", &id);
			(*sys)->all_rooms[i].challenges[j].challenge = find_challenge_by_id(
					id, (*sys));
			result = init_challenge_activity(
					&((*sys)->all_rooms[i].challenges[j]),
					(*sys)->all_rooms[i].challenges[j].challenge);
			if (result != OK)
				return result;
		}
	}

	fclose(file);

	return OK;
}
/*gets a pointer to a system. returning the first index which it's element
 is greater than 0. if there is no such- returns -1*/
static int find_first_min_time_index_greater_than_zero(ChallengeRoomSystem *sys) {
	for (int i = 0; i < sys->num_challenges; i++) {
		if (sys->all_challenges[i].best_time > 0)
			return i;
	}
	return -1;
}

Result destroy_system(ChallengeRoomSystem *sys, int destroy_time,
		char **most_popular_challenge_p, char **challenge_best_time) {
	Result result;
	int min_time_index, most_visited_index = 0;


	if (sys == NULL)
		return NULL_PARAMETER;

        if (destroy_time < sys->last_system_time) {
		free(sys);
		sys = NULL;
		return ILLEGAL_TIME;
	}
     if (sys->visitor_list->visitor != NULL) {
        result = all_visitors_quit(sys, destroy_time);
        if (result != OK)
            return result;
    }


	min_time_index = find_first_min_time_index_greater_than_zero(sys);
	for (int i = 0; i < sys->num_challenges; i++) {
		if (sys->all_challenges[i].num_visits
				> sys->all_challenges[most_visited_index].num_visits)
			most_visited_index = i;
		else if (sys->all_challenges[i].num_visits
				== sys->all_challenges[most_visited_index].num_visits) {
			if (strcmp(sys->all_challenges[i].name,
					sys->all_challenges[most_visited_index].name) < 0)
				most_visited_index = i;
		}
		if (min_time_index != -1) {
			if (sys->all_challenges[i].best_time > 0) {
				if (sys->all_challenges[i].best_time
						< sys->all_challenges[min_time_index].best_time)
					min_time_index = i;
				else if (sys->all_challenges[i].best_time
						== sys->all_challenges[min_time_index].best_time) {
					if (strcmp(sys->all_challenges[i].name,
							sys->all_challenges[min_time_index].name) < 0)
						min_time_index = i;
				}
			}
		}
	}
	if (sys->all_challenges[most_visited_index].num_visits == 0)
		most_popular_challenge_p = NULL;
	else {

        char *name_most_visited_challenge = (malloc(strlen(sys->all_challenges[most_visited_index].name)+1));
        if (name_most_visited_challenge == NULL)
            return MEMORY_PROBLEM;
         strcpy(name_most_visited_challenge,sys->all_challenges[most_visited_index].name);
        *most_popular_challenge_p = name_most_visited_challenge;

    }



	if (min_time_index == -1)
		challenge_best_time = NULL;
	else {

       char * name_best_challenge = (malloc(strlen(sys->all_challenges[min_time_index].name)+1));
        if (name_best_challenge == NULL)
            return MEMORY_PROBLEM;
        strcpy(name_best_challenge,sys->all_challenges[min_time_index].name);
        *challenge_best_time = name_best_challenge;

    }


    for (int i = 0; i < sys->num_challenges; i++) {
        reset_challenge(&(sys->all_challenges[i]));
    }
    free (sys->all_challenges);
    sys->all_challenges = NULL;
    sys->num_challenges = 0;

    for (int i = 0; i < sys->num_rooms; i++) {
        reset_room(&(sys->all_rooms[i]));
    }
    free (sys->all_rooms);
    sys->all_rooms = NULL;
    sys->num_rooms = 0;

    free (sys->name);
    sys->name = NULL;
    sys->visitor_list = NULL;
    sys->last_system_time = 0;
  //  free (sys);
	return OK;
}

/*gets visitor list and id from type int
 returns ALREADY IN ROOM if the visitor connected the id is in the system
 returns NOT IN ROOM is the visitor is not in the room
 else- returns NULL*/
static Result check_if_visitor_is_already_in_system(ChallengeRoomSystem *sys,
		int id) {
	if (sys->visitor_list == NULL)
		return NULL_PARAMETER;
	VisitorList curr_visitor = sys->visitor_list;

	if (sys->visitor_list->visitor == NULL)
		return NOT_IN_ROOM;

	if ((curr_visitor->visitor->visitor_id) == id) {
		return ALREADY_IN_ROOM;
	}

	while (curr_visitor->next_visitor != NULL) {
		if (curr_visitor->next_visitor->visitor->visitor_id == id) {
			return ALREADY_IN_ROOM;
		}
		curr_visitor = curr_visitor->next_visitor;
	}
	return NOT_IN_ROOM;
}

/*gets pointer to Visitor. creates and resets node in the linked list */
static VisitorList visitor_node_create(Visitor *visitor) {
	VisitorList new_visitor = (VisitorList) malloc(sizeof(VisitorList));
	if (new_visitor == NULL)
		return NULL;
	new_visitor->visitor = visitor;
	new_visitor->next_visitor = NULL;
	return new_visitor;
}

/*gets visitor list and a new visitor to add to it,
 adding the visitor to the end of the list
 returning OK if the adding worked, else returns NULL */
static Result add_visitor_to_list(ChallengeRoomSystem *sys,
		VisitorList new_visitor) {
	if (sys->visitor_list == NULL)
		return NULL_PARAMETER;

	VisitorList curr_visitor = sys->visitor_list;
	if (curr_visitor->visitor == NULL) {
		sys->visitor_list = new_visitor;
		return OK;
	}
	while (curr_visitor->next_visitor != NULL) {
		curr_visitor = curr_visitor->next_visitor;
	}
	curr_visitor->next_visitor = new_visitor;
	return OK;
}

/*gets a node needed to be deleted
 returning OK if the removal worked, else returns NULL */
static Result destroy_visitor_node(VisitorList node_to_destroy) {
	if (node_to_destroy == NULL)
		return NULL_PARAMETER;
	reset_visitor(node_to_destroy->visitor);
	free(node_to_destroy->visitor);
	node_to_destroy->visitor = NULL;
	free(node_to_destroy);
	node_to_destroy = NULL;
	return OK;
}

/*gets visitor list and a id num. which connected to specific visitor,
 removing the visitor from the list
 returning OK if the removal worked, else returns NULL */
static Result remove_visitor_from_list(ChallengeRoomSystem *sys, int id) {
	if (sys->visitor_list == NULL)
		return NULL_PARAMETER;
	VisitorList curr_visitor = sys->visitor_list;
	Result result = OK;

	if (curr_visitor->visitor->visitor_id == id) {
		if (curr_visitor->next_visitor == NULL) {
			reset_visitor(curr_visitor->visitor);
            free(curr_visitor->visitor);
            curr_visitor->visitor = NULL;
			destroy_visitor_node(curr_visitor);
			curr_visitor = NULL;
			return OK;
		}
		else {
			sys->visitor_list = curr_visitor->next_visitor;
            curr_visitor->next_visitor = NULL;
			result = destroy_visitor_node(curr_visitor);
			if (result != OK)
				return result;

			return OK;
		}
	}

	while (curr_visitor->next_visitor != NULL) {
		if (curr_visitor->next_visitor->visitor->visitor_id == id) {
			VisitorList to_delete = curr_visitor->next_visitor;
			curr_visitor->next_visitor =
					curr_visitor->next_visitor->next_visitor;
			destroy_visitor_node(to_delete);
			return OK;
		}
		curr_visitor = curr_visitor->next_visitor;
	}
	return OK;
}

Result visitor_arrive(ChallengeRoomSystem *sys, char *room_name,
		char *visitor_name, int visitor_id, Level level, int start_time) {

	if (sys == NULL)
		return NULL_PARAMETER;

	if (start_time < sys->last_system_time) {
		return ILLEGAL_TIME;
	}
	Result result = check_if_visitor_is_already_in_system(sys,
			visitor_id);
	if (result == ALREADY_IN_ROOM) {
		return ALREADY_IN_ROOM;
	}

	Visitor* new_visitor = (Visitor*) malloc(sizeof(Visitor));
	if (new_visitor == NULL)
		return MEMORY_PROBLEM;
	result = init_visitor(new_visitor, visitor_name, visitor_id);
	if (result != OK) {
		free(new_visitor);
		new_visitor = NULL;
		return result;
	}

	for (int i = 0; i < sys->num_rooms; i++) {
		if (strcmp(sys->all_rooms[i].name, room_name) == 0) {
			result = visitor_enter_room(&(sys->all_rooms[i]), new_visitor,
					level, start_time);
			if (result != OK) {
				reset_visitor(new_visitor);
				free(new_visitor);
				new_visitor = NULL;
				return result;
			}
			break;
		}
	}

	VisitorList new_visitor_node = visitor_node_create(new_visitor);
	if (new_visitor_node == NULL) {
		reset_visitor(new_visitor);
		free(new_visitor);
		new_visitor = NULL;
		return MEMORY_PROBLEM;
	}

	result = add_visitor_to_list(sys, new_visitor_node);
	if (result != OK) {
		reset_visitor(new_visitor);
		free(new_visitor);
		new_visitor = NULL;
		return result;
	}
	sys->last_system_time = start_time;
	return OK;
}

Result visitor_quit(ChallengeRoomSystem *sys, int visitor_id, int quit_time) {
	Result result;
	if (sys == NULL)
		return NULL_PARAMETER;

	if (quit_time < sys->last_system_time)
		return ILLEGAL_TIME;

	if (sys->visitor_list == NULL)
		return NULL_PARAMETER;

	int was_taken_out = 0;

	VisitorList curr_visitor = sys->visitor_list;
	while ((curr_visitor != NULL) && (was_taken_out == 0)) {
		if (curr_visitor->visitor->visitor_id == visitor_id) {
			result = visitor_quit_room(curr_visitor->visitor, quit_time);
			if (result != OK)
				return result;
			was_taken_out = 1;
		}
		if (was_taken_out == 0)
			curr_visitor = curr_visitor->next_visitor;
	}

	if (was_taken_out == 0)
		return NOT_IN_ROOM;

	result = remove_visitor_from_list(sys, visitor_id);
	if (result != OK)
		return result;

	sys->last_system_time = quit_time;
	return OK;

}

Result all_visitors_quit(ChallengeRoomSystem *sys, int quit_time) {
	Result result;
	if (sys == NULL)
		return NULL_PARAMETER;

	if (quit_time < sys->last_system_time)
		return ILLEGAL_TIME;

	if (sys->visitor_list == NULL)
		return NULL_PARAMETER;

	VisitorList curr_visitor = sys->visitor_list;
	while (curr_visitor->visitor != NULL) {
		result = visitor_quit(sys, curr_visitor->visitor->visitor_id,
				quit_time);
		if (result != OK)
			return result;

        curr_visitor = sys->visitor_list;

        if (curr_visitor->next_visitor != NULL)
            curr_visitor = curr_visitor->next_visitor;
        //}
	}

	sys->last_system_time = quit_time;
	return OK;
}
Result system_room_of_visitor(ChallengeRoomSystem *sys, char *visitor_name,
		char **room_name) {
//	Result result;
	if (sys == NULL)
		return NULL_PARAMETER;

	if (sys->visitor_list == NULL)
		return NULL_PARAMETER;

	VisitorList curr_visitor = sys->visitor_list;

    if (curr_visitor->visitor == NULL) {
        return NOT_IN_ROOM;
    }
	while (curr_visitor != NULL) {
		if (strcmp((curr_visitor)->visitor->visitor_name, visitor_name) == 0) {
			char *room = (char*) malloc(
					strlen(*(curr_visitor->visitor->room_name)) + 1);
			if (room == NULL)
				return MEMORY_PROBLEM;

			room = strcpy(room, *(curr_visitor->visitor->room_name));
			*room_name = room;
			return OK;
		}

		    curr_visitor = curr_visitor->next_visitor;
	}
	return NOT_IN_ROOM;
}

Result change_challenge_name(ChallengeRoomSystem *sys, int challenge_id,
		char *new_name) {
	if (sys == NULL || new_name == NULL)
		return NULL_PARAMETER;

	for (int i = 0; i < sys->num_challenges; i++) {
		if (sys->all_challenges[i].id == challenge_id) {
			Result result = change_name(&(sys->all_challenges[i]), new_name);
			if (result != OK)
				return result;
		}
	}
	return ILLEGAL_PARAMETER;
}

Result change_system_room_name(ChallengeRoomSystem *sys, char *current_name,
		char *new_name) {
	if (sys == NULL || current_name == NULL || new_name == NULL)
		return NULL_PARAMETER;

	for (int i = 0; i < sys->num_rooms; i++) {
		if (strcmp(sys->all_rooms[i].name, current_name) == 0) {
			Result result = change_room_name(&(sys->all_rooms[i]), new_name);
			if (result != OK)
				return result;
			return OK;
		}
	}
	return ILLEGAL_PARAMETER;
}

Result best_time_of_system_challenge(ChallengeRoomSystem *sys,
		char *challenge_name, int *time) {
	if (sys == NULL || challenge_name == NULL || time == NULL)
		return NULL_PARAMETER;

	for (int i = 0; i < sys->num_challenges; i++) {
		if (strcmp(sys->all_challenges[i].name, challenge_name) == 0) {
			Result result = best_time_of_challenge(&(sys->all_challenges[i]),
					time);
			if (result != OK)
				return result;
            return OK;
		}
	}
	return ILLEGAL_PARAMETER;
}

Result most_popular_challenge(ChallengeRoomSystem *sys, char **challenge_name) {
	int most_visited_index = 0;

	for (int i = 0; i < sys->num_challenges; i++) {
		if (sys->all_challenges[i].num_visits
				> sys->all_challenges[most_visited_index].num_visits)
			most_visited_index = i;
		else if (sys->all_challenges[i].num_visits
				== sys->all_challenges[most_visited_index].num_visits) {
			if (strcmp(sys->all_challenges[i].name,
					sys->all_challenges[most_visited_index].name) < 0)
				most_visited_index = i;
		}
	}

	if (sys->all_challenges[most_visited_index].num_visits == 0)
		*challenge_name = NULL;
	else {
		char *name = malloc(
				(strlen(sys->all_challenges[most_visited_index].name)) + 1);
		if (name == NULL)
			return MEMORY_PROBLEM;
		strcpy(name, sys->all_challenges[most_visited_index].name);
		*challenge_name = name;
	}
	return OK;
}
