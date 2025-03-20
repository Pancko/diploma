#include "cf_analyzer_session.h"
#include <QThread>

CF_Session::CF_Session(QObject *parent)
    : QObject{parent}
{}
CF_Session::CF_Session()
{}

CF_Session::~CF_Session()
{
    stopThreads();
}

void CF_Session::addThread(CF_Grammar *Grammar1, CF_Grammar *Grammar2, int Words_Lenght, int Words_Count, QTextEdit *tE)
{
    CF_Analyzer* analyzer = new CF_Analyzer(Grammar1, Grammar2, Words_Lenght, Words_Count, tE);
    QThread* thread = new QThread;
    analyzer->moveToThread(thread);

    connect(thread, SIGNAL(started()), analyzer, SLOT(process()));
    connect(analyzer, SIGNAL(finished()), thread, SLOT(quit()));
    connect(this, SIGNAL(stopAll()), analyzer, SLOT(stop()));
    connect(analyzer, SIGNAL(finished()), analyzer, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), this, SLOT(deleteLater()));
    thread->start();

    return;
}

void CF_Session::stopThreads()
{
    emit stopAll();
}
