// ТВГУ ПМиК ФИиИТ 2024
// Андреев Семен Витальевич
#include "CF_Grammar.h"
#include <QList>
#include <QDebug>
#include <QtAlgorithms>
#include <QtMath>
#include <QJsonArray>
#include <QJsonObject>

CF_Grammar::CF_Grammar()
{
    non_terminals = QMap<QString, QVector<Path>>();
    shortest_path = QMap<QString, Path>();
    bad_non_terminals = QSet<QString>();
    terminals = QSet<QString>();
    words = QSet<QString>();
}

CF_Grammar::~CF_Grammar()
{
    clear();
}

void CF_Grammar::clear()
{
    starting_non_terminal.clear();
    starting_non_terminal.squeeze();

    for (QVector<Path>& v : non_terminals)
    {
        for (Path& i_path : v)
            i_path.clear();
        v.clear();
        v.squeeze();
    }
    for(QString& s : non_terminals.keys())
    {
        s.clear();
        s.squeeze();
    }
    non_terminals.clear();

    for (Path& p : shortest_path)
        p.clear();
    for(QString& s : shortest_path.keys())
    {
        s.clear();
        s.squeeze();
    }
    shortest_path.clear();


    bad_non_terminals.clear();
    bad_non_terminals.squeeze();

    terminals.clear();
    terminals.squeeze();

    for (Rule& r : rules)
        r.clear();
    rules.clear();
    rules.squeeze();

    pathes_amount = 0;

    words.clear();
    words.squeeze();
}

//=============== Получить поля грамматики ==============================================================

QString CF_Grammar::GetStartingNT()
{
    return starting_non_terminal;
}

QMap<QString, QVector<Path> > CF_Grammar::GetNonTerminals()
{
    return non_terminals;
}

QSet<QString> CF_Grammar::GetTerminals()
{
    return terminals;
}

QVector<Rule> CF_Grammar::GetRules()
{
    return rules;
}

//=============== Считывание грамматики из файла и создание объекта =====================================

QString CF_Grammar::ReadFromTXT(QString& inputString)
{
    if (inputString.size() == 0){return "Ошибка: грамматика не найдена!";}
    QString current_string;
    QString sub_string;
    Rule current_rule;
    size_t position;
    QTextStream inputText(&inputString);
    int strNum = 0;

    while (inputText.readLineInto(&current_string))
    {
        strNum++;
        if(current_string.indexOf("->") < 1){return QString("Ошибка: в строке " + QString::number(strNum) + " не найдена структура '[НЕТЕРМИНАЛ]->'!");}
        if((current_string.contains("/*") && !current_string.contains("*/")) || (!current_string.contains("/*") && current_string.contains("*/")))
        {return QString("Ошибка: в строке " + QString::number(strNum) + " недописана структура /*ТЕРМИНАЛ*/");}

        QStringList list1 = current_string.split("->");
        if(list1[1].size() == 0){return QString("Ошибка: в строке " + QString::number(strNum) + " отсутствует правая часть правила. Чтобы вставить пустое слово используйте [EPS]");}
        current_rule.left_part = list1[0].trimmed();
        QStringList list2 = list1[1].split('|');
        for(int i = 0; i < list2.size(); i++)
        {
            sub_string = list2[i].trimmed();
            if (list2[i].trimmed().contains("/*") && !list2[i].trimmed().contains("*/")){
                for (int j = i; j < list2.size(); j++)
                {
                    if (list2[j].trimmed().contains("*/"))
                    {
                        for (int r = i + 1; r < j; r++)
                            sub_string += '|' + list2[r].trimmed();
                        sub_string += '|' + list2[j].trimmed();
                        i = j;
                        break;
                    }
                }
            }
            current_string = current_rule.left_part + "->" + sub_string;
            current_rule = GetRuleFromString(current_string);
            AddRule(current_rule);
        }
    }
    AnalyzeNonTerminals();
    return NULL;
}

QString CF_Grammar::ReadFromJSON(QJsonDocument& doc)
{
    QJsonObject object = doc.object();
    QJsonArray array = object["non-terminals"].toArray();
    starting_non_terminal = array[0].toString();
    foreach (const QJsonValue& str, array)
    {
        non_terminals.insert(str.toString(), QList<Path>());
    }
    array = object["terminals"].toArray();
    foreach (const QJsonValue& str, array)
    {
        terminals.insert(str.toString());
    }
    array = object["rules"].toArray();
    foreach (const QJsonValue& obj, array)
    {
        Rule rule;
        QString temp_str;
        rule.left_part = obj[0].toString();
        for (QChar& chr : obj[1].toString())
        {
            temp_str += chr;
            if (non_terminals.contains(temp_str) || terminals.contains(temp_str))
            {
                if (terminals.contains(temp_str))
                    rule.terminals_count++;
                rule.right_part.push_back(temp_str);
                temp_str.clear();
            }
        }
        if (!temp_str.isEmpty()) return "Ошибка в чтении правила " + QString::number(rules.size() + 1) + ". Правая часть содержит символы, не являющиеся терминалом или нетерминалом.";
        rule.complexity = obj[2].toInt();
        AddRule(rule);
    }
    //AnalyzeNonTerminals();
    return NULL;
}

Rule CF_Grammar::GetRuleFromString(const QString& String)
{
    Rule result;
    QString current_string;
    size_t position = String.indexOf("->");
    result.left_part = String.sliced(0, position);
    current_string = String.sliced(position + 2);

    if (starting_non_terminal.length() == 0)
        starting_non_terminal = result.left_part;

    size_t i_char = 0;

    while (true)
    {
        if (current_string[i_char] == '[')
        {
            result.right_part.push_back(current_string.sliced(i_char, current_string.sliced(i_char).indexOf(']') + 1));
            i_char += current_string.sliced(i_char).indexOf(']');
        }
        else if (current_string[i_char] == '/' && i_char != current_string.length() - 1 && current_string[i_char + 1] == '*')
        {
            result.right_part.push_back(current_string.sliced(i_char, current_string.sliced(i_char).indexOf("*/") + 2));
            i_char += current_string.sliced(i_char).indexOf("*/") + 1;
        }
        else
            result.right_part.push_back(current_string.sliced(i_char, 1));
        i_char++;
        if (i_char == current_string.length())
            break;
    }

    return result;
}

void CF_Grammar::AddRule(const Rule& New_Rule)
{
    Rule rule_to_add = New_Rule;

    if (!non_terminals.contains(New_Rule.left_part))
    {
        non_terminals.insert(New_Rule.left_part, QList<Path>());
    }
    for (QString i_string : New_Rule.right_part)
    {
        if (i_string == "[EPS]")
        {
            if (!terminals.contains(i_string))
                terminals.insert(i_string);
        }
        else if (i_string[0] == '[' || i_string[0].isUpper())
        {
            if (!non_terminals.contains(i_string))
                non_terminals.insert(i_string, QList<Path>());
        }
        else
        {
            rule_to_add.terminals_count++;
            if (!terminals.contains(i_string))
                terminals.insert(i_string);
        }
    }
    rules.push_back(rule_to_add);
}

void CF_Grammar::ModRule(int index, const QString &str)
{
    rules[index].right_part.push_back(str);
}

//=============== Анализ полученной грамматики, составление путей для нетерминалов ======================

void CF_Grammar::AnalyzeNonTerminals()
{
    GenerateBasicPathes();

    FindingBadNonTerminals();

    DeleteBadNonTerminals();

    FillShortestPathes();
}

void CF_Grammar::GenerateBasicPathes()
{
    Path current_path;
    QVector<PathPermutations> pathes;
    QMap<QString, QVector<Path>> new_found_pathes;
    QMap<QString, QVector<Path>> current_new_found_pathes;
    //QMap<QString, QVector<Path>> empty_pathes;
    QSet<QString> analyzed_non_terminlas;
    QString temp_str;
    int position = 0;

    for (Rule& i_rule : rules) // Базис. Заполнить массив правил
    {
        pathes.push_back(PathPermutations(i_rule));

        for (QString& i_string : i_rule.right_part)
            pathes[IndexOfRule(i_rule)].right_part.push_back(QPair<QString, QVector<QString>>(i_string, QVector<QString>()));

        if (!GotNonTerminal(i_rule.right_part)) // Если правило порождает нетерминальное слово
        {
            if (!new_found_pathes.contains(i_rule.left_part))
                new_found_pathes.insert(i_rule.left_part, QVector<Path>());
            if (i_rule.right_part[0] == "[EPS]")
                current_path.length = 0;
            else
            {
                current_path.word = i_rule.right_part;
                current_path.length = 1;
            }
            current_path.path_words.push_back(QVector<QString>({ i_rule.left_part }));
            current_path.path_rules.insert(current_path.path_rules.cbegin(), i_rule);
            current_path.path_words.push_back(current_path.word);

            non_terminals[i_rule.left_part].push_back(current_path);
            pathes_amount++;

            new_found_pathes[i_rule.left_part].push_back(current_path);
            analyzed_non_terminlas.insert(i_rule.left_part);

            current_path.path_rules.clear();
            current_path.path_words.clear();
            current_path.word.clear();
        }
    }

    while (new_found_pathes.size() > 0) // Рекуррентный шаг, создаем пути пока находим новые (только для новых нетерминалов)
    {
        for (Rule& i_rule : rules)
        {
            // Если для нетерминала ещё не найден ни один вывод
            if (IsRuleViable(i_rule, new_found_pathes) && !analyzed_non_terminlas.contains(i_rule.left_part))
            {
                position = 0;
                for (QString& i_string : i_rule.right_part)
                {
                    if (new_found_pathes.contains(i_string))
                    {
                        for (Path& i_path : new_found_pathes[i_string])
                        {
                            temp_str.clear();
                            for (QString& word_str : i_path.word)
                                temp_str += word_str;

                            if (!VecContStr(pathes[IndexOfRule(i_rule)].right_part[position].second, temp_str)) // Если такой путь еще не был применен к нетерминалу
                            {
                                current_path.length = 1;
                                current_path.path_rules.push_back(i_rule);
                                current_path.path_words.push_back(QVector<QString>({ i_rule.left_part }));
                                current_path.path_words.push_back(i_rule.right_part);
                                current_path.word = i_rule.right_part;

                                current_path = current_path.ApplyPath(i_path, position);

                                if (IsUniquePath(current_path, i_path, non_terminals))
                                {
                                    pathes[IndexOfRule(i_rule)].right_part[position].second.push_back(temp_str); // Добавляем новый вариант в таблицу
                                    if (GotNonTerminal(current_path)) // Генерируем новые пути
                                    {
                                        current_new_found_pathes = PathConvergence(current_new_found_pathes, GenerateSubPath(current_path));
                                        if (current_new_found_pathes.size() != 0)
                                            analyzed_non_terminlas.insert(i_rule.left_part);
                                    }
                                    else
                                    {
                                        non_terminals[i_rule.left_part].push_back(current_path);
                                        pathes_amount++;
                                        if (!current_new_found_pathes.contains(i_rule.left_part))
                                            current_new_found_pathes.insert(i_rule.left_part, QVector<Path>());
                                        current_new_found_pathes[i_rule.left_part].push_back(current_path);
                                        analyzed_non_terminlas.insert(i_rule.left_part);
                                    }
                                }

                                current_path.path_rules.clear();
                                current_path.path_words.clear();
                                current_path.word.clear();
                            }
                        }
                    }
                    position++;
                }
            }
        }

        new_found_pathes.clear();
        new_found_pathes = current_new_found_pathes;
        current_new_found_pathes.clear();
    }

    // Находим все уже известные пустые выводы
    analyzed_non_terminlas.clear();
    new_found_pathes.clear();
    for (auto [key, value] : non_terminals.asKeyValueRange())
    {
        for (Path& i_path : value)
        {
            if (i_path.word.size() == 0 && !analyzed_non_terminlas.contains(key))
            {
                analyzed_non_terminlas.insert(key);
                new_found_pathes.insert(key, QVector<Path>()); //////////{i_path}???
                new_found_pathes[key].push_back(i_path);
                break;
            }
        }
    }

    current_new_found_pathes = new_found_pathes;

    while (current_new_found_pathes.size() > 0) // Найти все пустые выводы
    {
        current_new_found_pathes.clear();
        for (Rule& i_rule : rules)
        {
            if (!analyzed_non_terminlas.contains(i_rule.left_part))
            {
                current_path.length = 1;
                current_path.path_rules.push_back(i_rule);
                current_path.path_words.push_back(QVector<QString>({ i_rule.left_part }));
                current_path.path_words.push_back(i_rule.right_part);
                current_path.word = i_rule.right_part;

                for (QString& i_string : i_rule.right_part)
                {
                    if (analyzed_non_terminlas.contains(i_string))
                    {
                        current_path += new_found_pathes[i_string][0];

                        if (current_path.word.size() == 0)
                        {
                            non_terminals[i_rule.left_part].push_back(current_path);
                            pathes_amount++;
                            analyzed_non_terminlas.insert(i_rule.left_part);
                            if (!current_new_found_pathes.contains(i_rule.left_part))
                                current_new_found_pathes.insert(i_rule.left_part, QVector<Path>());
                            current_new_found_pathes[i_rule.left_part].push_back(current_path);
                            if (!new_found_pathes.contains(i_rule.left_part))
                                new_found_pathes.insert(i_rule.left_part, QVector<Path>());
                            new_found_pathes[i_rule.left_part].push_back(current_path);
                            break;
                        }
                    }
                }
                current_path.path_rules.clear();
                current_path.path_words.clear();
                current_path.word.clear();
            }
        }
    }
}

void CF_Grammar::GeneratePathes()
{
    Path current_path;
    QVector<PathPermutations> pathes;
    QMap<QString, QVector<Path>> new_found_pathes;
    QMap<QString, QVector<Path>> current_new_found_pathes;
    QString temp_str;
    QString info;
    int position = 0;

    for (Rule& i_rule : rules) // Базис. Заполнить массив правил
    {
        pathes.push_back(PathPermutations(i_rule));

        for (QString i_string : i_rule.right_part)
            pathes[IndexOfRule(i_rule)].right_part.push_back(QPair<QString, QVector<QString>>(i_string, QVector<QString>()));

        if (!GotNonTerminal(i_rule.right_part)) // Если правило порождает нетерминальное слово
        {
            if (!new_found_pathes.contains(i_rule.left_part))
                new_found_pathes.insert(i_rule.left_part, QVector<Path>());
            if (i_rule.right_part[0] == "[EPS]")
                current_path.length = 0;
            else
            {
                current_path.word = i_rule.right_part;
                current_path.length = 1;
            }
            current_path.path_words.push_back(QVector<QString>({ i_rule.left_part }));
            current_path.path_rules.insert(current_path.path_rules.cbegin(), i_rule);
            current_path.path_words.push_back(current_path.word);

            non_terminals[i_rule.left_part].push_back(current_path);
            pathes_amount++;

            new_found_pathes[i_rule.left_part].push_back(current_path);

            current_path.path_rules.clear();
            current_path.path_words.clear();
            current_path.word.clear();
        }
    }
    int i = 1;
    while (new_found_pathes.size() > 0) // Рекуррентный шаг, создаем пути пока находим новые
    {
        info += "Generating pathes step " + QString::number(i) + ", found new pathes for non-terminals: ";

        for (auto [key, value] : new_found_pathes.asKeyValueRange())
        {
            info += key + ", ";
        }

        for (Rule& i_rule : rules)
        {
            if (IsRuleViable(i_rule, new_found_pathes))
            {
                position = 0;
                for (QString& i_string : i_rule.right_part)
                {
                    if (new_found_pathes.contains(i_string))
                    {
                        for (Path& i_path : new_found_pathes[i_string])
                        {
                            temp_str.clear();
                            for (QString& word_str : i_path.word)
                                temp_str += word_str;

                            if (!VecContStr(pathes[IndexOfRule(i_rule)].right_part[position].second, temp_str)) // Если такой путь еще не был применен к нетерминалу
                            {
                                current_path.length = 1;
                                current_path.path_rules.push_back(i_rule);
                                current_path.path_words.push_back(QVector<QString>({ i_rule.left_part }));
                                current_path.path_words.push_back(i_rule.right_part);
                                current_path.word = i_rule.right_part;

                                current_path = current_path.ApplyPath(i_path, position);

                                if (IsUniquePath(current_path, i_path, non_terminals))
                                {
                                    pathes[IndexOfRule(i_rule)].right_part[position].second.push_back(temp_str); // Добавляем новый вариант в таблицу
                                    if (GotNonTerminal(current_path)) // Генерируем новые пути
                                        current_new_found_pathes = PathConvergence(current_new_found_pathes, GenerateSubPath(current_path));
                                    else
                                    {
                                        non_terminals[i_rule.left_part].push_back(current_path);
                                        pathes_amount++;
                                        if (!current_new_found_pathes.contains(i_rule.left_part))
                                            current_new_found_pathes.insert(i_rule.left_part, QVector<Path>());
                                        current_new_found_pathes[i_rule.left_part].push_back(current_path);
                                    }
                                }

                                current_path.path_rules.clear();
                                current_path.path_words.clear();
                                current_path.word.clear();
                            }
                        }
                    }
                    position++;
                }
            }
        }

        new_found_pathes.clear();
        new_found_pathes = current_new_found_pathes;
        int count = 0;
        for (QList<Path>& i_element : new_found_pathes)
        {
            count += (int)i_element.size();
        }
        current_new_found_pathes.clear();
        i++;
        //std::cout << "time elapsed = " << t.elapsed() << ", new pathes = " << count << std::endl;
    }
}

QMap<QString, QVector<Path>> CF_Grammar::GenerateSubPath(const Path& Current_Path)
{
    QMap<QString, QVector<Path>> result;
    Path current_path;
    QMap<QString, QVector<Path>> old_pathes = non_terminals;
    for (const QString& i_string : Current_Path.word)      // Рассматриваем символы в слове
    {
        if (non_terminals.contains(i_string))           // Если символ является нетерминалом
        {
            for (Path& i_path : old_pathes[i_string])    // Пути этого нетерминала
            {
                current_path = Current_Path;
                current_path += i_path;
                if (IsUniquePath(current_path, i_path, non_terminals))
                {
                    if (GotNonTerminal(current_path))
                        result = PathConvergence(result, GenerateSubPath(current_path));
                    else
                    {
                        result.insert(current_path.path_rules[0].left_part, QVector<Path>());
                        result[current_path.path_rules[0].left_part].push_back(current_path);
                        non_terminals[current_path.path_rules[0].left_part].push_back(current_path);
                        pathes_amount++;
                    }
                }
            }
        }
    }
    return result;
}

bool CF_Grammar::IsRuleViable(const Rule& Current_Rule, const QMap<QString, QVector<Path>>& Non_Terminals)
{
    bool found_viable_non_terminal = false;
    QMap<QString, QVector<Path>> current_non_terminals = Non_Terminals;

    for (const QString& i_string : Current_Rule.right_part)
    {
        if (i_string == Current_Rule.left_part) return false; // В правой части правила содержится тот же нетерминал (цикл)

        if (current_non_terminals.contains(i_string) && current_non_terminals[i_string].size() == 0) return false; // Нетерминал, у которого нет путей

        if (current_non_terminals.contains(i_string)) found_viable_non_terminal = true; // Подходящий нетерминал
    }

    return found_viable_non_terminal;
}

bool CF_Grammar::IsUniquePath(const Path& Path_To_Check, const Path& Added_Path, const QMap<QString, QVector<Path>>& Current_Pathes)
{
    QMap<QString, QVector<Path>> current_pathes = Current_Pathes;

    // Если путь с таким заключительным словом уже существует
    for (Path& i_path : current_pathes[Path_To_Check.path_rules[0].left_part])
    {
        if (i_path.word == Path_To_Check.word) return false;
    }

    // Если только что добавленный путь является опустошающим (то есть просто убирает нетерминал), то дальше можно не расследовать
    if (Added_Path.path_rules.size() > 0 && Added_Path.word.size() == 0) return true;

    return true;
}

//=============== Поиск и удаление "плохих" нетерминалов, получение кратчайших путей =====================

void CF_Grammar::FindingBadNonTerminals()
{
    bool good_non_terminal = false;
    bool non_terminal_found = false;

    for (auto [key, value] : non_terminals.asKeyValueRange())
    {
        good_non_terminal = false;

        for (Path& i_path : value)
        {
            non_terminal_found = false;
            for (QString& i_string : i_path.word)
            {
                if (non_terminals.contains(i_string))
                {
                    non_terminal_found = true;
                    break;
                }
            }
            if (!non_terminal_found)
            {
                good_non_terminal = true;
                break;
            }
        }
        if (!good_non_terminal)
        {
            bad_non_terminals.insert(key);
        }
    }
}

void CF_Grammar::DeleteBadNonTerminals()
{
    QMap<QString, QVector<Path>> new_non_terminals = non_terminals;
    QVector<Rule> new_rules = rules;

    for (const QString& i_string : bad_non_terminals)
    {
        new_non_terminals.remove(i_string);

        for (Rule& i_rule : rules)
        {
            if (i_rule.left_part == i_string || i_rule.right_part.contains(i_string))
            {
                new_rules.removeOne(i_rule);
            }
        }

        shortest_path.remove(i_string);

        for (auto [key, value] : non_terminals.asKeyValueRange())
        {
            for (Path& i_path : non_terminals[key])
            {
                if (i_path.word.contains(i_string))
                {
                    new_non_terminals[key].removeOne(i_path);
                    // Пересмотреть длину кратчайшего пути
                    if (i_path.length == shortest_path[key].length)
                    {
                        shortest_path[key] = new_non_terminals[key][0];
                        for (QList<Path>& j_element : new_non_terminals)
                        {
                            for (Path& j_path : j_element)
                            {
                                if (shortest_path[key].length > j_path.length)
                                    shortest_path[key] = j_path;
                            }
                        }
                    }
                }
            }
        }
    }
    non_terminals.clear();
    non_terminals = new_non_terminals;
    rules.clear();
    rules = new_rules;
}

void CF_Grammar::FillShortestPathes()
{
    for (auto [key, value] : non_terminals.asKeyValueRange())
    {
        if (value.size() > 0)
            shortest_path[key] = value[0];
    }
    for (auto [key, value] : non_terminals.asKeyValueRange())
    {
        for (Path& i_path : value)
        {
            if (shortest_path[key].length > i_path.length)
                shortest_path[key] = i_path;
        }
    }
}

//=============== Анализ путей и слов на наличие в них нетерминалов ======================================

bool CF_Grammar::GotNonTerminal(const Path& Current_Path)
{
    for (const QString& i_string : Current_Path.word)
    {
        if (non_terminals.contains(i_string))
        {
            return true;
        }
    }
    return false;
}

bool CF_Grammar::GotNonTerminal(const QVector<QString>& Word)
{
    for (const QString& i_string : Word)
    {
        if (non_terminals.contains(i_string))
        {
            return true;
        }
    }
    return false;
}

//=============== Печать грамматики ======================================================================

QString CF_Grammar::PrintGrammar(bool IsDebug, bool ShowPath)
{
    int counter = 0;
    QString debugMsg;

    debugMsg += "Grammar:\n";
    debugMsg += "Non-Terminals:\n";

    for(auto [key, value]: non_terminals.asKeyValueRange())
    {
        debugMsg += key;
        if (counter != non_terminals.size() - 1)
            debugMsg += ", ";
        counter++;
    }

    counter = 0;
    debugMsg += "\nTerminals:\n";
    for (const QString& i_string : terminals)
    {
        debugMsg += i_string;
        if (counter != terminals.size() - 1)
            debugMsg += ", ";
        counter++;
    }

    debugMsg += "\nRules:\n";
    for (Rule& i_rule : rules)
    {
        debugMsg += QString(10, ' ') + i_rule.left_part + " -> ";
        for (QString& i_string : i_rule.right_part)
        {
            debugMsg += i_string;
        }
        if (IsDebug)
        {
            debugMsg += "           || terminals_count: " + QString::number(i_rule.terminals_count);
        }
        debugMsg += '\n';
    }

    counter = 0;
    debugMsg += "Bad non-terminals";
    if (bad_non_terminals.size() > 0)
    {
        debugMsg += ": ";
        for (const QString& i_string : bad_non_terminals)
        {
            debugMsg += i_string;
            if (counter != bad_non_terminals.size() - 1)
                debugMsg += ", ";
            counter++;
        }
       debugMsg += '\n';
    }
    else debugMsg += " not found\n";

    if (ShowPath)
    {
        debugMsg += "Pathes:\n";
        for (auto [key, value] : non_terminals.asKeyValueRange())
        {
            for (Path i_path : value)
                debugMsg += i_path.PrintPath();
        }
    }

    debugMsg += QString::number(pathes_amount) + " pathes\n";

    //counter = 0;

    if (IsDebug)
    {
        for (Path& i_element : shortest_path)
        {
            debugMsg += "Shortest path for " + i_element.path_words[0][0] + " is " + QString::number(i_element.length) + '\n';
        }
    }

    //qDebug() << debugMsg;

    return debugMsg;
}

//=============== Генерация рандомных слов и взаимодействие со словами ===================================

QPair<QString, int> CF_Grammar::GenerateWord(int Max_Length)
{
    QString result;
    QVector<QString> word;
    QVector<QString> final_word;
    QVector<Rule> appliable_rules;
    Path current_word_path;
    int rule_to_use = 0;
    int temp_int = 0;
    int complexity = 0;
    size_t expected_length = 0;
    size_t actual_length = 0;
    bool non_terminal_found = 0;

    // Начальный вид слова
    word.push_back(starting_non_terminal);
    current_word_path.path_words.push_back(QVector<QString>({ starting_non_terminal }));
    expected_length = shortest_path[starting_non_terminal].length;
    actual_length = 1;

    // Заполнить список правил, которые можно применить
    for (Rule& i_rule : rules)
    {
        if (i_rule.left_part != starting_non_terminal)
            break;
        appliable_rules.push_back(i_rule);
    }

    // Применение случайных правил к случайным нетерминалам
    while (expected_length < Max_Length)
    {
        // Рандомный выбор номера правила
        rule_to_use = rand() % appliable_rules.size();
        complexity += appliable_rules[rule_to_use].complexity;

        // Применение правила
        word = ApplyRule(word, appliable_rules[rule_to_use]);

        current_word_path.path_rules.push_back(appliable_rules[rule_to_use]);
        current_word_path.path_words.push_back(word);
        current_word_path.word = word;
        current_word_path.length++;

        // Проверка наличия нетерминалов в слове
        if (!GotNonTerminal(word)) break;

        // Изменение ожидаемой длины слова
        actual_length += appliable_rules[rule_to_use].right_part.size() - 1;
        expected_length += appliable_rules[rule_to_use].terminals_count;
        expected_length -= shortest_path[appliable_rules[rule_to_use].left_part].length;
        for (QString& i_string : appliable_rules[rule_to_use].right_part)
        {
            if (non_terminals.contains(i_string))
                expected_length += shortest_path[i_string].length;
        }
        if (actual_length > Max_Length) break;

        // Составление нового списка возможных для применения правил
        appliable_rules.clear();
        for (QString& i_string : word)
        {
            if (non_terminals.contains(i_string))
            {
                non_terminal_found = 0;
                for (Rule& i_rule : rules)
                {
                    if (i_rule.left_part == i_string)
                    {
                        non_terminal_found = 1;
                        appliable_rules.push_back(i_rule);
                        temp_int = std::max(temp_int, (int)i_rule.terminals_count);
                    }
                    else if (non_terminal_found && i_rule.left_part != i_string) break;
                }
            }
        }
        if (temp_int == 0) break;
        temp_int = 0;
    }

    // Доведение слова до конца
    final_word = word;
    while (GotNonTerminal(final_word))
    {
        for (QString& i_string : word)
        {
            if (non_terminals.contains(i_string))
            {
                appliable_rules.clear();
                non_terminal_found = 0;
                for (Rule& i_rule : rules)
                {
                    if (non_terminal_found && i_rule.left_part != i_string) break;
                    if (i_rule.left_part == i_string && !GotNonTerminal(i_rule.right_part)){
                        non_terminal_found = 1;
                        appliable_rules.push_back(i_rule);
                    }
                }
                if (appliable_rules.size() > 1){
                    rule_to_use = rand() % appliable_rules.size();
                    Rule i_rule = appliable_rules[rule_to_use];
                    final_word = ApplyRule(final_word, i_rule);

                    current_word_path.path_rules.push_back(i_rule);
                    current_word_path.path_words.push_back(final_word);
                    current_word_path.word = final_word;
                    current_word_path.length++;
                }
                else{
                    for (Rule& i_rule : shortest_path[i_string].path_rules) /////////////RANDOM????
                    {
                        final_word = ApplyRule(final_word, i_rule);

                        current_word_path.path_rules.push_back(i_rule);
                        current_word_path.path_words.push_back(final_word);
                        current_word_path.word = final_word;
                        current_word_path.length++;
                    }
                }
            }
        }
    }
    word = final_word;

    for (QString& i_string : word)
    {
        result += i_string;
    }

    if (!words.contains(result))
    {
        words.insert(result);
    }

    return QPair<QString, int>(result, complexity);
}

QVector<QString> CF_Grammar::GenerateMultipleWords(int Amount, int Max_Length)
{
    QVector<QString> result;
    int iterations = 0;
    int words_size;
    QString temp_str;

    while (words.size() < Amount)
    {
        words_size = (int)words.size();
        temp_str = GenerateWord(Max_Length).first;
        result.push_back(temp_str);

        if (words_size == words.size()) iterations++;
        else iterations = 0;

        if (iterations > Amount)
        {
            for (Path& i_path : non_terminals[starting_non_terminal])
            {
                temp_str.clear();
                for (QString& i_string : i_path.word)
                {
                    if(i_string.contains("/*") && i_string.contains("*/"))
                    {
                        QString temp = i_string;
                        temp.remove(0,2);
                        temp.chop(2);
                        temp_str += temp;
                    }
                    else
                        temp_str += i_string;
                }
                if (temp_str == "[EPS]")
                    temp_str = "";
                if (!words.contains(temp_str))
                    words.insert(temp_str);
            }
            break;
        }
    }
    return result;
}

void CF_Grammar::PrintWords(bool IsDebug)
{
    QString debugMsg;
    if (IsDebug)
        for (const QString& i_string : words)
        {
            debugMsg += "word = " + i_string + ", length = " + QString::number(i_string.length()) + '\n';
        }
    debugMsg += "Words count = " + QString::number(words.size()) + '\n';

    qDebug() << debugMsg;
}

QSet<QString> CF_Grammar::GetWords()
{
    return words;
}

//=============== Алгоритмы анализа принадлежности слова грамматике =======================================

bool CF_Grammar::CYK_Alg_Modified(const QString& Word)
{
    bool result = false;
    bool temp_bool = false;
    int temp_int = 0;
    int max_right_part_length = 0;
    int index_of_rule = 0;
    int index_of_non_terminal = 0;
    int j = 0;
    QString temp_str;
    QString word = Word;

    if (word.size() == 0)
        word = "[EPS]";

    // a[A][i][j] = true, если из нетерминала А можно вывести подстроку word[i...j - 1]
    QMap<QString, QVector<QVector<bool>>> a;
    for(auto [key, value] : non_terminals.asKeyValueRange()){
        a.insert(key, QVector<QVector<bool>>());
        a[key].resize(2 * word.size() + 1);
        for(int j = 0; j < 2 * word.size() + 1; j++)
            a[key][j].resize(2 * word.size() + 1);
    }
    for(const QString& key : terminals){
        a.insert(key, QVector<QVector<bool>>());
        a[key].resize(2 * word.size() + 1);
        for(int j = 0; j < 2 * word.size() + 1; j++)
            a[key][j].resize(2 * word.size() + 1);
    }

    for (Rule& i_rule : rules)
    {
        max_right_part_length = std::max(max_right_part_length, (int)i_rule.right_part.size());
    }

    // h[A -> alpha][i][j][k] = true, если из префикса длины k правила A -> alpha можно вывести подстроку word[i...j - 1]
    QMap<Rule, QVector<QVector<QVector<bool>>>> h;
    for(const Rule& key : rules){
        h.insert(key, QVector<QVector<QVector<bool>>>());
        h[key].resize(2 * word.size() + 1);
        for (int j = 0; j < 2 * word.size() + 1; j++){
            h[key][j].resize(2 * word.size() + 1);
            for (int k = 0; k < 2 * word.size() + 1; k++)
                h[key][j][k].resize(max_right_part_length + 1);
        }
    }

    for (int i = 0; i <= word.size(); i++)
    {
        // Выводимость из нетерминалов
        for (j = i + 1; j <= word.size() + 1; j++)
        {
            temp_str.clear();
            if (word == "[EPS]")
                temp_str = "";
            else
                for (int r = i; r < std::min(j, (int)word.size()); r++)
                {
                    temp_str += word[r];
                }
            for (auto [key, value] : non_terminals.asKeyValueRange())
            {
                for (Path& i_path : non_terminals[key])
                {
                    if (i_path.word.size() == 0)
                    {
                        a[key][i][i] = true;
                    }
                    if (VectorToString(i_path.word) == temp_str)
                    {
                        a[key][i][j] = true;
                        break;
                    }
                }
            }
        }
        if (a[starting_non_terminal][0][Word.size()]) return true;
        // Выводимость терминалов
        for (const QString& i_string : terminals)
        {
            if (i < word.size() && word[i] == i_string[0] && i_string != "[EPS]")
                a[i_string][i][i + 1] = true;
        }
        for (Rule& i_rule : rules)
        {
            h[i_rule][i][i][0] = true;
        }
    }
    for (int m = 0; m < word.size(); m++)
    {
        for (int i = 0; i < word.size(); i++)
        {
            j = i + m;
            for (int times = 0; times <= non_terminals.size(); times++)
            {
                for (Rule& i_rule : rules)
                {
                    for (int k = 1; k <= i_rule.right_part.size(); k++)
                    {
                        for (int r = i; r <= j + 1; r++)
                        {
                            if (h[i_rule][i][j + 1][k] == true) break;
                            h[i_rule][i][j + 1][k] = (h[i_rule][i][r][k - 1] * a[i_rule.right_part[k - 1]][r][j + 1]);
                            if (!h[i_rule][i][j + 1][k])
                            {
                                temp_bool = true;
                                for (int v = 0; v < k - 1; v++)
                                {
                                    temp_bool &= a[i_rule.right_part[v]][0][0];
                                    if (!temp_bool) break;
                                }
                                if (temp_bool)
                                {
                                    h[i_rule][i][j + 1][k] = h[i_rule][i][j + 1][k] + a[i_rule.right_part[k - 1]][i][j + 1];
                                }
                            }
                        }
                    }
                    if (h[i_rule][i][j + 1][i_rule.right_part.size()] == true)
                    {
                        a[i_rule.left_part][i][j + 1] = true;
                    }
                }
            }
        }
    }

    result = a[starting_non_terminal][0][Word.size()];
    //qDebug() << "word = " + word + ", result = " + QString::number(result);
    return result;
}

int CF_Grammar::IndexOfRule(const Rule& Current_Rule)
{
    return std::max(-1, (int)std::distance(rules.begin(), std::find(rules.begin(), rules.end(), Current_Rule)));
}

QString CF_Grammar::VectorToString(const QVector<QString>& Object)
{
    QString result;

    for (int i = 0; i < Object.size(); i++)
        result += Object[i];

    return result;
}

QVector<Rule> CF_Grammar::NonTerminalRules(const QString& Non_Terminal)
{
    QVector<Rule> result;
    bool found_one = false;
    for (Rule& i_rule : rules)
    {
        if (i_rule.left_part == Non_Terminal)
        {
            result.push_back(i_rule);
            found_one = true;
        }
        else if (!found_one) break;
    }
    return result;
}

//=============== Методы структуры "Правило" ==============================================================

bool Rule::operator==(const Rule& Object) const
{
    if (this->left_part != Object.left_part) return false;
    if (this->right_part.size() != Object.right_part.size()) return false;
    if (this->right_part != Object.right_part) return false;

    return true;
}
bool Rule::operator!=(const Rule& Object) const
{
    if (this->left_part != Object.left_part) return true;
    if (this->right_part.size() != Object.right_part.size()) return true;
    if (this->right_part != Object.right_part) return true;

    return false;
}

bool Rule::operator<(const Rule& Object) const
{
    if (this->left_part < Object.left_part)
        return true;
    return false;
}

Rule::Rule()
{
    terminals_count = 0;
    complexity = 0;
    right_part = QVector<QString>();
}

Rule::Rule(QString Left_Part, QVector<QString> Right_Part, int Complexity)
{
    left_part = Left_Part;
    terminals_count = Right_Part.size();
    for (QString& i_string : Right_Part)
    {
        right_part.push_back(i_string);
    }
    complexity = Complexity;
}

Rule::~Rule()
{
    terminals_count = 0;
    complexity = 0;
    left_part.clear();
    left_part.squeeze();
    right_part.clear();
    right_part.squeeze();
}

void Rule::clear()
{
    terminals_count = 0;
    complexity = 0;
    left_part.clear();
    left_part.squeeze();
    right_part.clear();
    right_part.squeeze();
}

//=============== Методы структуры "Путь" =================================================================

bool Path::operator==(const Path& Object) const
{
    if (this->length != Object.length) return false;
    if (this->path_rules.size() != Object.path_rules.size()) return false;
    for (int i = 0; i < path_rules.size(); i++)
    {
        if (this->path_rules[i] != Object.path_rules[i]) return false;
    }

    return true;
}

bool Path::operator+=(const Path& Object)
{
    Path new_path;
    new_path = *this;
    length += Object.length;
    for (Rule i_rule : Object.path_rules)
    {
        new_path.path_rules.push_back(i_rule);
        new_path.word = ApplyRule(new_path.word, i_rule, 0);
        new_path.path_words.push_back(new_path.word);
    }
    word = new_path.word;
    path_words = new_path.path_words;
    path_rules = new_path.path_rules;
    //pathes_used.push_back(Object);

    return true;
}

Path Path::ApplyPath(const Path& Object, int position)
{
    Path new_path;
    int pos = 0;
    int place = 0;
    new_path = *this;
    Path obj = Object;
    new_path.length += Object.length;
    //new_path.pathes_used.push_back(Object);
    for (Rule i_rule : Object.path_rules)
    {
        for (pos = 0; pos < std::min(obj.path_words[place].size(), obj.path_words[place + 1].size()); pos++)
        {
            if (obj.path_words[place][pos] != obj.path_words[place + 1][pos])
                break;
        }
        new_path.path_rules.push_back(i_rule);
        new_path.word = ApplyRule(new_path.word, i_rule, position + pos);
        new_path.path_words.push_back(new_path.word);
        place++;
    }

    return new_path;
}

QString Path::PrintPath(bool IsDebug)
{
    QString debugMsg;
    debugMsg += path_rules[0].left_part + " -> ";
    for (int i = 1; i < path_words.size(); i++)
    {
        if (i != path_words.size() - 1)
        {
            for (QString i_string : path_words[i])
            {
                debugMsg += i_string;
            }
            debugMsg += " -> ";
        }
        else
            for (QString i_string : this->word)
                debugMsg += i_string;
    }
    debugMsg += " size = " + QString::number(this->word.size()) + '\n';
    //qDebug() << debugMsg;
    return debugMsg;
}

Path::Path()
{
    length = 0;
}

Path::~Path()
{
    length = 0;
    for(Rule& r : path_rules)
        r.clear();
    path_rules.clear();
    path_rules.squeeze();

    for(QVector<QString> v : path_words)
    {v.clear(); v.squeeze();}
    path_words.clear();
    path_words.squeeze();
    word.clear();
    word.squeeze();
}

void Path::clear()
{
    length = 0;
    for(Rule& r : path_rules)
        r.clear();
    path_rules.clear();
    path_rules.squeeze();

    for(QVector<QString> v : path_words)
    {v.clear(); v.squeeze();}
    path_words.clear();
    path_words.squeeze();
    word.clear();
    word.squeeze();
}

//=============== Применение правила к слову ===============================================================
QVector<QString> ApplyRule(const QVector<QString>& String, const Rule& Rule, int Non_Terminal_Number)
{
    QVector<QString> result = String;
    QVector<QString> replace_string;
    int non_terminal_number = 0;
    QVector<QString>::iterator position = std::find(result.begin(), result.end(), Rule.left_part);

    if (Non_Terminal_Number > 0)
        position = result.begin() + Non_Terminal_Number;

    // Составление строки-замены
    for (const QString& i_string : Rule.right_part)
    {
        if (i_string != "[EPS]")
        {
            if(i_string.contains("/*") && i_string.contains("*/"))
            {
                QString temp = i_string;
                temp.remove(0,2);
                temp.chop(2);
                replace_string.push_back(temp);
            }
            else
                replace_string.push_back(i_string);
        }
    }

    // Вставка строки-замены на место нетерминала
    position = result.erase(position);
    for (QString& i_string : replace_string)
    {
        position = result.insert(position, i_string);
        position++;
    }

    return result;
}

//=============== Совмещение двух наборов правил ===========================================================
QMap<QString, QVector<Path>> PathConvergence(const QMap<QString, QVector<Path>>& First_Object, const QMap<QString, QVector<Path>>& Second_Object)
{
    QMap<QString, QVector<Path>> result = First_Object;
    for (auto [key, value] : Second_Object.asKeyValueRange())
    {
        if (!result.contains(key))
            result.insert(key, QVector<Path>());
        for (const Path& i_path : value)
        {
            if (CF_Grammar().IsUniquePath(i_path, Path(), result))
                result[key].push_back(i_path);
        }
    }
    return result;
}

//=============== Содержится ли строка в векторе ===========================================================
bool VecContStr(const QVector<QString>& Vector, const QString& String)
{
    if (std::find(Vector.begin(), Vector.end(), String) != Vector.end())
        return true;
    return false;
}

//=============== Генерация и проверка слов двух грамматик =================================================
QString EquivalenceTest(const CF_Grammar& Grammar1, const CF_Grammar& Grammar2, int Words_Lenght, int Words_Count)
{
    CF_Grammar grammar1 = Grammar1;
    CF_Grammar grammar2 = Grammar2;

    QSet<QString> words;
    QVector<QString> incorrect_words;

    bool temp_bool = false;
    float temp_float = 0;
    float grammar1_in_grammar2 = 0;
    float grammar2_in_grammar1 = 0;
    int number_of_words = Words_Count;
    QString debugMsg;

    // Timer timer;
    // timer.reset();

    // Генерируем слова в первой грамматике и проверяем их выводимость во второй
    while (number_of_words < Words_Count * 100 + 1)
    {
        debugMsg += "\nGenerating " + QString::number(number_of_words) + " words in 1 grammar...\n";
        temp_float = 0;
        grammar1.GenerateMultipleWords(number_of_words, Words_Lenght);
        words.clear();
        words = grammar1.GetWords();

        for (const QString& i_string : words)
        {
            temp_bool = grammar2.CYK_Alg_Modified(i_string);
            if (!temp_bool)
                incorrect_words.push_back(i_string);
            else
                temp_float++;
        }

        if (incorrect_words.size() > 0)
        {
            debugMsg += "Second grammar can't produce these words:\n";
            for (QString& i_string : incorrect_words)
            {
                if (i_string == "")
                    debugMsg += "' '\n";
                else
                    debugMsg += i_string + '\n';
            }
            break;
        }
        else
            debugMsg += "Second grammar can produce all " + QString::number(words.size()) + " words!\n";
        number_of_words *= 10;
    }

    grammar1_in_grammar2 = 100 * temp_float / words.size();
    number_of_words = Words_Count;
    incorrect_words.clear();
    words.clear();

    //debugMsg += "Generation and checking took: " << timer.elapsed() << std::endl;
    // timer.reset();

    // Генерируем слова во второй грамматике и проверяем их выводимость в первой
    while (number_of_words < Words_Count * 100 + 1)
    {
        debugMsg += "\nGenerating " + QString::number(number_of_words) + " words in 2 grammar...\n";
        temp_float = 0;
        grammar2.GenerateMultipleWords(number_of_words, Words_Lenght);

        words = grammar2.GetWords();

        for (const QString& i_string : words)
        {
            temp_bool = grammar1.CYK_Alg_Modified(i_string);
            if (!temp_bool)
                incorrect_words.push_back(i_string);
            else
                temp_float++;
        }

        if (incorrect_words.size() > 0)
        {
            debugMsg += "First grammar can't produce these words:\n";
            for (QString& i_string : incorrect_words)
                debugMsg += i_string + '\n';
            break;
        }
        else
            debugMsg += "First grammar can produce all " + QString::number(words.size()) + " words!\n";
        number_of_words *= 10;
    }

    grammar2_in_grammar1 = 100 * temp_float / words.size();
    incorrect_words.clear();
    words.clear();

    //debugMsg += "Generation and checking took: " << timer.elapsed() << std::endl;

    debugMsg += '\n';
    debugMsg += "Grammar 2 can replicate " + QString::number(grammar1_in_grammar2) + "% of grammar 1 words\n";
    debugMsg += "Grammar 1 can replicate " + QString::number(grammar2_in_grammar1) + "% of grammar 2 words\n";

    //qDebug() << debugMsg;
    return debugMsg;
}


PathPermutations::~PathPermutations()
{
    rule.clear();
    for(QPair<QString, QVector<QString>>& pair : right_part)
    {
        pair.first.clear();
        pair.first.squeeze();
        pair.second.clear();
        pair.second.squeeze();
    }
    right_part.clear();
    right_part.squeeze();
}

void PathPermutations::clear()
{
    rule.clear();
    for(QPair<QString, QVector<QString>>& pair : right_part)
    {
        pair.first.clear();
        pair.first.squeeze();
        pair.second.clear();
        pair.second.squeeze();
    }
    right_part.clear();
    right_part.squeeze();
}
