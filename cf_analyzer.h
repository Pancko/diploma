#ifndef CF_ANALYZER_H
#define CF_ANALYZER_H

#include <QObject>
#include <QMutex>
#include <QTextEdit>
#include "cf_grammar.h"

class CF_Analyzer: public QObject {
    Q_OBJECT
private:
    QMutex mutex;
    bool stopped;
    bool isStopped();

    CF_Grammar grammar1;
    CF_Grammar grammar2;
    int wordLength;
    int wordCount;
    QTextEdit* outputTextEdit;
public:
    QString output;
    void setLocals(const CF_Grammar& Grammar1, const CF_Grammar& Grammar2, int Words_Lenght, int Words_Count, QTextEdit* tE);
    CF_Analyzer();
public slots:
    /// This is the method which runs in the thread.
    void compare();
    /// Sets the stop flag.
    void stop();
    void writeOutput();
signals:
    /// A child process exited normally.
    void exited();
    /// A child process crashed (Unix only).
    void signalled(int ospid, int signal);
    /// Something happened to a child (Unix only).
    void stateChanged(int ospid);
};

#endif // CF_ANALYZER_H
