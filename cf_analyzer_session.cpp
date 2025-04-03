#include "cf_analyzer_session.h"

CF_Session::CF_Session(QObject *parent)
    : QObject{parent}
{}

CF_Session::~CF_Session()
{
    stopThreads();
}

void CF_Session::addThread(CF_Grammar* Grammar1, CF_Grammar* Grammar2, int Words_Length, int Words_Count, QTextEdit* tE) {
    QThread* thread = new QThread();
    CF_Analyzer* analyzer = new CF_Analyzer();

    analyzer->setLocals(Grammar1, Grammar2, Words_Length, Words_Count);
    analyzer->moveToThread(thread);

    // Сигналы и слоты
    connect(thread, &QThread::started, analyzer, &CF_Analyzer::process);
    connect(analyzer, &CF_Analyzer::updateText, analyzer, &CF_Analyzer::stop);
    connect(analyzer, &CF_Analyzer::finished, thread, &QThread::quit);
    connect(analyzer, &CF_Analyzer::finished, analyzer, &CF_Analyzer::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(thread, &QThread::finished, this, &CF_Session::stopThreads);

    // Передача данных в GUI поток
    connect(analyzer, &CF_Analyzer::updateText, tE, [tE](const QString& text) {
        if (tE) tE->append(text); // Проверка на nullptr
    }, Qt::QueuedConnection);

    // Остановка
    connect(this, &CF_Session::stopAll, analyzer, &CF_Analyzer::stop, Qt::QueuedConnection);

    m_threads.append(thread);
    thread->start();
}

void CF_Session::stopThreads() {
    emit stopAll();
    for (QThread* thread : m_threads) {
        if (thread->isRunning()) {
            thread->quit();
            thread->wait(500); // Максимум 500 мс на завершение
        }
    }
    m_threads.clear();
}
