/*triedriver.c*/
/*
 * To Compile : gcc -o trie trie.c test/triedriver.c (-DDEBUG)
 * To run: ./trie
 *
 * Code found and modified from:
 * http://simplestcodings.blogspot.com/2012/11/trie-implementation-in-c.html
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "../trie.h"

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
//:: This function parses the line that is read and stores data in  
//:: the "place" struct.
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
 
int main(int argc, char *argv[])
{
    char merge[255];
    int i = 0;
    trieNode_t *result;
    TrieCreate(&root);
    readFile();

    strcpy(merge, argv[2]);
    strcat(merge, argv[1]);
    for (i = 0; merge[i]; i++)
    {
      merge[i] = tolower(merge[i]);
    }

    /* Exact match with trie entry, should return value associated with "seattle" */
    result = TrieSearchPartial(root->children, merge);
    if (result)
        printf("Found \"%s\", lat:%f lon:%f, %s\n", merge, result->latitude, result->longitude, result->entry);
    else
        printf("%s not found\n", merge);
 
    /*Destroy the trie*/
    TrieDestroy(root);
}
