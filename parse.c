#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parse.h"

static Token* t_mk();
static Token* t_append(Token *t1, Token *t2);
static Token* t_rewind(Token *t);
static void t_tab(int level);
static char* op_pp(enum operator_type op);
static void pp(Token *t, int level);
static void t_pp(Token *t, int level);
static Token* tokenize(char *input);
static Token* process(Token *t);

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

static void t_tab(int level) {
  while(level--) {
    printf("\t");
  }
}

static char* op_pp(enum operator_type op) {
  switch(op) {
    case ADD: return "+";
    case SUBTRACT: return "-";
    case MULTIPLY: return "*";
    case DIVIDE: return "/";
    case L_THAN: return "<";
    case G_THAN: return ">";
    case L_THAN_EQ: return "<=";
    case G_THAN_EQ: return ">=";
    case IF: return "if";
    default: return "unknown";
  }
}

static void pp(Token *t, int level) {
  switch(t->type) {
    case ATOM:
      t_tab(level);
      printf("atom: %g\n", t->n_value);
      break;
    case NIL:
      t_tab(level);
      printf("NIL\n");
      break;
    case T:
      t_tab(level);
      printf("T\n");
      break;
    case OPERATOR:
      t_tab(level);
      printf("%s:\n", op_pp(t->o_type));
      break;
    case LIST:
      t_tab(level);
      printf("list:\n");
      t_pp(t->head, level+1);
      break;
  }
}

static void t_pp(Token *t, int level) {
  if(t != NULL) {
    do {
      pp(t, level);
    } while ((t = t->next));
  }
}

static Token* t_append_op(Token *t, enum operator_type o_type) {
  Token *t2 = t_mk();
  t2->type = OPERATOR;
  t2->o_type = o_type;
  return t_append(t, t2);
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
        strncpy(buf, m_start, m_count-1);
        buf[m_count-2] = '\0';
        Token *t2 = t_mk();
        t2->type = LIST;
        t2->head = t_rewind(tokenize(buf));
        last_token = t_append(last_token, t2);
      }
      i += m_count-1;
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
      i += m_count-1;
    } else if (input[i] == '+') {
      last_token = t_append_op(last_token, ADD);
    } else if(input[i] == '-') {
      last_token = t_append_op(last_token, SUBTRACT);
    } else if(input[i] == '*') {
      last_token = t_append_op(last_token, MULTIPLY);
    } else if(input[i] == '/') {
      last_token = t_append_op(last_token, DIVIDE);
    } else if(input[i] == '<') {
      if(input[i+1] == '=') {
        last_token = t_append_op(last_token, L_THAN_EQ);
        i++;
      } else {
        last_token = t_append_op(last_token, L_THAN);
      }
    } else if(input[i] == '>') {
      if(input[i+1] == '=') {
        last_token = t_append_op(last_token, G_THAN_EQ);
        i++;
      } else {
        last_token = t_append_op(last_token, G_THAN);
      }
    } else if(input[i] == 'i' && input[i+1] == 'f') {
      last_token = t_append_op(last_token, IF);
      i++;
    } else if(input[i] == 'n' && input[i+1] == 'i' && input[i+2] == 'l') {
      Token *t2 = t_mk();
      t2->type = NIL;
      last_token = t_append(last_token, t2);
      i += 2;
    }
  }

  return last_token;
}

// recursively compute tokens and operators
static Token* process(Token *t) {
  if(t->type == LIST) {
    Token *head = t->head;
    if(head->type == OPERATOR) {
      enum operator_type o_type = head->o_type;
      if(head->next) {
        head = head->next;
        double res = 0;
        int nil = 0;
        switch(o_type) {
          case ADD:

            do {
              head = process(head);
              if(head->type != ATOM) {
                fprintf(stderr, "unable to resolve list to atom\n");
                exit(1);
              } else {
                res += head->n_value;
              }
            } while((head = head->next));
            t->type = ATOM;
            t->n_value = res;
            break;

          case SUBTRACT:

            head = process(head);
            if(head->type != ATOM) {
              fprintf(stderr, "unable to resolve list to atom\n");
              exit(1);
            } else {
              res = head->n_value;
            }

            while((head = head->next)) {
              head = process(head);
              if(head->type != ATOM) {
                fprintf(stderr, "unable to resolve list to atom\n");
                exit(1);
              } else {
                res -= head->n_value;
              }
            }
            t->type = ATOM;
            t->n_value = res;
            break;

          case MULTIPLY:

            head = process(head);
            if(head->type != ATOM) {
              fprintf(stderr, "unable to resolve list to atom\n");
              exit(1);
            } else {
              res = head->n_value;
            }

            while((head = head->next)) {
              head = process(head);
              if(head->type != ATOM) {
                fprintf(stderr, "unable to resolve list to atom\n");
                exit(1);
              } else {
                res *= head->n_value;
              }
            }
            t->type = ATOM;
            t->n_value = res;
            break;

          case DIVIDE:

            head = process(head);
            if(head->type != ATOM) {
              fprintf(stderr, "unable to resolve list to atom\n");
              exit(1);
            } else {
              res = head->n_value;
            }

            while((head = head->next)) {
              head = process(head);
              if(head->type != ATOM) {
                fprintf(stderr, "unable to resolve list to atom\n");
                exit(1);
              } else {
                if(head->n_value == 0) {
                  fprintf(stderr, "divide by zero\n");
                  exit(1);
                }

                res /= head->n_value;
              }
            }
            t->type = ATOM;
            t->n_value = res;

            break;

          case L_THAN:
            head = process(head);
            if(head->type != ATOM) {
              fprintf(stderr, "unable to resolve list to atom\n");
              exit(1);
            } else {
              res = head->n_value;
            }

            while((head = head->next)) {
              head = process(head);
              if(head->type != ATOM) {
                fprintf(stderr, "unable to resolve list to atom\n");
                exit(1);
              } else {
                if(head->n_value == 0) {
                  fprintf(stderr, "divide by zero\n");
                  exit(1);
                }

                if(res < head->n_value) {
                  nil = 0;
                } else {
                  nil = 1;
                }

                res = head->n_value;
              }
            }
            if(nil) {
              t->type = NIL;
            } else {
              t->type = T;
            }
            break;

          case L_THAN_EQ:
            head = process(head);
            if(head->type != ATOM) {
              fprintf(stderr, "unable to resolve list to atom\n");
              exit(1);
            } else {
              res = head->n_value;
            }

            while((head = head->next)) {
              head = process(head);
              if(head->type != ATOM) {
                fprintf(stderr, "unable to resolve list to atom\n");
                exit(1);
              } else {
                if(head->n_value == 0) {
                  fprintf(stderr, "divide by zero\n");
                  exit(1);
                }

                if(res <= head->n_value) {
                  nil = 0;
                } else {
                  nil = 1;
                }

                res = head->n_value;
              }
            }
            if(nil) {
              t->type = NIL;
            } else {
              t->type = T;
            }
            break;

          case G_THAN:
            head = process(head);
            if(head->type != ATOM) {
              fprintf(stderr, "unable to resolve list to atom\n");
              exit(1);
            } else {
              res = head->n_value;
            }

            while((head = head->next)) {
              head = process(head);
              if(head->type != ATOM) {
                fprintf(stderr, "unable to resolve list to atom\n");
                exit(1);
              } else {
                if(head->n_value == 0) {
                  fprintf(stderr, "divide by zero\n");
                  exit(1);
                }

                if(res > head->n_value) {
                  nil = 0;
                } else {
                  nil = 1;
                }

                res = head->n_value;
              }
            }
            if(nil) {
              t->type = NIL;
            } else {
              t->type = T;
            }
            break;
            
          case G_THAN_EQ:
            head = process(head);
            if(head->type != ATOM) {
              fprintf(stderr, "unable to resolve list to atom\n");
              exit(1);
            } else {
              res = head->n_value;
            }

            while((head = head->next)) {
              head = process(head);
              if(head->type != ATOM) {
                fprintf(stderr, "unable to resolve list to atom\n");
                exit(1);
              } else {
                if(head->n_value == 0) {
                  fprintf(stderr, "divide by zero\n");
                  exit(1);
                }

                if(res >= head->n_value) {
                  nil = 0;
                } else {
                  nil = 1;
                }

                res = head->n_value;
              }
            }
            if(nil) {
              t->type = NIL;
            } else {
              t->type = T;
            }
            break;

          case IF:

            head = process(head);
            if(head->type == NIL) {
              if(head->next->next) {
                t = process(head->next->next);
              } else {
                t->type = NIL;
              }
            } else {
              if(head->next) {
                t = process(head->next);
              } else {
                t->type = NIL;
              }
            }
            break;
        }
      } else {
        fprintf(stderr, "expecting arguments for %s\n", op_pp(head->o_type));
        exit(1);
      }
    } 
  } 

  return t;
}

void eval(char *input) {
  Token *root = tokenize(input);
  printf("parse tree:\n");
  t_pp(root, 0);
  printf("-----------------\nresult:\n");
  pp(process(root), 0);
}
