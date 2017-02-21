#include "graph.h"

QTextStream &operator<<(QTextStream &st, id_t id)
{
    return st<<"( "<<id.first<<' '<<id.second<<" )";
}
QTextStream &operator>>(QTextStream &st, id_t& id)
{
    st.skipWhiteSpace();
    st.seek(st.pos()+2);
    st>>id.first;
    st>>id.second;
    st.seek(st.pos()+2);
    return st;
}


void graph::addconnect(id_t a, id_t b, number weight)
{
    check(a);
    check(b);
    vertexes[a]->connects[b] = weight;
    vertexes[b]->connects[a] = weight;
    vertexes[a]->getParent()->parent = vertexes[b]->getParent();
}

bool graph::thereIsWay(id_t from, id_t to)
{
    return vertexes[from]->getParent() == vertexes[to]->getParent();
}

QList<id_t> graph::get_vertexes()
{
    return vertexes.keys();
}

void graph::clear()
{
    for(node* curr: vertexes)
        delete curr;
    vertexes.clear();
}

void graph::check(id_t &a)
{
    if(!vertexes.contains(a))
        vertexes[a] = new node();
}

QHash<id_t, graph::task_t> graph::calc(id_t from)
{
    //if(!thereIsWay(from,to))return 0;

    QQueue<task_t> tasks;
    task_t curr(QVector<id_t>()<<from,0);
    tasks<<curr;
    QHash<id_t,task_t> results;
    results[from] = curr;
    while(!tasks.isEmpty())
    {
        curr = tasks[0];
        tasks.pop_front();
        if(results[curr.first.last()].second == curr.second)
            for(id_t& vertex: vertexes[curr.first.last()]->connects.keys())
                if(! curr.first.contains(vertex) &&
                        (! results.contains(vertex) ||
                         results[vertex].second>curr.second+vertexes[curr.first.last()]->connects[vertex]))
                {
                    task_t buff(QVector<id_t>()<<curr.first<<vertex,curr.second+vertexes[curr.first.last()]->connects[vertex]);
                    results[vertex] = buff;
                    tasks.append(buff);
                }
    }
    return results;
}

QString graph::toStr()
{
    QString ret;
    QTextStream res(&ret);
    res<<vertexes.size()<<"\r\n";
    for(id_t& id:vertexes.keys())
        res<<id<<"\r\n";
    for(id_t& id:vertexes.keys())
    {
        for(id_t& curr:vertexes[id]->connects.keys())
            if(curr.first<id.first || (curr.first == id.first && curr.second < id.second))
                res<<id<<' '<<curr<<' '<<vertexes[id]->connects[curr]<<"\r\n";
    }

    return ret;
}
