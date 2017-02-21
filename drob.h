#ifndef DROB_H
#define DROB_H

#include <iostream>
#include <QHash>
#include <QVector>
#include <QDebug>
#include <QStringList>
#include <QIODevice>
#include <QFile>
#include <QTextStream>
class simpler
{
public:
    simpler();

    QHash<size_t,size_t> convert(size_t a);

private:
    void calc(size_t to);
    size_t size() const;
    size_t operator[](size_t i) const;
    size_t max()const;
    QVector<size_t> simples;
    size_t maxsimum;
};



class udrob//АХТУНГ! класс не знает про отрицательные числа, не ломайте ему психику пожалуйста
{
public:
    udrob(size_t a = 0,size_t b = 1);
    explicit udrob(double a);

    udrob operator*(udrob a);
    udrob operator/(udrob a);
    udrob operator+(udrob a);
    udrob operator-(udrob a);
    udrob& operator *=(udrob& a);
    udrob& operator/=(udrob a);
    udrob& operator+=(udrob a);
    udrob& operator-=(udrob a);
    bool operator ==(const udrob a) const;
    inline bool operator !=(const udrob a) const;
    bool operator >(const udrob a) const;
    inline bool operator >=(const udrob a) const;
    inline bool operator <(const udrob a) const;
    inline bool operator <=(const udrob a) const;

    template<class any>
    friend udrob operator*(any a,udrob b)
    {
        return b*a;
    }
    template<class any>
    friend udrob operator/(any a,udrob b)
    {
        return b/a;
    }
    template<class any>
    friend udrob operator+(any a,udrob b)
    {
        return b+a;
    }
    template<class any>
    friend udrob operator-(any a,udrob b)
    {
        return udrob(a)-b;
    }

    friend QIODevice& operator<<(QIODevice&os,udrob a)
    {
        //return os<<toint(a.up)<<" / "<<toint(a.down);
        os.write(QByteArray((QString::number(toint(a.up))+"/"+QString::number(toint(a.down))).toStdString().c_str()));
        return os;
    }
    friend QDebug operator<<(QDebug os,udrob a)
    {
        QDebug sp = os.space();
        os.nospace();
        os<<toint(a.up)<<"/"<<toint(a.down);
        os.space() = sp;
        return os;
    }
    friend QTextStream& operator<<(QTextStream& os,udrob a)
    {
       os<<toint(a.up)<<"/"<<toint(a.down);
       return os;
    }
    friend QTextStream& operator>>(QTextStream& is,udrob& a)
    {
        size_t buff;
        is>>buff;
        a.up = udrob::s.convert(buff);
        is.seek(is.pos()+1);
        is>>buff;
        a.down = udrob::s.convert(buff);
        udrob::socr(a.up,a.down);
        return is;
    }
    friend QIODevice& operator>>(QIODevice&is,udrob& a)
    {
        QStringList buff = QString(is.read(100)).split('/');
        a.up = udrob::s.convert(buff[0].toInt());
        a.down = udrob::s.convert(buff[1].toInt());
        udrob::socr(a.up,a.down);
        return is;
    }

    operator QString()
    {
        size_t u = toint(up);
        size_t d = toint(down);
        if(!u && !d)
        {
            return "NaN";
        }
        if(!d)
        {
            return "Inf";
        }
        if(!u)
        {
            return "0";
        }
        if(d == 1)
        {
            return QString::number(u);
        }
        return QString::number(u)+"/"+QString::number(d);
    }
    bool isInf()
    {
        return down.contains(0);
    }

private:
    udrob(QHash<size_t,size_t>a,QHash<size_t,size_t>b);

    static QHash<size_t,size_t> ob(QHash<size_t,size_t>a,QHash<size_t,size_t>b);
    static QHash<size_t,size_t> su(QHash<size_t,size_t>a,QHash<size_t,size_t>b);
    static QHash<size_t,size_t> mi(QHash<size_t,size_t>a,QHash<size_t,size_t>b);

    static QHash<size_t,size_t> pr(QHash<size_t,size_t>a,QHash<size_t,size_t>&b);
    static void socr(QHash<size_t,size_t>&a,QHash<size_t,size_t>&b);
    static size_t toint(const QHash<size_t,size_t>&a);

    static simpler s;
    QHash<size_t,size_t> up,down;
};




#endif // DROB_H
