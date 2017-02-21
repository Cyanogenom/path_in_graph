#include "coder.h"
#include <QCryptographicHash>
#include<QChar>
#include<QString>
#include<QDebug>

const char key = 66;
QByteArray coder(QByteArray a)
{
    QByteArray res;
    unsigned long long sum = 0;
    for(char curr:a)
    {
        sum+= curr;
        res.append(char(key+curr));
    }
    qDebug()<<sum;
    return QCryptographicHash::hash(QString::number(sum).toLocal8Bit(),QCryptographicHash::Md5).toStdString().c_str()+QString("\r\n").toLocal8Bit()+res;
}

QByteArray decoder(QByteArray a, bool *ok)
{
    QByteArray res;
    char buff;
    unsigned long long sum = 0;
    QTextStream s(&a);
    QString hash = s.readLine();
    while(!s.atEnd())
    {
        s>>buff;
        res.append(char(buff-key));
        sum += (char(buff-key));
    }
    qDebug()<<"he";
    //if(ok) *ok = (hash==QString::number(sum));
    QByteArray newhash = QCryptographicHash::hash(QString::number(sum).toLocal8Bit(),QCryptographicHash::Md5);
    QTextStream b(&newhash);
    QString bufff;
    //b.setCodec("ANSI");
    bufff = b.readLine();
    if(ok)*ok = (hash == bufff);
    return res;
}
