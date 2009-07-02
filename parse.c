#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parse.h"

static Token* t_mk();

static Token* t_mk() {
  return (Token*)malloc(sizeof(Token));
}

static Token* t_append(Token *t1, Token *t2) {
  if(t1 != NULL) {
    t1->next = t2;
    t2->prev = t1;
    return t2;
  } else {
    return t2;
  }
}

static Token* t_rewind(Token *t) {
  while(t->prev) {
    t = t->prev;
  }
  return t;
}

static void t_pp(Token *t) {
  if(t != NULL) {
    t = t_rewind(t);
    do {
      switch(t->type) {
        case ATOM:
          printf("val: %f\n", t->n_value);
          break;
        case OPERATOR:
          printf("operator\n");
          break;
        case LIST:
          printf("list\n");
          break;
      }
    } while ((t = t->next));
  }
}

static Token* tokenize(char *input) {
  int i, m_count;
  char *m_start;
  m_count = 0;
  Token *last_token = NULL;
  char buf[100];
  for(i = 0; i < strlen(input); i++) {
    //numbers
    if(isdigit(input[i])) {
      m_start = &input[i];
      m_count = 1;
      while(isdigit(m_start[m_count]) || m_start[m_count] == '.'){
        m_count++;
      }
      Token *t2 = t_mk();
      t2->type = ATOM;
      t2->a_type = NUMBER;
      strncpy(buf, m_start, m_count);
      buf[m_count+1] = '\0';
      t2->n_value = atof(buf);
      last_token = t_append(last_token, t2);
      i += m_count;
    } else if (input[i] == '+') {
      Token *t2 = t_mk();
      t2->type = OPERATOR;
      t2->o_type = ADD;
      last_token = t_append(last_token, t2);
      i++;
    }
  }

  t_pp(last_token);
  return last_token;
}

void eval(char *input) {
  tokenize(input);
}
