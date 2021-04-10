#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <error.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

int pipes[2];

void parent_pro(int i,FILE *fp){
	close(pipes[0]);
	ssize_t s;
	char buf[1024];
	char *ptr;
	while(1){
		ptr = fgets(buf,1024, fp);
		if(ptr == NULL) break;
		write(pipes[1], buf, strlen(buf));
	}
	close(pipes[1]);


}

void child_pro(char filename[100], FILE* write){
	close(pipes[1]);
	char buf[1024];
	ssize_t s;
	fputs("\n",write);
	filename[strlen(filename)] = '\n';
	fputs(filename,write);
	
	while(s = read(pipes[0], buf, 31)>0){
		//printf("%s\n",buf);
		fputs(buf, write);
	}
	exit(0);
}

int main(){
	pid_t pid;
	int exit_code;
	FILE *fp;
	FILE *write;
	char filename[100];
	FILE *stat = fopen("summary.txt","a");
	fprintf(stat, "\n---------- SUMMARY ----------\n");
	fprintf(stat, "solution.c compile successed.\n");
	fclose(stat);
	remove("solution_result.txt");
	for(int i=0; i<10; i++){
		sprintf(filename,"Tests/test%d.txt",i+1);
		if((fp = fopen(filename,"r"))==NULL){
			printf("No such file\n");
			exit(1);
		}
		if((write = fopen("solution_result.txt", "a")) == NULL){
			printf("No such write file");
			exit(1);
		}
		//printf("-------------%s-------------\n",filename);

		if(pipe(pipes)!=0){
			perror("Error");
			exit(1);
		}
		pid = fork();
		if (pid>0){
			parent_pro(i, fp);
		}
		else if(pid ==0){
			child_pro(filename, write);
		}
		else{
			printf("fork failed!\n");
			exit(1);
		}
		fclose(fp);
		wait(&exit_code);
	}
	exit(0);
	return 0;
}
