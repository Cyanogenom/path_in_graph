#include "drob.h"
simpler udrob::s;
simpler::simpler()
{
    maxsimum = 10;
    simples<<2<<3<<5<<7;
}

void simpler::calc(size_t to)
{
    if(to>maxsimum)
    {
        bool size[to - maxsimum];
        for(size_t i = 0;i < to - maxsimum;i++)
            size[i] = 0;
        for(size_t& curr:simples)
            for(size_t i = 2;to >= i*curr;i++)
                if(i*curr > maxsimum)
                    size[i*curr-maxsimum-1] = 1;
        for(size_t i = 0;i < to - maxsimum;i++)
            if(!size[i])
            {
                size_t curr = i+maxsimum+1;
                simples<<curr;
                for(size_t i = 2;to >= i*curr;i++)
                    if(i*curr > maxsimum)
                        size[i*curr-maxsimum-1] = 1;
            }
        maxsimum = to;
    }

}

QHash<size_t, size_t> simpler::convert(size_t a)
{
    QHash<size_t,size_t> res;
    if(a<=1)
    {
        res[a] = 1;
        return res;
    }
    calc(a);
    while(a > 1)
    {
        for(size_t i = simples.size();i;--i)
        {
            while(!(a%simples[i-1]))
            {
                res[simples[i-1]] += 1;
                a /= simples[i-1];
            }
        }
    }
    return res;
}

size_t simpler::size() const
{
    return simples.size();
}

size_t simpler::operator[](size_t i) const
{
    return simples[i];
}

size_t simpler::max() const
{
    return maxsimum;
}

udrob::udrob(size_t a, size_t b)
{
    down = s.convert(b);
    up = s.convert(a);
}

udrob::udrob(double a)
{
    while(a!=size_t(a))
    {
        down[5]+=1;
        down[2]+=1;
        a*=10;
    }
    up = s.convert(a);
    qDebug()<<up<<down;
    socr(up,down);
}

udrob udrob::operator*(udrob a)
{
    udrob res(pr(a.up,up),pr(a.down,down));
    socr(res.up,res.down);
    return res;
}

udrob udrob::operator/(udrob a)
{
    udrob res(pr(a.down,up),pr(a.up,down));
    socr(res.up,res.down);
    return res;
}

udrob udrob::operator+(udrob a)
{
    udrob res(su(pr(a.up,down),pr(up,a.down)),pr(a.down,down));
    socr(res.up,res.down);
    return res;
}

udrob udrob::operator-(udrob a)
{
    udrob res(mi(pr(up,a.down),pr(a.up,down)),pr(a.down,down));
    socr(res.up,res.down);
    return res;
}

udrob &udrob::operator *=(udrob &a)
{
    up = pr(a.up,up);
    down = pr(a.down,down);
    socr(up,down);
    return *this;
}

udrob &udrob::operator/=(udrob a)
{
    up = pr(a.down,up);
    down = pr(a.up,down);
    socr(up,down);
    return *this;
}

udrob &udrob::operator+=(udrob a)
{
    up = su(pr(up,a.down),pr(a.up,down));
    down = pr(a.down,down);
    socr(up,down);
    return *this;
}

udrob &udrob::operator-=(udrob a)
{
    up = mi(pr(up,a.down),pr(a.up,down));
    down = pr(a.down,down);
    socr(up,down);
    return *this;
}

bool udrob::operator ==(const udrob a) const
{
    return (a.up.contains(0) && up.contains(0)) || (a.up == up && a.down == down);
}

bool udrob::operator !=(const udrob a) const
{
    return !operator==(a);
}

bool udrob::operator >(const udrob a) const//правильно реализовывать влом, так что готовьтесь к весёлым лагам
{
    return double(toint(up))/toint(down) > double(toint(a.up))/toint(a.down);
}

bool udrob::operator >=(const udrob a) const
{
    return operator==(a) || operator>(a);
}

bool udrob::operator <(const udrob a) const
{
    return !operator>=(a);
}

bool udrob::operator <=(const udrob a) const
{
    return !operator>(a);
}


udrob::udrob(QHash<size_t, size_t> a, QHash<size_t, size_t> b)
{
    up = a;
    down = b;
}

QHash<size_t, size_t> udrob::ob(QHash<size_t, size_t> a, QHash<size_t, size_t> b)//and
{
    QHash<size_t,size_t> res;
    for(size_t&curr:a.keys())
        if(b.contains(curr))
        {
            res[curr] = qMin(a[curr],b[curr]);
        }
    return res;
}

QHash<size_t, size_t> udrob::su(QHash<size_t, size_t> a, QHash<size_t, size_t> b)//сумма
{
    QHash<size_t,size_t> buff = ob(a,b);
    socr(a,b);
    return pr(s.convert(toint(a)+toint(b)),buff);
}

QHash<size_t, size_t> udrob::mi(QHash<size_t, size_t> a, QHash<size_t, size_t> b)//разность
{
    QHash<size_t,size_t> buff = ob(a,b);
    socr(a,b);
    return pr(s.convert(toint(a)-toint(b)),buff);
}

QHash<size_t, size_t> udrob::pr(QHash<size_t, size_t> a, QHash<size_t, size_t> &b)//произведение
{
    if(a.contains(0) || b.contains(0))
    {
        a.clear();
        a[0] = 1;
        return a;
    }
    for(size_t& curr:b.keys())
    {
        a[curr] += b[curr];
    }
    if(a.size() != 1 && a.contains(1))
        a.remove(1);
    return a;
}

void udrob::socr(QHash<size_t, size_t> &a, QHash<size_t, size_t> &b)//xor
{
    for(size_t&curr:a.keys())
        if(b.contains(curr) && curr)
        {
            size_t min = qMin(a[curr],b[curr]);
            a[curr]-=min;
            b[curr]-=min;
            if(!a[curr])
                a.remove(curr);
            if(!b[curr])
                b.remove(curr);
        }
    if(!a.size())
        a[1] = 1;
    if(!b.size())
        b[1] = 1;
}

size_t udrob::toint(const QHash<size_t, size_t> &a)
{
    size_t res = 1;
    for(size_t& curr: a.keys())
    {
        for(size_t i = a[curr];i;--i)
        {
            res *= curr;
        }
    }
    return res;
}
