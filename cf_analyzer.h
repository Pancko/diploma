#ifndef CF_ANALYZER_H
#define CF_ANALYZER_H

#include <QObject>
#include <QMutex>
#include <QTextEdit>
#include "cf_grammar.h"

class CF_Analyzer : public QObject {
    Q_OBJECT
private:
    CF_Grammar *grammar1;
    CF_Grammar *grammar2;
    int wordLength;
    int wordCount;
public:
    void setLocals(CF_Grammar* Grammar1, CF_Grammar* Grammar2, int Words_Lenght, int Words_Count);
    CF_Analyzer(QObject *parent = 0);
    CF_Analyzer(CF_Grammar* Grammar1, CF_Grammar* Grammar2, int Words_Lenght, int Words_Count);
    ~CF_Analyzer();
public slots:
    void process();
    void stop();
signals:
    void finished();
    void updateText(const QString& text);
};

#endif // CF_ANALYZER_H
