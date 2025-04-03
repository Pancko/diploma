#include "cf_analyzer.h"

void CF_Analyzer::setLocals(CF_Grammar* Grammar1, CF_Grammar* Grammar2, int Words_Lenght, int Words_Count)
{
    grammar1 = Grammar1;
    grammar2 = Grammar2;
    wordLength = Words_Lenght;
    wordCount = Words_Count;
}

CF_Analyzer::CF_Analyzer(QObject *parent) : QObject(parent)
{
    grammar1 = NULL;
    grammar2 = NULL;
    wordLength = 0;
    wordCount = 0;
}

CF_Analyzer::CF_Analyzer(CF_Grammar *Grammar1, CF_Grammar *Grammar2, int Words_Lenght, int Words_Count)
{
    grammar1 = Grammar1;
    grammar2 = Grammar2;
    wordLength = Words_Lenght;
    wordCount = Words_Count;
}

CF_Analyzer::~CF_Analyzer()
{
}

void CF_Analyzer::process()
{
    if(grammar1 == NULL || grammar2 == NULL)
    {
        emit finished();
        return;
    }
    QString output = EquivalenceTest(grammar1, grammar2, wordLength, wordCount);
    emit updateText(output + "\n=========================\n");
    emit finished();
}

void CF_Analyzer::stop()
{
    emit finished();
}
