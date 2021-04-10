#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <error.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

int pipes[2];
extern int errno;

void parent_proc(int i, FILE* fp){
	char buf [32];
	int s;
	int sent =0;
	close(pipes[0]);
	char* ptr;
	while(1){
		ptr = fgets(buf,32, fp);
		if(ptr == NULL) break;
		sent = write(pipes[1], buf, strlen(buf));
		if(sent==-1) printf("test%d error\n",i+1);
	}
	close(pipes[1]);

}

void child_proc(char filename[100],FILE* write){

	char buf[32];
	ssize_t s;
	close(pipes[1]);
	fputs("\n",write);
	filename[strlen(filename)] = '\n';
	fputs(filename,write);
	
	while(s=read(pipes[0],buf,31)>0){
		// printf("%s\n",buf);
		fputs(buf, write);

	}
	exit(0);
}
void max_min_sum_exetime(double time[]){
	FILE* stat = fopen("summary.txt","a");
	double max = time[0];
	double min = time[0];
	double sum = 0;
	for(int i=0; i<10; i++){
		if(time[i]>max) max = time[i];
		if(time[i]<min) min = time[i];
		sum += time[i];
	}
	fprintf(stat, "\nmaximum execution time: %f ms\n",max);
	fprintf(stat, "minimum execution time: %f ms\n",min);
	fprintf(stat, "sum of all execution time: %f ms\n\n",sum);

}
int main(){
	pid_t child_pid;
	int exit_code;
	int errcode;
	struct timespec begin, end;
	time_t startTime, now;
	float current;
	double result[10];
	FILE* fp;
	FILE* write;
	char filename[100];
	FILE* stat;
	int correct_exe =0;
	int fail_exe =0;
	remove("target_result.txt");
	if((write = fopen("target_result.txt", "a")) == NULL){
		printf("No such write file");
		exit(1);
	}
	stat = fopen("summary.txt","a");
	fputs("target.c compile successed.\n",stat);
	fclose(stat);
	
	for(int i=0; i<10; i++){
		sprintf(filename,"Tests/test%d.txt",i+1);
		if((fp = fopen(filename,"r"))==NULL){
			errcode = errno;
			printf("Fail to open file: %s\n",strerror(errcode));
		}
		
		// printf("-------------%s-------------\n",filename);

		if(pipe(pipes) != 0){
			perror("Error");
			exit(1);
		}
		clock_gettime(CLOCK_MONOTONIC, &begin);
		time(&startTime);
		if(child_pid = fork()){
			parent_proc(i,fp);
		}	
		else{
			child_proc(filename, write);
		}
		clock_gettime(CLOCK_MONOTONIC, &end);
		now = time(NULL);
		current = difftime(now,startTime);
		if(current > 1){
			printf("%s file time over!", filename);
			fail_exe++;
			i++;
		}
		wait(&exit_code);
		result[i] = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec);
		result[i] = result[i]/1000000;
		// printf("Time : %f ms\n",result[i]);
		correct_exe++;
	}
	stat = fopen("summary.txt","a");
	fprintf(stat, "\nThe number of correct execution: %d\n", correct_exe);
	fprintf(stat, "The number of fail execution: %d\n", fail_exe);
	fclose(stat);
	max_min_sum_exetime(result);
	exit(0);
	return 0;
}


