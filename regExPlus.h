#ifndef REGEXPLUS_H
#define REGEXPLUS_H

#include <QString>
#include <QStringList>

struct sLetter
{
    QVector<sLetter> *parent;
    QVector<sLetter> brackets;
    QString value;
    bool isPointer;
    bool isBrackets;
    bool havePow;
    bool isIntPow;
    int intPow;
    QString chPow;

    sLetter();
    sLetter(const QString& in);

    void addPow(const QString& in);

    bool operator==(const sLetter& Object) const;
    bool operator!=(const sLetter& Object) const;
};

int combineInBlock(QVector<sLetter> &block, int pos);
QString reduce(const QString &language, const QStringList &sigma);

#endif // REGEXPLUS_H
