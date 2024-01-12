// Zachary Taylor - Viginere Cipher


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>



int main(int argc, char** argv){
	char* KFN = argv[1];
	char* PFN = argv[2];

	FILE* KF = fopen(KFN, "r");
	FILE* PF = fopen(PFN, "r");
	
	char q, d, key[513], plaintext[513], ciphertext[513];
	int a, i, K[513], PT[513], C[513];


	


//collect input from files and filter punctuation
	i = 0;

	q=fgetc(KF);

	while(i<512){
		if(q==EOF)
			break;
		if(isalpha(q)){
			key[i]=tolower(q);
			i++;
			}
		q=fgetc(KF);
	}
	key[i]='\0';

	i=0;
		q=fgetc(PF);

	while(i<512){
		if(q==EOF)
			break;
		if(isalpha(q)){
			plaintext[i]=tolower(q);
			i++;
			}
		q=fgetc(PF);
	}
	while(i<512){
		plaintext[i]='x';
		i++;
	}
	plaintext[i]='\0';
			
//convert EVERYTHING
for(i=0;i<512;i++){
	ciphertext[i]=(plaintext[i]+(key[i%strlen(key)])-'a'-'a')%26+'a';
}

//print Ciphertext
	printf("\nVigenere Key:\n");
	for(i=0;i<strlen(key);i++){
		if(i%80==0)
			printf("\n");
		printf("%c",key[i]);
	}
		printf("\n");
	printf("\n\nPlaintext:\n\n");
	for(i=0;i<strlen(plaintext);i++){
		if(i%80==0)
			printf("\n");
		printf("%c",plaintext[i]);
	}
		printf("\n");
	printf("\nCiphertext:\n");
		printf("\n");
	for(i=0;i<strlen(ciphertext);i++){
		if(i%80==0)
			printf("\n");
		printf("%c",ciphertext[i]);
	}
		printf("\n");
	
	
	
	
	
	return 0;
}
