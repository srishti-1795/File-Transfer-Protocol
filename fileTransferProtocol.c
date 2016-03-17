#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <time.h>
#include <openssl/md5.h>
#include <dirent.h>
#include <regex.h>

int errno;

void reuse(int socket){
	int optval = 1;
	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}

char* toString(long int n){
	char *num = (char*) malloc(sizeof(char) * 200);
	long int i, j;
	for(i = 0; n > 0; i++){
		num[i] = (n % 10) + '0';
		n /= 10;
	}
	num[i] = '\0';
	i--;
	for(j = 0; i > j; i--, j++){
		char tmp = num[j];
		num[j] = num[i];
		num[i] = tmp;
	}
	return num;
}

const char* getMD5(char* filepath){
	MD5_CTX mdContext; 
	int bytes; 
	FILE *inFile = fopen (filepath, "rb"); 
	unsigned char data[1024]; 
	unsigned char hash[16] = {0}; 
	static char md5string[33]; 
	MD5_Init (&mdContext); 
	while ((bytes = fread (data, 1, 1024, inFile)) != 0) 
		MD5_Update (&mdContext, data, bytes); 
	MD5_Final (hash, &mdContext); 
	fclose(inFile); int i; 
	for(i = 0; i < 16; ++i) 
		sprintf(&md5string[i*2], "%02x", (unsigned int)hash[i]); 
	return md5string; 
}

void client(int portno,char *ip){
	int ClientSocket = 0;
	struct sockaddr_in serv_addr;

	// Creating a socket

	ClientSocket = socket(AF_INET,SOCK_STREAM,0);
	reuse(ClientSocket);
	if(ClientSocket<0){
		printf("ERROR WHILE CREATING A SOCKET\n");
	}
	else
		printf("[CLIENT] Socket created \n");

	//int portno = 5005;

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(portno);
		serv_addr.sin_addr.s_addr = inet_addr(ip);

	//Connection Establishment

	while(connect(ClientSocket,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0);

	char buffer[1024],nameDownload[100],hash1[33];
	const char* hash2;
	long int size,lm;
	int downStage = 0;;
	while(1){
		bzero(buffer,1023);bzero(nameDownload,99);//bzero(hash1,99);//bzero(size,99);
		size=0;lm=0;

		printf("\n message for server:");
		char* response = fgets(buffer,1023,stdin);
		if (response == NULL) {
			exit(0);
		}
		if(strncmp(buffer,"IndexGet",8)==0){
			if(send(ClientSocket, buffer, strlen(buffer), 0) < 0){
					printf("Error while writing to the socket.\n");
					return;
			}
			bzero(buffer, 1024);
			int sz = recv(ClientSocket,buffer, 1023, 0);
			if(sz <= 0){
				printf("Error while reading from socket.\n");
				break;
			}
			while(strncmp(buffer, "indexing complete", 17)!=0)
			{
				printf("%s", buffer);
				if (strncmp(buffer + strlen(buffer) - 17, "indexing complete", 17) == 0) break;
				bzero(buffer, 1024);
				int sz = recv(ClientSocket,buffer, 1023, 0);
				if(sz <= 0){
					printf("Error while reading from socket.\n");
					break;
				}
			}
			bzero(buffer,1023);
			//if(send(ClientSocket,buffer,strlen(buffer),0)<0)return;
		}
		else if(strncmp(buffer,"FileDownload ",13)==0){
			downStage = 1;
			if(send(ClientSocket,buffer,strlen(buffer),0)<0){
				printf("ERROR while writing to the socket\n");
				return;
			}
			bzero(buffer,1024);
			if(recv(ClientSocket,buffer,1023,0)<0){
				printf("ERROR while reading from the socket\n"); 
				return;
			}
			printf("\nMessage received by client: ");
			sscanf(buffer,"Filename: %s\nFilesize: %ld\nLastModified: %ld\nMD5hash: %s\n",nameDownload,&size,&lm,hash1);
			printf("Filename: %s\nFilesize: %ld\nLastModified:%sMD5hash: %s\n",nameDownload,size,ctime((time_t*)&lm),hash1);
			strcpy(buffer,"i will now send");
			if(send(ClientSocket,buffer,strlen(buffer),0)<0)return;
		}

		else if(strncmp(buffer,"FileUpload ",11)==0){
			if(send(ClientSocket,buffer,strlen(buffer),0)<0){
				printf("ERROR while writing to the socket\n");
				return;
			}
			struct stat file;
			char nameUpload[100];bzero(nameUpload,100);
			const char* hash;
			sscanf(buffer,"FileUpload %s\n",nameUpload);
			bzero(buffer,1024);
			char s1[1024],s[1024],size[100];
			bzero(s,1023);bzero(s1,1023);bzero(size,99);
			if(stat(nameUpload,&file)<0){
				strcpy(buffer,"File does not exist");
				perror("File does not exist");
				return;
			}
			else{
				hash = getMD5(nameUpload);
				bzero(buffer,1023);
				sprintf(size,"%ld",(long int)file.st_size);
				strcat(buffer,"Filename: ");strcat(buffer,nameUpload);strcat(buffer,"\n");
				strcat(buffer,"Filesize: ");strcat(buffer,size);strcat(buffer,"\n");
				strcat(buffer,"LastModified: ");sprintf(buffer+strlen(buffer),"%ld",(long int)file.st_mtime);strcat(buffer,"\n");
				strcat(buffer,"MD5hash: ");strcat(buffer,hash);strcat(buffer,"\n");
				printf("%s\n",buffer);
			}
			if(send(ClientSocket,buffer,1023,0)<0)return;

			bzero(buffer,1024);
			if(recv(ClientSocket,buffer,1023,0)<0){
				printf("ERROR while reading from the socket\n"); 
				return;
			}
			long int n1;
			FILE *fd1;
			fd1=fopen(nameUpload,"rb");
			do{
				bzero(buffer,1023);
				n1=fread(buffer,1,1023,fd1);
				if(send(ClientSocket,buffer,n1,0)<0)return;
			}while(!feof(fd1));
			fclose(fd1);
		
		}
		else{
			printf("ERROR: use correct syntax\n");
			return;
		}

		if (downStage == 1) {
			//printf("hello\n");
			long int n,doneSize=0;
			FILE* fd;
			fd = fopen(nameDownload,"wb");
			while(doneSize < size){
				bzero(buffer,1024);
				//printf("world\n");
				n=recv(ClientSocket,buffer,1023,0);
				//printf("%ld\n",n);
				if(n<=0){
					printf("ERROR while reading from the socket\n"); 
					return;
				}
				//printf("%s\n",buffer);
				fwrite(buffer,1,n,fd);
				doneSize+=n;
			}
			fclose(fd);
			downStage = 0;
			hash2 = getMD5(nameDownload);
			//printf("%s\n",hash2);
			if(strcmp(hash1,hash2)){printf("ERROR: MD5 did not match. Try again.");}
		}
		
	}	

	printf("Closing Connection\n");
	close(ClientSocket);
}

void server(int portno){
	
	int listenSocket = 0;	// This is my server's socket which is created to 
							//	listen to incoming connections
	int connectionSocket = 0;
	struct sockaddr_in serv_addr;		// This is for addrport for listening
	// Creating a socket
	listenSocket = socket(AF_INET,SOCK_STREAM,0);
	if(listenSocket<0){
		printf("ERROR WHILE CREATING A SOCKET\n");
	}
	else
		printf("[SERVER] SOCKET ESTABLISHED SUCCESSFULLY\n\n");

	reuse(listenSocket);
	// Its a general practice to make the entries 0 to clear them of malicious entry

	bzero((char *) &serv_addr,sizeof(serv_addr));

	// Binding the socket
	serv_addr.sin_family = AF_INET;	//For a remote machine
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portno);

	if(bind(listenSocket,(struct sockaddr * )&serv_addr,sizeof(serv_addr))<0) {
		printf("ERROR WHILE BINDING THE SOCKET\n");
		perror(strerror(errno));
	}
	else
		printf("[SERVER] SOCKET BINDED SUCCESSFULLY\n");

	// Listening to connections
	if(listen(listenSocket,10) == -1)	//maximum connections listening to 10
	{
		printf("[SERVER] FAILED TO ESTABLISH LISTENING \n\n");
		return;
	}
	printf("[SERVER] Waiting fo client to connect....\n" );

	// Accepting connections
	while((connectionSocket=accept(listenSocket , (struct sockaddr*)NULL,NULL))<0);

	// NULL will get filled in by the client's sockaddr once a connection is establised

	printf("[CONNECTED]\n");

	char buffer[1024];
	
	while(1){
		bzero(buffer,1024);
		if(recv(connectionSocket,buffer,1023,0)<=0){
			printf("ERROR while reading from Client\n");
			return;
		}
		printf("\nMessage received from client: %s\n",buffer );
		if(strncmp(buffer,"IndexGet",8)==0){
			int listType=-1;
			//strcat(buffer,"gonna do ls now\n");
			DIR *dir = opendir("../share");
			struct dirent *d;

			if(strncmp(buffer +9,"--longlist",10)==0){
				listType = 1;
			} 
			else if(strncmp(buffer +9,"--shortlist",11)==0){
				listType = 0; 
			}
			else if(strncmp(buffer +9,"--regex",7)==0){
				listType = 2;
			}
			bzero(buffer,1023);
			if(listType == 0 || listType == 2 ){
				strcpy(buffer,"not implemented yet\n");
				if(send(connectionSocket,buffer,strlen(buffer),0)<0)return;
			}
			else if(listType == 1){
				while((d = readdir(dir)) != NULL)
				{
					bzero(buffer,1023);
					struct stat s;
					char file[1024];

					strcpy(file, "../share/");
					strcat(file, d->d_name);

					if(lstat(file, &s) < 0){
						printf("Error!\n");
						continue;
					}

					bzero(buffer, 1024);
					strcpy(buffer, d->d_name); //name

					int sz = s.st_size;
					char *size = toString(sz);
					strcat(buffer, "\t");
					strcat(buffer, size);	//size

					//char *t = toString(s.st_mtime);
					strcat(buffer, "\t");
					sprintf(buffer+strlen(buffer),"%ld",(long int)s.st_mtime) ; //last modified time

					strcat(buffer, "\t");
					if(d->d_type == DT_REG){
						strcat(buffer,"-");
					}
					else if(d->d_type == DT_DIR){
						strcat(buffer,"d");
					}
					else
						strcat(buffer,"something else");
					
					buffer[strlen(buffer)] = '\n';
					buffer[strlen(buffer)] = '\0';
					//printf("%s",buffer);
					if(send(connectionSocket, buffer, strlen(buffer), 0) < 0){
						printf("Error while sending data.");
						return;
					}
				}
			}
			else{
				strcpy(buffer,"ERROR: use correct syntax\n");
				if(send(connectionSocket, buffer, strlen(buffer), 0)<0)return;
			}
			//if(recv(connectionSocket,buffer,1023,0)<=0)return;
			bzero(buffer,1023);
			strcpy(buffer,"indexing complete");
			if(send(connectionSocket,buffer,strlen(buffer),0)<0)return;
			bzero(buffer,1023);

			//printf("reached end at server");
			//if(recv(connectionSocket,buffer,1023,0)<=0)return;
		}
		else if(strncmp(buffer,"FileDownload ",13)==0){
			char s1[1024], s2[1024],s[1024],size[100];
			bzero(s,1023);bzero(s1,1023);bzero(s2,1023);bzero(size,99);//bzero(data,33);
			sscanf(buffer,"%s %s",s1,s2);
			bzero(buffer,1023);
			strcpy(s,"../share/");strcat(s,s2); //s has the file address now
			printf("message sent to client: ");
			struct stat file;
			const char* hash;
			hash = getMD5(s);
			if(stat(s,&file)<0){
				strcpy(buffer,"File does not exist");
				perror("File does not exist");
			}
			else{
				sprintf(size,"%ld",(long int)file.st_size);
				strcat(buffer,"Filename: ");strcat(buffer,s2);strcat(buffer,"\n");
				strcat(buffer,"Filesize: ");strcat(buffer,size);strcat(buffer,"\n");
				strcat(buffer,"LastModified: ");sprintf(buffer+strlen(buffer),"%ld",(long int)file.st_mtime);strcat(buffer,"\n");
				strcat(buffer,"MD5hash: ");strcat(buffer,hash);strcat(buffer,"\n");
			}
			printf("%s",buffer);
			if(send(connectionSocket,buffer,strlen(buffer),0)<0)return;

			if(recv(connectionSocket,buffer,1023,0)<=0)return;

			long int n;
			FILE* fd;
			fd = fopen(s,"rb");
			do{
				long int x=0;
				bzero(buffer,1023);
				n=fread(buffer,1,1023,fd);
				if((x=send(connectionSocket,buffer,n,0))<0)return;
			}while(!feof(fd));
			fclose(fd);
		}
		else if(strncmp(buffer,"FileUpload ",11)==0){ 
			char nameUpload[100],hash1[33],garbage[100];bzero(nameUpload,100);bzero(hash1,33);
			long int size,lm;
			bzero(buffer,1023);          	

			if(recv(connectionSocket,buffer,1023,0)<0)return;

			long int n,doneSize=0;
			char nameCreate[100];bzero(nameCreate,100);
			strcpy(nameCreate,"../share/");strcat(nameCreate,nameUpload);
			printf("\nMessage received by client: ");
			//printf("%s\n\n\n\n\n",buffer);
			sscanf(buffer,"Filename: %s\nFilesize: %ld\nLastModified: %ld\nMD5hash: %s\n",nameUpload,&size,&lm,hash1);
			printf("Filename: %s\nFilesize: %ld\nLastModified:%sMD5hash: %s\n",nameUpload,size,ctime((time_t*)&lm),hash1);
			strcat(nameCreate,nameUpload);
			bzero(buffer,1024);

			strcpy(buffer,"i will now send");
			if(send(connectionSocket,buffer,strlen(buffer),0)<0)return;

			FILE* fd2;
			fd2 = fopen(nameCreate,"wb");
			while(doneSize < size){
				bzero(buffer,1024);
				n=recv(connectionSocket,buffer,1023,0);
				if(n<=0){
					printf("ERROR while reading from the socket\n"); 
					return;
				}
				fwrite(buffer,1,n,fd2);
				doneSize+=n;
			}
			fclose(fd2);
			const char* hash2;
			hash2 = getMD5(nameCreate);
			//printf("%s\n",hash2);
			if(strcmp(hash1,hash2)){printf("ERROR: md5 did not match.try again.");}
		}
	}
	printf("\nClosing connection\n");
	close(connectionSocket);
	close(listenSocket);
}

int main(int argc, char* argv[]){
	setbuf(stdout, NULL);
	if(argc < 4){
		fprintf(stderr,"Error:give ports for server and client to connect\n");
		return 1;
	}
	int pid;
	pid = fork();
	if(pid < 0) {
		return 1;//Error
	}
	else if(pid > 0){
		int port=atoi(argv[1]);
		server(port);
	}
	else{
		char *ip = argv[3];
		int c=atoi(argv[2]);
		client(c,ip);
	}
	return 0;
}