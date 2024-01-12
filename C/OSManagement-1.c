//written by Zachary Taylor

//Library inclusions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>


//Constant Variable Declarations
#define  Exit 0
#define  Invalid -1
#define  Memory 1
#define  Files 2
#define  Thread 3
#define  First 0
#define  Best 1
#define  Worst 2
#define  Next 3


//Functions

int displayMenu(){
	int choice = -1;

	//user selection loop - prints interface, collects input, and checks input validity
	while(choice == -1)
	{
		//interface output
		printf("********* Operating System Management Menu *********\n\n");
		printf("Select the OS program to run, enter the number of your selections.\n");
		printf("1. Memory Management\n");
		printf("2. File Management\n");
		printf("3. Multithreading\n");
		printf("0. Exit\n");
		
		//input collection
		scanf("%d", &choice);

		//logic check for user input
		if((choice > 3)||(choice < 0)){
			choice = -1;
		}
	}

	//returns user selection
	return choice;
}

void clearScreen(){//cant get scanf or fgets to take enter as comparable input
	system("clear");
}

void displayProcess(int allocation[], int numP, int processes[]){
	int i = 0;
	char str[] = "Not Allocated";
  //heading print
	printf("Process No.		Process Size		Block no.\n");

	while(i < numP){//loops output based on if it was allocated or not
		if(allocation[i] == -1){
		printf("%11d %16d %16s\n", (i+1), processes[i], str);
		}
		else{
		printf("%11d %16d %16d\n", (i+1), processes[i], (allocation[i]+1));
		}
		i++;
	}
}

void firstFit(int numB, int numP, int blocks[], int processes[]){
	int allocation[numP];
	int i, j;

  //sets all of allocation arry = -1
	memset(allocation, -1, sizeof(allocation));
  //allocation logic
  for(i = 0; i < numP; i++){
    for(j = 0; j < numB; j++){
      if(blocks[j] >= processes[i]){
        allocation[i] = j;
        blocks[j] = blocks[j] - processes[i];
        break;
      }
    }
  }//begin printing stored output
	printf(" ********** First Fit **********\n\n");
	displayProcess(allocation, numP, processes);
}

void worstFit(int numB, int numP, int blocks[], int processes[]){
	int allocation[numP];
	int maxIndex = -1;

	memset(allocation, -1, sizeof(allocation));
  for(int i = 0; i < numP; i++){
    maxIndex = 0;
    for(int j = 0; j < numB; j++){
      if(blocks[j] >= processes[i]){
        if(blocks[maxIndex] <= blocks[j]){
          maxIndex = j;
        }
      }
    }
    if(maxIndex > -1){
      allocation[i] = maxIndex;
      blocks[maxIndex] = blocks[maxIndex] - processes[i];
      maxIndex = -1;
    }
  }//begin printing stored output
	printf(" ********** Worst Fit **********\n\n");
	displayProcess(allocation, numP, processes);
}

void bestFit(int numB, int numP, int blocks[], int processes[]){
	int allocation[numP];
	int maxIndex;
  //sets all of allocation arry = -1
	memset(allocation, -1, sizeof(allocation));
  //allocation logic
	for(int i = 0; i < numP; i++){
		for (int j = 0; j < numB; j++){
			if(blocks[j] >= processes[i]){
				if(blocks[maxIndex] >= blocks[j]){
          maxIndex = j;
        }
			}
		}
    if(maxIndex > -1){
      allocation[i] = maxIndex;
      blocks[maxIndex] = blocks[maxIndex] - processes[i];
      maxIndex = -1;
    }
	}//begin printing stored output
	printf(" ********** Best Fit **********\n\n");
	displayProcess(allocation, numP, processes);
}

void nextFit(int numB, int numP, int blocks[], int processes[]){
	int allocation[numP];
	int i, j;

	memset(allocation, -1, sizeof(allocation));
	for(i = 0; i < numP; i++){
    for(j = 0; j < numB; j++){
      if(blocks[j] >= processes[i]){
        allocation[i] = j;
        blocks[j] = blocks[j] - processes[i];
        break;
      }
    }
  }
	printf(" ********** Next Fit **********\n\n");
	displayProcess(allocation, numP, processes);
}

void memoryManagement(){
	int i = 0;
	clearScreen();
	//run through each fit method
	for(i = 0; i < 4; i++){
		int numBlocks = 5;
		int numProcesses = 5;
		int blockSize[] = {50, 20, 30, 45, 60};
		int processSize[] = {55, 40, 20, 25, 30};

		//call 1st -> best -> worst -> next storage methods
		switch(i){
			case First:
				firstFit(numBlocks, numProcesses, blockSize, processSize);
				printf("\n");
				break;
			case Best:
				bestFit(numBlocks, numProcesses, blockSize, processSize);
				printf("\n");
				break;
			case Worst:
				worstFit(numBlocks, numProcesses, blockSize, processSize);
				printf("\n");
				break;
			case Next:
				nextFit(numBlocks, numProcesses, blockSize, processSize);
				break;
		}		
	}
}


void printAttributes(char name[], struct stat statBuff){
  char timeStr[100];
  time_t t;

  //output file information
  printf("-------------------- File = %s --------------------\n", name);
  printf("Device id = %lu\n", statBuff.st_dev);
  printf("File serial number = %lu\n", statBuff.st_ino);
  printf("File user id = %u\n", statBuff.st_uid);
  printf("File group id = %u\n", statBuff.st_gid);
  printf("File mode = %d\n", statBuff.st_mode);
  //evaluate permissions for different groups
  for(int i = 0; i < 3; i++){
    if(i == 0){
      printf("Owner permissions: ");
      if(statBuff.st_mode & S_IRUSR){
        printf("read ");
      }
      if(statBuff.st_mode & S_IWUSR){
        printf("write ");
      }
      if(statBuff.st_mode & S_IXUSR){
        printf("execute ");
      }
      printf("\n");
    }
    if(i == 2){
      printf("Group permissions: ");
      if(statBuff.st_mode & S_IRGRP){
        printf("read ");
      }
      if(statBuff.st_mode & S_IWGRP){
        printf("write ");
      }
      if(statBuff.st_mode & S_IXGRP){
        printf("execute ");
      }
      printf("\n");
    }
    if(i == 3){
      printf("Others permissions: ");
      if(statBuff.st_mode & S_IROTH){
        printf("read ");
      }
      if(statBuff.st_mode & S_IWOTH){
        printf("write ");
      }
      if(statBuff.st_mode & S_IXOTH){
        printf("execute ");
      }
      printf("\n");
    }
  }

  t = statBuff.st_ctime;
  strcpy(timeStr, ctime(&t));
  printf("Date created = %s\n", timeStr);
  t = statBuff.st_mtime;
  strcpy(timeStr, ctime(&t));
  printf("Date modified = %s\n", timeStr);
  t = statBuff.st_atime;
  strcpy(timeStr, ctime(&t));
  printf("Time file was last accessed = %s\n", timeStr);
  printf("File Size = %lu bytes\n", statBuff.st_size);
}


void fileAttributes(){
  struct stat statBuff; 
  struct dirent *de;
  DIR *dr = opendir(".");
  int err;
  char c;

  if(dr == NULL){//checks id opendir returned working results
    printf("Could not open current directory");
  }
  else{
    while((de = readdir(dr)) != NULL){
      err = stat(de->d_name, &statBuff);
      if(err == -1){
        printf("Error in stat\nPress 'Enter' to continue...");
        c = getchar();
      }
      printAttributes(de->d_name, statBuff);
    }
  }
}


//Main

int main()
{
	//local initializations
	int choice = Invalid;

	while(choice == Invalid){
		//call displayMenu for input collection
		choice = displayMenu();
		switch(choice){//decide what to do based off collected input -- displayMenu catches the case for if selection does not match available options
			case Exit://this is the exit call choice
				exit(0);
				break;
			case Memory://selects memory management
				memoryManagement();
				break;
			case Files://selects file management
				fileAttributes();
				printf("Error! Reference source not found.");
				break;
			case Thread://selects multithreading
				//do nothing
				printf("Error! Reference source not found.");
				break;
		}
	}

	//exit program
	exit(0);
}