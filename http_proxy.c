#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct request{
	char method[1000];
	char path[1000];
	char version[1000];
	char host[1000];
	char body[10000];
}req;

char mbuf[5000];

/*Faz o parse da resposta do servidor e salva a mesma em um arquivo*/
int parseResponse(char *buffer, int size){
	char headerN[100][1000];
	char headerV[100][1000];
	int headerL[100];
	int hcount;
	hcount = 0;
	int i = 0, j = 0;
	char ver[1000], code[1000], desc[1000];
	while(buffer[i] != ' '){
		ver[j] = buffer[i];
		i++;
		j++;
	}
	ver[j] = '\0';
	i++;
	j = 0;
	while(buffer[i] != ' '){
		code[j] = buffer[i];
		i++;
		j++;
	}
	code[j] = '\0';
	i++;
	j = 0;
	while(buffer[i] != '\r'){
		desc[j] = buffer[i];
		i++;
		j++;
	}
	desc[j] = '\0';
	i = i + 2;
	j = 0;

Y:while(buffer[i] != ':'){
		headerN[hcount][j] = buffer[i];
		i++;
		j++;
	}
	headerN[hcount][j] = '\0';
	i = i + 2;
	j = 0;
	while(buffer[i] != '\r'){
			headerV[hcount][j] = buffer[i];
			i++;
			j++;
	}
	headerV[hcount][j] = '\0';
	j = 0;
	headerL[hcount] = strlen(headerN[hcount]) + strlen(headerV[hcount]);
	hcount++;
	if(buffer[i+2] != '\r'){
		i = i + 2;
		goto Y;
	}
	i = i + 4;
	int hsize = i -1;
	size -= hsize;
	char aux[5000];
	while(buffer[i] != '\0'){
		aux[j] = buffer[i];
		i++;
		j++;
	}
	aux[j] = '\0';
	//changes happen here

	FILE *fp;
	fp = fopen("response.txt", "w");
	fprintf(fp, "%d\n%d\n%s\n%s\n%s\n", getpid(), hcount, ver, code, desc);
	for(i = 0; i < hcount; i++)
		fprintf(fp, "%s: %s\n", headerN[i], headerV[i]);
	fclose(fp);
	kill(getpid(), SIGSTOP);

	//fp = fopen("response.txt", "r");
	//if(fp != NULL){
	//	fscanf(fp, "%s\n%s\n%s\n", ver, code, desc);
	//	for(i = 0; i < hcount; i++)
	//		fscanf(fp, "%s: %[^\n]\n", headerN[i], headerV[i]);
	//}
	//*/
	//char mbuf[5000];
	memset(mbuf, 0, 5000);
	int k;
	sprintf(mbuf, "%s %s %s\r\n", ver, code, desc);
	for(k = 0; k < hcount; k++){
		strcat(mbuf, headerN[k]);
		strcat(mbuf, ": ");
		strcat(mbuf, headerV[k]);
		strcat(mbuf, "\r\n");
	}
	strcat(mbuf, "\r\n");
	hsize = strlen(mbuf);
	//int cur = strlen(mbuf);
	//buffer += cur;
	strcat(mbuf, aux);
	//*size -= cur;
	size += hsize;
	size --;
	//printf("%d\n%s\n", size, mbuf);
	return size;
}

/*Faz conexao com o servidor do request, envia o request para este servidor recebe a resposta do servidor e a envia
para o cliente(browser)*/
int serverSocket(char *host, char *Buf, int client, int size){
	struct addrinfo hints;
	struct addrinfo *resp;
	int fd_server;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if(getaddrinfo(host, (char*)"80", &hints, &resp) != 0){
		printf("Error in server address!!\n");
		exit(1);
	}
	if((fd_server = socket(resp->ai_family, resp->ai_socktype, resp->ai_protocol)) < 0){
		printf("Error in server socket creation!!\n");
		exit(1);
	}
	if(connect(fd_server, resp->ai_addr, resp->ai_addrlen) < 0){
		printf("Error in server connection!!\n");
		exit(1);
	}
	int sent = 0, sd;
	//printf("REQUEST:\n%s\nPress enter to send Request\n", Buf);
	//getchar();
	while(sent < size){
		if((sd = send(fd_server, (void*) (Buf + sent), size - sent, 0)) < 0){
			printf("Error on sending request to server!\n");
			exit(1);
		}
		sent += sd;
	}
	sent = 0;
	char response[5000], buffer[1000][5000];
	int received;
	int step, total = 0, bufsizes[1000], bufs = 0;
	//memset(buffer,0,sizeof buffer);
	while((bufsizes[bufs] = recv(fd_server, buffer[bufs], 5000, 0)) > 0){
			if(bufsizes[bufs] < 0){
				printf("Error receiving response from server\n");
				exit(1);
			}
			printf("%s\n",buffer[bufs]);
			bufs++;
	}
	//printf("%d\n", bufsizes[0]);
	bufsizes[0] = parseResponse(buffer[0], bufsizes[0]);
	strcpy(buffer[0], mbuf);
	//printf("%d\n", bufsizes[0]);
	//printf("%s\n",buffer[0]);
	for(int i = 0; i < bufs; i++){
		while (sent < bufsizes[i]) {
			if ((sd = send(client, (void *) (buffer[i] + sent), bufsizes[i] - sent, 0)) < 0) {
				printf("Error in sending to client!\n");
				exit (1);
			}
			sent += sd;
		}
		sent = 0;
	}
	//buffer[total] = '\0';
	//printf("RESPONSE:\n%s\nPress enter to send response\n",buffer);
	//getchar();
	/*int a;
	while((a = recv(fd_server, response, 5000, 0)) > 0){
		//printf("%s\n", response);
		if(a < 0){
			printf("Error receiving response from server\n");
			exit(1);
		}
		//printf("a = %d\n", a);
		while (sent < a) {
			if ((sd = send(client, (void *) (response + sent), a - sent, 0)) < 0) {
				printf("Error in sending to client!\n");
				exit (1);
			}
			//printf("sd = %d\n", sd);
			sent += sd;
		}
		total += a;
		//printf("sent = %d\n", sent);
		sent = 0;
		memset(response,0,sizeof response);
	}*/
	//printf("total = %d\n", total);
	return fd_server;
}

/*Recebe uma request do cliente (browser), faz o parse e salva o mesmo em um arquivo*/
int getHTTP(void* client, int pid){
	char headerN[100][1000];
	char headerV[100][1000];
	int headerL[100];
	int hcount;
	hcount = 0;
	int buf_size = 5000;
	char buf[buf_size];
	int fd_client = *((int*)client);
	buf[0] = '\0';
	int get;
	while(strstr(buf, "\r\n\r\n") == NULL){
		get = recv(fd_client, buf, buf_size, 0);
		if(get == 0)
			break;
		else
			buf[get] = '\0';
	}
	//printf("%s\n", buf);
	int i = 0, j = 0;
	req request;
	char aux[1000];
	while(buf[i] != ' '){
		request.method[j] = buf[i];
		i++;
		j++;
	}
	request.method[j] = '\0';
	if(strcmp(request.method, "GET") !=0 && strcmp(request.method, "POST") !=0 && strcmp(request.method, "PUT") !=0 &&
    strcmp(request.method, "HEAD") !=0 && strcmp(request.method, "DELETE") !=0 && strcmp(request.method, "PATCH") !=0 &&
		strcmp(request.method, "OPTIONS") !=0){
		printf("Only HTTP with GET allowed!\n");
		exit(0);
	}
	i ++;
	j = 0;
	while(buf[i] != ' '){
		request.path[j] = buf[i];
		i++;
		j++;
	}
	request.path[j] = '\0';
	i++;
	j = 0;
	while(buf[i] != '\r'){
		request.version[j] = buf[i];
		i++;
		j++;
	}
	request.version[j] = '\0';
	i = i + 2;
	j = 0;
X:while(buf[i] != ':'){
		aux[j] = buf[i];
		i++;
		j++;
	}
	aux[j] = '\0';
	strcpy(headerN[hcount], aux);
	i = i + 2;
	j = 0;
	while(buf[i] != '\r'){
			aux[j] = buf[i];
			i++;
			j++;
	}
	aux[j] = '\0';
	strcpy(headerV[hcount], aux);
	j = 0;
	headerL[hcount] = strlen(headerN[hcount]) + strlen(headerV[hcount]);
	hcount++;
	if(buf[i+2] != '\r'){
		i = i + 2;
		goto X;
	}
	int k;
	i = i + 4;
	while(buf[i] != '\0'){
		aux[j] = buf[i];
		i++;
		j++;
	}
	char ref[10000];
	int reflag = 0;
	memset(ref, 0, 10000);
	aux[j] = '\0';
	strcpy(request.body, aux);
	char mesBuf[1000000];
	for(k = 0; k < hcount; k++){
		if(strcmp(headerN[k], "Host") == 0)
			strcpy(request.host, headerV[k]);
		if(strcmp(headerN[k], "Accept-Encoding") == 0){
			strcpy(headerV[k], "identity");
			headerL[k] = strlen(headerN[k]) + strlen(headerV[k]);
		}
		if(strcmp(headerN[k], "Connection") == 0){
			strcpy(headerV[k], "close");
			headerL[k] = strlen(headerN[k]) + strlen(headerV[k]);
		}
		if(strcmp(headerN[k], "Referer") == 0){
			strcpy(ref, headerV[k]);
			reflag = 1;
		}
	}
	int l, m, temp;
	char help[1000];
	for(l = 0; l < hcount -1; l++){
		for(m = 0; m < hcount - l - 1; m++){
			if(headerL[m] > headerL[m+1]){
				strcpy(help, headerN[m]);
				strcpy(headerN[m], headerN[m+1]);
				strcpy(headerN[m+1], help);
				strcpy(help, headerV[m]);
				strcpy(headerV[m], headerV[m+1]);
				strcpy(headerV[m+1], help);
				temp = headerL[m];
				headerL[m] = headerL[m+1];
				headerL[m+1] = temp;
			}
		}
	}

	i = 7, k = 0;
	char tmp[1000];
	while(request.path[i] != '/')
		i++;
	while(request.path[i] != '\0'){
		tmp[k] = request.path[i];
		i++;
		k++;
	}
	tmp[k] = '\0';

	if(reflag == 0){
		strcat(ref, request.host);
		strcat(ref, "  ");
		strcat(ref, tmp);
	}
	//changes happen here
	FILE *fp;
	fp = fopen("request.txt", "w");
	fprintf(fp, "%d\n%d\n%s\n%s\n%s\n", getpid(), hcount, request.method, tmp, request.version);
	for(i = 0; i < hcount; i++)
		fprintf(fp, "%s: %s\n", headerN[i], headerV[i]);
	fclose(fp);

	remove("ref.txt");
	fp = fopen("ref.txt", "w");
	int g = 7;
	if(ref[0] == 'h' && ref[1] == 't' && ref[2] == 't' && ref[3] == 'p' && ref[4] == ':'){
		while(ref[g] != '\0'){
			fprintf(fp, "%c", ref[g]);
			if(ref[g] != ' ' && ref[g+1] == '/' && reflag == 1){
					fprintf(fp, "  ");
					reflag = 0;
			}
			g++;
		}
	}else{
			fprintf(fp, "%s", ref);
	}
	fclose(fp);

	//kill(pid, SIGSTOP);
	kill(getpid(), SIGSTOP);

	//fp = fopen("request.txt", "r");
	//if(fp != NULL){
	//	fscanf(fp, "%s\n%s\n%s\n", request.method, request.path, request.version);
	//	for(i = 0; i < hcount; i++)
	//		fscanf(fp, "%s: %[^\n]\n", headerN[i], headerV[i]);
	//}
	//*/

	sprintf(mesBuf, "%s %s %s\r\n", request.method, tmp, request.version);
	for(k = 0; k < hcount; k++){
		strcat(mesBuf, headerN[k]);
		strcat(mesBuf, ": ");
		strcat(mesBuf, headerV[k]);
		strcat(mesBuf, "\r\n");
	}
	strcat(mesBuf, "\r\n");
	strcat(mesBuf, request.body);
	printf("\n\n%s\n\n", mesBuf);
	//printf("%s\n",request.host);
	int serverfd = serverSocket(request.host, mesBuf, fd_client, get);
	close(fd_client);
	close(serverfd);
	return 0;
}

/*Inicializa uma socket para ouvir requests do browser no endereço 127.0.0.1:8228*/
int main(int argc, char *argv[]){
	if(!fork()){
		execl("./gtk", "./gtk", NULL);
		exit(0);
	}

	int port = 8228;
	//printf("%d\n", argc);
	if(argc == 3){
		port = atoi(argv[2]);
	}

	struct sockaddr_in server_addr;
	struct sockaddr client_addr;
  //create socket
	int fd_server, fd_client;
	fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_server < 0){
		printf("Socket error\n");
		return 1;
	}
  memset(&server_addr, 0, sizeof(server_addr));
	// specify address for socket
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
  //binding to address
  int binded, estado;
	binded = bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if(binded < 0){
		printf("Bind error\n");
		return 1;
	}
  int client_len = sizeof(struct sockaddr);
	listen(fd_server, 500);
	int pid = getpid();
	while(1){
		fd_client = accept(fd_server, &client_addr, (socklen_t*) &client_len);
    if(fd_client < 0){
			printf("Connection failed.....\n");
			continue;
		}
    if(!fork()){
			/*child*/
			getHTTP((void*)&fd_client, pid);
			close(fd_client);
			_exit(0);
		}else{
			/*father*/
			wait(&estado);
			close(fd_client);
		}
	}
  close(fd_server);
	return 0;
}
