/* TO BE COMPLETED HERE THE FIELDS OF THE DATA TYPE ChallengeRoomSystem */
#ifndef CHALLENGE_SYSTEM_FIELDS_H_
#define CHALLENGE_SYSTEM_FIELDS_H_


	char *name;
	int num_challenges;
	int num_rooms;
	int last_system_time;
	Challenge *all_challenges;
	ChallengeRoom *all_rooms;
	VisitorList visitor_list;


#endif // CHALLENGE_SYSTEM_FIELDS_H_
