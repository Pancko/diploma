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

private:

    int pos = 0; // Текущая позиция лексического анализатора
    QString language;

    const static int state_number = S_END;      // Число состояний (без S_END)
    const static int tokens_number = T_END + 1; // Число возможных токенов
    // Таблицы
    function_pointer table[state_number][tokens_number];
    QVector<std::tuple<char, int, function_pointer>> detect_table;
    QMap<QChar, int> keyword_begin;
    //
    QVector<int> prev_states;                           // Список предыдущих состояний
    QVector<sLetter> blocks;                            // Последовательно идущие блоки
    QVector<sLetter> *current_block;                    // Текущий блок куда записываются подблоки //Подряд идущие блоки одинаковых букв, например вида: a<sup>2</sup>a<sup>*</sup>
    QMap<QPair<QString, QString>, QString> block_rules; // По сути небольшой автомат который мы генерируем для анализа скобок (A, b) -> B
    //
    void grammar_add_any(const QString& left_part, const QStringList *allowed_sigma);               // Добавляет в грамматику правила вида: left_part -> allowed_sigma*
    void grammar_add_alpha_any(const QString& left_part, const QStringList *allowed_sigma, bool empty_rule = false);    // Добавляет в грамматику правила вида: left_part -> allowed_sigma[ANY]
    void grammar_add_any_plus(const QString& left_part, const QStringList *allowed_sigma);          // Добавляет в грамматику правила вида: left_part -> allowed_sigma+
    void grammar_add_any_int(const QString& left_part, const QStringList *allowed_sigma,
                             int val_, const QStringList *disallowed_sigma);                        // Добавляет в грамматику правила вида: left_part -> allowed_sigma^n[ALPHA], n != val_
    void grammar_add_int(const QString& left_part, const QStringList *allowed_sigma, int val_);     // Добавляет в грамматику правила вида: left_part -> allowed_sigma^val_
    void grammar_add_comp(const QString& left_part, const sLetter &l);
    void grammar_add_rule(const QString& left_part, const QString& term, const QString& next_part, const QString& find_in, bool force = false);
    //
    QVector<sLetter>::iterator find_symbol(const QVector<sLetter>::iterator &current_symbol, int next = 0, bool non_eps = false);

    QStringList sigma;

    SymbolicToken token;    // Токены для транслитерации

    CF_Grammar* resultGrammar;
    QChar prev_symbol;
    QString keyword;
    int state;
    int keyw_detection;
    int error_state;
    int error_symbolicTokenClass;
    int non_terminals;
    int brackets; // Отсчёт количества скобок

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
    int BlockEnd();
    int BlockAnalyze();
    int BlockBracketsStart();
    int BlockBracketsFollow();
    int BlockBracketsEnd();

    int End();

};





#endif // AUTOMATA_H
