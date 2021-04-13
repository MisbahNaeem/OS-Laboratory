#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/

int get_non_zero_data(char *line){
	int i;
	for (i = 0;i < strlen(line); i++){
		if (line[i] == '0'){
			break;
		}
	}
	i = i-2;
	return i;
}

int check_for_amp_sign(char *line){
	int i,out=0;
	for (i = 0; i < strlen(line); i++){
		if (line[i] == '&'){
			if (line[++i] == '&'){
				out = 1;
				if (line[++i] == '&'){
					out = 2;
					break;
				}
			}
		}
	}
	return out;
}


char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  char char1[1024] = "/bin/";
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	if (tokenNo == 0){
	  strcat(char1, token);
	  strcpy(tokens[tokenNo++], char1);
	  strcpy(char1, "/bin/");
	}
	else{
	  if (strcmp(token,"&") != 0 && strcmp(token,"&&") != 0 && strcmp(token,"&&&") != 0){
	    strcpy(tokens[tokenNo++], token);
	  }
	}
	//printf("this %s ",tokens[tokenNo-1]);
	tokenIndex = 0; 
      }
    } 
    else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}


int main(int argc, char *argv[]) {
	char  line[MAX_INPUT_SIZE],temp_line[MAX_INPUT_SIZE],temp_line2[MAX_INPUT_SIZE];          
	char  **tokens;              
	int i,output,func_check=0;
	char cwd[256];
	char *token2;

	FILE* fp;
	FILE* hist;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp < 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}

	while(1) {			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if(argc == 2) { // batch mode
			if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;	
			}
			line[strlen(line) - 1] = '\0';
		} else { // interactive mode
			if (getcwd(cwd, sizeof(cwd)) != NULL){
				printf("%s",cwd);
			}
			printf(" $ ");
			scanf("%[^\n]", line);
			getchar();
		}

		/* END: TAKING INPUT */

		func_check = check_for_amp_sign(line);


		if (func_check == 0){
			if (strcmp(line,"exit") == 0){
				kill(0,0);
				exit(0);
			}
			
			if (strcmp(line,"!!") == 0){
				hist = fopen(".history.txt","r");
				if (hist == NULL){
					printf("No commands in history\n");
					continue;
				}
				fgets(line,MAX_INPUT_SIZE,hist);
				fclose(hist);
				printf("%s\n",line);
			}
			else{
				hist = fopen(".history.txt","w");
				fprintf(hist,"%s",line);
				fclose(hist);
			}

			strcpy(temp_line,line);
			line[strlen(line)] = '\n'; //terminate with new line
			tokens = tokenize(line);

			if (line[get_non_zero_data(line)] == '&'){
				//printf("& check\n");
				if (fork() == 0){
					output = execv(tokens[0],tokens);
					if (output == -1){
						printf("%s is not a recognized command\n",temp_line);			
					}
					else{
						printf("\n");
					}
					exit(0);
				}
				continue;
			}
	   
	       		//do whatever you want with the commands, here we just print them

			for(i=0;tokens[i]!=NULL;i++){
				//printf("found token %s (remove this debug output later)\n", tokens[i]);
			}

			if (strcmp(tokens[0],"/bin/cd") == 0){
				chdir(tokens[1]);
			}
			else{
				if (fork() == 0){
					output = execv(tokens[0],tokens);
					if (output == -1){
						printf("%s is not a recognized command\n",temp_line);			
					}
					else{
						printf("\n");
					}
					exit(0);
				}
				else{
					wait(NULL);
		       		}
			}
		}


		else if (func_check == 1){
			token2 = strtok(line,"&&");
			while (token2 != NULL){
				if (strcmp(token2,"exit") == 0){
					kill(0,0);
					exit(0);
				}
				
				if (strcmp(token2,"!!") == 0){
					hist = fopen(".history.txt","r");
					if (hist == NULL){
						printf("No commands in history\n");
						continue;
					}
					fgets(token2,MAX_INPUT_SIZE,hist);
					fclose(hist);
					printf("%s\n",token2);
				}
				else{
					hist = fopen(".history.txt","w");
					fprintf(hist,"%s",token2);
					fclose(hist);
				}

				strcpy(temp_line,token2);
				token2[strlen(token2)] = '\n'; //terminate with new token2
				tokens = tokenize(token2);
		   
		       		//do whatever you want with the commands, here we just print them

				for(i=0;tokens[i]!=NULL;i++){
					//printf("found token %s (remove this debug output later)\n", tokens[i]);
				}

				if (strcmp(tokens[0],"/bin/cd") == 0){
					chdir(tokens[1]);
				}
				else{
					if (fork() == 0){
						output = execv(tokens[0],tokens);
						if (output == -1){
							printf("%s is not a recognized command\n",temp_line);			
						}
						else{
							printf("\n");
						}
						exit(0);
					}
					else{
						wait(NULL);
			       		}
				}
				token2 = strtok(NULL,"&&");
				if (strcmp(token2,"\n") == 0){
					break;
				}
			}
			continue;
		}


		else {
			token2 = strtok(line,"&&&");
			while (token2 != NULL){
				printf("%s\n",token2);
				if (strcmp(token2,"exit") == 0){
					kill(0,0);
					exit(0);
				}
				
				if (strcmp(token2,"!!") == 0){
					hist = fopen(".history.txt","r");
					if (hist == NULL){
						printf("No commands in history\n");
						continue;
					}
					fgets(token2,MAX_INPUT_SIZE,hist);
					fclose(hist);
					printf("%s\n",token2);
				}
				else{
					hist = fopen(".history.txt","w");
					fprintf(hist,"%s",token2);
					fclose(hist);
				}

				strcpy(temp_line,token2);
				token2[strlen(token2)] = '\n'; //terminate with new token2
				tokens = tokenize(token2);
		   
		       		//do whatever you want with the commands, here we just print them

				for(i=0;tokens[i]!=NULL;i++){
					//printf("found token %s (remove this debug output later)\n", tokens[i]);
				}

				if (strcmp(tokens[0],"/bin/cd") == 0){
					chdir(tokens[1]);
				}
				else{
					if (fork() == 0){
						output = execv(tokens[0],tokens);
						if (output == -1){
							printf("%s is not a recognized command\n",temp_line);			
						}
						else{
							printf("\n");
						}
						exit(0);
					}
				}
				token2 = strtok(NULL,"&&&");
				if (strcmp(token2,"\n") == 0){
					break;
				}
			}
			continue;
		}
		
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
