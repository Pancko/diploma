#ifndef AUTOMATA_H
#define AUTOMATA_H

#include "cf_grammar.h"

class Automata
{
public:
    Automata();
    void initialize_table();
    CF_Grammar parse(const QString* lang);
private:
    CF_Grammar resultGrammar;
};

enum States{START, WEQ, WNEQ, POL, END};

#endif // AUTOMATA_H
