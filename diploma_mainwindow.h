#ifndef DIPLOMA_MAINWINDOW_H
#define DIPLOMA_MAINWINDOW_H

#include <QMainWindow>
#include "automata.h"

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
    void save(const QString& text);
    QString load();

    void on_Library_pB_clicked();

    void on_GrammarEQ_pB_clicked();

    void on_PremadeTasks_pB_clicked();

    void on_InfiniteTasks_pB_clicked();

    void on_Compare_pB_clicked();

    void on_CF1Load_pB_clicked();

    void on_CF2Load_pB_clicked();

    void on_CF1Save_pB_clicked();

    void on_CF2Save_pB_clicked();

    void on_clearOutput_pB_clicked();

    void on_length_lineEdit_textChanged(const QString &arg1);

    void on_wordCount_lineEdit_textChanged(const QString &arg1);

    void on_langGenerate_pB_clicked();

    void on_sigma_lineEdit_textChanged(const QString &arg1);

private:
    Ui::Diploma_MainWindow *ui;

    Automata automata;
    CF_Grammar* languageCFG;
    CF_Grammar* generatedCFG;

    int wordLength;
    int wordCount;

};

#endif // DIPLOMA_MAINWINDOW_H
