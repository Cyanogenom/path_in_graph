#ifndef BRED_FROM_ME_H
#define BRED_FROM_ME_H

#include <QHash>
#include <QPair>
#include <QVector>

#include"typedefes.h"

class node
{
private:
    QHash<node*,cost_t> costs;

    bool is_main_node;
public:
    node()
    {
        is_main_node = true;
    }

    cost_t get_weight(node* nodik)
    {
        return costs[nodik];
    }
    bool contains(node* nodik)
    {
        return costs.contains(nodik);
    }
    void set_weight(node *nodik, cost_t new_cost)
    {
        costs[nodik] = new_cost;
    }
    bool get_is_main_node()
    {
        return is_main_node;
    }
    void set_is_main_node(bool is)
    {
        is_main_node = is;
    }
    QList<node*> connects()
    {
        return costs.keys();
    }

    void remove_connect(node* nodik)
    {
        costs.remove(nodik);
    }
};

#endif // BRED_FROM_ME_H
