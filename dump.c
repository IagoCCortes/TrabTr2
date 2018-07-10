#include "dump.h"

#define BUFFER_SIZE 1000000

GtkWidget *p_window, *swindow, *p_box, *p_text;

char assets[100000];
char fassets[100000];
char mes[100000];
int count;
char status[100000];

void createF(char *path, char *resp, char *host){
	int i = 0, j = 0;
	struct stat st = {0};
	char nameAux[100000], name[100000];
	memset(name, 0, 100000);
	strcat(name, host);
	if(path[i] == '/')
		i++;
	while(1){
		nameAux[j] = '/';
		j++;
		while(path[i] != '/' && path[i] != '\0'){
				nameAux[j] = path[i];
				j++;
				i++;
		}
		if(path[i] == '/'){
			nameAux[j] = '\0';
			j++;
			i++;
			strcat(name, nameAux);
			printf("%s\n", name);
			//memset(status, 0, 100000);
			strcat(status, "Criando o diretório: ");
			strcat(status, name);
			strcat(status, " \n");
			gtk_label_set_text(GTK_LABEL(p_text), status);
			if(stat(name, &st) == -1)
				mkdir(name, 0700);
			j = 0;
		}else{
			nameAux[j] = '\0';
			strcat(name, nameAux);
			break;
		}
	}
	char body[1000000];
	i = 0;
	j = 0;
	while(resp[i] != '\r' || resp[i+1] != '\n' || resp[i+2] != '\r' || resp[i+3] != '\n'){
		i++;
	}
	i += 4;
	while(resp[i] != '\0'){
		body[j] = resp[i];
		j++;
		i++;
	}
	body[j] = '\0';
	printf("%s\n",name);
	//memset(status, 0, 1000000);
	strcat(status, "Salvando o arquivo: ");
	strcat(status, name);
	strcat(status, " \n");
	gtk_label_set_text(GTK_LABEL(p_text), status);
	FILE *fp;
	fp = fopen(name, "w");
	fprintf(fp, "%s", body);
	fclose(fp);
}

void parseMes(char *mes, char *host){
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
				strcat(fassets, "  ");
				count++;
			}
			i += 2;
		}else if(aux[0] == 'h' && aux[1] == 't' && aux[2] == 't' && aux[3] == 'p'){
			i += 2;
		}else{
			if(strlen(aux) > 1){
				strcat(fassets, aux);
				strcat(fassets, "  ");
				count++;
			}
			i += 2;
		}
		if(assets[i] == '\0')
			break;
	}
}

void getMes(char *host, char *path){
	char dir[1000];
	strcat(dir,host);
	strcat(dir, path);
	mkdir(dir, 0700);
	memset(status, 0, 100000);
	strcpy(status, "Criando o diretório raiz: ");
	strcat(status, dir);
	strcat(status, " \n");
	gtk_label_set_text(GTK_LABEL(p_text), status);
	//gtk_widget_show_now(p_window);
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
	for(int i = 0; i < count + 1; i++){
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
		printf("%s\n", mes);
		//memset(status, 0, 100000);
		strcat(status, "Enviando request ao server\n");
		gtk_label_set_text(GTK_LABEL(p_text), status);
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
		//memset(status, 0, 100000);
		strcat(status, "Recebendo response do server\n");
		gtk_label_set_text(GTK_LABEL(p_text), status);
		do{
			bytes = read(sockfd, resp+received, total-received);
			if(bytes<0){
				printf("Error on receiving response from server\n");
				exit(0);
			}
			printf("%s\n",resp);
			if(bytes == 0)
				break;
			received+=bytes;
		}while(received < total);
		printf("%s\n", resp);

		close(sockfd);
		if(first){
			parseMes(resp, host);
			first = 0;
			createF("index.html", resp, host);
		}else{
			createF(aux, resp, host);
		}
		memset(mes, 0, 1000);
		j = 0;
		if(fassets[k] == '\0')
			break;
		while(fassets[k] != ' '){
				aux[j] = fassets[k];
				j++;
				k++;
		}
		aux[j] = '\0';

	  k += 2;
		strcpy(mes, "GE");
		if(aux[0] != '/')
			strcat(mes, "T /");
		else
			strcat(mes, "T ");
		strcat(mes, aux);
		strcat(mes, " HTTP/1.1\r\nHost: ");
		strcat(mes, host);
		strcat(mes, "\r\nConnection: close");
		strcat(mes, "\r\n\r\n");
	}
}

void doDump(GtkWidget *p_widget, gpointer data){
  //GtkWidget *p_window, *swindow, *p_box, *p_text;
  p_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(p_window), "Dump");
  gtk_window_set_default_size(GTK_WINDOW(p_window), 650, 650);

  p_text = gtk_label_new(NULL);

  p_box = gtk_box_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(p_window), p_box);

  swindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_box_pack_start(GTK_BOX(p_box), swindow, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(swindow), p_text);
  gtk_box_pack_start(GTK_BOX(p_box), p_text, TRUE, TRUE, 0);

  FILE *fp;
	fp = fopen("ref.txt", "r");
  char p1[10000], p2[10000];
	fscanf(fp, "%s  %s", p1, p2);
	fclose(fp);
  printf("%s %s\n", p1, p2);

  getMes(p1, p2);
  //gtk_label_set_text(GTK_LABEL(p_text), list);
  gtk_widget_show_all(p_window);
}

/*int main(int argc, char *argv[]){
	mkdir(argv[1], 0700);
	getMes(argv[1], argv[2]);
	return 0;
}*/
