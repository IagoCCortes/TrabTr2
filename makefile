all: aracne gtk

aracne: http_proxy.c
		gcc -o aracne http_proxy.c

gtk: gtk.c
		gcc gtk.c spider.c dump.c -o gtk `pkg-config --cflags --libs gtk+-3.0`

#spider: spider.c
#		gcc -o spider spider.c

#dump: dump.c
#		gcc dump.c -o dump `pkg-config --cflags --libs gtk+-3.0`
