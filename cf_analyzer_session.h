#ifndef CF_ANALYZER_SESSION_H
#define CF_ANALYZER_SESSION_H

#include <QObject>
#include "cf_analyzer.h"

class CF_Session : public QObject
{
    Q_OBJECT
public:
    CF_Session(QObject *parent);
    CF_Session();
    ~CF_Session();

    //void buildReports();
    void addThread(CF_Grammar *Grammar1, CF_Grammar *Grammar2, int Words_Lenght, int Words_Count, QTextEdit *tE);

private:
    void stopThreads();

signals:
    void stopAll(); //остановка всех потоков
};

#endif // CF_ANALYZER_SESSION_H
