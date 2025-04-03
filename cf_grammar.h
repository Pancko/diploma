#ifndef CF_GRAMMAR_H
#define CF_GRAMMAR_H

// ТВГУ ПМиК ФИиИТ 2024
// Андреев Семен Витальевич
#pragma once
#include <QString>
#include <QVector>
#include <QTextStream>
#include <QSet>
#include <QMap>
#include <QJsonDocument>
#include <QThread>

// Правило из КС-грамматики
struct Rule
{
    QString left_part;				// Левая часть правила
    QVector<QString> right_part;	// Правая часть правила
    size_t terminals_count = 0;		// Счетчик терминалов
    int complexity = 0;             // Сложность правила для генерации

    bool operator==(const Rule& Object) const;
    bool operator!=(const Rule& Object) const;

    bool operator<(const Rule& Object) const;

    Rule();
    Rule(QString Left_Part, QVector<QString> Right_Part, int Complexity = 0);
    ~Rule();// = default;
    void clear();
};

// Путь из нетерминала до определенного слова
struct Path
{
    int length;								// Длина пути
    QVector<Rule> path_rules;				// Правила
    QVector<QVector<QString>> path_words;	// Последовательность слов
    QVector<QString> word;					// Конечное слово

    bool operator==(const Path& Object) const;
    bool operator+=(const Path& Object);
    Path ApplyPath(const Path& Object, int position = 0);

    QString PrintPath(bool IsDebug = false);

    Path();
    ~Path();// = default;
    void clear();
};

// Подстановка всех возможных терминальных слов в каждый нетерминал правила (Для таблицы динамического программирования)
struct PathPermutations
{
    Rule rule;
    QVector<QPair<QString, QVector<QString>>> right_part;
public:
    PathPermutations(Rule _rule) { rule = _rule; }
    ~PathPermutations();// = default;
    void clear();
};

// Основной класс - КС-грамматика
class CF_Grammar: public QObject
{
    Q_OBJECT
private:
    QString starting_non_terminal;					// S
    QMap<QString, QVector<Path>> non_terminals;     // N
    QMap<QString, Path> shortest_path;				// Кратчайшие пути для нетерминалов
    QSet<QString> bad_non_terminals;				// "Плохие" нетерминалы
    QSet<QString> terminals;						// Sigma
    QVector<Rule> rules;							// P
    int pathes_amount = 0;                          // Количество путей
    QSet<QString> words;							// Сгенерированныые слова

public:
    explicit CF_Grammar(QObject *parent = 0);
    ~CF_Grammar();// = default;
    void clear();

    // Получить поля
    QString GetStartingNT();
    QMap<QString, QVector<Path>> GetNonTerminals();
    QSet<QString> GetTerminals();
    QVector<Rule> GetRules();

    // Считывание грамматики из файла
    QString ReadFromTXT(QString& inputString);
    QString ReadFromJSON(QJsonDocument& doc);
    // Получение правила из строки
    Rule GetRuleFromString(const QString& String);
    // Добавление правила в грамматику
    void AddRule(const Rule& New_Rule);
    void ModRule(int index, const QString &str);


    // Генерация путей. Анализ и удаление циклов, бесполезных нетерминалов
    void AnalyzeNonTerminals();
    // Быстрая генерация путей, нахождение первых быстрых выводов
    void GenerateBasicPathes();
    // Полная генерация путей.
    void GeneratePathes();
    // Генерация путей, доведение всех правил до терминальных слов
    QMap<QString, QVector<Path>> GenerateSubPath(const Path& Current_Path);
    // Проверка пути на уникальность
    bool IsUniquePath(const Path& Path_To_Check, const Path& Added_Path, const QMap<QString, QVector<Path>>& Current_Pathes);
    // Проверка правой части правила на удовлетворение условиям создания нового пути
    bool IsRuleViable(const Rule& Current_Rule, const QMap<QString, QVector<Path>>& Non_Terminals);


    // Вычисление и пометка "плохих" нетерминалов (из которых невозможно вывести полностью терминальное слово)
    void FindingBadNonTerminals();
    // Удаление "плохих" нетерминалов
    void DeleteBadNonTerminals();
    // Заполнение вектора кратчайших путей для использования в генерации
    void FillShortestPathes();


    // Проверка наличия нетерминала в конце пути
    bool GotNonTerminal(const Path& Current_Path);
    // Проверка наличия нетерминала в слове
    bool GotNonTerminal(const QVector<QString>& Word);


    // Вывод грамматики в консоль
    QString PrintGrammar(bool IsDebug = false, bool ShowPath = false);


    // Генерация случайного терминального слова
    QPair<QString, int> GenerateWord(int Max_Length);
    // Генерация нескольких случайных терминальных слов с заданной максимальной длиной
    QVector<QString> GenerateMultipleWords(int Amount, int Max_Length);

    // Печать сгенерированных слов
    void PrintWords(bool IsDebug = false);
    // Получение сгенерированных слов
    QSet<QString> GetWords();


    // Алгоритм Кока-Янгера-Касами, модификация для произвольной грамматики
    bool CYK_Alg_Modified(const QString& Word);


    // Вспомогательные функции для алгоритмов:
    // Номер правила
    int IndexOfRule(const Rule& Current_Rule);
    // Приведение вектора строк к строке
    QString VectorToString(const QVector<QString>& Object);
    // Получение всех правил данного нетерминала
    QVector<Rule> NonTerminalRules(const QString& Non_Terminal);
};

// Применение правила к слову
QVector<QString> ApplyRule(const QVector<QString>& String, const Rule& Rule, int Non_Terminal_Number = 0);
// Совмещение двух наборов правил
QMap<QString, QVector<Path>> PathConvergence(const QMap<QString, QVector<Path>>& First_Object, const QMap<QString, QVector<Path>>& Second_Object);
// Содержится ли строка в векторе
bool VecContStr(const QVector<QString>& Vector, const QString& String);

// Генерация и проверка выводимости слов в двух грамматиках методом Кока-Янгера-Касами
QString EquivalenceTest(CF_Grammar* Grammar1, CF_Grammar* Grammar2, int Words_Lenght = 10, int Words_Count = 10);

#endif // CF_GRAMMAR_H
