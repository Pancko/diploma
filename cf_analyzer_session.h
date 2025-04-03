#ifndef CF_ANALYZER_SESSION_H
#define CF_ANALYZER_SESSION_H

#include <QObject>
#include <QThread>
#include <QList>
#include "cf_analyzer.h"

class CF_Session : public QObject
{
    Q_OBJECT
public:
    explicit CF_Session(QObject *parent = nullptr);
    ~CF_Session();
    void addThread(CF_Grammar* Grammar1, CF_Grammar* Grammar2, int Words_Length, int Words_Count, QTextEdit* tE);
    void extracted();
    void stopThreads();
    void stopSpinner();

signals:
    void stopAll();
    void stopSpin();

private:
    QList<QThread*> m_threads;
};

#endif // CF_ANALYZER_SESSION_H
