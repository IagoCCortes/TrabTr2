#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <gtk/gtk.h>

void createF(char *path, char *resp, char *host);
void parseMes(char *mes, char *host);
void getMes(char *host, char *path);
void doDump(GtkWidget *p_widget, gpointer data);
