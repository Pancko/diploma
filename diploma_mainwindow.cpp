#include "diploma_mainwindow.h"
#include "./ui_diploma_mainwindow.h"
#include <QIntValidator>
#include <QThread>

Diploma_MainWindow::Diploma_MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Diploma_MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->mainMenu_page);

    ui->length_lineEdit->setPlaceholderText("30");
    ui->wordCount_lineEdit->setPlaceholderText("10");
    ui->length_lineEdit->setText(ui->length_lineEdit->placeholderText());
    ui->wordCount_lineEdit->setText(ui->wordCount_lineEdit->placeholderText());
    ui->length_lineEdit->setInputMask("09");
    ui->wordCount_lineEdit->setInputMask("09");
}

Diploma_MainWindow::~Diploma_MainWindow()
{
    delete ui;
}

void Diploma_MainWindow::on_Library_pB_clicked()
{

}

void Diploma_MainWindow::on_GrammarEQ_pB_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->grammarEQ_page);
}

void Diploma_MainWindow::on_PremadeTasks_pB_clicked()
{

}

void Diploma_MainWindow::on_InfiniteTasks_pB_clicked()
{

}


void Diploma_MainWindow::on_Compare_pB_clicked()
{
    CF_Grammar grammar1;
    CF_Grammar grammar2;
    QString str1 = ui->grammar1_textEdit->toPlainText();
    QString str2 = ui->grammar2_textEdit->toPlainText();
    QString error;
    bool isDebug = ui->debug_chBox->checkState();
    bool isPath = ui->path_chBox->checkState();
    bool errorFound = false;

    QThread* CF_AnalyzerThread = new QThread();
    CF_Analyzer *CF_analyzer = new CF_Analyzer();
    CF_analyzer->moveToThread(CF_AnalyzerThread);

    //connect(CF_analyzer, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(CF_AnalyzerThread, SIGNAL(started()), CF_analyzer, SLOT(compare()));
    connect(CF_analyzer, SIGNAL(exited()), CF_AnalyzerThread, SLOT(quit()));
    connect(CF_analyzer, SIGNAL(exited()), CF_analyzer, SLOT(deleteLater()));
    connect(CF_AnalyzerThread, SIGNAL(finished()), CF_analyzer, SLOT(writeOutput()));
    connect(CF_AnalyzerThread, SIGNAL(finished()), CF_AnalyzerThread, SLOT(deleteLater()));


    error = grammar1.ReadFromFile(str1);
    if (error.size() > 0){
        ui->output_textEdit->append("Грамматика 1. " + error);
        errorFound = true;
    }
    error = grammar2.ReadFromFile(str2);
    if (error.size() > 0){
        ui->output_textEdit->append("Грамматика 2. " + error);
        errorFound = true;
    }
    if (!errorFound){
        ui->output_textEdit->append(grammar1.PrintGrammar(isDebug, isPath));
        ui->output_textEdit->append(grammar2.PrintGrammar(isDebug, isPath));
        CF_analyzer->setLocals(grammar1, grammar2, wordLength, wordCount,  ui->output_textEdit);
        CF_AnalyzerThread->start();
        ui->output_textEdit->append(CF_analyzer->output);
    } else
        ui->output_textEdit->append("=========================\n");
}


void Diploma_MainWindow::on_CF1Load_pB_clicked()
{
    ui->grammar1_textEdit->insertPlainText(load());
}


void Diploma_MainWindow::on_CF2Load_pB_clicked()
{
    ui->grammar2_textEdit->insertPlainText(load());
}


void Diploma_MainWindow::on_CF1Save_pB_clicked()
{

}


void Diploma_MainWindow::on_CF2Save_pB_clicked()
{

}

void Diploma_MainWindow::save(const QString& text)
{

}


QString Diploma_MainWindow::load()
{
    QString result;
    return result;
}

void Diploma_MainWindow::on_clearOutput_pB_clicked()
{
    ui->output_textEdit->clear();
}


void Diploma_MainWindow::on_length_lineEdit_textChanged(const QString &arg1)
{
    wordLength = arg1.toInt();
    if (arg1.isEmpty())
        wordLength = ui->length_lineEdit->placeholderText().toInt();
}


void Diploma_MainWindow::on_wordCount_lineEdit_textChanged(const QString &arg1)
{
    wordCount = arg1.toInt();
    if (arg1.isEmpty())
        wordCount = ui->wordCount_lineEdit->placeholderText().toInt();
}


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
