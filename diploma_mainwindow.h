#ifndef DIPLOMA_MAINWINDOW_H
#define DIPLOMA_MAINWINDOW_H

#include <QMainWindow>
#include "cf_grammar.h"
#include <QMutex>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class Diploma_MainWindow;
}
QT_END_NAMESPACE

class Diploma_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Diploma_MainWindow(QWidget *parent = nullptr);
    ~Diploma_MainWindow();

private slots:
    void on_Library_pB_clicked();

    void on_GrammarEQ_pB_clicked();

    void on_PremadeTasks_pB_clicked();

    void on_InfiniteTasks_pB_clicked();

    void on_Compare_pB_clicked();

    void on_CF1Load_pB_clicked();

    void on_CF2Load_pB_clicked();

    void on_CF1Save_pB_clicked();

    void on_CF2Save_pB_clicked();

    void save(const QString& text);
    QString load();

    void on_clearOutput_pB_clicked();

    void on_length_lineEdit_textChanged(const QString &arg1);

    void on_wordCount_lineEdit_textChanged(const QString &arg1);

private:
    Ui::Diploma_MainWindow *ui;

    int wordLength;
    int wordCount;
};

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
#endif // DIPLOMA_MAINWINDOW_H
