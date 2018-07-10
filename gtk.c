#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <gtk/gtk.h>
#include "spider.h"
#include "dump.h"

static GtkWidget *rqfield[100], *rpfield[100], *met, *pat, *ver, *grid, *vers, *cod, *des, *rrequest, *rreply,
 *srequest, *sreply;
int hcount;

/*Quando o botao spider e clicado essa funcao e rodada para abrir uma nova janela e mostrar os links que saem da pagina no
arquivo ref.txt*/
void listSpider(GtkWidget *p_widget, gpointer data){
  GtkWidget *p_window, *swindow, *p_box, *p_text;
  p_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(p_window), "Spider");
  gtk_window_set_default_size(GTK_WINDOW(p_window), 500, 650);

  p_text = gtk_label_new(NULL);

  p_box = gtk_box_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(p_window), p_box);

  swindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_box_pack_start(GTK_BOX(p_box), swindow, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(swindow), p_text);
  gtk_box_pack_start(GTK_BOX(p_box), p_text, TRUE, TRUE, 0);
  char list[100000];

  FILE *fp;
	fp = fopen("ref.txt", "r");
  char p1[10000], p2[10000];
	fscanf(fp, "%s  %s", p1, p2);
	fclose(fp);
  printf("%s %s\n", p1, p2);

  getMesS(p1, p2, list);
  gtk_label_set_text(GTK_LABEL(p_text), list);
  printf("%s\n", list);
  gtk_widget_show_all(p_window);
}

/*Quando o botao send request e clicado essa funcao e rodada para enviar o request ao cliente*/
void sendRequest(GtkWidget *srequest, gpointer data){
	int i, j, pd;
	FILE *fp;
	fp = fopen("request.txt", "r");
	if(fp != NULL){
		fscanf(fp, "%d\n", &pd);
	}
	fclose(fp);
	remove("request.txt");
	gtk_entry_set_text(GTK_ENTRY(vers), "");
	gtk_entry_set_text(GTK_ENTRY(cod), "");
	gtk_entry_set_text(GTK_ENTRY(des), "");
	for(i = 0; i < 18; i++){
		gtk_entry_set_text(GTK_ENTRY(rpfield[i]), "");
		gtk_entry_set_text(GTK_ENTRY(rpfield[i]), "");
	}
	/*char method[1000], path[1000], version[1000];
	char header[100][1000];
	fp = fopen("request.txt", "W");
	if(fp != NULL){
		strcpy(header[0], gtk_entry_get_text(GTK_ENTRY(met)));
		strcpy(header[1], gtk_entry_get_text(GTK_ENTRY(pat)));
		strcpy(header[2], gtk_entry_get_text(GTK_ENTRY(ver)));
		fprintf(fp, "%s\n%s\n%s\n", header[0], header[1], header[2]);
		for(i = 3, j = 0; j < hcount; i++, j++){
			strcpy(header[i], gtk_entry_get_text(GTK_ENTRY(rqfield[j])));
			fprintf(fp, "%s\n", header[i]);
		}
		fclose(fp);
	}*/
	gtk_widget_set_sensitive(srequest, FALSE);
	gtk_widget_set_sensitive(rreply, TRUE);
	kill(pd, SIGCONT);
	//so deixa enviar uma vez
}

/*Quando o botao send response e clicado essa funcao e rodada para enviar a response ao cliente*/
void sendResponse(GtkWidget *sreply, gpointer data){
	int i, j, pd;
	FILE *fp;
	fp = fopen("response.txt", "r");
	if(fp != NULL){
		fscanf(fp, "%d\n", &pd);
	}
	fclose(fp);
	remove("response.txt");
	gtk_entry_set_text(GTK_ENTRY(met), "");
	gtk_entry_set_text(GTK_ENTRY(pat), "");
	gtk_entry_set_text(GTK_ENTRY(ver), "");
	for(i = 0; i < 18; i++){
		gtk_entry_set_text(GTK_ENTRY(rqfield[i]), "");
		gtk_entry_set_text(GTK_ENTRY(rqfield[i]), "");
	}
	/*char method[1000], path[1000], version[1000];
	char header[100][1000];
	fp = fopen("response.txt", "W");
	if(fp != NULL){
		strcpy(header[0], gtk_entry_get_text(GTK_ENTRY(vers)));
		strcpy(header[1], gtk_entry_get_text(GTK_ENTRY(cod)));
		strcpy(header[2], gtk_entry_get_text(GTK_ENTRY(des)));
		fprintf(fp, "%s\n%s\n%s\n", header[0], header[1], header[2]);
		for(i = 3, j = 0; j < hcount; i++, j++){
			strcpy(header[i], gtk_entry_get_text(GTK_ENTRY(rpfield[j])));
			fprintf(fp, "%s\n", header[i]);
		}
		fclose(fp);
	}
	printf("%d\n",pd);*/
	gtk_widget_set_sensitive(sreply, FALSE);
	gtk_widget_set_sensitive(rrequest, TRUE);
	kill(pd, SIGCONT);
	//so deixa enviar uma vez
}

/*Quando o botao receive request e clicado essa funcao e rodada para carregar a request na interface*/
void setRequest(GtkWidget *rrequest, gpointer data){
	int i, pd;
	char method[1000], path[1000], version[1000];
	char header[100][1000];
	FILE *fp;
	fp = fopen("request.txt", "r");
	if(fp != NULL){
		fscanf(fp, "%d\n%d\n%s\n%s\n%s\n", &pd, &hcount, method, path, version);
		for(i = 0; i < hcount; i++)
			fscanf(fp, "%[^\n]\n", header[i]);
			//fgets(header[i], sizeof header[i], fp);
		fclose(fp);

		gtk_entry_set_text(GTK_ENTRY(met), method);
		gtk_entry_set_text(GTK_ENTRY(pat), path);
		gtk_entry_set_text(GTK_ENTRY(ver), version);

		for(i = 0; i < hcount; i++){
			gtk_entry_set_text(GTK_ENTRY(rqfield[i]), header[i]);

			//gtk_entry_set_text(GTK_ENTRY(rqfield[i]), header[i]);
		}
		//remove("request.txt");
		gtk_widget_set_sensitive(rrequest, FALSE);
		gtk_widget_set_sensitive(srequest, TRUE);
	}
}

/*Quando o botao receive response e clicado essa funcao e rodada para carregar a response na interface*/
void setResponse(GtkWidget *rreply, gpointer data){
	int i, pd;
	char version[1000], code[1000], desc[1000];
	char header[100][1000];
	FILE *fp;
	fp = fopen("response.txt", "r");
	if(fp != NULL){
		fscanf(fp, "%d\n%d\n%s\n%s\n", &pd, &hcount, version, code);
		fscanf(fp, "%[^\n]\n", desc);
		for(i = 0; i < hcount; i++)
			fscanf(fp, "%[^\n]\n", header[i]);
			//fgets(header[i], sizeof header[i], fp);
		fclose(fp);

		gtk_entry_set_text(GTK_ENTRY(vers), version);
		gtk_entry_set_text(GTK_ENTRY(cod), code);
		gtk_entry_set_text(GTK_ENTRY(des), desc);

		for(i = 0; i < hcount; i++){
			gtk_entry_set_text(GTK_ENTRY(rpfield[i]), header[i]);

			//gtk_entry_set_text(GTK_ENTRY(rpfield[i]), header[i]);
		}
		//remove("response.txt");
		gtk_widget_set_sensitive(rreply, FALSE);
		gtk_widget_set_sensitive(sreply, TRUE);
	}
}

/*Inicializa os widgets utilizados na tela principal da interface gráfica e fica a espera de interacao com os botoes*/
int main(int argc, char *argv[]){
	GtkWidget *window, *spider, *dump, *scrollW;// *srequest, *sreply, *rrequest, *rreply;
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_title(GTK_WINDOW(window), "Trabalho TR2");

	grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(window), grid);

  //scrollW = gtk_scrolled_window_new(NULL, NULL);

	spider = gtk_button_new_with_label("Spider");
  g_signal_connect(spider, "clicked", G_CALLBACK(listSpider), NULL);
	gtk_grid_attach(GTK_GRID(grid), spider, 0, 0, 1, 1);

	dump = gtk_button_new_with_label("Dump");
  g_signal_connect(dump, "clicked", G_CALLBACK(doDump), NULL);
	gtk_grid_attach(GTK_GRID(grid), dump, 1, 0, 1, 1);

	rrequest = gtk_button_new_with_label("Receive request");
	g_signal_connect(rrequest, "clicked", G_CALLBACK(setRequest), NULL);
	gtk_grid_attach(GTK_GRID(grid), rrequest, 0, 1, 1, 1);

	rreply = gtk_button_new_with_label("Receive reply");
	gtk_widget_set_sensitive(rreply, FALSE);
	g_signal_connect(rreply, "clicked", G_CALLBACK(setResponse), NULL);
	gtk_grid_attach(GTK_GRID(grid), rreply, 1, 1, 1, 1);

	srequest = gtk_button_new_with_label("Send request");
	gtk_widget_set_sensitive(srequest, FALSE);
	g_signal_connect(srequest, "clicked", G_CALLBACK(sendRequest), NULL);
	gtk_grid_attach(GTK_GRID(grid), srequest, 0, 2, 1, 1);

	sreply = gtk_button_new_with_label("Send reply");
	gtk_widget_set_sensitive(sreply, FALSE);
	g_signal_connect(sreply, "clicked", G_CALLBACK(sendResponse), NULL);
	gtk_grid_attach(GTK_GRID(grid), sreply, 1, 2, 1, 1);

	met = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), met, 0, 3, 1, 1);
	gtk_entry_set_width_chars(GTK_ENTRY(met), 83);

	pat = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), pat, 0, 4, 1, 1);
	gtk_entry_set_width_chars(GTK_ENTRY(pat), 83);

	ver = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), ver, 0, 5, 1, 1);
	gtk_entry_set_width_chars(GTK_ENTRY(ver), 83);

	vers = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), vers, 1, 3, 1, 1);
	gtk_entry_set_width_chars(GTK_ENTRY(vers), 83);

	cod = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), cod, 1, 4, 1, 1);
	gtk_entry_set_width_chars(GTK_ENTRY(cod), 83);

	des = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), des, 1, 5, 1, 1);
	gtk_entry_set_width_chars(GTK_ENTRY(des), 83);

	for(int i = 0; i < 18; i++){
		rqfield[i] = gtk_entry_new();
		gtk_grid_attach(GTK_GRID(grid), rqfield[i], 0, i+6, 1, 1);
		gtk_entry_set_width_chars(GTK_ENTRY(rqfield[i]), 83);

		rpfield[i] = gtk_entry_new();
		gtk_grid_attach(GTK_GRID(grid), rpfield[i], 1, i+6, 1, 1);
		gtk_entry_set_width_chars(GTK_ENTRY(rpfield[i]), 83);
	}

	gtk_widget_show_all(window);
	gtk_main();
}
