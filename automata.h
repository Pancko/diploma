#ifndef AUTOMATA_H
#define AUTOMATA_H

#include "cf_grammar.h"
class Automata;
typedef int (Automata::*function_pointer)();

enum Tokens{
    T_SPACE,
    T_VERTICALBAR,
    T_EQ,
    T_EXCLAMATION,
    T_LESS,
    T_MORE,
    T_SLASH,
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_LETTER,
    T_DIGIT,
    T_LEFTROUNDBR,
    T_RIGHTROUNDBR,
    T_LEFTSQBR,
    T_RIGHTSQBR,
    T_ERROR,
    T_END
};
enum States{
    S_START,
    S_W,
    S_EXCLAMATION,
    S_WEQ,
    S_WNEQ,
    S_POL,
    S_POL2,
    S_END};

struct SymbolicToken
{
    Tokens tokenClass;
    QString val;
};

class Automata
{
public:

    Automata();

    SymbolicToken transliterator(const QChar& symbol);
    void initialize_table();
    CF_Grammar parse(const QString& lang);

private:

    const static int state_number = S_END; // Число состояний (без S_END)
    const static int tokens_number = T_END + 1; // Число возможных токенов
    function_pointer table[state_number][tokens_number];

    SymbolicToken token;      // Токены для транслитерации

    CF_Grammar resultGrammar;
    QChar prev_symbol;
    int state;
    int error_state;
    int error_symbolicTokenClass;

    // Функции автомата:
    int Error();
    int Letter();
    int Space();
    int Exclamation();
    int Eq();
    int Polynome();

    int End();

};





#endif // AUTOMATA_H
