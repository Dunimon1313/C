/*
Name		Zachary Taylor
Assignment	FA2020 COP3502 Programming Assignment 2
Date		10/20/2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "leak_detector_c.h"

//Structures
typedef struct failfish {
	int sequence_number;
	struct failfish* next_failfish;
	struct failfish* prev_failfish;
}failfish;

typedef struct failgroup {
	int pondnumber;
	char* pondname;
	int n; 
	int e;
	int th;
	struct failfish* head_failfish;
	struct failfish* tail_failfish;
	struct failgroup* next_failgroup;
	struct failgroup* prev_failgroup;
} failgroup;

typedef struct failgroup_queue {
	struct failgroup* front_failgroup;
	struct failgroup* rear_failgroup;
}failgroup_queue;

//Function prototypes
int parse_number_of_failgroups(FILE* finptr);
void solve(FILE* finptr, FILE* foutptr, int number_failgroups);

//failfish functions
failfish* create_failfish(int sequence_number);
void populate_failfish(failgroup* failfish_group);
void insert_failfish(failfish** head_failfish, failfish** tail_failfish, failfish* new_fish);
failfish* delete_failfish(failfish** head_failfish, failfish** tail_failfish, failfish* delete_fish);

//failgroup functions
failgroup* create_failgroup(int pondnumber, char* pondname, int n, int e, int th);
void insert_failgroup(failgroup** front_failgroup, failgroup** rear_failgroup, failgroup* new_failgroup);
void delete_failgroup(failgroup** head_failgroup, failgroup** tail_failgroup, failgroup* failgroup);

//course 1 & course 2 functions
void course1(FILE* foutptr, failgroup* failfish_group);
void course2(FILE* foutptr, failgroup_queue* f_queue, int number_failgroups);
int course2_count_failfish(failgroup_queue* f_queue, int count_coursegroup);
void pop(FILE* foutptr, failgroup_queue* f_queue, int max, int count_coursegroup);
int peek(failgroup_queue* f_queue, int count_coursegroup);

//failgroup functions
failgroup_queue* create_failgroup_queue();
void print_pond_status(FILE* foutptr, failgroup_queue* f_queue, int number_failgroups);
void print_last_failfish(FILE* foutptr, failgroup_queue*f_queue, int number_failgroups);

//destructor functions collected together
void destroy(failgroup_queue* fq, int count_failgroup);
void free_failfish(failfish* ff);
void free_failgroup(failgroup* fg);
void free_failgroup_queue(failgroup_queue* fq);

int main()
{	
	atexit(report_mem_leak);
	
	//open both input & output pointers
	FILE* finptr = fopen("cop3502-as2-input.txt", "r");
	FILE* foutptr = fopen("cop3502-as2-output-taylor-zachary.txt", "w");
	
	//parse num of failgroups from the file
	int number_failgroups = parse_number_of_failgroups(finptr);
	
	//read input file
	//apply course1 & course2 logic
	solve(finptr, foutptr, number_failgroups);
	
	//close both input & output pointers
	fclose(finptr);
	fclose(foutptr);
	return 0;
}

//parse first line of input
int parse_number_of_failgroups(FILE* finptr)
{
	char line[256];
	
	int number_failgroups;
	fgets(line, sizeof(line), finptr);
	sscanf(line, "%d", &number_failgroups);
	
	return number_failgroups;
}

//read input file
//apply course1 & course2 logic
void solve(FILE* finptr, FILE* foutptr, int number_failgroups)
{
	failgroup_queue *f_queue = create_failgroup_queue();
	
	char line[256], pondname[256];
	int pondnumber, n, e, th;
	
	//read input
	for(int i = 0; i < number_failgroups; i++)
	{
		
		fgets(line, sizeof(line), finptr);
		sscanf(line, "%d %s %d %d %d", &pondnumber, pondname, &n, &e, &th);
	
		failgroup* failfish_group = create_failgroup(pondnumber, pondname, n, e, th);
		insert_failgroup(&f_queue->front_failgroup, &f_queue->rear_failgroup, failfish_group);
	}
	
	//populate failfishes
	
	failgroup* temp = f_queue->front_failgroup;
	for(int i = 0; i < number_failgroups; i++)
	{		
		populate_failfish(temp);		
		temp = temp->next_failgroup;
	}
	
	fprintf(foutptr, "Initial Pond Status\n");
	
	//print pond status before course1
	print_pond_status(foutptr, f_queue, number_failgroups);
	
	
	//course1 logic
	fprintf(foutptr, "\nFirst Course\n\n");
	
	failgroup* temp2 = f_queue->front_failgroup;
	for(int i = 0; i < number_failgroups; i++)
	{
		fprintf(foutptr, "Pond %d: %s\n", temp2->pondnumber, temp2->pondname);
		course1(foutptr, temp2);
		fprintf(foutptr, "\n");
		temp2 = temp2->next_failgroup;
	}
	
	fprintf(foutptr, "\nEnd of Course pond Status\n");
	
	//print pond status after course1
	print_pond_status(foutptr, f_queue, number_failgroups);
	
	//course2 logic
	fprintf(foutptr, "\nSecond Course\n");
	course2(foutptr, f_queue, number_failgroups);
	
	//free all memory
	destroy(f_queue, number_failgroups);
}

//print pond status before course1 & after course1
//loop all failgroups & failfishes within the failgroup
void print_pond_status(FILE* foutptr, failgroup_queue* f_queue, int number_failgroups)
{
	failgroup* temp = f_queue->front_failgroup;
	for(int i = 0; i < number_failgroups; i++)
	{
		fprintf(foutptr, "%d %s ", temp->pondnumber, temp->pondname);
		if(temp->head_failfish->sequence_number != -1)
		{
			failfish* temp2 = temp->head_failfish;
			while(temp2 != temp->tail_failfish)
			{
				fprintf(foutptr, "%d ", temp2->sequence_number);
				temp2 = temp2->next_failfish;
			}
			fprintf(foutptr, "%d \n", temp2->sequence_number);
		}
		temp = temp->next_failgroup;
	}
}

//apply course2 logic
//while there are more than 1 fish in the queue, keep peeking & popping fishes
//when 1 fish remains, call function to print it
void course2(FILE* foutptr, failgroup_queue* f_queue, int number_failgroups)
{
	while(course2_count_failfish(f_queue, number_failgroups) > 1)
	{
		int max = peek(f_queue, number_failgroups);
		pop(foutptr, f_queue, max, number_failgroups);
	}
	print_last_failfish(foutptr, f_queue, number_failgroups);
}

//print the last remaining fish in course2
void print_last_failfish(FILE* foutptr, failgroup_queue*f_queue, int number_failgroups)
{
	failgroup* temp = f_queue->front_failgroup;
	for (int i = 0; i < number_failgroups; i++)
	{
		if(temp->head_failfish->sequence_number != -1)
		{
			fprintf(foutptr, "\n\nFailfish %d from pond %d remains", temp->head_failfish->sequence_number, temp->pondnumber);
		}
		temp = temp->next_failgroup;
	}
}

//helper function to count how many fishes are in the queue
//course2 function loops as long as this function returns > 1
//counts all failfishes inside of all failgroups 
int course2_count_failfish(failgroup_queue* f_queue, int count_coursegroup)
{
	int counter = 0;
	failgroup* temp = f_queue->front_failgroup;
	
	for(int i = 0; i < count_coursegroup; i++)
	{
		if(temp->head_failfish->sequence_number != -1)
		{
			failfish* temp2 = temp->head_failfish;
			while(temp2 != temp->tail_failfish)
			{
				temp2 = temp2->next_failfish;
				counter++;
			}
			counter++;
		}
		temp = temp->next_failgroup;
	}

	return counter;
}

//pop function uses the "max" value passed in here from the peek function
//(effectively) removes the matching fish from further consideration
void pop(FILE* foutptr, failgroup_queue* f_queue, int max, int count_coursegroup)
{
	failgroup *loop_failgroup = f_queue->front_failgroup;
	
	for(int i = 0; i < count_coursegroup; i++)
	{
		if(loop_failgroup->head_failfish->sequence_number != -1)
		{
			if(loop_failgroup->head_failfish->sequence_number == max)
			{
				fprintf(foutptr, "\nEaten: Failfish %d from pond %d", loop_failgroup->head_failfish->sequence_number,loop_failgroup->pondnumber);
				if(loop_failgroup->head_failfish->next_failfish == loop_failgroup->head_failfish)
				{
					loop_failgroup->head_failfish->sequence_number = -1;
					return;
				}
				else
				{
					failfish* temp = delete_failfish(&loop_failgroup->head_failfish, &loop_failgroup->tail_failfish, loop_failgroup->head_failfish);
					return;
				}
				
			}
		}
		loop_failgroup = loop_failgroup->next_failgroup;
	}
}

//reads all of the head fishes inside of failgroups
//returns the greatest value
int peek(failgroup_queue* f_queue, int count_coursegroup)
{
	int max = 0;
	failgroup *temp = f_queue->front_failgroup;
	for(int i = 0; i < count_coursegroup; i++)
	{
		if(temp->head_failfish != NULL)
		{
			if(temp->head_failfish->sequence_number > max)
			{
				max = temp->head_failfish->sequence_number;
			}
		}
		temp = temp->next_failgroup;
	}
	return max;
}

//called at last - to delete all failfishes, failgroups, and failgroup_queue
void destroy(failgroup_queue* fq, int count_failgroup)
{
	failgroup* temp = fq->front_failgroup;
	for(int i = 0; i < count_failgroup; i++)
	{
		free_failfish(temp->head_failfish);
		temp = temp->next_failgroup;
	}
	
	
	temp = fq->front_failgroup;
	for(int i = 0; i < count_failgroup; i++)
	{
		failgroup* temp2 = temp;
		temp = temp->next_failgroup;
		free_failgroup(temp2);
	}
	
	free_failgroup_queue(fq);
}

//failfish destructor
void free_failfish(failfish* ff)
{
	free(ff);
}

//failgroup destructor
void free_failgroup(failgroup* fg)
{
	free(fg->pondname);
	free(fg);
}

//failgroup_queue destructor
void free_failgroup_queue(failgroup_queue* fq)
{
	free(fq);
}

//failgroup_queue constructor
failgroup_queue* create_failgroup_queue()
{
	failgroup_queue *f_queue = malloc(sizeof(failgroup_queue));
	f_queue->front_failgroup = NULL;
	f_queue->rear_failgroup = NULL;
	return f_queue;
}

//delete the failgroup being passed in
void delete_failgroup(failgroup** head_failgroup, failgroup** tail_failgroup, failgroup* delete_failgroup)
{
	//is the deleted group the head & is the head the only group?
	if(*head_failgroup == delete_failgroup && *head_failgroup == *tail_failgroup)
	{
		free_failgroup(*head_failgroup);
	}
	//is the deleted group the head but there are more groups
	else if(*head_failgroup == delete_failgroup)
	{
		failgroup* temp = *head_failgroup;
		(*tail_failgroup)->next_failgroup = (*head_failgroup)->next_failgroup;
		(*head_failgroup)->next_failgroup->prev_failgroup = *tail_failgroup;
		*head_failgroup = (*head_failgroup)->next_failgroup;
		free_failgroup(temp);
	}
	//is the deleted group the tail
	else if(*tail_failgroup == delete_failgroup)
	{
		failgroup* temp = *tail_failgroup;
		(*tail_failgroup)->prev_failgroup->next_failgroup = *head_failgroup;
		(*head_failgroup)->prev_failgroup = (*tail_failgroup)->prev_failgroup;
		*tail_failgroup = (*tail_failgroup)->prev_failgroup;
		free_failgroup(temp);
	}
	//the deleted group is in the middle
	else
	{
		delete_failgroup->prev_failgroup->next_failgroup = delete_failgroup->next_failgroup;
		delete_failgroup->next_failgroup->prev_failgroup = delete_failgroup->prev_failgroup;
		free_failgroup(delete_failgroup);
	}

}

//insert a new failgroup inside a failgroup queue
void insert_failgroup(failgroup** front_failgroup, failgroup** rear_failgroup, failgroup* new_failgroup)
{
	//insert as the first failgroup? && the head is empty
	if(*front_failgroup == NULL)
	{
		*front_failgroup = new_failgroup;
		*rear_failgroup = new_failgroup;
		new_failgroup->next_failgroup = new_failgroup;
		new_failgroup->prev_failgroup = new_failgroup;
	}
	//insert as the first failgroup? && the head is not empty
	else if(new_failgroup->pondnumber < (*front_failgroup)->pondnumber)
	{	
		new_failgroup->next_failgroup = *front_failgroup;
		new_failgroup->prev_failgroup = *rear_failgroup;
		(*front_failgroup)->prev_failgroup = new_failgroup;
		(*rear_failgroup)->next_failgroup = new_failgroup;
		*front_failgroup = new_failgroup;
	}
	//insert as the tail failgroup?
	else if(new_failgroup->pondnumber > (*rear_failgroup)->pondnumber)
	{
		new_failgroup->next_failgroup = *front_failgroup;
		new_failgroup->prev_failgroup = *rear_failgroup;
		(*front_failgroup)->prev_failgroup = new_failgroup;
		(*rear_failgroup)->next_failgroup = new_failgroup;
		*rear_failgroup = new_failgroup;	
	}
	//insert in the middle
	else
	{
		failgroup* temp = *front_failgroup;
		while(new_failgroup->pondnumber > temp->pondnumber)
		{
			temp = temp->next_failgroup;
		}
		new_failgroup->next_failgroup = temp;
		new_failgroup->prev_failgroup = temp->prev_failgroup;
		temp->prev_failgroup->next_failgroup = new_failgroup;
		temp->prev_failgroup = new_failgroup;	
	}	
}

//course1 logic
//loop through until threshold is reached - loop difference is the "e" value in the input file
//delete every fish that is landed upon
void course1(FILE* foutptr, failgroup* failfish_group)
{
	int counter = failfish_group->n - failfish_group->th;
	failfish* temp = failfish_group->head_failfish->prev_failfish;
	while (counter > 0)
	{
		for (int i = 0; i < (failfish_group->e); i++)
		{
			temp = temp->next_failfish;
		}
		fprintf(foutptr, "Failfish %d eaten\n", temp->sequence_number);
		temp = delete_failfish(&failfish_group->head_failfish, &failfish_group->tail_failfish, temp);
		counter--;
	}
}

//delete failfish that is passed in
failfish* delete_failfish(failfish** head_failfish, failfish** tail_failfish, failfish* delete_fish)
{	
	//delete the head node?
	if(*head_failfish == delete_fish)
	{
		failfish* temp = *head_failfish;
		(*tail_failfish)->next_failfish = (*head_failfish)->next_failfish;
		(*head_failfish)->next_failfish->prev_failfish = *tail_failfish;
		(*head_failfish) = (*head_failfish)->next_failfish;
		free_failfish(temp);
		return *tail_failfish;
	}
	//delete the tail node?
	else if(*tail_failfish == delete_fish)
	{
		failfish* temp = *tail_failfish;
		(*head_failfish)->prev_failfish = (*tail_failfish)->prev_failfish;
		(*tail_failfish)->prev_failfish->next_failfish = *head_failfish;
		(*tail_failfish) = (*tail_failfish)->prev_failfish;
		free_failfish(temp);
		return *tail_failfish;
	}
	//delete a middle node?
	else
	{
		failfish* temp = delete_fish->prev_failfish;
		delete_fish->next_failfish->prev_failfish = delete_fish->prev_failfish;
		delete_fish->prev_failfish->next_failfish = delete_fish->next_failfish;
		free_failfish(delete_fish);
		return temp;
	}	
}

//populate failfishes
//loop n times for failgroup passed in - create & insert failfish into failgroup
void populate_failfish(failgroup* failfish_group)
{
	for(int i = 1; i <= failfish_group->n; i++)
	{
		failfish* new_fish = create_failfish(i);
		insert_failfish(&failfish_group->head_failfish, &failfish_group->tail_failfish, new_fish);
	}
}

//insert failfish passed in
void insert_failfish(failfish** head_failfish, failfish** tail_failfish, failfish* new_fish)
{
	//insert as head?
	if(*head_failfish == NULL)
	{
		*head_failfish = new_fish;
		*tail_failfish = new_fish;
		new_fish->prev_failfish = new_fish;
		new_fish->next_failfish = new_fish;
	}
	//insert as tail
	else
	{
		new_fish->next_failfish = *head_failfish;
		new_fish->prev_failfish = *tail_failfish;		
		(*head_failfish)->prev_failfish = new_fish;
		(*tail_failfish)->next_failfish = new_fish;
		*tail_failfish = new_fish;
	}
}

//constructor for failfish
failfish* create_failfish(int sequence_number)
{
	failfish* failfish = malloc(sizeof(failfish));
	failfish->sequence_number = sequence_number;
	failfish->next_failfish = NULL;
	failfish->prev_failfish = NULL;
	return failfish;
}

//constructor for failgroup
failgroup* create_failgroup(int pondnumber, char* pondname, int n, int e, int th)
{
	failgroup* failfish_group = malloc(sizeof(failgroup));
	failfish_group->pondname = malloc(sizeof(1 + strlen(pondname)));
	
	failfish_group->pondnumber = pondnumber;
	strcpy(failfish_group->pondname,pondname);
	failfish_group->n = n;
	failfish_group->e = e;
	failfish_group->th = th;
	
	failfish_group->head_failfish = NULL;
	failfish_group->tail_failfish = NULL;
	
	failfish_group->next_failgroup = NULL;
	failfish_group->prev_failgroup = NULL;
	
	return failfish_group;
}
