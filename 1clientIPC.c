#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MSGMAX 256
struct Message
{
	long type; 		
	char data[MSGMAX];
};

int main(int argc, char** argv)
{
	FILE* file;
	struct Message mymsg;
	key_t key;
	int msgid;
	char buf[MSGMAX];
	pid_t pid = getpid();
	
	//создания ключа и очереди сообщений
	key = ftok("keyfile",'a');
	msgid = msgget(key, 0666 | IPC_CREAT);
	
	//считывание строки и отпрака сообщения
	msgrcv(msgid, (struct msgbuf*)&mymsg, MSGMAX, 2, 0);
	while(mymsg.data[0]!=0)
	{
		if ((file = fopen("ansfile", "a+"))==NULL) 
		{
			printf("Cannot open file.\n");
			exit(1);
		}
		fprintf(file,"%d ",pid);
		fflush(file);
		fprintf(file,"%s",mymsg.data);
		fflush(file);
		mymsg.type = 4;
		msgsnd(msgid, (struct msgbuf*)&mymsg,MSGMAX, 0);
		fclose(file);
		msgrcv(msgid, (struct msgbuf*)&mymsg, MSGMAX, 2, 0);
	}
	exit(0);
}