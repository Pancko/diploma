#include "automata.h"

Automata::Automata() {
    initialize_table();
    initialize_detect_table();
    initialize_keyword_begin();
    resultGrammar = CF_Grammar();
}

Automata::~Automata()
{
    delete[] &table;
    detect_table.clear();
    detect_table.squeeze();
    keyword_begin.clear();
    prev_states.clear();
    prev_states.squeeze();
    sigma.clear();
    sigma.squeeze();
    resultGrammar.clear();
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

    table[S_WNEQ][T_LETTER] = &Automata::Letter;

    table[S_PAL][T_LESS] = &Automata::KeywordStart;
    table[S_PAL][T_MINUS] = &Automata::KeywordStart;
    table[S_PAL][T_END] = &Automata::Palindrome;

    // table[S_KEYWORD][T_LESS] = &Automata::KeywordStart;
    // table[S_KEYWORD][T_MINUS] = &Automata::KeywordStart;
    // table[S_KEYWORD][T_PLUS] = &Automata::KeywordStart;
    // table[S_KEYWORD][T_STAR] = &Automata::KeywordStart;
    table[S_KEYWORD][T_LETTER] = &Automata::Keyword;
    table[S_KEYWORD][T_MORE] = &Automata::Keyword;
    table[S_KEYWORD][T_DIGIT] = &Automata::Keyword;
    table[S_KEYWORD][T_SLASH] = &Automata::Keyword;
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
    detect_table.reserve(15);
    detect_table[0] = std::tuple<char, int, function_pointer>('<', 0, &Automata::KeywordNext);
    detect_table[1] = std::tuple<char, int, function_pointer>('/', 3, &Automata::KeywordNext);
    detect_table[2] = std::tuple<char, int, function_pointer>('s', 0, &Automata::KeywordNext);
    detect_table[3] = std::tuple<char, int, function_pointer>('u', 0, &Automata::KeywordNext);
    detect_table[4] = std::tuple<char, int, function_pointer>('p', 0, &Automata::KeywordNext);
    detect_table[5] = std::tuple<char, int, function_pointer>('>', 0, &Automata::KeywordFound);
    detect_table[6] = std::tuple<char, int, function_pointer>('1', 0, &Automata::KeywordFound);
}

CF_Grammar* Automata::parse(const QString& lang)
{
    resultGrammar.clear();
    prev_states.clear();

    QChar symbol;

    int pos = lang.indexOf("L = {w ∈ ∑<sup>*</sup> : ");
    if (pos == -1) throw "Неправильный язык";
    pos += 25;
    state = S_START;
    prev_states.push_back(state);
    while (state != S_END)
    {
        qDebug() << "state = " << QString::number(state);
        if (prev_states.last() != state)
            prev_states.push_back(state);
        prev_symbol = symbol;
        symbol = lang[pos];
        qDebug() << "symbol = " << symbol;
        token = transliterator(symbol);
        qDebug() << "token = " << QString::number(token.tokenClass);
        error_state = state;
        error_symbolicTokenClass = token.tokenClass;
        state = (this->*table[state][token.tokenClass])();
        pos++;
    }

    return &resultGrammar;
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
    int st;
    while(true)
    {
        if ((st = prev_states.last()) != S_KEYWORD)
            break;
        prev_states.pop_back();
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

    if((state == S_WEQ || state == S_WNEQ) && token.val == 'w')
        return S_PAL;

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
    if (keyword == "<sup>-1</sup>")
    {
        keyword.clear();
        if(prev_states.contains(S_WEQ)){
            for(QString& symbol : sigma)
            {
                resultGrammar.AddRule(Rule("S", QVector<QString>({symbol})));
                resultGrammar.AddRule(Rule("S", QVector<QString>({symbol, "S", symbol})));
            }
            resultGrammar.AddRule(Rule("S", QVector<QString>({"[EPS]"})));
        }
        if(prev_states.contains(S_WNEQ)){
            if (sigma.size() < 2) return Error();
            //resultGrammar.AddRule(Rule("S", QVector<QString>({"[EPS]"}))); // если пустое слово не является полиномом
            for(int i = 0; i < sigma.size(); i++)
            {
                resultGrammar.AddRule(Rule("S", QVector<QString>({sigma[i]})));
                resultGrammar.AddRule(Rule("S", QVector<QString>({sigma[i], "[S']", sigma[i]})));
                resultGrammar.AddRule(Rule("[S']", QVector<QString>({sigma[i], "[S']", sigma[i]})));
                for (int j = i + 1; j < sigma.size(); j++)
                {
                    resultGrammar.AddRule(Rule("S", QVector<QString>({sigma[i], "S", sigma[j]})));
                    resultGrammar.AddRule(Rule("S", QVector<QString>({sigma[j], "S", sigma[i]})));

                    resultGrammar.AddRule(Rule("[S']", QVector<QString>({sigma[i], "S", sigma[j]})));
                    resultGrammar.AddRule(Rule("[S']", QVector<QString>({sigma[j], "S", sigma[i]})));
                }
            }
        }
        resultGrammar.AnalyzeNonTerminals();
        return S_END;
    }
    return Error();
}

int Automata::End()
{
    return S_END;
}
