/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "placesairports.h"
#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static void
placesprog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		struct location get_places_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case get_places:
		_xdr_argument = (xdrproc_t) xdr_location;
		_xdr_result = (xdrproc_t) xdr_readplaces_ret;
		local = (char *(*)(char *, struct svc_req *)) get_places_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

static const char filename[] = "places2k.txt";
trieNode_t *root;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: Struct to store place (line in text file).
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

struct place {
  char name[255];
  char state[2];
  float latitude;
  float longitude;
};

typedef struct place Place;


char *trim(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0) 
	return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: Struct to store place (line in text file).
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
Place *  parseLine(char * line){

  Place *p = malloc(sizeof *p);
  char * temp = malloc(sizeof *temp);

  strncpy(p->name, line+9, 64); // name
  strncpy(p->name, trim(p->name), strlen(p->name)); // trimming spaces

  strncpy(p->state, line, 2);   // state

  strncpy(temp, line+143, 10);
  p->latitude = atof(temp);     // latitude

  strncpy(temp, line+153, 11);  // longitude
  p->longitude = atof(temp);

  return p;
};


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: Function to read file places2k.txt and add that information
//:: to the trie structure.
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void readFile() {
  FILE *file = fopen(filename, "r");
  char line[255]; //temp storage for line

  if (file != NULL) {
	while (fgets(line, sizeof line, file) != NULL) { //read line
	  // parse line and get data in struct
	  Place *p = parseLine(line);
	  int i = 0;

	  // concatenate state and city names and convert to lowercase to form a single unique entry to add to trie
	  // "state|city" eg. "Seattle, WA" becomes "waseattle"
	  char merge[255];
	  strcpy(merge, p->state);
	  strcat(merge, p->name);
	  unsigned int lastSpace = 0;
	  for (i = 0; merge[i]; i++)
		{
		  merge[i] = tolower(merge[i]);
		  if (merge[i] == ' ') {
			lastSpace = i;
		  }
		}
	  	  
	  // add new entry to trie
	  TrieAdd(&root, merge, p->latitude, p->longitude);

	  // discard p, its data has been added to trie so it's no longer needed
	  free(p);
	}
	// Close file before exit
	fclose(file);
  }
  
}

int
main (int argc, char **argv)
{ 
	register SVCXPRT *transp;

	pmap_unset (PLACESPROG, PLACES_VERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, PLACESPROG, PLACES_VERS, placesprog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (PLACESPROG, PLACES_VERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, PLACESPROG, PLACES_VERS, placesprog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (PLACESPROG, PLACES_VERS, tcp).");
		exit(1);
	}
	
	TrieCreate(&root);
	readFile();
	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}

