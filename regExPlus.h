#ifndef REGEXPLUS_H
#define REGEXPLUS_H

#include <QString>
#include <QStringList>

struct sLetter
{
    QString value;
    bool havePow;
    bool isIntPow;
    int intPow;
    QString chPow;

    sLetter();
    sLetter(const QString& in);

    void addPow(const QString& in);
};

int combineInBlock(QVector<sLetter> &block, int pos);
QString reduce(const QString &language, const QStringList &sigma);

#endif // REGEXPLUS_H
