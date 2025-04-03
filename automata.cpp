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
    // delete[] &table;
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
    table[S_BLOCK][T_END] = &Automata::BlockAnalyze;

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
    resultGrammar->clear();

    prev_states.clear();
    prev_states.squeeze();

    current_block.clear();
    current_block.squeeze();

    sub_strs.clear();
    sub_strs.squeeze();

    blocks_stack.clear();
    blocks_stack.squeeze();

    keyword.clear();
    keyword.squeeze();

    state = 0;
    keyw_detection = 0;
    error_state = 0;
    error_symbolicTokenClass = 0;
    non_terminals = 0;
    brackets = 0;
    str_layer = 0;

    QChar symbol;
    prev_symbol = symbol;

    int pos = lang.indexOf("L = {w ∈ ∑<sup>*</sup> : ");
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

        qDebug() << "State1: " << debugState(state) << ", symbol = " << symbol << "token = " << debugToken(token.tokenClass);

        error_state = state;
        error_symbolicTokenClass = token.tokenClass;
        state = (this->*table[state][token.tokenClass])();

        qDebug() << "State2: " << debugState(state);

        pos++;

        // if(current_block.isEmpty())
        //     qDebug() << "BLOCK EMPTY";
        // else
        //     qDebug() << "BLOCK" << current_block[0].value;
    }

    return resultGrammar;
}

void Automata::grammar_add_any(const QString& left_part, const QStringList *allowed_sigma)
{
    Rule new_rule;
    new_rule.left_part = left_part;
    for(const QString &str : *allowed_sigma)
    {
        new_rule.right_part.push_back(new_rule.left_part);
        new_rule.right_part.push_back(str);
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
        new_rule.right_part.push_back(new_left_part);
        new_rule.right_part.push_back(str);
        resultGrammar->AddRule(new_rule);
        new_rule.right_part.clear();
    }
    new_rule.left_part = new_left_part;
    for(const QString &str : *allowed_sigma)
    {
        new_rule.right_part.push_back(new_rule.left_part);
        new_rule.right_part.push_back(str);
        resultGrammar->AddRule(new_rule);
        new_rule.right_part.clear();
    }
    new_rule.right_part.push_back("[EPS]");
    resultGrammar->AddRule(new_rule);
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
    if (st == S_BLOCK && keyword.contains("</sup>"))
    {
        current_block.last().addPow(keyword);
    }
    if (st == S_BLOCKBRWAITINGPOW && keyword.contains("</sup>")){
        sLetter l;
        QStringList variants {"+", "*"};
        Rule new_rule;
        l.addPow(keyword);
        keyword.clear();
        if (prev_states.contains(S_WEQ))
        {
            QString temp = blocks_stack.top().first;
            new_rule.left_part = temp;
            switch(variants.indexOf(l.chPow))
            {
            case -1: //число
            {
                temp = temp.insert(temp.indexOf(']'), "''");
                for(int j = 0; j < l.intPow; j++)
                    new_rule.right_part.push_back(temp);
                resultGrammar->AddRule(new_rule);
                break;
            }
            case 0: // +
            {
                //qDebug() << "POW + ";
                temp = temp.insert(temp.indexOf(']'), "''");
                new_rule.right_part.push_back(temp);
                temp = blocks_stack.top().first;
                temp = temp.insert(temp.indexOf(']'), "'");
                new_rule.right_part.push_back(temp);
                resultGrammar->AddRule(new_rule);

                new_rule.clear();
                temp = blocks_stack.top().first;
                new_rule.left_part = temp.insert(temp.indexOf(']'), "'");
                temp = blocks_stack.top().first;
                new_rule.right_part.push_back(temp.insert(temp.indexOf(']'), "''"));
                temp = blocks_stack.top().first;
                new_rule.right_part.push_back(temp.insert(temp.indexOf(']'), "'"));
                resultGrammar->AddRule(new_rule);

                new_rule.right_part.clear();
                new_rule.right_part.push_back("[EPS]");
                resultGrammar->AddRule(new_rule);
                break;
            }
            case 1: // *
            {
                //qDebug() << "POW * ";
                new_rule.right_part.push_back(temp.insert(temp.indexOf(']'), "''"));
                temp = blocks_stack.top().first;
                new_rule.right_part.push_back(temp);
                resultGrammar->AddRule(new_rule);

                new_rule.right_part.clear();
                new_rule.right_part.push_back("[EPS]");
                resultGrammar->AddRule(new_rule);
                break;
            }
            }
            new_rule.clear();
            temp = blocks_stack.top().first;
            new_rule.left_part = temp.insert(temp.indexOf(']'), "''");
            resultGrammar->AddRule(new_rule);
            blocks_stack.top().second = resultGrammar->GetRules().indexOf(new_rule); // местонахождение нового порождающего блока (как нулевое правило S обычно)
            st = BlockParse();
        }
    }
    if (keyword.contains("</sup>"))
        keyword.clear();

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
    // if (keyword == "<sup>-1</sup>")
    // {
    //     keyword.clear();
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
            //resultGrammar->AddRule(Rule("S", QVector<QString>({"[EPS]"}))); // если пустое слово не является палиндромом
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
    // }
    return Error();
}

int Automata::BlockLetter()
{
    int st = S_BLOCK;
    if(!current_block.isEmpty())
    {
        if(current_block.last().value != token.val)
            st = BlockAnalyze();
    }
    sLetter l;
    l.value = token.val;
    current_block.push_back(l);
    return st;
}

int Automata::BlockDigit()
{
    keyword += token.val;
    return state;
}

int Automata::BlockAnalyze()
{
    sLetter l;
    QStringList variants {"+", "*"};
    int rule_pos = 0;
    bool s_weq = prev_states.contains(S_WEQ);
    bool s_wneq = prev_states.contains(S_WNEQ);
    QStringList allowed_sigma = sigma;
    if(s_weq || s_wneq)
    {
        if(!blocks_stack.isEmpty())
            rule_pos = blocks_stack.top().second;
        for(int i = 0; i < current_block.size(); i++)
        {
            Rule new_rule;
            l = current_block[i];

            if(non_terminals == 0)
            {
                resultGrammar->AddRule(Rule("[0]", QVector<QString>({"[1]"})));
                if (s_wneq){
                    allowed_sigma.removeAll(l.value);
                    grammar_add_any("[0]", &allowed_sigma);
                }
            }
            else
                resultGrammar->ModRule(rule_pos, QString("[" + QString::number(non_terminals + 1) + "]"));

            non_terminals++;

            if (l.havePow)
            {
                //qDebug() << "int pow = " << QString::number(l.intPow);
                switch(variants.indexOf(l.chPow))
                {
                case -1: //число a^5 w!= a^5: [EPS],a,aa,aaa,aaaa,a^5Sigma+
                {
                    new_rule.left_part = QString("[" + QString::number(non_terminals) + "]");
                    for(int j = 0; j < l.intPow; j++){
                        new_rule.right_part.push_back(l.value);
                        if (s_wneq){
                            if (j == l.intPow - 1){
                                QString new_left_part = new_rule.left_part;
                                new_left_part.insert(new_left_part.size() - 1, "'");
                                new_rule.right_part.push_back(new_left_part);
                                grammar_add_any_plus(new_left_part, &sigma);
                            }
                            resultGrammar->AddRule(new_rule);
                        }
                    }
                    if (!s_wneq)
                        resultGrammar->AddRule(new_rule);
                    else
                    {
                        new_rule.right_part.clear();
                        new_rule.right_part.push_back("[EPS]");
                    }
                    break;
                }
                case 0: // + w != a+: [EPS], if last: a+E+alpha*: E = sigma\a, alpha = sigma
                {
                    //qDebug() << "POW + ";
                    new_rule.left_part = QString("[" + QString::number(non_terminals) + "]");
                    new_rule.right_part.push_back(l.value);
                    new_rule.right_part.push_back(QString("[" + QString::number(non_terminals) + "']"));
                    if(s_wneq && (i == current_block.size() - 1))
                    {
                        new_rule.right_part.push_back(QString("[" + QString::number(non_terminals) + "'']"));
                        new_rule.right_part.push_back(QString("[" + QString::number(non_terminals) + "''']"));
                        QStringList allowed_sigma = sigma;
                        allowed_sigma.removeAll(l.value);
                        grammar_add_any_plus(QString("[" + QString::number(non_terminals) + "'']"), &allowed_sigma);
                        grammar_add_any(QString("[" + QString::number(non_terminals) + "''']"), &sigma);
                    }
                    resultGrammar->AddRule(new_rule);

                    if (s_wneq)
                    {
                        new_rule.right_part.clear();
                        new_rule.right_part.push_back("[EPS]");
                        resultGrammar->AddRule(new_rule);
                    }

                    new_rule.clear();
                    new_rule.left_part = QString("[" + QString::number(non_terminals) + "']");
                    new_rule.right_part.push_back(l.value);
                    new_rule.right_part.push_back(QString("[" + QString::number(non_terminals) + "']"));
                    resultGrammar->AddRule(new_rule);

                    new_rule.right_part.clear();
                    new_rule.right_part.push_back("[EPS]");
                    resultGrammar->AddRule(new_rule);

                    break;
                }
                case 1: // * a^* w != a^*: aplha^+:alpha = sigma\a
                {
                    //qDebug() << "POW * ";
                    new_rule.left_part = QString("[" + QString::number(non_terminals) + "]");
                    new_rule.right_part.push_back(l.value);
                    new_rule.right_part.push_back(QString("[" + QString::number(non_terminals) + "]"));
                    if(s_wneq)
                    {
                        if (i != current_block.size() - 1){
                            resultGrammar->AddRule(new_rule);
                            new_rule.right_part.clear();
                        }
                        new_rule.right_part.push_back(QString("[" + QString::number(non_terminals) + "']"));
                        QStringList allowed_sigma = sigma;
                        allowed_sigma.removeAll(l.value);
                        grammar_add_any_plus(QString("[" + QString::number(non_terminals) + "']"), &allowed_sigma);
                    }
                    resultGrammar->AddRule(new_rule);

                    new_rule.right_part.clear();
                    if(s_wneq && (i == current_block.size() - 1))
                        new_rule.right_part.push_back(QString("[" + QString::number(non_terminals) + "']"));
                    else
                        new_rule.right_part.push_back("[EPS]");
                    resultGrammar->AddRule(new_rule);

                    break;
                }
                }
            }
            else { // single character w=a w!=a: [EPS], a[Sigma]*, E[Sigma]*: E!=a
                new_rule.left_part = QString("[" + QString::number(non_terminals) + "]");
                new_rule.right_part.push_back(QString(l.value));
                if (s_wneq)
                {
                    new_rule.right_part.push_back(QString("[" + QString::number(non_terminals) + "']"));
                    if (i == current_block.size() - 1)
                        grammar_add_any_plus(QString("[" + QString::number(non_terminals) + "']"), &sigma);
                    else
                        grammar_add_any(QString("[" + QString::number(non_terminals) + "']"), &sigma);
                    resultGrammar->AddRule(new_rule);

                    new_rule.right_part.clear();
                    foreach(QString str, sigma)
                    {
                        if (str != l.value)
                        {
                            new_rule.right_part.push_back(str);
                            new_rule.right_part.push_back(QString("[" + QString::number(non_terminals) + "'']"));
                            grammar_add_any(QString("[" + QString::number(non_terminals) + "'']"), &sigma);
                            new_rule.right_part.clear();
                        }
                    }

                    new_rule.right_part.clear();
                    new_rule.right_part.push_back("[EPS]");
                }
                resultGrammar->AddRule(new_rule);
            }
        }
    }
    current_block.clear();
    if(token.tokenClass == T_END)
        return S_END;
    return state;
}

int Automata::BlockBracketsStart()
{
    brackets++;
    int st = 0;
    if(!current_block.isEmpty())
    {
        st = BlockAnalyze();
    }
    if (st == S_END) return S_END;
    if(resultGrammar->GetRules().isEmpty())
        resultGrammar->AddRule(Rule("[0]", QVector<QString>({QString("[Br" + QString::number(non_terminals + 1) + "]")})));
    else{
        if  (str_layer == 0)
            resultGrammar->ModRule(0, QString("[Br" + QString::number(non_terminals + 1) + "]"));
        else{
            resultGrammar->ModRule(blocks_stack[str_layer - 1].second, QString("[Br" + QString::number(non_terminals + 1) + "]"));
        }
    }
    blocks_stack.push(QPair<QString, int>(QString("[Br" + QString::number(non_terminals + 1) + "]"), -1));
    non_terminals++;
    return S_BLOCKBRACKETS;
}

int Automata::BlockBracketsFollow()
{
    if (sub_strs.size() < brackets)
        sub_strs.resize(brackets);
    for (int i = 0; i < brackets; i++)
        sub_strs[i] += token.val;
    if (token.tokenClass == T_LEFTROUNDBR)
        brackets++;
    return S_BLOCKBRACKETS;
}

int Automata::BlockBracketsEnd()
{
    if (token.tokenClass != T_EOS)
        brackets--;
    if(brackets == 0)
    {
        if (state == S_BLOCKBRACKETS)
            return S_BLOCKBRWAITINGPOW;
        if (state == S_BLOCK){
            if (!current_block.isEmpty())
                BlockAnalyze();
            blocks_stack.pop();
            return S_SKIPPOW;
        }
        brackets = 0;
    } else return BlockBracketsFollow();
    return Error();
}

int Automata::BlockParse()
{
    qDebug() << "IN BLOCK PARSER";
    QChar symbol;
    QString sub = sub_strs[str_layer] + '{';
    str_layer++;
    int pos = 0;
    state = S_BLOCK;
    while (state != S_SKIPPOW)
    {
        if (prev_states.last() != state)
            prev_states.push_back(state);
        prev_symbol = symbol;
        symbol = sub[pos];
        token = transliterator(symbol);
        qDebug() << "State1: " << debugState(state) << ", symbol = " << symbol << "token = " << debugToken(token.tokenClass);
        error_state = state;
        error_symbolicTokenClass = token.tokenClass;
        state = (this->*table[state][token.tokenClass])();
        qDebug() << "State2: " << debugState(state);
        pos++;
    }
    str_layer--;
    sub_strs[str_layer].clear();
    qDebug() << "EXITED BLOCK PARSER";
    return S_BLOCK;
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
