#include "cf_analyzer.h"
#include <QThread>

bool CF_Analyzer::isStopped()
{
    bool stopped;
    mutex.lock();
    stopped = this->stopped;
    mutex.unlock();
    return stopped;
}

void CF_Analyzer::setLocals(const CF_Grammar &Grammar1, const CF_Grammar &Grammar2, int Words_Lenght, int Words_Count, QTextEdit* tE)
{
    grammar1 = Grammar1;
    grammar2 = Grammar2;
    wordLength = Words_Lenght;
    wordCount = Words_Count;
    outputTextEdit = tE;
}

CF_Analyzer::CF_Analyzer()
{

}

void CF_Analyzer::compare()
{
    output = EquivalenceTest(grammar1, grammar2, wordLength, wordCount);
    emit exited();
    this->thread()->quit();
}

void CF_Analyzer::stop()
{
    mutex.lock();
    stopped = true;
    mutex.unlock();
}

void CF_Analyzer::writeOutput()
{
    outputTextEdit->append(output);
    outputTextEdit->append("=========================\n");
}
