/*************************************************************
* hashtab.c -          hash table for use in symbol table
* Language:            Micro
*
**************************************************************/

#include "hashtab.h"

static unsigned 
hash(const char* s){

  unsigned hashval;

  for (hashval = 0; *s != '\0'; s++)
    hashval = *s + 31*hashval;

  return hashval%HASHSIZE;
}

static char* 
mystrdup(const char* s){

  char* p;

  p = malloc(strlen(s)+1);
  if (p != NULL)
    strcpy(p, s);
  return p;
}

static struct nlist* 
findprior(struct nlist** hashtab, const char* s){

  struct nlist* np;
  struct nlist* np_prior;

  for (np_prior = np = hashtab[hash(s)]; np != NULL; np = np->next){
    if (strcmp(s, np->name) == 0)
      return np_prior;
    np_prior = np;
  }

  return np_prior;
}

struct nlist* 
lookup(struct nlist** hashtab, const char* s){

  struct nlist* np;

  for (np = hashtab[hash(s)]; np != NULL; np = np->next)
    if (strcmp(s, np->name) == 0)
      return np;

  return NULL;
}

// in linked list rooted at hash(name), find nlist* of same hash value, 
// if any, preceding the name to be undefined; then re-link properly
int 
undef(struct nlist** hashtab, const char* name){

  struct nlist* p;
  struct nlist* p_prior;

  p = lookup(hashtab, name);
  if (p == NULL)
    return -1;
  p_prior = findprior(hashtab, name);

  if (!(p_prior == p))
    p_prior->next = p->next;
  else
    hashtab[hash(name)] = NULL;
  free ( (void*) p->name);
  free ( (void*) p->type);
  free ( (void*) p->scope);
  free ( (void*) p->storage);

  return 0;
}

struct nlist* 
install(struct nlist** hashtab, char* name, char* type, 
				char* scope, char* storage){

  struct nlist* np;
  unsigned hashval;
	const char* pH = "placeholder";

  if ( (np = lookup(hashtab, name)) == NULL){
    np = (struct nlist*) malloc(sizeof(struct nlist));
    if (np == NULL || (np->name = mystrdup(name)) == NULL)
      return NULL;
    hashval = hash(name);
    np->next = hashtab[hashval];
    hashtab[hashval] = np;
  }
  else{
		free ( (void*) np->type);
		free ( (void*) np->scope);
		free ( (void*) np->storage);
	}

	if ( NULL == type )
		type = (char*) pH;		
	if ( NULL == scope )
		scope = (char*) pH;	
	if ( NULL == storage )
		storage = (char*) pH;	

  if( (NULL == (np->type = mystrdup(type)) ) || 
			(NULL == (np->scope = mystrdup(scope)) ) || 
			(NULL == (np->storage = mystrdup(storage)) ) )
		return NULL;

	return np;
}

void
printHashTable(struct nlist** hashtab){

	struct nlist* np;
	int i;

	for (i = 0; i< HASHSIZE; i++)
		for (np = hashtab[i]; np!= NULL; np = np->next)
			printf("%s = %s, %s, %s\n", np->name, np->type, np->scope, np->storage); 
}
