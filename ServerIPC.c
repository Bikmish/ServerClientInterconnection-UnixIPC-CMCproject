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
	struct Message mymsg; mymsg.type = 1;
	key_t key;
	int msgid;
	char buf[MSGMAX];
	
	//открытие файла, создания ключа и очереди сообщений
	if(argc<2)
	{
		printf("Lack of arguments.\n");
		exit(1);
	}
	if ((file = fopen(argv[1], "r"))==NULL) 
	{
		printf("Cannot open file.\n");
		exit(1);
	}
	key = ftok("keyfile",'a');
	msgid = msgget(key, 0666 | IPC_CREAT);
	
	//считывание строки и отправка сообщения
	while(fgets(buf,MSGMAX,file))
	{
		strcpy(mymsg.data, buf);
		msgsnd(msgid, (struct msgbuf*)&mymsg,MSGMAX, 0);
		msgrcv(msgid, (struct msgbuf*)&mymsg, MSGMAX, mymsg.type+2, 0); //for sync
		mymsg.type = mymsg.type%2 + 1; //1->2, 2->1
	}
	
	mymsg.data[0] = 0;
	mymsg.type = 1;
	msgsnd(msgid, (struct msgbuf*)&mymsg,MSGMAX, 0);
	mymsg.type = 2;
	msgsnd(msgid, (struct msgbuf*)&mymsg,MSGMAX, 0);
	sleep(3);
	
	//удаление очереди
	msgctl(msgid, IPC_RMID, NULL);
	exit(0);
}