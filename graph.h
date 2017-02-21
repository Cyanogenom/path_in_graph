#ifndef GRAPH_H
#define GRAPH_H
#include <QVector>
#include <QHash>
#include <QQueue>
#include <QPair>
#include <QSet>
#include <QDebug>
#include <QTextStream>
#include "typedefes.h"
typedef cost_t number;
typedef r2d2point id_t;

QTextStream &operator<<(QTextStream &st, id_t id);
QTextStream &operator>>(QTextStream &st, id_t& id);

class graph
{
private:
    struct node
    {
        QHash<id_t,number> connects;
        node*parent;
        node()
        {
            parent = this;
        }
        inline node* getParent()
        {
            return (parent == this) ? this: parent->getParent();
        }
    };
    QHash<id_t,node*> vertexes;

 public:
typedef QPair<QVector<id_t>,number> task_t;
    void addconnect(id_t a,id_t b,number weight = 1);
    bool thereIsWay(id_t from,id_t to);
    QList<id_t> get_vertexes();
    void clear();
    void check(id_t& a);
    QHash<id_t,task_t> calc(id_t from);
    QString toStr();

};


#endif // GRAPH_H
