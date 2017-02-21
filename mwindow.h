#ifndef MWINDOW_H
#define MWINDOW_H

#include <QPair>

#include <QDebug>

#include "bred_from_me.h"
#include "typedefes.h"
#include "graph.h"


class grid_net
{
private:
    QHash<QPair<coord, coord>, node*> pairs;



    void connect(coord x1, coord y1, coord x2, coord y2, cost_t weight, bool is_main = true)
    {
        r2d2point p1 = r2d2point(x1, y1);
        r2d2point p2 = r2d2point(x2, y2);
        if(!pairs.contains(p1))
        {
            add(x1,y1, is_main);
        }
        if(!pairs.contains(p2))
        {
            add(x2, y2, is_main);
        }
        pairs[p1]->set_weight(pairs[p2], weight);
        pairs[p2]->set_weight(pairs[p1], weight);
    }
    void disconnect(coord x1, coord y1, coord x2, coord y2)
    {
        r2d2point p1 = r2d2point(x1, y1);
        r2d2point p2 = r2d2point(x2, y2);
        if(!pairs.contains(p1))
        {
            add(x1,y1);
        }
        if(!pairs.contains(p2))
        {
            add(x2, y2);
        }
        pairs[p1]->remove_connect(pairs[p2]);
        pairs[p2]->remove_connect(pairs[p1]);

        if(!pairs[p1]->get_is_main_node() && pairs[p1]->connects().empty())
        {
            pairs.remove(p1);
        }
        if(!pairs[p2]->get_is_main_node() && pairs[p2]->connects().empty())
        {
            pairs.remove(p2);
        }
    }

public:
    static coord find_NOD(coord deltx, coord delty)
    {
        while (deltx && delty)
        {
            ((deltx>delty)?deltx:delty) = qMax(deltx,delty)%qMin(deltx,delty);
        }
        return deltx + delty;
    }

    void gengra(graph& res)
    {
        res.clear();
        for(r2d2point &curr: pairs.keys())
        {
            if(pairs[curr]->get_is_main_node())
            {
                res.check(curr);
                for(r2d2point &tek:connects(curr))
                    if(tek.first<curr.first||
                        (tek.first==curr.first && tek.second<=curr.second))
                    {
                        coord deltx = tek.first-curr.first, delty = tek.second-curr.second;
                        coord currx = curr.first, curry = curr.second;
                        cost_t costa = 0;
                        do
                        {
                            costa += pairs[r2d2point(currx,curry)]->get_weight(pairs[r2d2point(currx+deltx,curry+delty)]);
                            currx += deltx;
                            curry += delty;
                        }
                        while(!pairs[r2d2point(currx,curry)]->get_is_main_node());

                        res.addconnect(curr,r2d2point(currx,curry),costa);
                    }
            }
        }
    }
    void clear()
    {
        for(node* curr:pairs)
            delete curr;
        pairs.clear();
    }

    bool isExists(r2d2point p)
    {
        return pairs.contains(p);
    }
    void add(coord x, coord y,bool is_main = true)
    {
        pairs[r2d2point(x,y)] = new node;
        pairs[r2d2point(x,y)]->set_is_main_node(is_main);
    }
    void delete_node(coord x, coord y)
    {
        foreach(node* curr, pairs[r2d2point(x, y)]->connects())
        {
            r2d2point position = pairs.key(curr);
            coord deltax = position.first - x, deltay = position.second - y;
            int k = 1;
            while(!pairs[r2d2point(x+deltax*k, y+deltay*k)]->get_is_main_node())
            {
                k++;
            }

            disconnect_road(x+deltax*k, y+deltay*k, x, y);
        }

        pairs.remove(r2d2point(x, y));
    }

    void add_if_there_is_no_that(coord x, coord y)
    {
        if(!pairs.contains(r2d2point(x,y)))
            pairs[r2d2point(x,y)] = new node;
        pairs[r2d2point(x,y)]->set_is_main_node(true);
    }

    void connect_road(coord x1, coord y1, coord x2, coord y2, cost_t weight)
    {
        coord deltx = x1-x2, delty = y1-y2;

        coord deliver = find_NOD(abs(deltx), abs(delty));

        cost_t tek_weight = weight/deliver;
        coord i = x2,j = y2;
        do
        {

            connect(i, j, i+deltx/deliver, j+delty/deliver, tek_weight, false);
            i+=deltx/deliver, j+=delty/deliver;
        }
        while(( i != x1 || j != y1));
        pairs[r2d2point(x1,y1)]->set_is_main_node(true);
        pairs[r2d2point(x2,y2)]->set_is_main_node(true);
    }
    void disconnect_road(coord x1, coord y1, coord x2, coord y2)
    {
        if(pairs.contains(r2d2point(x1, y1)) && pairs.contains(r2d2point(x2, y2)))
        {
            coord deltx = x1-x2, delty = y1-y2;

            coord deliver = find_NOD(abs(deltx), abs(delty));
            for(coord i = x2,j = y2; i != x1 or j != y1; i+=deltx/deliver, j+=delty/deliver)
            {
                disconnect(i, j, i+deltx/deliver, j+delty/deliver);
            }
        }
    }
    QList<r2d2point> get_coords()
    {
        return pairs.keys();
    }
    node* operator[](r2d2point point)
    {
        return pairs[point];
    }
    QList<r2d2point> connects(r2d2point point)
    {
        QList<r2d2point> res;
        foreach (node* curr, pairs[point]->connects())
            res.append(pairs.key(curr));
        return res;
    }

    cost_t safe_weight(r2d2point a,r2d2point b,bool * ok = nullptr)
    {
        coord nod = find_NOD(abs(a.first - b.first),abs(a.second - b.second));
        r2d2point step((a.first - b.first)/nod,(a.second - b.second)/nod);
        cost_t res = 0;
        r2d2point curr = b;
        while(pairs.contains(curr) && pairs.contains(r2d2point(curr.first+step.first,curr.second+step.second))&&
              pairs[curr]->contains(pairs[r2d2point(curr.first+step.first,curr.second+step.second)]) && curr!=a)
        {
            res += pairs[curr]->get_weight(pairs[r2d2point(curr.first+step.first,curr.second+step.second)]) ;
            curr = r2d2point(curr.first+step.first,curr.second+step.second);
        }
        if(curr == a)
        {
            if(ok)*ok = true;
            return res;
        }
        else
        {
            if(ok)*ok = false;
            res = 1;
            res /= 0;
            return res;
        }
    }

    cost_t get_weight(coord x1, coord y1, coord x2, coord y2)
    {
        coord deltx = x1-x2, delty = y1-y2;

        coord deliver = find_NOD(abs(deltx), abs(delty));
        deltx = deltx/deliver;
        delty = delty/deliver;
        coord currx = x2+deltx, curry = y2+delty;
        cost_t res = 0;
        unsigned int steps = 1;

        while(currx != x1 or curry != y1)
        {
            if(pairs[r2d2point(currx, curry)]->get_is_main_node())
            {
                res += steps*pairs[r2d2point(currx, curry)]->get_weight(pairs[r2d2point(currx-deltx,curry-delty)]);
                steps = 0;
            }
            steps++;
            currx += deltx;
            curry += delty;
        }
        res += steps*pairs[r2d2point(currx, curry)]->get_weight(pairs[r2d2point(currx-deltx,curry-delty)]);
        return res;
    }
};

#endif // MWINDOW_H
