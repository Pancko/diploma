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

    // current_block->clear();
    // current_block->squeeze();

    sub_strs.clear();
    sub_strs.squeeze();

    blocks_stack.clear();
    blocks_stack.squeeze();

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

void Automata::grammar_add_any_plus(const QString& left_part, const QStringList *allowed_sigma)
{
    Rule new_rule;
    QString new_left_part = left_part;
    new_left_part.insert(left_part.size() - 1, "'");
    new_rule.left_part = left_part;
    for(const QString &str : *allowed_sigma)
    {
        new_rule.right_part.push_back(str);
        new_rule.right_part.push_back(new_left_part);
        resultGrammar->AddRule(new_rule);
        new_rule.right_part.clear();
    }
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
    QString br_non_terminal;
    QString left_part;
    QString temp;
    QString debugMsg;

    QStringList variants {"+", "*"};
    QStringList literal;
    // QStringList allowed_sigma = sigma;

    // bool s_weq = prev_states.contains(S_WEQ);
    // bool s_wneq = prev_states.contains(S_WNEQ);

    QVector<sLetter> block = *current_block;

    QVector<sLetter>::iterator l;

    for(l = block.begin(); l != block.end(); l++)
    {
        debugMsg.clear();
        if(!l->isPointer)
        {
            literal = {l->value};
            left_part = "[" + QString::number(non_terminals + 1) + "]";
            temp = "[" + QString::number(non_terminals + 1) + "]";

            if(non_terminals == 0)
            {
                resultGrammar->AddRule(Rule("[0]", QVector<QString>({"[>1]"})));
                // if (s_wneq){
                //     allowed_sigma.removeAll(l.value);
                //     grammar_add_any("[0]", &allowed_sigma);
                // }
            }

            if(brackets == 0) // делаем так, что каждый блок i имеет схему [>i]->[i][>i+1]
            {
                non_terminals++;
                new_rule.left_part = QString("[>" + QString::number(non_terminals) + "]");
                new_rule.right_part.push_back(QString("[" + QString::number(non_terminals) + "]"));

                if ((l - block.begin()) != (block.size() - 1))  // в последнем блоке такого быть конечно не должно
                    new_rule.right_part.push_back(QString("[>" + QString::number(non_terminals + 1) + "]"));

                resultGrammar->AddRule(new_rule);
                new_rule.clear();
            }
            else // Для скобок
            {
                temp = "[" + QString::number(non_terminals) + "]";
                for (int i = 0; i < brackets; i++){
                    temp.insert(temp.indexOf('[') + 1, '<');
                    temp.insert(temp.indexOf(']'), '>');
                }
                br_non_terminal = temp;

                br_block++;
                if ((l - block.begin()) == 1) /////////////////////////////////////////////////////////////////////////////NO
                    new_rule.left_part = temp;
                else
                {
                    temp.insert(temp.indexOf('>'), QString("_" + QString::number(br_block) + "."));
                    new_rule.left_part = temp;
                }
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

                debugMsg += ")";
                switch(variants.indexOf(l->chPow))
                {
                case -1: //число a^5 w!= a^5: [EPS],a,aa,aaa,aaaa,a^5Sigma+
                {
                    debugMsg += QString("^" + QString::number(l->intPow));
                    grammar_add_int(left_part, &literal, l->intPow);
                    break;
                }
                case 0: // + w != a+: [EPS], if last: a+E+alpha*: E = sigma\a, alpha = sigma
                {
                    debugMsg += QString("^" + l->chPow);
                    grammar_add_any_plus(left_part, &literal);
                    break;
                }
                case 1: // * a^* w != a^*: aplha^+:alpha = sigma\a
                {
                    debugMsg += QString("^" + l->chPow);
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
                if (l->havePow)
                {
                    switch(variants.indexOf(l->chPow))
                    {
                    case -1: //число a^5 w!= a^5: [EPS],a,aa,aaa,aaaa,a^5Sigma+
                    {
                        debugMsg += QString("^" + QString::number(l->intPow));
                        grammar_add_int(left_part, &literal, l->intPow);
                        break;
                    }
                    case 0: // + w != a+: [EPS], if last: a+E+alpha*: E = sigma\a, alpha = sigma
                    {
                        debugMsg += QString("^" + l->chPow);
                        grammar_add_any_plus(left_part, &literal);
                        break;
                    }
                    case 1: // * a^* w != a^*: aplha^+:alpha = sigma\a
                    {
                        debugMsg += QString("^" + l->chPow);
                        grammar_add_any(left_part, &literal);
                        break;
                    }
                    }
                }
                else
                {
                    grammar_add_int(left_part, &literal, 1);
                }
            }

            qDebug() << debugMsg;
        }
    }
    if (block.first().parent != nullptr)
        current_block = block.first().parent;
    if(token.tokenClass == T_END)
        return S_END;
    return state;
}

int Automata::BlockBracketsStart()
{
    sLetter l;
    l.isBrackets = 1;
    // l.parent = current_block->first().parent;

    sLetter temp_l;
    temp_l.isPointer = 1;
    temp_l.parent = current_block;

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
