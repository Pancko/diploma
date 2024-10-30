#ifndef REGEXPLUS_H
#define REGEXPLUS_H

#include <QString>
#include <QStringList>

struct Letter
{
    QString value;
    bool havePow;
    bool isIntPow;
    int intPow;
    QString chPow;

    Letter();
    Letter(const QString& in);
};

int combineInBlock(QVector<Letter> &block, int pos);

#endif // REGEXPLUS_H
