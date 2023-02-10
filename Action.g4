// Grammar for C++ runtime actions
grammar Action;

import ModuleLex;

@header {
  #include <stack>
}

@parser::members {
  void skipToEndOfObject() {
    std::stack<char> curly_stack;
    int t = _input->LA(1);
    while (t != antlr4::Token::EOF &&
           !(curly_stack.empty() && t == CLOSING_CURLY)) {
      if (t == PRE_ELSE) {
        std::stack<char> if_def_stack;
        consume();
        t = _input->LA(1);

        while (t != antlr4::Token::EOF &&
              !(if_def_stack.empty() && (t == PRE_ENDIF))) {
          if (t == PRE_IF) if_def_stack.push(0);
          else if (t == PRE_ENDIF) if_def_stack.pop();
            consume();
            t = _input->LA(1);
        }
      }
      if (t == OPENING_CURLY) curly_stack.push(0);
      else if (t == CLOSING_CURLY) curly_stack.pop();
      consume();
      t = _input->LA(1);
    }
    if (t != antlr4::Token::EOF) consume();
  }

  void preProcSkipToEnd() {
    std::stack<char> curly_stack;
    int t = _input->LA(1);

    while(t != antlr4::Token::EOF &&
          !(curly_stack.empty() && t == PRE_ENDIF)){

        if(t == PRE_IF) curly_stack.push(0);
        else if(t == PRE_ENDIF) curly_stack.pop();

        consume();
        t = _input->LA(1);
    }
    if(t != antlr4::Token::EOF) consume();
}
}

skip_to_end_of_object: {skipToEndOfObject();};

pre_proc_skip_to_end: {preProcSkipToEnd();};
