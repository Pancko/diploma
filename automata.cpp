#include "automata.h"

Automata::Automata(QObject *parent): QObject(parent)
{
    initialize_table();
    initialize_detect_table();
    initialize_keyword_begin();
    resultGrammar = new CF_Grammar(this);
}

Automata::~Automata()
{
    detect_table.clear();
    detect_table.squeeze();
    keyword_begin.clear();
    prev_states.clear();
    prev_states.squeeze();
    sigma.clear();
    sigma.squeeze();
    resultGrammar->clear();
}

SymbolicToken Automata::transliterator(const QChar &symbol)
{
    SymbolicToken res;
    res.val = symbol;
    if (symbol.isDigit()){
        res.tokenClass = T_DIGIT;
        return res;
    }
    if(symbol.isLetter()){
        res.tokenClass = T_LETTER;
        return res;
    }
    if(symbol == ' '){
        res.tokenClass = T_SPACE;
        return res;
    }
    if(symbol == '|'){
        res.tokenClass = T_VERTICALBAR;
        return res;
    }
    if(symbol == '='){
        res.tokenClass = T_EQ;
        return res;
    }
    if(symbol == '='){
        res.tokenClass = T_EQ;
        return res;
    }
    if(symbol == '!'){
        res.tokenClass = T_EXCLAMATION;
        return res;
    }
    if(symbol == '<'){
        res.tokenClass = T_LESS;
        return res;
    }
    if(symbol == '>'){
        res.tokenClass = T_MORE;
        return res;
    }
    if(symbol == '/'){
        res.tokenClass = T_SLASH;
        return res;
    }
    if(symbol == '+'){
        res.tokenClass = T_PLUS;
        return res;
    }
    if(symbol == '-'){
        res.tokenClass = T_MINUS;
        return res;
    }
    if(symbol == '*'){
        res.tokenClass = T_STAR;
        return res;
    }
    if(symbol == '('){
        res.tokenClass = T_LEFTROUNDBR;
        return res;
    }
    if(symbol == ')'){
        res.tokenClass = T_RIGHTROUNDBR;
        return res;
    }
    if(symbol == '['){
        res.tokenClass = T_LEFTSQBR;
        return res;
    }
    if(symbol == ']'){
        res.tokenClass = T_RIGHTSQBR;
        return res;
    }
    if(symbol == '{'){
        res.tokenClass = T_EOS;
        return res;
    }
    if(symbol == '}'){
        res.tokenClass = T_END;
        return res;
    }
    res.tokenClass = T_ERROR;
    return res;
}

void Automata::initialize_table()
{

    for (int i = 0; i < state_number; i++)
    {
        for (int j = 0; j < tokens_number; j++)
        {
            table[i][j] = &Automata::Error;
        }
        table[i][T_SPACE] = &Automata::Space;
        table[i][T_END] = &Automata::End;
    }

    table[S_START][T_LETTER] = &Automata::Letter;

    table[S_W][T_EXCLAMATION] = &Automata::Exclamation;
    table[S_W][T_EQ] = &Automata::Eq;

    table[S_EXCLAMATION][T_EQ] = &Automata::Eq;

    table[S_WEQ][T_LETTER] = &Automata::Letter;
    table[S_WEQ][T_LEFTROUNDBR] = &Automata::BlockBracketsStart;

    table[S_WNEQ][T_LETTER] = &Automata::Letter;
    table[S_WNEQ][T_LEFTROUNDBR] = &Automata::BlockBracketsStart;

    table[S_PAL][T_LESS] = &Automata::KeywordStart;
    table[S_PAL][T_MINUS] = &Automata::KeywordStart;
    table[S_PAL][T_END] = &Automata::Palindrome;

    table[S_KEYWORD][T_LESS] = &Automata::KeywordStart;
    table[S_KEYWORD][T_MINUS] = &Automata::KeywordStart;
    table[S_KEYWORD][T_PLUS] = &Automata::KeywordStart;
    table[S_KEYWORD][T_STAR] = &Automata::KeywordStart;
    table[S_KEYWORD][T_LETTER] = &Automata::Keyword;
    table[S_KEYWORD][T_MORE] = &Automata::Keyword;
    table[S_KEYWORD][T_DIGIT] = &Automata::Keyword;
    table[S_KEYWORD][T_SLASH] = &Automata::Keyword;

    table[S_BLOCK][T_LETTER] = &Automata::BlockLetter;
    table[S_BLOCK][T_LESS] = &Automata::KeywordStart;
    table[S_BLOCK][T_DIGIT] = &Automata::BlockDigit;
    table[S_BLOCK][T_PLUS] = &Automata::KeywordStart;
    table[S_BLOCK][T_STAR] = &Automata::KeywordStart;
    table[S_BLOCK][T_LEFTROUNDBR] = &Automata::BlockBracketsStart;
    table[S_BLOCK][T_RIGHTROUNDBR] = &Automata::BlockBracketsEnd;
    table[S_BLOCK][T_EOS] = &Automata::BlockBracketsEnd; ////////////
    table[S_BLOCK][T_END] = &Automata::BlockEnd;

    for (int j = 0; j < tokens_number; j++)
    {
        table[S_BLOCKBRACKETS][j] = &Automata::BlockBracketsFollow;
    }
    table[S_BLOCKBRACKETS][T_RIGHTROUNDBR] = &Automata::BlockBracketsEnd;

    table[S_BLOCKBRWAITINGPOW][T_LESS] = &Automata::KeywordStart;
    table[S_BLOCKBRWAITINGPOW][T_PLUS] = &Automata::KeywordStart;
    table[S_BLOCKBRWAITINGPOW][T_STAR] = &Automata::KeywordStart;
    table[S_BLOCKBRWAITINGPOW][T_DIGIT] = &Automata::BlockDigit;
}

void Automata::initialize_keyword_begin()
{
    keyword_begin.insert('+', 1);
    keyword_begin.insert('*', 1);
    keyword_begin.insert('<', 2);
    keyword_begin.insert('-', 7);
}

void Automata::initialize_sigma(const QStringList &S)
{
    sigma = S;
}

void Automata::initialize_detect_table()
{
    detect_table.push_back(std::tuple<char, int, function_pointer>('<', 0, &Automata::KeywordNext));
    detect_table.push_back(std::tuple<char, int, function_pointer>('/', 3, &Automata::KeywordNext));
    detect_table.push_back(std::tuple<char, int, function_pointer>('s', 0, &Automata::KeywordNext));
    detect_table.push_back(std::tuple<char, int, function_pointer>('u', 0, &Automata::KeywordNext));
    detect_table.push_back(std::tuple<char, int, function_pointer>('p', 0, &Automata::KeywordNext));
    detect_table.push_back(std::tuple<char, int, function_pointer>('>', 0, &Automata::KeywordFound));
    detect_table.push_back(std::tuple<char, int, function_pointer>('1', 0, &Automata::KeywordFound));
}

CF_Grammar* Automata::parse(const QString& lang)
{
    language = lang;
    resultGrammar->clear();

    prev_states.clear();
    prev_states.squeeze();

    block_rules.clear();

    keyword.clear();
    keyword.squeeze();

    blocks.clear();
    blocks.squeeze();
    sLetter l;
    l.isPointer = 1;
    blocks.push_back(l);

    current_block = &blocks;

    state = 0;
    keyw_detection = 0;
    error_state = 0;
    error_symbolicTokenClass = 0;
    non_terminals = 0;
    brackets = 0;

    QChar symbol;
    prev_symbol = symbol;

    pos = lang.indexOf("L = {w ∈ ∑<sup>*</sup> : ");
    if (pos == -1) throw "Неправильный язык";
    pos += 25;
    state = S_START;
    prev_states.push_back(state);
    while (state != S_END)
    {
        if (prev_states.last() != state)
            prev_states.push_back(state);
        prev_symbol = symbol;
        symbol = lang[pos];
        token = transliterator(symbol);

        // qDebug() << "State1: " << debugState(state) << ", symbol = " << symbol << "token = " << debugToken(token.tokenClass);

        error_state = state;
        error_symbolicTokenClass = token.tokenClass;
        state = (this->*table[state][token.tokenClass])();

        // qDebug() << "State2: " << debugState(state);

        pos++;
    }

    return resultGrammar;
}

void Automata::grammar_add_any(const QString& left_part, const QStringList *allowed_sigma)
{
    Rule new_rule;
    new_rule.left_part = left_part;
    for(const QString &str : *allowed_sigma)
    {
        new_rule.right_part.push_back(str);
        new_rule.right_part.push_back(new_rule.left_part);
        resultGrammar->AddRule(new_rule);
        new_rule.right_part.clear();
    }
    new_rule.right_part.push_back("[EPS]");
    resultGrammar->AddRule(new_rule);
}

void Automata::grammar_add_alpha_any(const QString &left_part, const QStringList *allowed_sigma, bool empty_rule)
{
    Rule new_rule;
    new_rule.left_part = left_part;
    for(const QString &str : *allowed_sigma)
    {
        new_rule.right_part.push_back(str);
        new_rule.right_part.push_back("[ANY]");
        resultGrammar->AddRule(new_rule);
        new_rule.right_part.clear();
    }
    if (empty_rule)
    {
        new_rule.right_part.push_back("[EPS]");
        resultGrammar->AddRule(new_rule);
    }
    if(resultGrammar->ContainsRuleWithNT("[ANY]")) return;
    grammar_add_any("[ANY]", &sigma);
}

void Automata::grammar_add_any_plus(const QString& left_part, const QStringList *allowed_sigma)
{
    Rule new_rule;
    QString new_left_part = left_part;
    new_left_part.insert(left_part.size() - 1, "'");
    new_rule.left_part = left_part;
    for(const QString &str : *allowed_sigma)
    {
        new_rule.right_part.push_back(str);
        if(left_part == "[ANYPLUS]")
            new_rule.right_part.push_back("[ANY]");
        else
            new_rule.right_part.push_back(new_left_part);
        resultGrammar->AddRule(new_rule);
        new_rule.right_part.clear();
    }
    if(left_part == "[ANYPLUS]"){
        if(resultGrammar->ContainsRuleWithNT("[ANY]")) return;
        grammar_add_any("[ANY]", &sigma);
    }
    else{
        new_rule.left_part = new_left_part;
        for(const QString &str : *allowed_sigma)
        {
            new_rule.right_part.push_back(str);
            new_rule.right_part.push_back(new_rule.left_part);
            resultGrammar->AddRule(new_rule);
            new_rule.right_part.clear();
        }
        new_rule.right_part.push_back("[EPS]");
        resultGrammar->AddRule(new_rule);
    }
}

void Automata::grammar_add_any_int(const QString &left_part, const QStringList *allowed_sigma, int val_, const QStringList *disallowed_sigma)
{
    QStringList new_sigma = sigma;
    QString alpha = left_part;//"[" + QString::number(non_terminals) + "_ALPHA]";
    alpha.insert(alpha.indexOf(']'), "_ALPHA");
    Rule new_rule;
    new_rule.left_part = left_part;
    new_rule.right_part.push_back(alpha);
    resultGrammar->AddRule(new_rule);
    new_rule.right_part.clear();

    for(const QString &str : *disallowed_sigma)
        new_sigma.removeAll(str);

    for(const QString &str : *allowed_sigma)
    {
        for(int j = 0; j < val_; j++){
            new_rule.right_part.push_back(str);
            new_rule.right_part.push_back(alpha);
            resultGrammar->AddRule(new_rule);
            new_rule.right_part.removeAll(alpha);
        }
    }
    grammar_add_alpha_any(alpha, &new_sigma);
}

void Automata::grammar_add_int(const QString &left_part, const QStringList *allowed_sigma, int val_)
{
    Rule new_rule;
    new_rule.left_part = left_part;

    for(const QString &str : *allowed_sigma)
    {
        for(int j = 0; j < val_; j++){
            new_rule.right_part.push_back(str);
        }
        resultGrammar->AddRule(new_rule);
        new_rule.right_part.clear();
    }
}

void Automata::grammar_add_comp(const QString &left_part, const sLetter &l)
{
    if(!l.isBrackets) return;
    int comp = 0;
    QStringList variants {"+", "*"};
    QString comp_s = left_part;
    QString comp_s0 = left_part;
    QString comp_s1 = left_part;
    QString comp_s2 = left_part;
    QStringList comp_sigma = sigma;
    QVector<sLetter> block = l.brackets;
    QVector<sLetter>::iterator iter_l;
    QVector<sLetter>::iterator temp_iter;
    Rule new_rule;

    if(l.chPow == '*')
        comp_s0.insert(comp_s0.indexOf(']'), '1');
    else
        comp_s0.insert(comp_s0.indexOf(']'), '0');
    new_rule.left_part = comp_s0;
    new_rule.right_part.push_back("[EPS]");
    resultGrammar->AddRule(new_rule);
    new_rule.right_part.clear();

    comp_s0 = left_part;
    comp_s0.insert(comp_s0.indexOf(']'), '0');

    for(iter_l = block.begin(); iter_l != block.end(); iter_l++)
    {
        if (!iter_l->isPointer)
        {
            if (iter_l->isBrackets)
            {
                comp_s.insert(comp_s.indexOf(']'), QString(QString::number(comp) + "_"));
                grammar_add_comp(comp_s, *iter_l);
            }
            comp_sigma = sigma;
            comp_sigma.removeAll(iter_l->value);
            comp_s = left_part;
            comp_s1 = left_part;
            comp_s2 = left_part;
            comp_s.insert(comp_s.indexOf(']'), QString::number(comp));
            comp_s1.insert(comp_s1.indexOf(']'), QString::number(comp));
            comp_s2.insert(comp_s2.indexOf(']'), QString::number(comp + 1));

            switch(variants.indexOf(iter_l->chPow))
            {
            case -1: //число a^5 w!= a^5
            {
                for(int i = 0; i < iter_l->intPow; i++)
                {
                    comp_s1 = left_part;
                    comp_s2 = left_part;
                    comp_s1.insert(comp_s1.indexOf(']'), QString::number(comp));
                    comp_s2.insert(comp_s2.indexOf(']'), QString::number(comp + 1));

                    new_rule.left_part = comp_s1;
                    new_rule.right_part.push_back(iter_l->value);
                    if(((iter_l - block.begin()) == (block.size() - 1)) && i == iter_l->intPow - 1)
                    {
                        new_rule.right_part.push_back(comp_s0);
                        block_rules.insert(QPair<QString, QString>(comp_s1, iter_l->value), comp_s0);
                    }
                    else
                    {
                        new_rule.right_part.push_back(comp_s2);
                        block_rules.insert(QPair<QString, QString>(comp_s1, iter_l->value), comp_s2);
                    }
                    resultGrammar->AddRule(new_rule);
                    new_rule.right_part.clear();

                    for(const QString &str : comp_sigma)
                    {
                        new_rule.right_part.push_back(str);
                        if(block_rules.contains(QPair<QString, QString>(comp_s0, str))){
                            new_rule.right_part.push_back(block_rules[QPair<QString, QString>(comp_s0, str)]);
                            block_rules.insert(QPair<QString, QString>(comp_s1, str), block_rules[QPair<QString, QString>(comp_s0, str)]);
                        }
                        else{
                            new_rule.right_part.push_back(comp_s0);
                            block_rules.insert(QPair<QString, QString>(comp_s1, str), comp_s0);
                        }
                        resultGrammar->AddRule(new_rule);
                        new_rule.right_part.clear();
                    }
                    new_rule.clear();

                    comp++;
                }
                comp--;
                break;
            }
            case 0: // + w != a+
            {
                new_rule.left_part = comp_s1;
                new_rule.right_part.push_back(iter_l->value);
                new_rule.right_part.push_back(comp_s2);
                resultGrammar->AddRule(new_rule);
                new_rule.right_part.clear();
                block_rules.insert(QPair<QString, QString>(comp_s1, iter_l->value), comp_s2);

                for(const QString &str : comp_sigma)
                {
                    new_rule.right_part.push_back(str);
                    if(block_rules.contains(QPair<QString, QString>(comp_s0, str))){
                        new_rule.right_part.push_back(block_rules[QPair<QString, QString>(comp_s0, str)]);
                        block_rules.insert(QPair<QString, QString>(comp_s1, str), block_rules[QPair<QString, QString>(comp_s0, str)]);
                    }
                    else{
                        new_rule.right_part.push_back(comp_s0);
                        block_rules.insert(QPair<QString, QString>(comp_s1, str), comp_s0);
                    }
                    resultGrammar->AddRule(new_rule);
                    new_rule.right_part.clear();
                }

                comp++;

                comp_s1 = left_part;
                comp_s2 = left_part;
                comp_s1.insert(comp_s1.indexOf(']'), QString::number(comp));
                comp_s2.insert(comp_s2.indexOf(']'), QString::number(comp + 1));

                new_rule.left_part = comp_s1;
                new_rule.right_part.push_back(iter_l->value);
                new_rule.right_part.push_back(comp_s1);
                resultGrammar->AddRule(new_rule);
                new_rule.right_part.clear();
                block_rules.insert(QPair<QString, QString>(comp_s1, iter_l->value), comp_s1);

                if((iter_l - block.begin()) != (block.size() - 1))
                {
                    temp_iter = iter_l + 1;
                    QString temp_str = find_symbol(temp_iter)->value;
                    new_rule.right_part.push_back(temp_str);
                    new_rule.right_part.push_back(comp_s2);
                    resultGrammar->AddRule(new_rule);
                    new_rule.right_part.clear();
                    block_rules.insert(QPair<QString, QString>(comp_s1, temp_str), comp_s2);
                    comp_sigma.removeAll(temp_str);
                }

                for(const QString &str : comp_sigma)
                {
                    new_rule.right_part.push_back(str);
                    if(block_rules.contains(QPair<QString, QString>(comp_s0, str))){
                        new_rule.right_part.push_back(block_rules[QPair<QString, QString>(comp_s0, str)]);
                        block_rules.insert(QPair<QString, QString>(comp_s1, str), block_rules[QPair<QString, QString>(comp_s0, str)]);
                    }
                    else{
                        new_rule.right_part.push_back(comp_s0);
                        block_rules.insert(QPair<QString, QString>(comp_s1, str), comp_s0);
                    }
                    resultGrammar->AddRule(new_rule);
                    new_rule.right_part.clear();
                }
                new_rule.clear();
                break;
            }
            case 1: // * a^* w != a^*
            {
                new_rule.left_part = comp_s1;
                new_rule.right_part.push_back(iter_l->value);
                new_rule.right_part.push_back(comp_s1);
                resultGrammar->AddRule(new_rule);
                new_rule.right_part.clear();
                block_rules.insert(QPair<QString, QString>(comp_s1, iter_l->value), comp_s1);

                if((iter_l - block.begin()) != (block.size() - 1))
                {
                    temp_iter = iter_l + 1;
                    QString temp_str = find_symbol(temp_iter)->value;
                    new_rule.right_part.push_back(temp_str);
                    new_rule.right_part.push_back(comp_s2);
                    resultGrammar->AddRule(new_rule);
                    new_rule.right_part.clear();
                    block_rules.insert(QPair<QString, QString>(comp_s1, temp_str), comp_s2);
                    comp_sigma.removeAll(temp_str);
                }

                for(const QString &str : comp_sigma)
                {
                    new_rule.right_part.push_back(str);
                    if(block_rules.contains(QPair<QString, QString>(comp_s0, str))){
                        new_rule.right_part.push_back(block_rules[QPair<QString, QString>(comp_s0, str)]);
                        block_rules.insert(QPair<QString, QString>(comp_s1, str), block_rules[QPair<QString, QString>(comp_s0, str)]);
                    }
                    else{
                        new_rule.right_part.push_back(comp_s0);
                        block_rules.insert(QPair<QString, QString>(comp_s1, str), comp_s0);
                    }
                    resultGrammar->AddRule(new_rule);
                    new_rule.right_part.clear();
                }
                new_rule.clear();
                break;
            }
            }

            comp++;
        }
    }
}

QVector<sLetter>::iterator Automata::find_symbol(const QVector<sLetter>::iterator &current_symbol, int next, bool non_eps)
{
    QVector<sLetter> block = *current_block;
    QVector<sLetter>::iterator temp_l = current_symbol;
    while (temp_l->isBrackets){
        block = temp_l->brackets;
        temp_l = block.begin() + 1;
    }
    QVector<sLetter>::iterator curr = temp_l;
    if (next == 0) // Вернуть этот же символ (самый первый в скобках)
    {
        return curr;
    }
    for (int i = 0; i < next; i++)
    {
        while ((temp_l - block.begin()) == (block.size() - 1)) // последний элемент в блоке (выход из скобок)
        {
            if (temp_l->parent == nullptr) return curr;
            block = *temp_l->parent;
            temp_l = block.begin() + temp_l->pos;
        }

        temp_l += 1;

        while (temp_l->isBrackets) // a(b(ca))a вход в скобки
        {
            if (temp_l->parent == nullptr)
                block = blocks;
            else
                block = *temp_l->parent;
            temp_l = temp_l->brackets.begin() + 1;
        }

        // if (non_eps && temp_l->chPow == '*')
        //     i--;
        curr = temp_l;
    }
    return curr;
}

int Automata::KeywordStart()
{
    keyword += (token.val);
    if (keyword_begin.find(token.val) == keyword_begin.end())
        return Error();

    keyw_detection = keyword_begin[token.val];
    return S_KEYWORD;
}

int Automata::Keyword()
{
    if (keyw_detection == 0)
    {
        keyword += (token.val);
        return Error();
    }
    if (token.val == std::get<0>(detect_table[(keyw_detection - 1)]))
    {
        keyword += (token.val);
        return (this->*std::get<2>(detect_table[(keyw_detection - 1)]))();
    }
    keyw_detection = std::get<1>(detect_table[(keyw_detection - 1)]);
    if (keyw_detection == 0)
    {
        keyword += (token.val);
        return Error();
    }
    return Keyword();
}

int Automata::KeywordNext()
{
    keyw_detection++;
    return S_KEYWORD;
}

int Automata::KeywordFound()
{
    qDebug() << "keyword found = " << keyword;
    int st;
    while(true)
    {
        if ((st = prev_states.last()) != S_KEYWORD)
            break;
        prev_states.pop_back();
    }

    if (keyword.contains("</sup>"))
    {
        current_block->last().addPow(keyword);
        keyword.clear();
    }

    return st;
}

int Automata::Error()
{
    qDebug() << "error found";
    return S_END;
}

int Automata::Letter()
{
    if(state == S_START && token.val == 'w')
        return S_W;

    if(state == S_WEQ || state == S_WNEQ)
    {
        if (token.val == 'w')
            return S_PAL;
        if (sigma.contains(token.val))
            return BlockLetter();
    }

    return Error();
}

int Automata::Space()
{
    return state;
}

int Automata::Exclamation()
{
    if(token.val == '!')
        return S_EXCLAMATION;

    return Error();
}

int Automata::Eq()
{
    if (state == S_W)
        return S_WEQ;

    else if (state == S_EXCLAMATION)
        return S_WNEQ;

    return Error();
}

int Automata::Palindrome()
{
    if(prev_states.contains(S_WEQ)){
        for(QString& symbol : sigma)
        {
            resultGrammar->AddRule(Rule("S", QVector<QString>({symbol})));
            resultGrammar->AddRule(Rule("S", QVector<QString>({symbol, "S", symbol})));
        }
        resultGrammar->AddRule(Rule("S", QVector<QString>({"[EPS]"})));
    }
    if(prev_states.contains(S_WNEQ)){
        if (sigma.size() < 2) return Error();
        for(int i = 0; i < sigma.size(); i++)
        {
            resultGrammar->AddRule(Rule("S", QVector<QString>({sigma[i]})));
            resultGrammar->AddRule(Rule("S", QVector<QString>({sigma[i], "[S']", sigma[i]})));
            resultGrammar->AddRule(Rule("[S']", QVector<QString>({sigma[i], "[S']", sigma[i]})));
            for (int j = i + 1; j < sigma.size(); j++)
            {
                resultGrammar->AddRule(Rule("S", QVector<QString>({sigma[i], "S", sigma[j]})));
                resultGrammar->AddRule(Rule("S", QVector<QString>({sigma[j], "S", sigma[i]})));

                resultGrammar->AddRule(Rule("[S']", QVector<QString>({sigma[i], "S", sigma[j]})));
                resultGrammar->AddRule(Rule("[S']", QVector<QString>({sigma[j], "S", sigma[i]})));
            }
        }
    }
    resultGrammar->AnalyzeNonTerminals();
    return S_END;
    return Error();
}

int Automata::BlockLetter()
{
    sLetter l;
    l.value = token.val;
    if(current_block->size() > 0){
        l.parent = current_block->first().parent;
        l.pos = current_block->first().pos;
    }
    current_block->push_back(l);

    return S_BLOCK;
}

int Automata::BlockDigit()
{
    keyword += token.val;
    return state;
}

int Automata::BlockEnd()
{
    current_block = &blocks;
    return BlockAnalyze();
}

int Automata::BlockAnalyze()
{
    Rule new_rule;
    int br_block = 0;
    int comp = 0;
    QString br_non_terminal;
    QString left_part;
    QString parent_nt;
    QString comp_s;
    QString temp;
    QString debugMsg;

    QStringList variants {"+", "*"};
    QStringList literal;
    QStringList allowed_sigma = sigma;
    QStringList anyplus_sigma = sigma;

    // bool s_weq = prev_states.contains(S_WEQ);
    bool s_wneq = prev_states.contains(S_WNEQ);

    QVector<sLetter> block = *current_block;

    QVector<sLetter>::iterator l;

    for(l = block.begin(); l != block.end(); l++)
    {
        debugMsg.clear();
        if(!l->isPointer)
        {
            literal = {l->value};
            allowed_sigma = sigma;
            if(!l->isBrackets)
                allowed_sigma.removeAll(l->value);
            else
            {
                sLetter temp_l = l->brackets[1];
                while (temp_l.isBrackets)
                    temp_l = temp_l.brackets[1];
                allowed_sigma.removeAll(temp_l.value);
            }
            parent_nt = QString("[>" + QString::number(non_terminals + 1) + "]");
            comp_s = QString("[>" + QString::number(non_terminals + 1) + "_COMP_]");
            left_part = "[" + QString::number(non_terminals + 1) + "]";
            temp = "[" + QString::number(non_terminals + 1) + "]";

            if(non_terminals == 0)
                resultGrammar->AddRule(Rule("[0]", QVector<QString>({"[>1]"})));

            if(brackets == 0) // делаем так, что каждый блок i имеет схему [>i]->[i][>i+1]
            {
                non_terminals++;
                new_rule.left_part = parent_nt;
                new_rule.right_part.push_back(left_part);

                if ((l - block.begin()) == (block.size() - 1))  // в последнем блоке такого быть конечно не должно
                {
                    if (s_wneq){
                        if (variants.contains(l->chPow))
                            anyplus_sigma.removeAll(l->value);
                        new_rule.right_part.push_back("[ANYPLUS]");
                    }
                }
                else
                    new_rule.right_part.push_back(QString("[>" + QString::number(non_terminals + 1) + "]"));

                resultGrammar->AddRule(new_rule);

                if (s_wneq){
                    new_rule.right_part.clear();
                    temp = comp_s;
                    temp.insert(temp.indexOf(']'), "0");
                    new_rule.right_part.push_back(temp);
                    resultGrammar->AddRule(new_rule);
                }
                // if(s_wneq){
                //     if (l->havePow && l->chPow == '*' && ((l - block.begin()) != (block.size() - 1)))
                //     {
                //         QVector<sLetter>::iterator temp_l = find_symbol(l, 1, 1);
                //         allowed_sigma.removeAll(temp_l->value);
                //         grammar_add_alpha_any(new_rule.left_part, &allowed_sigma);
                //     }
                // }

                new_rule.clear();
            }
            else // Для скобок [i]->[<i>]
            {
                temp = "[" + QString::number(non_terminals) + "]";
                for (int i = 0; i < brackets; i++){
                    temp.insert(temp.indexOf('[') + 1, '<');
                    temp.insert(temp.indexOf(']'), '>');
                }
                br_non_terminal = temp;

                if(s_wneq)
                {
                    comp_s = br_non_terminal;
                    comp_s.insert(comp_s.indexOf(']'), "_COMP_");
                }

                br_block++;
                if ((l - block.begin()) == 1)
                    new_rule.left_part = temp;
                else
                {
                    temp.insert(temp.indexOf('>'), QString("_" + QString::number(br_block) + "."));
                    new_rule.left_part = temp;
                }
                parent_nt = new_rule.left_part;
                temp = br_non_terminal;
                temp.insert(temp.indexOf('>'), QString("_" + QString::number(br_block)));
                left_part = temp;
                new_rule.right_part.push_back(temp);

                if ((l - block.begin()) != (block.size() - 1))  // в последнем блоке такого быть конечно не должно
                {
                    temp = br_non_terminal;
                    temp.insert(temp.indexOf('>'), QString("_" + QString::number(br_block + 1) + "."));
                    new_rule.right_part.push_back(temp);
                }

                resultGrammar->AddRule(new_rule);
                new_rule.clear();
            }

            if(l->isBrackets)
            {
                brackets++;
                temp = "[" + QString::number(non_terminals) + "]";
                for (int i = 0; i < brackets; i++){
                    temp.insert(temp.indexOf('[') + 1, '<');
                    temp.insert(temp.indexOf(']'), '>');
                }
                literal = {temp};
                if (s_wneq)
                {
                    grammar_add_comp(comp_s, *l);
                }

                QStringList temp_list;
                QVector<sLetter>::iterator temp_l = find_symbol(l);
                temp_list.push_back(temp_l->value);
                temp_l = find_symbol(l, 1, 1);
                temp_list.push_back(temp_l->value);

                debugMsg += ")";
                switch(variants.indexOf(l->chPow))
                {
                case -1: //число a^5 w!= a^5: [EPS],a,aa,aaa,aaaa,a^5Sigma+
                {
                    debugMsg += QString("^" + QString::number(l->intPow));
                    // if (s_wneq)
                    //     grammar_add_any_int(parent_nt, &literal, l->intPow, &temp_list);
                    grammar_add_int(left_part, &literal, l->intPow);
                    break;
                }
                case 0: // + w != a+: [EPS], if last: a+E+alpha*: E = sigma\a, alpha = sigma
                {
                    debugMsg += QString("^" + l->chPow);
                    // if (s_wneq)
                    // {
                    //     new_rule.clear();
                    //     new_rule.left_part = parent_nt;
                    //     new_rule.right_part.push_back("[EPS]");
                    //     resultGrammar->AddRule(new_rule);
                    //     new_rule.clear();
                    // }
                        // grammar_add_alpha_any(parent_nt, &literal, 1);////////////////[>1] -> [<1>][ANY] - bad
                    grammar_add_any_plus(left_part, &literal);
                    break;
                }
                case 1: // * a^* w != a^*: aplha^+:alpha = sigma\a
                {
                    debugMsg += QString("^" + l->chPow);
                    // if (s_wneq)
                    //     grammar_add_alpha_any(parent_nt, &literal);
                    grammar_add_any(left_part, &literal);
                    break;
                }
                }
                current_block = &l->brackets;
                qDebug() << "(";
                BlockAnalyze();
                brackets--;
            }
            else
            {
                debugMsg += l->value;
                switch(variants.indexOf(l->chPow))
                {
                case -1: //число a^5 w!= a^5: [EPS],a,aa,aaa,aaaa,a^5Sigma+
                {
                    debugMsg += QString("^" + QString::number(l->intPow));
                    // if (s_wneq)
                    //     grammar_add_any_int(parent_nt, &literal, l->intPow, &literal);
                    grammar_add_int(left_part, &literal, l->intPow);
                    break;
                }
                case 0: // + w != a+: [EPS], if last: a+E+alpha*: E = sigma\a, alpha = sigma
                {
                    debugMsg += QString("^" + l->chPow);
                    // if (s_wneq){
                    //     grammar_add_alpha_any(parent_nt, &allowed_sigma, 1);
                    // }
                    grammar_add_any_plus(left_part, &literal);
                    break;
                }
                case 1: // * a^* w != a^*: aplha^+:alpha = sigma\a
                {
                    debugMsg += QString("^" + l->chPow);
                    // if (s_wneq)
                    //     grammar_add_alpha_any(parent_nt, &allowed_sigma);
                    grammar_add_any(left_part, &literal);
                    break;
                }
                }
            }

            qDebug() << debugMsg;
        }
    }
    if (block.first().parent != nullptr)
        current_block = block.first().parent;
    if (resultGrammar->GetNonTerminals().contains("[ANYPLUS]") && !resultGrammar->ContainsRuleWithNT("[ANYPLUS]"))
    {
        if (variants.contains(block.last().chPow))
        {
        }
        grammar_add_any_plus("[ANYPLUS]", &anyplus_sigma);
    }
    if(token.tokenClass == T_END)
        return S_END;
    return state;
}

int Automata::BlockBracketsStart()
{
    sLetter l;
    l.isBrackets = 1;
    l.parent = current_block->first().parent;
    // l.pos = current_block->size();

    sLetter temp_l;
    temp_l.isPointer = 1;
    temp_l.parent = current_block;
    temp_l.pos = current_block->size();

    l.brackets.push_back(temp_l);

    current_block->push_back(l);
    current_block = &current_block->last().brackets;
    qDebug() << "DOWN ONE";
    return state;
}

int Automata::BlockBracketsFollow()
{
    switch(token.tokenClass)
    {
    case T_LEFTROUNDBR: return BlockBracketsStart();
    case T_LETTER: return BlockLetter();
    case T_LESS: return KeywordStart();
    default: return S_BLOCKBRACKETS;
    }
}

int Automata::BlockBracketsEnd()
{
    if(current_block->first().parent != nullptr){
        qDebug() << "UP ONE";
        current_block = current_block->first().parent;
    }
    return state;
}

int Automata::End()
{
    return S_END;
}


QString debugState(int i)
{
    switch(i){
    case S_START: return "S_START";
    case S_W: return "S_W";
    case S_EXCLAMATION: return "S_EXCLAMATION";
    case S_WEQ: return "S_WEQ";
    case S_WNEQ: return "S_WNEQ";
    case S_PAL: return "S_PAL";
    case S_KEYWORD: return "S_KEYWORD";
    case S_BLOCK: return "S_BLOCK";
    case S_BLOCKBRACKETS: return "S_BLOCKBRACKETS";
    case S_BLOCKBRWAITINGPOW: return "S_BLOCKBRWAITINGPOW";
    case S_BLOCKPARSE: return "S_BLOCKPARSE";
    case S_SKIPPOW: return "S_SKIPPOW";
    case S_END: return "S_END";
    }
    return "err?";
}

QString debugToken(int i)
{
    switch(i){
    case T_SPACE: return "T_SPACE";
    case T_VERTICALBAR: return "T_VERTICALBAR";
    case T_EQ: return "T_EQ";
    case T_EXCLAMATION: return "T_EXCLAMATION";
    case T_LESS: return "T_LESS";
    case T_MORE: return "T_MORE";
    case T_SLASH: return "T_SLASH";
    case T_PLUS: return "T_PLUS";
    case T_MINUS: return "T_MINUS";
    case T_STAR: return "T_STAR";
    case T_LETTER: return "T_LETTER";
    case T_DIGIT: return "T_DIGIT";
    case T_LEFTROUNDBR: return "T_LEFTROUNDBR";
    case T_RIGHTROUNDBR: return "T_RIGHTROUNDBR";
    case T_LEFTSQBR: return "T_LEFTSQBR";
    case T_RIGHTSQBR: return "T_RIGHTSQBR";
    case T_ERROR: return "T_ERROR";
    case T_EOS: return "T_EOS";
    case T_END: return "T_END";
    }
    return "err?";
}
