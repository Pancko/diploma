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

Diploma_MainWindow::Diploma_MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Diploma_MainWindow)
{
    ui->setupUi(this);
    languageCFG = new CF_Grammar(this);
    automata = new Automata(this);
    spinner = new WaitingSpinnerWidget(ui->output_textEdit);

    QString saveDir = QDir::currentPath() + "/Data/Saves";
    QDir dir;
    if (!dir.exists(saveDir)) {
        dir.mkpath(saveDir);
    }

    ui->stackedWidget->setCurrentWidget(ui->mainMenu_page);

    ui->length_lineEdit->setPlaceholderText("10");
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

    panel_left = new PanelLeftSide(this);
    panel_left->setOpenEasingCurve (QEasingCurve::Type::OutElastic);
    panel_left->setCloseEasingCurve(QEasingCurve::Type::InElastic);
    panel_left->init();
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
    CF_Grammar *grammar1 = new CF_Grammar(this);
    CF_Grammar *grammar2 = new CF_Grammar(this);
    QString str1 = ui->grammar1_textEdit->toPlainText();
    QString str2 = ui->grammar2_textEdit->toPlainText();
    QString error;
    bool isDebug = ui->debug_chBox->checkState();
    bool isPath = ui->path_chBox->checkState();
    bool errorFound = false;

    error = grammar1->ReadFromTXT(str1);
    if (error.size() > 0){
        ui->output_textEdit->append("Грамматика 1. " + error);
        errorFound = true;
    }
    error = grammar2->ReadFromTXT(str2);
    if (error.size() > 0){
        ui->output_textEdit->append("Грамматика 2. " + error);
        errorFound = true;
    }
    if (!errorFound){
        ui->output_textEdit->append(grammar1->PrintGrammar(isDebug, isPath));
        ui->output_textEdit->append(grammar2->PrintGrammar(isDebug, isPath));

        CF_Session* session = new CF_Session();
        spinner->start();
        ui->clearOutput_pB->setDisabled(1);
        ui->Compare_pB->setDisabled(1);
        session->addThread(grammar1, grammar2, wordLength, wordCount,  ui->output_textEdit);
        connect(session, SIGNAL(stopAll()), this, SLOT(stop_spinner()));
        connect(session, SIGNAL(stopSpin()), this, SLOT(stop_spinner()));
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
        for (const QString& str : std::as_const(terms))
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
    QString directory = QFileDialog::getOpenFileName(this, "Выбрать файл", QDir::currentPath() + "/Data/Saves", "(*.txt *.json)"
                                                     ,0,QFileDialog::DontUseNativeDialog);

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

void Diploma_MainWindow::stop_spinner()
{
    ui->Compare_pB->setDisabled(0);
    ui->clearOutput_pB->setDisabled(0);
    spinner->stop();
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
        QFile file = QFile(":/Resources/LangGr.json");
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
        qDebug() << language.first;                                         ////////////////////////////////////////
        QString temp;
        while(temp != language.first)
        {
            temp = language.first;
            language.first = reduce(language.first, sigma);
        }
        qDebug() << language.first << Qt::endl;                             ////////////////////////////////////////
        ui->language_Label->setText("Язык: " + language.first);
        automata->initialize_sigma(sigma);
        generatedCFG = automata->parse(language.first);

        ui->langCFG_textEdit->setText(generatedCFG->PrintGrammar(0,0));
        generatedCFG->AnalyzeNonTerminals();
        if (!generatedCFG->ContaisBadNT())
            ui->langCFG_textEdit->setText(generatedCFG->PrintGrammar(0,0));
        else
            ui->langCFG_textEdit->append(generatedCFG->PrintGrammar(0,0));

        //qDebug() << generatedCFG->PrintGrammar(1,1);

        if(!generatedCFG->ContaisBadNT() && (generatedCFG->GetRules().size() != 0))
        {
            generatedCFG->GenerateMultipleWords(100, 50);
            QStringList words = generatedCFG->GetWords();
            words.sort();
            for (const QString &word : std::as_const(words))
                ui->langCFG_textEdit->append(word);
        }
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

void Diploma_MainWindow::on_menu_pB_clicked()
{
    QVBoxLayout* lay = new QVBoxLayout;
    QPushButton* btn = new QPushButton("Отмена", this);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(btn, &QAbstractButton::clicked, this, [=] {
        panel_left->closePanel();
    });
    lay->addWidget(btn);
    for (int i = 0; i < ui->stackedWidget->count(); i ++)
    {
        if (i != ui->stackedWidget->currentIndex())
        {
            QPushButton* btn = new QPushButton(pages[i], this);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            connect(btn, &QAbstractButton::clicked, this, [=] {
                ui->stackedWidget->setCurrentIndex(i);
                panel_left->closePanel();
            });
            lay->addWidget(btn);
        }
    }
    lay->setAlignment(Qt::AlignCenter);
    QWidget* proxy = new QWidget(this);
    proxy->setLayout(lay);
    panel_left->setWidget(proxy);
    panel_left->setFixedWidth(proxy->size().width() * 1.1);
    panel_left->setFixedHeight(proxy->size().height() * 1.2);
    panel_left->setAlignment(Qt::AlignCenter);
    panel_left->openPanel();
}

