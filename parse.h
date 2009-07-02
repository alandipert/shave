enum type {
  ATOM,
  OPERATOR,
  NIL,
  T,
  LIST
};

enum operator_type {
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  L_THAN,
  G_THAN,
  L_THAN_EQ,
  G_THAN_EQ,
  IF
};

enum atom_type {
  NUMBER,
  STRING
};

typedef struct Token {
  // for everything
  enum type type;
  struct Token *next;
  struct Token *prev;
  //for atoms
  enum atom_type a_type;
  double n_value;
  char *s_value;
  //for operators
  enum operator_type o_type;
  //for lists
  struct Token *head;
  struct Token *tail;
} Token;

void eval(char *input);
