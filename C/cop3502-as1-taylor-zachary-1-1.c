/*
Name		Zachary Taylor
Assignment	FA2020 COP3502 Programming Assignment 1
Date		09/29/2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "leak_detector_c.h"

//Structures provided in the assignment
typedef struct monster {
	 int id; 
	 char *name; 
	 char *element; 
	 int population; 
} monster; 
 
typedef struct region { 
	 char *name; 
	 int nmonsters; 
	 int total_population; 
	 monster **monsters; 
} region; 
 
typedef struct itinerary { 
	 int nregions; 
	 region **regions; 
	 int captures; 
} itinerary; 
 
typedef struct trainer { 
	 char *name; 
	 itinerary *visits; 
} trainer; 

//Counters
int countmonsters(FILE* finptr);
int countregions(FILE* finptr);
int counttrainers(FILE* finptr);

//Readers - related to data input
void readmonsters(FILE* finptr, monster** mon, int nummonsters);
void readregions(FILE* finptr, monster** mon, int nummonsters, region** reg, int numregions);
void readtrainers(FILE* finptr, region** reg, int numregions, trainer** tra, int numtrainers);

//Filter and search functions
void skip_blank_lines(FILE* fptr);
monster* findmonsterbyname(monster** mon, char* name, int nummonsters);
region* findregionbyname(region** reg, char* name, int numregions);

//Memory Allocation
monster* new_monster(int id, char* name, char* element, int population);
region* new_region(char* name, int nmonsters, int total_population, monster** monster);
trainer* new_trainer(char* name, itinerary* visits);
itinerary* new_itinerary(int nregions, int captures, region** regions);

//Free's
void free_monster(monster** mon, int nummonsters);
void free_region(region** reg, int numregions);
void free_trainer(trainer** tra, int numtrainers);
void free_itinerary(itinerary* iti);

//Output functions
double calculate_captures(double population, double total_population, double captures);
void output(FILE* foutptr, int nummonsters, monster** mon, region** reg, int numtrainer, trainer** tra);

int main()
{	
	atexit(report_mem_leak);

	//intializes monsters, regions, and trainers - then 
	FILE* finptr = fopen("as1-sample-input.txt", "r");

		
	int nummonsters = countmonsters(finptr);
	monster** mon = (monster**) malloc(sizeof(monster*) * nummonsters);	
	readmonsters(finptr, mon, nummonsters);
		
	int numregions = countregions(finptr);
	region** reg = (region**) malloc(sizeof(region*) * numregions);
	readregions(finptr, mon, nummonsters, reg, numregions);
	
	int numtrainers = counttrainers(finptr);
	trainer** tra = (trainer**) malloc(sizeof(trainer*) * numtrainers);
	readtrainers(finptr, reg, numregions, tra, numtrainers);



	fclose(finptr);

	
	//creates output file and saves data to it
	FILE* foutptr = fopen("cop3502-as1-output-taylor-zachary.txt", "w");
	output(foutptr, nummonsters, mon, reg, numtrainers, tra);
	
	fclose(foutptr);
	
	//frees memory
	free_monster(mon, nummonsters);
	free_region(reg, numregions);
	free_trainer(tra, numtrainers);
	
	return 0;

}
/*
void skip_blank_lines(FILE *fptr)
{
	char line[256];
	long int pos = ftell(fptr);
  printf("%ld\n", pos);

	while(fgets(line, sizeof(line), fptr) != NULL)
	{	
//I've tried this If clause with 2 different behaviours of this statement on 2 different machines, if there is any error related to broken input, it can be fixed by switching this (line[0]=='\n') with the current comparator for the if statement.
		if(strcmp(line,"\n") == 0||strcmp(line,"\0")==0)// As I have no access to Eusits at this time, I am unable to test this in the final environment and make sure the correct statement is in place.
		{
			pos = ftell(fptr);
		}
		else break;
	}
  printf("%ld\n", pos);
	fseek(fptr, pos, SEEK_SET);
  printf("%ld\n", ftell(fptr));
}*/

//Monster constructor - creates & returns a monster*
monster* new_monster(int id, char* name, char* element, int population)
{
	monster* mon = malloc(sizeof(monster));
	mon->id = id;
	
	mon->name = malloc (1 + strlen (name));
	mon->element = malloc (1 + strlen(element));
	
    if (mon->name) strcpy (mon->name, name);
    else  fprintf (stderr, "malloc failure on monster name");
	
	if (mon->element) strcpy (mon->element, element);
	else fprintf (stderr, "malloc failure on monster element");
	
	mon->population = population;
	return mon;
}

//Monster destructor - frees every malloc'd memory
void free_monster(monster** mon, int nummonsters)
{
	for (int i = 0; i < nummonsters; i++)
	{
		free(mon[i]->name);
		free(mon[i]->element);
		free(mon[i]);
	}
	free(mon);
}

//Region constructor - creates & returns a region*
region* new_region(char* name, int nmonsters, int total_population, monster** monster)
{

	region* reg = malloc(sizeof(region));
	reg->name = malloc(1 + strlen(name));
	
	strcpy(reg->name, name);
	reg->nmonsters = nmonsters;
	reg->total_population=total_population;
	reg->monsters = monster;
		
	return reg;
}

//Region destructor - frees every malloc'd memory
void free_region(region** reg, int numregions)
{
	for(int i = 0; i < numregions; i++)
	{
		free(reg[i]->name);
		free(reg[i]->monsters);
		free(reg[i]);
	}
	free(reg);
}

//Trainer constructor - creates & returns a trainer*
trainer* new_trainer(char* name, itinerary* visits)
{
	trainer* tra = malloc(sizeof(trainer));
	
	tra->name = malloc(1 + strlen(name));
	
	strcpy(tra->name, name);
	tra->visits = visits;
	
	return tra;
}

//Trainer destructor - frees every malloc'd memory
void free_trainer(trainer** tra, int numtrainers)
{
	for(int i = 0; i < numtrainers; i++)
	{
		free(tra[i]->name);
		free_itinerary(tra[i]->visits);
		free(tra[i]);
	}
	free(tra);
}

//Itinerary constructor - creates & returns a itinerary*
itinerary* new_itinerary(int nregions, int captures, region** regions)
{
	itinerary* iti = malloc(sizeof(itinerary));
	
	iti->nregions = nregions;
	iti->captures = captures;
	iti->regions = regions;
		
	return iti;
}

//Itinerary destructor - frees every malloc'd memory
void free_itinerary(itinerary* iti)
{
	free(iti->regions);
	free(iti);
}

//Parse & return the number of monsters in the input file - Exit if 0 monsters
int countmonsters(FILE* finptr)
{
	char line[256];
	int nummonsters;
	
	//skip_blank_lines(finptr);
	fgets(line, sizeof(line), finptr);
	//Parse
	sscanf(line, "%d", &nummonsters);
	
	//Exit if there are 0 monsters
	if (nummonsters == 0)
	{
		printf("There are 0 monsters provided. Exiting...\n");
		exit(0);
	}
	return nummonsters;
}

//Parse & return the number of trainers in the input file - Exit if 0 trainers
int counttrainers(FILE* finptr)
{
	char line[256];
	int numtrainers;
	
  fgets(line, sizeof(line), finptr);
	//Parse
	sscanf(line, "%d", &numtrainers);
	if (numtrainers == 0)
	{
		printf("There are 0 trainers provided. Exiting...\n");
		exit(0);
	}
	
	//skip_blank_lines(finptr);
  fgets(line, sizeof(line), finptr);
	
	return numtrainers;
}

//Parse & return the number of regions in the input file - Exit if 0 regions
int countregions(FILE *finptr)
{
	char line[256];
	int numregions;
	
	//skip_blank_lines(finptr);
	fgets(line, sizeof(line), finptr);
	//Parse
	sscanf(line, "%d", &numregions);
	
	if (numregions == 0)
	{
		printf("There are 0 regions provided. Exiting...\n");
		exit(0);
	}
  //skip_blank_lines(finptr);
	fgets(line, sizeof(line), finptr);
	return numregions;
}

//Returns a pointer to the monster element whose name matches monster name read from the file
monster* findmonsterbyname(monster** mon, char* name, int nummonsters)
{
	for (int i = 0; i < nummonsters; i++)
	{
		if(strcmp(mon[i]->name,name) == 0) return mon[i];
	}
	return NULL;
}

//Returns a pointer to the region element whose name matches region name read from the file
region* findregionbyname(region** reg, char* name, int numregions)
{
	for(int i = 0; i < numregions; i++)
	{
		if(strcmp(reg[i]->name, name) == 0)	return reg[i];
	}
	return NULL;
}


//Iterates the input file using nummonsters - calls the constructor to create a new monster - places the monster into mon**
void readmonsters(FILE* finptr, monster** mon, int nummonsters)
{
	char line[256], name[256], element[256];
	int population;
	
	//Loop nummonsters count
	for(int i = 0; i < nummonsters; i++)
	{
		fgets(line, sizeof(line), finptr);
		sscanf(line, "%s %s %d", name, element, &population);
		//Call constructor - place the monster into mon**
		mon[i]=new_monster(i, name, element, population);
	}
  fgets(line, sizeof(line), finptr);
  //skip_blank_lines(finptr);
}

//Iterates the input file using numregions - calls the constructor to create a new region - places the region into reg**
void readregions(FILE*finptr, monster** mon, int nummonsters, region** reg, int numregions)
{
	char line[256], region_name[256], monster_name[256];
	int nmonsters, total_population;

	//Loop numregions count
	for(int i = 0; i < numregions; i++)
	{
		//skip_blank_lines(finptr);
		fgets(line, sizeof(line), finptr);
		sscanf(line, "%s", region_name);
		//skip_blank_lines(finptr);
		fgets(line, sizeof(line), finptr);
		sscanf(line, "%d", &nmonsters);

		monster** monsters = (monster**) malloc(sizeof(monster*) * nmonsters);
		total_population = 0;

		//For each region, loop nmonsters (varies by region) count
		for(int j = 0; j < nmonsters; j++)
		{
			//skip_blank_lines(finptr);
			fgets(line, sizeof(line), finptr);
			sscanf(line, "%s", monster_name);

			//find the pointer to the monster read - place in monsters**
			monsters[j] = findmonsterbyname(mon, monster_name, nummonsters);

			total_population += monsters[j]->population;

		}

		//Call constructor - place the region into reg**
		reg[i]=new_region(region_name, nmonsters, total_population, monsters);
		//skip_blank_lines(finptr);
    fgets(line, sizeof(line), finptr);
	}
}

//Iterates the input file using numtrainers - calls the constructor to create a new trainer - places the trainer into tra**
void readtrainers(FILE *finptr, region** reg, int numregions, trainer** tra, int numtrainers)
{
	char line[256], trainer_name[256], region_name[256];
	int ncaptures, nregions;
		
	//Loop numtrainers count
	for(int i = 0; i < numtrainers; i++)
	{
		//skip_blank_lines(finptr);
		fgets(line, sizeof(line), finptr);
		sscanf(line, "%s", trainer_name);
		
		//skip_blank_lines(finptr);
		fgets(line, sizeof(line), finptr);
		sscanf(line, "%d", &ncaptures);
		
		//skip_blank_lines(finptr);
		fgets(line, sizeof(line), finptr);
		sscanf(line, "%d", &nregions);
		
		region** region = (struct region**) malloc(sizeof(region) * nregions);

		
		//For each trainer, loop nregions (varies by trainer) count
		for(int j = 0; j < nregions; j++)
		{
			//skip_blank_lines(finptr);
			fgets(line, sizeof(line), finptr);
			sscanf(line, "%s", region_name);
			
			//find the pointer to the region read - place in regions**
			region[j] = findregionbyname(reg, region_name, numregions);
	
		}
    //skip_blank_lines(finptr);
    fgets(line, sizeof(line), finptr);
		//Call constructor - store the itinerary
		itinerary* iti = new_itinerary(nregions, ncaptures, region);

		//Call constructor - place the trainer into tra**
		tra[i]= new_trainer(trainer_name, iti);

	}

}

//Formula for calculating the captures of a trainer 
double calculate_captures(double population, double total_population, double captures)
{
		return round(population /  total_population * captures);
}

//Loop each trainer - loop each region for each trainer - loop each monster for each region for each trainer - calculate captures! - output captures!!
void output(FILE* foutptr, int nummonsters, monster** mon, region** reg, int numtrainer, trainer** tra)
{
	//Loop each trainer - output trainer name
	for(int i = 0; i < numtrainer; i++)
	{
		fprintf(foutptr, "%s\n", tra[i]->name);
		//Loop each region for each trainer - output region name
		for(int j = 0; j < tra[i]->visits->nregions; j++)
		{
			fprintf(foutptr, "%s\n", tra[i]->visits->regions[j]->name);
			//Loop each monster for each region for each trainer - output calculations
			for(int k = 0; k < tra[i]->visits->regions[j]->nmonsters; k++)
			{
				//Calculate captures
				double captures = calculate_captures (tra[i]->visits->regions[j]->monsters[k]->population, tra[i]->visits->regions[j]->total_population, tra[i]->visits->captures);
				//Output if captures > 0
				if(captures > 0) fprintf(foutptr, "%0.lf %s\n", captures, tra[i]->visits->regions[j]->monsters[k]->name);
				
			}
		}
		fprintf(foutptr, "\n");
	}
}

