#include "cf_analyzer.h"

void CF_Analyzer::setLocals(CF_Grammar* Grammar1, CF_Grammar* Grammar2, int Words_Lenght, int Words_Count, QTextEdit* tE)
{
    grammar1 = Grammar1;
    grammar2 = Grammar2;
    wordLength = Words_Lenght;
    wordCount = Words_Count;
    outputTextEdit = tE;
}

CF_Analyzer::CF_Analyzer()
{
    grammar1 = NULL;
    grammar2 = NULL;
    wordLength = 0;
    wordCount = 0;
    outputTextEdit = NULL;
}

CF_Analyzer::CF_Analyzer(CF_Grammar *Grammar1, CF_Grammar *Grammar2, int Words_Lenght, int Words_Count, QTextEdit *tE)
{
    grammar1 = Grammar1;
    grammar2 = Grammar2;
    wordLength = Words_Lenght;
    wordCount = Words_Count;
    outputTextEdit = tE;
}

void CF_Analyzer::process()
{
    if(grammar1 == NULL || grammar2 == NULL)
    {
        emit finished();
        return;
    }
    output = EquivalenceTest(*grammar1, *grammar2, wordLength, wordCount);
    writeOutput();
    emit finished();
}

void CF_Analyzer::stop()
{
    grammar1->~CF_Grammar();
    grammar2->~CF_Grammar();
    emit finished();
}

void CF_Analyzer::writeOutput()
{
    outputTextEdit->append(output);
    outputTextEdit->append("=========================\n");
}
