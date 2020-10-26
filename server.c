#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "dyad.h"

#define CLIENT_MAX 10

#define STRING_MAX 256

dyad_Stream *clients[CLIENT_MAX];

static void broadcast(const char *fmt,...) {
	int i;
	va_list args;
	for(i=0;i<CLIENT_MAX;i++) {
		if(clients[i]) {
			va_start(args,fmt);
		  dyad_vwritef(clients[i],fmt,args);
			va_end(args);
		}
	}
}

static void onLine(dyad_Event *e) {
	printf("%d: %s\n",*(int*)e->udata,e->data);
	broadcast("%d: %s\n",*(int*)e->udata,e->data);
}

static void onClose(dyad_Event *e) {
	broadcast("%d -> disconnected.\n",*(int*)e->udata);
	printf("%d -> disconnected.\n",*(int*)e->udata);
	clients[*(int*)e->udata]=NULL;
	free(e->udata);
}

static void onAccept(dyad_Event *e) {
	int i,j;
	
	j=-1;
  for(i=0;i<CLIENT_MAX;i++) {
		if(clients[i]==NULL) {
			j=i;
			break;
		}
	}

	if(j!=-1) {
		clients[j]=e->remote;
		int *k=malloc(sizeof(int));
		*k=j;
	  dyad_addListener(e->remote, DYAD_EVENT_LINE,    onLine,    k);
	  dyad_addListener(e->remote, DYAD_EVENT_CLOSE,   onClose,   k);
		dyad_writef(e->remote,"you are number %d.\n",j);
		broadcast("number %d joins.\n",j);
	} else {
		dyad_writef(e->remote,"error: cannot connect server full.\n");
		dyad_end(e->remote);
	}
}

int main(void) {

	int i;

	for(i=0;i<CLIENT_MAX;i++) clients[i]=NULL;

  dyad_init();

  dyad_Stream *s = dyad_newStream();
  dyad_addListener(s, DYAD_EVENT_ACCEPT,  onAccept,  NULL);
  dyad_listen(s, 5254);

  while (dyad_getStreamCount() > 0) {
    dyad_update();
  }

  dyad_shutdown();

  return 0;
}
