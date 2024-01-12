/*
	This is the lex skeleton for the UCF Summer 2021 Systems Software Project
	Implement the function lexanalyzer, add as many functions and global
	variables as desired, but do not alter printerror or printtokens.
	Include your name (and your partner's name) in this comment in order to 
	earn the points for compiling
	by Zachary Taylor and Najah Russell
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"


lexeme *list;
int lex_index;

void printerror(int type);
void printtokens();
int doesMatch(char* str, char oneAhead, char oneBehind);

lexeme *lexanalyzer(char *input)
{
	list = malloc(500 * sizeof(lexeme));
	lex_index = 0;
	int i = 0;
	char* isWord = malloc(500 * sizeof(char));
	char peak;
	char oneBehind;
	int check, digitLimit, letterLimit;
	digitLimit = 0;
	letterLimit = 0;
	
	while (input[i]!='\0') {
		if(i == 0) {
			oneBehind = '\0';
		}
		else {
			oneBehind = input[i-1];
		}
		//adds one character to isWord per loop
		strncat(isWord, &input[i], 1);
		
		peak = input[i+1];
		//checks if isWord matches list of possible lexemes
		check = doesMatch(isWord, peak, oneBehind);
		if(check > 0) {//if it matches, it assigns a new lexeme struct and erases the current contents of isWord
			list[lex_index].type = check;
			if(check == 33){//if isWord is a number and gets stored
				int identifierConvert = atoi(isWord);
				list[lex_index].value = identifierConvert;
			}
			else if(check == 32){//if isWord is an identifier and gets stored
				strcpy(list[lex_index].name, isWord);
			}
			
			digitLimit = 0;
			letterLimit = 0;
			isWord[0] = '\0';
			lex_index++;
		}
		else{//if it doesn't match, it checks for errors
			if(isalpha(isWord[0])!=0) {//if isword contains an identifier
				letterLimit++;
				if(letterLimit > 11) {//identifier length check
					printerror(4);
					exit(1);
				}
			}
			else if(isdigit(isWord[0])!=0) {//if isword contains a number
				digitLimit++;
				if(digitLimit > 5) {//digit length check
					printerror(3);
					exit(1);
				}
			}
			else if(check == -69420) {//if the isWord hits EOF without the comment ending
				printerror(5);
				exit(1);
			}
			else if(check == -123456) {//if there is an invalid identifier
				printerror(2);
				exit(0);
			}
			else if(check == -666666) {//if forbidden symbol gets a hit
				printerror(6);
				exit(1);
			}
			else if(check == -42069) {//if there is whitespace or a 
				isWord[0] = '\0';
			}
		}
		
		i++;
	}
	
	

	printtokens();
	return list;
}

int doesMatch(char* str, char oneAhead, char oneBehind) {
	int i = 0;
	
	//whitespace check
	if(iscntrl(str[0])!=0) {
		return -42069;
	}
	
	//forbidden symbol check
	if(strcmp(str, "#")==0 || strcmp(str, "$")==0 || (strcmp(str, "=")==0 && (!(oneAhead == '=' || oneBehind == '=' || oneBehind == '<' || oneBehind == '>' || oneBehind == ':'))) || 
	   strcmp(str, "}")==0 || strcmp(str, "{")==0 || strcmp(str, "?")==0 &&strcmp(str, "!")==0 || 
	   strcmp(str, "'")==0 || strcmp(str, "\"")==0 || strcmp(str, "@")==0 || strcmp(str, "[")==0 || 
	   strcmp(str, "]")==0 || strcmp(str, "|")==0) {
		return -666666;
	}
	
	//<=, >=, ==, := check
	if(strcmp(str, "<")==0 || strcmp(str, ">")==0 || strcmp(str, ":")==0 || strcmp(str, "=")==0) {
		if(oneAhead == '='){
			return -1;
		}
	}
	//check for <>
	if(strcmp(str, "<")== 0 && oneAhead == '>') return -1;
	
	
	
	//comment deletion and never-ending comment check
	if(str[0]== '/' && oneAhead == '*'){//if str is beginning of comment
		return -1;
	}
	else if(str[0]=='/' &&str[1]== '*') {//while str is filled with comment, will check for ending
		int n = strlen(str);
		if(oneAhead == '\0') {//if str is a neverending comment
			return -69420;
		}
		else if(str[n-1-1] == '*' && str[n-1] == '/') {//if str is about to end
			return -42069;
		}
		else {//continually adds next char to str until comment ends or file input ends
			return -1;
		}
	}
	
	while(str[i]!='\0') {
		if(isdigit(str[0])!=0) {//digit check
			if(isdigit(oneAhead)!=0) {//returns -1 so that next number can be collected
				return -1;
			if(isalpha(oneAhead)!=0) {
				return -123456;
			}
			}else {
				break;
			}
		}
		else if(isalpha(str[0])!=0) {//letter check
			if(isalpha(oneAhead)!=0){//returns -1 so that next letter can be collected
				return -1;
			}
			else{//puts input string into all lowercase
				str[i] = tolower(str[i]);
				break;
			}
		}
		else{
			i++;
		}
	}
	
	//checks for a match between str and desired conditions
	if(strcmp(str, "odd")==0) return 1;
	else if(strcmp(str, "==")==0)	return 2;
	else if(strcmp(str, "<>")==0)	return 3;
	else if(strcmp(str, "<")==0)	return 4;
	else if(strcmp(str, "<=")==0)	return 5;
	else if(strcmp(str, ">")==0)	return 6;
	else if(strcmp(str, ">=")==0)	return 7;
	else if(strcmp(str, "%")==0)	return 8;
	else if(strcmp(str, "*")==0)	return 9;
	else if(strcmp(str, "/")==0)	return 10;
	else if(strcmp(str, "+")==0)	return 11;
	else if(strcmp(str, "-")==0)	return 12;
	else if(strcmp(str, "(")==0)	return 13;
	else if(strcmp(str, ")")==0)	return 14;
	else if(strcmp(str, ",")==0)	return 15;
	else if(strcmp(str, ".")==0)	return 16;
	else if(strcmp(str, ";")==0)	return 17;
	else if(strcmp(str, ":=")==0)	return 18;
	else if(strcmp(str, "begin")==0)	return 19;
	else if(strcmp(str, "end")==0)	return 20;
	else if(strcmp(str, "if")==0)	return 21;
	else if(strcmp(str, "then")==0)	return 22;
	else if(strcmp(str, "else")==0)	return 23;
	else if(strcmp(str, "while")==0)	return 24;
	else if(strcmp(str, "do")==0)	return 25;
	else if(strcmp(str, "call")==0)	return 26;
	else if(strcmp(str, "write")==0)	return 27;
	else if(strcmp(str, "read")==0)	return 28;
	else if(strcmp(str, "const")==0)	return 29;
	else if(strcmp(str, "var")==0)	return 30;
	else if(strcmp(str, "procedure")==0)	return 31;
	else if(isalpha(str[0])!=0)	return 32;
	else if(isdigit(str[0])!=0)	return 33;
	else{
		return -1;
	}

}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "<>", neqsym);
				break;
			case lessym:
				printf("%11s\t%d", "<", lessym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case slashsym:
				printf("%11s\t%d", "/", slashsym);
				break;
			case plussym:
				printf("%11s\t%d", "+", plussym);
				break;
			case minussym:
				printf("%11s\t%d", "-", minussym);
				break;
			case lparentsym:
				printf("%11s\t%d", "(", lparentsym);
				break;
			case rparentsym:
				printf("%11s\t%d", ")", rparentsym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case becomessym:
				printf("%11s\t%d", ":=", becomessym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else if (type == 5)
		printf("Lexical Analyzer Error: Neverending Comment\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");
	
	free(list);
	return;
}
