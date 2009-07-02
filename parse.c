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

//recursively free child tokens
static void t_free(Token *t) {
}

static Token* t_rewind(Token *t) {
  while(t->prev) {
    t = t->prev;
  }
  return t;
}

static void t_tab(int level) {
  while(level--) {
    printf("\t");
  }
}

static char* op_pp(enum operator_type op) {
  switch(op) {
    case ADD: return "add";
    case SUBTRACT: return "subtract";
    case MULTIPLY: return "multiply";
    case DIVIDE: return "divide";
    default: return "unknown";
  }
}

static void t_pp(Token *t, int level) {
  if(t != NULL) {
    t = t_rewind(t);
    do {
      switch(t->type) {
        case ATOM:
          t_tab(level);
          printf("atom: %f\n", t->n_value);
          break;
        case OPERATOR:
          t_tab(level);
          printf("%s\n", op_pp(t->o_type));
          break;
        case LIST:
          t_tab(level);
          printf("list\n");
          t_pp(t->head, level+1);
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
    if(input[i] == '(') {
      int p_count = 1;
      char tmp;
      m_start = &input[i+1];
      m_count = 0;
      while((tmp = input[i+(++m_count)]) != '\0' && p_count != 0) {
        if(tmp == '(') {
          p_count++;
        } else if(tmp == ')') {
          p_count--;
        }
      }
      if(p_count) {
        fprintf(stderr, "unmatched parens\n");
        exit(1);
      } else {
        /*printf("all parens match\n");*/
        strncpy(buf, m_start, m_count-1);
        buf[m_count-2] = '\0';
        Token *t2 = t_mk();
        t2->type = LIST;
        t2->head = tokenize(buf);
        last_token = t_append(last_token, t2);
      }
      i += m_count;
    } else if(isdigit(input[i])) {
      m_start = &input[i];
      m_count = 1;
      while(isdigit(m_start[m_count]) || m_start[m_count] == '.'){
        m_count++;
      }
      Token *t2 = t_mk();
      t2->type = ATOM;
      t2->a_type = NUMBER;
      strncpy(buf, m_start, m_count);
      buf[m_count] = '\0';
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

  return last_token;
}

void eval(char *input) {
  t_pp(tokenize(input), 0);
}
