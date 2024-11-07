#include "diploma_mainwindow.h"
#include "./ui_diploma_mainwindow.h"
#include <QIntValidator>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>
#include "regExPlus.h"
#include "cf_analyzer.h"

Diploma_MainWindow::Diploma_MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Diploma_MainWindow)
{
    languageCFG = new CF_Grammar();
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->mainMenu_page);

    ui->length_lineEdit->setPlaceholderText("30");
    ui->wordCount_lineEdit->setPlaceholderText("10");
    ui->length_lineEdit->setText(ui->length_lineEdit->placeholderText());
    ui->wordCount_lineEdit->setText(ui->wordCount_lineEdit->placeholderText());
    ui->length_lineEdit->setInputMask("09");
    ui->wordCount_lineEdit->setInputMask("09");

    ui->sigma_lineEdit->setPlaceholderText("a,b,c");
    ui->sigma_lineEdit->setText(ui->sigma_lineEdit->placeholderText());
    QRegularExpression rx("([a-vx-z],)*|[a-vx-z]");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    ui->sigma_lineEdit->setValidator(validator);
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
    ui->stackedWidget->setCurrentWidget(ui->infiniteTask_page);
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


    error = grammar1.ReadFromTXT(str1);
    if (error.size() > 0){
        ui->output_textEdit->append("Грамматика 1. " + error);
        errorFound = true;
    }
    error = grammar2.ReadFromTXT(str2);
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
    ui->grammar1_textEdit->clear();
    ui->grammar1_textEdit->insertPlainText(load());
}


void Diploma_MainWindow::on_CF2Load_pB_clicked()
{
    ui->grammar2_textEdit->clear();
    ui->grammar2_textEdit->insertPlainText(load());
}


void Diploma_MainWindow::on_CF1Save_pB_clicked()
{
    save(ui->grammar1_textEdit->toPlainText());
}


void Diploma_MainWindow::on_CF2Save_pB_clicked()
{
    save(ui->grammar2_textEdit->toPlainText());
}

void Diploma_MainWindow::save(const QString& text)
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить файл", QDir::currentPath() + "/Data/Saves", "JSON(*.json);;TXT(*.txt)");
    QFile file(fileName);
    file.open(QIODeviceBase::WriteOnly);
    if(QFileInfo(fileName).suffix() == "txt")
    {
        QTextStream out(&file);
        out << text;
    }
    else if (QFileInfo(fileName).suffix() == "json")
    {
        CF_Grammar gr;
        QString str = text;
        QJsonDocument doc;
        QJsonArray array;
        QJsonObject obj;
        gr.ReadFromTXT(str);
        array.append(gr.GetStartingNT());
        QMap<QString, QVector<Path>> non_terms = gr.GetNonTerminals();
        QSet<QString> terms = gr.GetTerminals();
        QVector<Rule> rules = gr.GetRules();
        for (auto [key, value] : non_terms.asKeyValueRange())
        {
            if(!array.contains(key)) array.append(key);
        }
        obj.insert("non-terminals", array);
        while(array.count()) {
            array.pop_back();
        }
        for (const QString& str : terms)
        {
            array.append(str);
        }
        obj.insert("terminals", array);
        while(array.count()) {
            array.pop_back();
        }
        for (Rule& rule : rules)
        {
            QString temp_str;
            QJsonArray temp_array;
            for (QString& str : rule.right_part)
                temp_str += str;
            temp_array.append(rule.left_part);
            temp_array.append(temp_str);
            temp_array.append(rule.complexity);
            array.append(temp_array);
        }
        obj.insert("rules", array);
        doc.setObject(obj);
        file.write(doc.toJson());
    }
    file.close();
}


QString Diploma_MainWindow::load()
{
    QString result;
    QString directory = QFileDialog::getOpenFileName(this, "Выбрать файл", QDir::currentPath() + "/Data/Saves", "(*.txt *.json)");

    if (!directory.isEmpty())
    {
        QFile file = QFile(directory);
        file.open(QIODevice::ReadOnly);
        if(QFileInfo(directory).suffix() == "txt"){
            result = QByteArray(file.readAll());
        }
        else if (QFileInfo(directory).suffix() == "json"){
            QString temp_str;
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            QJsonArray rules = doc["rules"].toArray();
            for(QJsonValueRef obj : rules)
            {
                if (temp_str != obj[0].toString()){
                    temp_str = obj[0].toString();
                    if (result.isEmpty()) result += temp_str + " -> " + obj[1].toString();
                    else result += "\n" + temp_str + " -> " + obj[1].toString();
                }
                else { result += " | " + obj[1].toString(); }
            }
        }
        file.close();
    }
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

void Diploma_MainWindow::on_langGenerate_pB_clicked()
{
    QString err;
    QStringList sigma = ui->sigma_lineEdit->text().split(',', Qt::SkipEmptyParts);
    if (languageCFG->GetTerminals().isEmpty())
    {
        QFile file = QFile(QDir::currentPath() + "/Data/Resources/LangGr.json");
        file.open(QIODevice::ReadOnly);
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        err = languageCFG->ReadFromJSON(doc);
        foreach(QString str, sigma)
        {
            languageCFG->AddRule(Rule("∑", QList<QString>({str}), 1));
        }
        for(int i = 2; i < 6 ;i++)
            languageCFG->AddRule(Rule("N", QList<QString>({QString::number(i)}), 1));
        languageCFG->AnalyzeNonTerminals();
        file.close();
    }
    if(err.isEmpty()){
        int difficulty = ui->langDifficulty_spinBox->value();
        QPair<QString, int> language = languageCFG->GenerateWord(difficulty * 10);
        while((language.second < (difficulty - 1) * 10) || (language.second > difficulty * 10))
            language = languageCFG->GenerateWord(difficulty * 10);
        qDebug() << language.first;                                         ////////////////////////////
        QString temp;
        while(temp != language.first)
        {
            temp = language.first;
            language.first = reduce(language.first, sigma);
        }
        qDebug() << language.first << Qt::endl;                             ////////////////////////////////////////
        ui->language_Label->setText("Язык: " + language.first);
        automata.initialize_sigma(sigma);
        generatedCFG = automata.parse(language.first);
        ui->langCFG_textEdit->setText(/*languageCFG->PrintGrammar(1,1) + */generatedCFG->PrintGrammar(0,0));
        generatedCFG->AnalyzeNonTerminals();
        ui->langCFG_textEdit->setText(ui->langCFG_textEdit->toPlainText() + generatedCFG->PrintGrammar(1,1));
    }
    else
        ui->language_Label->setText("Язык: " + err);
}


void Diploma_MainWindow::on_sigma_lineEdit_textChanged(const QString &arg1)
{
    QChar newCh = arg1.back();
    QString newStr = arg1.chopped(1);
    if (newCh.isLetter() && newStr.contains(newCh))
        ui->sigma_lineEdit->setText(newStr);
}

