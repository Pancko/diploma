#ifndef AUTOMATA_H
#define AUTOMATA_H

#include "cf_grammar.h"
#include "regExPlus.h"
#include <QStack>
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
    T_EOS,
    T_END
};
enum States{
    S_START,
    S_W,
    S_EXCLAMATION,
    S_WEQ,
    S_WNEQ,
    S_PAL,
    S_KEYWORD,
    S_BLOCK,
    S_BLOCKBRACKETS,
    S_BLOCKBRWAITINGPOW,
    S_BLOCKPARSE,
    S_SKIPPOW,
    S_END};

QString debugState(int i);
QString debugToken(int i);

struct SymbolicToken
{
    Tokens tokenClass;
    QChar val;
};

class Automata: public QObject
{
    Q_OBJECT
public:

    explicit Automata(QObject *parent = 0);
    ~Automata();

    SymbolicToken transliterator(const QChar& symbol);
    void initialize_table();
    void initialize_detect_table();
    void initialize_keyword_begin();
    void initialize_sigma(const QStringList& S);
    CF_Grammar* parse(const QString& lang);
    void grammar_add_any(const QString& left_part, const QStringList *allowed_sigma);
    void grammar_add_any_plus(const QString& left_part, const QStringList *allowed_sigma);

private:

    const static int state_number = S_END; // Число состояний (без S_END)
    const static int tokens_number = T_END + 1; // Число возможных токенов
    // Таблицы
    function_pointer table[state_number][tokens_number];
    QVector<std::tuple<char, int, function_pointer>> detect_table;
    QMap<QChar, int> keyword_begin;
    //
    QVector<int> prev_states;
    QVector<sLetter> current_block;
    QStack<QPair<QString, int>> blocks_stack;
    QVector<QString> sub_strs;

    QStringList sigma;

    SymbolicToken token;      // Токены для транслитерации

    CF_Grammar* resultGrammar;
    QChar prev_symbol;
    QString keyword;
    int state;
    int keyw_detection;
    int error_state;
    int error_symbolicTokenClass;
    int non_terminals;
    int brackets;
    int str_layer;

    // Функции автомата:
    int KeywordStart();
    int Keyword();
    int KeywordNext();
    int KeywordFound();

    int Error();
    int Letter();
    int Space();
    int Exclamation();
    int Eq();
    int Palindrome();

    int BlockLetter();
    int BlockDigit();
    int BlockAnalyze();
    int BlockBracketsStart();
    int BlockBracketsFollow();
    int BlockBracketsEnd();
    int BlockParse();

    int End();

};





#endif // AUTOMATA_H
