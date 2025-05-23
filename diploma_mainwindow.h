#ifndef DIPLOMA_MAINWINDOW_H
#define DIPLOMA_MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include "automata.h"
#include "waitingspinnerwidget.h"
#include "QSidePanel/QSidePanel/PanelLeftSide.hpp"
#include "cf_analyzer_session.h"

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

    void stop_spinner();

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

    void on_menu_pB_clicked();

    void on_showGrammarCheckBox_checkStateChanged(const Qt::CheckState &arg1);

    void on_compare_Inf_pB_clicked();

    void on_clear_Inf_pB_clicked();

    void on_length_Inf_LineEdit_textChanged(const QString &arg1);

    void on_wordCount_Inf_lineEdit_textChanged(const QString &arg1);

private:
    Ui::Diploma_MainWindow *ui;

    Automata* automata;
    CF_Grammar* languageCFG;
    CF_Grammar* generatedCFG;

    int wordLength;
    int wordCount;

    int wordLengthInf;
    int wordCountInf;

    QVector<WaitingSpinnerWidget*> spinners;

    PanelLeftSide* panel_left;

    QString generated_text;
    QString masked_text;

    QStringList pages = {"Главное меню", "Генерация грамматик", "Анализ эквивалентности"};
};

#endif // DIPLOMA_MAINWINDOW_H
