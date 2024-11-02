#include "regExPlus.h"

Letter::Letter()
{
    value = nullptr;
    havePow = false;
    isIntPow = false;
    intPow = -1;
    chPow = nullptr;
}

Letter::Letter(const QString& in)
{
    value = in;
    if (!in.contains("<sup>"))
    {
        havePow = false;
        isIntPow = false;
        intPow = -1;
        chPow = nullptr;
        return;
    }
    havePow = true;
    value = in.first(in.indexOf("<sup>"));
    QStringList variants {"+", "*"};
    switch(variants.indexOf(in[in.indexOf("</sup>") - 1]))
    {
    case -1: //число
    {
        isIntPow = true;
        int temp = in.indexOf("<sup>") + 5;
        intPow = in.mid(temp, in.indexOf("</sup>") - temp).toInt(); //a<sup>2</sup>
        chPow = nullptr;
        return;
    }
    case 0: // +
    {
        isIntPow = false;
        intPow = -1;
        chPow = "+";
        return;
    }
    case 1: // *
    {
        isIntPow = false;
        intPow = -1;
        chPow = "*";
        return;
    }
    }
}

int combineInBlock(QVector<Letter>& block, int pos)
{
    Letter *first = &block[pos - 1];
    Letter *second = &block[pos];
    if (!first->havePow && !second->havePow){
        first->havePow = true;
        first->isIntPow = true;
        first->intPow = 2;
        block.remove(pos);
        return pos - 1;
    }
    if (!first->havePow && second->havePow){
        if (second->isIntPow){
            first->havePow = true;
            first->isIntPow = true;
            first->intPow = second->intPow + 1;
            block.remove(pos);
            return pos - 1;
        }
        if (second->chPow == "+"){
            first->havePow = true;
            first->isIntPow = true;
            first->intPow = 2;
            second->chPow = "*";
            return pos;
        }
        return pos;
    }
    if (first->havePow && !second->havePow){
        if (first->isIntPow){
            first->intPow++;
            block.remove(pos);
            return pos - 1;
        }
        if (first->chPow == "+"){
            first->isIntPow = true;
            first->intPow = 2;
            first->chPow = nullptr;
            second->havePow = true;
            second->chPow = "*";
            return pos;
        }
        first->chPow = "+";
        block.remove(pos);
        return pos - 1;
    }
    if (first->isIntPow) {
        if(second->isIntPow){
            first->intPow = second->intPow + first->intPow;
            block.remove(pos);
            return pos - 1;
        }
        if(second->chPow == "+"){
            first->intPow++;
            second->chPow = "*";
            return pos;
        }
        return pos;
    }
    if(first->chPow == "+"){
        if(second->isIntPow){
            first->isIntPow = true;
            first->intPow = second->intPow + 1;
            first->chPow = nullptr;
            second->isIntPow = false;
            second->intPow = -1;
            second->chPow = "*";
            return pos;
        }
        if(second->chPow == "+"){
            first->isIntPow = true;
            first->intPow = 2;
            first->chPow = nullptr;
            second->chPow = "*";
            return pos;
        }
        first->havePow = false;
        first->isIntPow = false;
        first->intPow = -1;
        first->chPow = nullptr;
        return pos;
    }
    if(second->isIntPow){
        first->isIntPow = true;
        first->intPow = second->intPow;
        first->chPow = nullptr;
        second->isIntPow = false;
        second->intPow = -1;
        second->chPow = "*";
        return pos;
    }
    if(second->chPow == "+"){
        first->havePow = false;
        first->intPow = -1;
        first->chPow = nullptr;
        second->chPow = "*";
        return pos;
    }
    block.remove(pos);
    return pos - 1;
}

QString reduce(const QString& lang, const QStringList& sigma)
{
    QString ch;
    QString result = "L = {w ∈ ∑<sup>*</sup> : ";
    QVector<Letter> block;
    for(int i = result.size(); i < lang.size(); i++)
    {
        ch = lang[i];

        if (sigma.contains(ch) && lang[i + 1] != '}') // нашли символ из алфавита (начало блока)
        {
            int pos = i;
            while(true) // читаем очередной блок одинаковых букв
            {
                if(lang[pos] != ch) break;
                if(lang[pos + 1] == '<'){
                    QString temp = lang.mid(pos, lang.indexOf("</sup>", pos) - pos + 6);
                    block.push_back(Letter(temp));
                    pos = lang.indexOf("</sup>", pos) + 6;
                }
                else {
                    block.push_back(Letter(ch));
                    pos++;
                }
            }
            i = pos - 1;
            if(block.size() == 1) {
                result += ch;
                if(block[0].havePow)
                {
                    result += "<sup>";
                    if (block[0].isIntPow)
                        result += QString::number(block[0].intPow);
                    else
                        result += block[0].chPow;
                    result += "</sup>";
                }
            }
            else
            {
                for (int j = 1; j < block.size(); j ++)
                {
                    j = combineInBlock(block, j);
                }
                for (Letter &l: block)
                {
                    if (!l.havePow)
                        result += l.value;
                    else
                    {
                        if (l.isIntPow)
                            result += l.value + "<sup>" + QString::number(l.intPow) + "</sup>";
                        else
                            result += l.value + "<sup>" + l.chPow + "</sup>";
                    }
                }
            }
            block.clear();
        }
        else result += ch;
    }
    return result;
}
