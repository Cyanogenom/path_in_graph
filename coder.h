#ifndef CODER_H
#define CODER_H
#include <QByteArray>

QByteArray coder(QByteArray a);

QByteArray decoder(QByteArray a,bool * ok = nullptr);


#endif // CODER_H
