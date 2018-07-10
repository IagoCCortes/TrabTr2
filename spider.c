#include "spider.h"

#define BUFFER_SIZE 1000000

char assets[100000];
char fassets[100000];
char mes[100000];
int count;

/*Faz o parser de mes para identificar os links a serem mostrados*/
void parseMesS(char *mes, char *host){
	char aux[100000];
	int i = 0, j;
	memset(assets, 0, 100000);
	while(1){
		while(mes[i] != 'h' && mes[i] != 's' && mes[i] != '<'){
			i++;
		}
		if(mes[i] == '<' && mes[i+1] == '/' && mes[i+2] == 'h' && mes[i+3] == 't' && mes[i+4] == 'm' && mes[i+5] == 'l')
			break;
		if(mes[i] == 'h' && mes[i+1] == 'r' && mes[i+2] == 'e' && mes[i+3] == 'f' && mes[i+4] == '=' && mes[i+5] == '"'){
			i += 6;
			j = 0;
			while(mes[i] != '"' && mes[i] != 39){
				aux[j] = mes[i];
				j++;
				i++;
			}
			aux[j] = '\0';
			strcat(assets, aux);
			strcat(assets, "  ");
		}
		if(mes[i] == 's' && mes[i+1] == 'r' && mes[i+2] == 'c' && mes[i+3] == '=' && mes[i+4] == '"'){
			i += 5;
			j = 0;
			while(mes[i] != '"' && mes[i] != 39){
				aux[j] = mes[i];
				j++;
				i++;
			}
			aux[j] = '\0';
			strcat(assets, aux);
			strcat(assets, "  ");
		}
		i++;
	}
	i = 0;
	char aux2[100000];
	memset(fassets, 0, 100000);
	count = 0;
	while(1){
		j = 0;
		while(assets[i] != ' '){
				aux[j] = assets[i];
				j++;
				i++;
		}
		aux[j] = '\0';
		if(aux[0] == 'h' && aux[1] == 't' && aux[2] == 't' && aux[3] == 'p' && aux[4] == 's'){
			i += 2;
		}else if(strstr(aux, host) != NULL){
			j = 0;
			while(aux[j] != '.' && aux[j+4] != '/')
				j++;
			int k = 0;
			while(aux[j] != '\0'){
				aux2[k] = aux[j];
				k++;
				j++;
			}
			aux2[k] = '\0';
			if(strlen(aux2) > 1){
				strcat(fassets, aux);
				strcat(fassets, "\n");
				count++;
			}
			i += 2;
		}else if(aux[0] == 'h' && aux[1] == 't' && aux[2] == 't' && aux[3] == 'p'){
			i += 2;
		}else{
			//if(strstr(aux, ".css") == NULL && strstr(aux, ".js") == NULL && strstr(aux, ".png") == NULL && strstr(aux, ".jpg") == NULL){
			if(strlen(aux) > 1){
				strcat(fassets, aux);
				strcat(fassets, "\n");
				count++;
			}
			i += 2;
		}
		if(assets[i] == '\0')
			break;
	}
	//printf("%s\n", fassets);
}

/*E responsavel por fazer o request para o spider*/
void getMesS(char *host, char *path, char *list){
	int sockfd, port = 80, bytes, sent, received, total, first = 1;
	strcpy(mes, "GET ");
	strcat(mes, path);
	strcat(mes, " HTTP/1.1\r\nHost: ");
	strcat(mes, host);
	strcat(mes, "\r\nConnection: close");
	strcat(mes, "\r\n\r\n");
	struct hostent *server;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE], resp[BUFFER_SIZE];
	int j = 0, k = 0;
	char aux[100000];
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Error on creating socket\n");
		exit(0);
	}

	server = gethostbyname(host);
	if(server == NULL){
		printf("Error on geting host by name\n");
		exit(0);
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("Error on connecting to socket\n");
		exit(0);
	}
	total = strlen(mes);
	sent = 0;

	do{
		bytes = write(sockfd, mes+sent, total-sent);
		if(bytes<0){
			printf("Error on sending request to server\n");
			exit(0);
		}
		if(bytes == 0)
			break;
		sent+=bytes;
	}while(sent < total);

	memset(resp, 0, sizeof(resp));
	total = sizeof(resp)-1;
	received = 0;
	do{
		bytes = read(sockfd, resp+received, total-received);
		if(bytes<0){
			printf("Error on receiving response from server\n");
			exit(0);
		}
		if(bytes == 0)
			break;
		received+=bytes;
	}while(received < total);

	close(sockfd);
	parseMesS(resp, host);
	strcpy(list, fassets);
	//printf("%s\n",list);
}

/*int main(int argc, char *argv[]){
	char list[100000];
	getMesS(argv[1], argv[2], list);
	printf("%s\n", list);
	return 0;
}*/
