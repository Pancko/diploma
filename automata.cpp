#include "automata.h"

Automata::Automata() {}

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

    table[S_W][T_EXCLAMATION]   = &Automata::Exclamation;
    table[S_W][T_EQ]            = &Automata::Eq;

    table[S_EXCLAMATION][T_EQ] = &Automata::Eq;

    table[S_WEQ][T_LETTER] = &Automata::Letter;

    table[S_WNEQ][T_LETTER] = &Automata::Letter;

    table[S_POL][T_LESS]    = &Automata::Polynome;
    table[S_POL][T_LETTER]  = &Automata::Polynome;
    table[S_POL][T_MORE]    = &Automata::Polynome;
    table[S_POL][T_MINUS]   = &Automata::Polynome;
    table[S_POL][T_DIGIT]   = &Automata::Polynome;
    table[S_POL][T_SLASH]   = &Automata::Polynome;
}

CF_Grammar Automata::parse(const QString& lang)
{
    resultGrammar.~CF_Grammar();

    QChar symbol;

    int pos = lang.indexOf("L = {w ∈ ∑<sup>*</sup> : ");
    if (pos == -1) throw "Неправильный язык";
    pos += 25;
    state = S_START;
    while (state != S_END)
    {
        prev_symbol = symbol;
        symbol = lang[pos];
        token = transliterator(symbol);
        error_state = state;
        error_symbolicTokenClass = token.tokenClass;
        state = (this->*table[state][token.tokenClass])();
        pos++;
    }

    return resultGrammar;
}

int Automata::Error()
{
    return S_END;
}

int Automata::Letter()
{
    if(state == S_START && token.val == 'w')
        return S_W;

    if((state == S_WEQ || state == S_WNEQ) && token.val == 'w')
        return S_POL;

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

int Automata::Polynome()
{
    // if(prev_symbol == 'w' && token.val == '<')
    //     return S_POL;

    // if(prev_symbol == '<' && (token.val == 's' || token.val == '/'))
    //     return S_POL;

    // if(prev_symbol == 's' && token.val == 'u')
    //     return S_POL;

    // if(prev_symbol == 'u' && token.val == 'p')
    //     return S_POL;

    // if(prev_symbol == 'p' && token.val == '>')
    // {
    //     if (state == S_POL)
    //         return S_POL2;
    //     return S_POL2;
    // }
    // DETECTION_TABLE NEEDED

    return Error();
}

int Automata::End()
{
    return S_END;
}
