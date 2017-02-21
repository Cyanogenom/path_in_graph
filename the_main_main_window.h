#ifndef THE_MAIN_MAIN_WINDOW_H
#define THE_MAIN_MAIN_WINDOW_H
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "mwindow.h"
#include "typedefes.h"
#include <QPointF>
#include <QWheelEvent>
//#define whall
#include<QFrame>

class Paint_view: public QWidget
{
    Q_OBJECT
private:
    const double max_zoom = 3, min_zoom = 0.9,left_border = 0,top_border = 0;
    const int step = 15, size = 6, wid = 3;
    double scale;
    QPointF shift;
    QPainter painter;
    grid_net &grid;
    r2d2point anchor;
    cost_t def_weight;
    QPen bold, light, boldway,lightway;
    QVector<r2d2point> currres;
    bool move_mode;
    bool build_mode;
    QPointF move_anchor;

    r2d2point reda,grena;
    bool thereisr,thereisg;

    void setBuildMode(bool build)
    {
        build_mode = build;
    }
public:
    Paint_view(grid_net& grid,bool build_mode = true): QWidget(), grid(grid),build_mode(build_mode)
    {
        //QPalette pal = palette();
        //pal.setColor(QPalette::Active,QPalette::Base,Qt::black);
        //pal.setColor(QPalette::Inactive,QPalette::QPalette::Base,Qt::black);
        //setPalette(pal);
        scale = 1;
        shift = QPointF(0,0);
        def_weight = 1;
        bold.setWidthF(wid*scale);
        boldway = bold;
        boldway.setColor(Qt::blue);
        light.setWidth(0);
        lightway = light;
        lightway.setColor(Qt::blue);
        setMinimumSize(300,300);
        move_mode = 0;
        thereisg = 0;
        thereisr = 0;
    }
    id_t start()
    {
        return grena;
    }
    id_t stop()
    {
        return reda;
    }
    void setWay(QVector<r2d2point> way)
    {
        currres = way;
        //calc_line();
        update();
    }
    void clear()
    {
        changed();
        grid.clear();
        currres.clear();
        thereisr = false;
        thereisg = false;
        scale = 1;
        shift = QPointF(0,0);
        update();
    }
    void fromarray(QByteArray arr)
    {
        QTextStream s(&arr);
        size_t num;
        id_t curr,con;
        cost_t drob;
        s>>num;
        clear();
        while (num--)
        {
             s>>curr;
             grid.add(curr.first,curr.second);
        }
        while(!s.atEnd())
        {
            s>>curr;
            s>>con;
            s>>drob;
            grid.connect_road(curr.first,curr.second,con.first,con.second,drob);
            s.skipWhiteSpace();
        }
        update();
    }


public slots:
    void setDefaultWeight(cost_t def)
    {
        def_weight = def;
    }

    void keyPressEvent(QKeyEvent*event)
    {
        static const double stepic = 5;
        switch(event->key())
        {
        case Qt::Key_Right:
            shift.setX(shift.x()-stepic/scale);
            break;
        case Qt::Key_Left:
            shift.setX(shift.x()+stepic/scale);
            break;
        case Qt::Key_Up:
            shift.setY(shift.y()+stepic/scale);
            break;
        case Qt::Key_Down:
            shift.setY(shift.y()-stepic/scale);
            break;
        case Qt::Key_Control:
            if(build_mode)
                build_mode = false;
            break;
        default:
            break;
        }

        update();
    }
    void keyReleaseEvent(QKeyEvent*event)
    {
        if(event->key() == Qt::Key_Control && !build_mode)
        {
            build_mode = true;
        }
    }
    void calc_line()
    {
        if(thereisg && thereisr)
        {
            bool ok;
            cost_t res = grid.safe_weight(reda,grena,&ok);
            emit selected(res,(ok)?("Вес прямой между выбранными вершинами равен "+QString(res)):("Между выбранными вершинами нет прямого пути"),grena,reda);
        }
    }
signals:
    void changed();
    void selected(cost_t w,QString comment,r2d2point a,r2d2point b);
    void deselected(bool);
private:
    r2d2point convert_to_grid_net(QPointF position)
    {
        position += shift;

        return r2d2point((position.x()+((position.x()>=0)?(step*scale/2):(-step*scale/2)))/scale/step,
                         (position.y()+((position.y()>=0)?(step*scale/2):(-step*scale/2)))/scale/step);
    }
    QPointF convert_to_QPoint(r2d2point position)
    {
        return QPointF(((position.first)*scale*step)-shift.x(), (position.second)*scale*step-shift.y());
    }
    void mousePressEvent(QMouseEvent *mevent)
    {
        anchor = convert_to_grid_net(mevent->pos());
        /*
        if(mevent->button() == Qt::LeftButton)
        {
            grid.add_if_there_is_no_that(anchor.first, anchor.second);
        }*/

        switch(mevent->button())
        {
        case Qt::LeftButton:
            if(build_mode)
            {
                grid.add_if_there_is_no_that(anchor.first, anchor.second);
                currres.clear();
                emit changed();
            }
            else
            {
                if(grid.isExists(anchor) && grid[anchor]->get_is_main_node())
                {
                    if(!thereisg || (grena.first!=anchor.first||grena.second!=anchor.second))
                    {
                        if(!thereisr || anchor !=reda)
                        {

                            grena = anchor;
                            currres.clear();
                            thereisg = true;
                        }

                    }
                    else
                    {
                        currres.clear();
                        emit deselected(1);
                        thereisg = false;
                    }
                }
            }
            break;
        case Qt::RightButton:
            if(!build_mode)
            {
                if(grid.isExists(anchor) && grid[anchor]->get_is_main_node())
                {
                    if(!thereisr || (reda.first!=anchor.first||reda.second!=anchor.second))
                    {
                        if(!thereisg || grena!=anchor)
                        {
                            reda = anchor;
                            currres.clear();
                            thereisr = true;
                        }

                    }
                    else
                    {
                        currres.clear();
                        emit deselected(1);
                        thereisr = false;
                    }
                }
            }
            break;
        case Qt::MidButton:
            move_mode = 1;
            move_anchor = mevent->pos();
            break;
        default:
            break;
        }
        update();

    }
    void mouseReleaseEvent(QMouseEvent *mevent)
    {
        r2d2point curr_anchor = convert_to_grid_net(mevent->pos());
        switch(mevent->button())
        {
        case Qt::LeftButton:
            if(build_mode)
            {
                if(curr_anchor.first != anchor.first || curr_anchor.second != anchor.second)
                {
                    grid.connect_road(curr_anchor.first, curr_anchor.second, anchor.first, anchor.second, def_weight);
                    currres.clear();
                    emit changed();
                }
            }
            break;
        case Qt::RightButton:
            if(build_mode)
            {
                if(curr_anchor.first != anchor.first || curr_anchor.second != anchor.second)
                {
                    if(grid.isExists(curr_anchor) && grid.isExists(anchor)
                            && grid[curr_anchor]->get_is_main_node() && grid[anchor]->get_is_main_node())
                    {
                        grid.disconnect_road(curr_anchor.first, curr_anchor.second, anchor.first, anchor.second);
                        currres.clear();
                        emit changed();
                    }
                }
                else
                {
                    if(grid.isExists(curr_anchor) && grid[curr_anchor]->get_is_main_node())
                    {
                        grid.delete_node(curr_anchor.first, curr_anchor.second);
                        check();
                        currres.clear();
                        emit changed();
                    }
                }
            }
            break;
        case Qt::MidButton:
            move_mode = 0;
            break;
        default:
            break;
        }
        update();
        calc_line();
    }
    void paintEvent(QPaintEvent *event)
    {
        painter.begin(this);
        painter.fillRect(event->rect(),Qt::white);
        QPointF shifter;
        for(r2d2point& curr: grid.get_coords())
        {

            if(grid[curr]->get_is_main_node())
            {
                painter.setBrush(QBrush());
                painter.setPen(bold);
            }
            else
            {
                painter.setBrush(Qt::black);
                painter.setPen(light);
            }
            painter.drawEllipse(convert_to_QPoint(curr),size*scale/2,size*scale/2);
            painter.setPen(bold);
            for(r2d2point&cur_cor:grid.connects(curr))
                if(cur_cor.first < curr.first ||
                        (cur_cor.first == curr.first && cur_cor.second < curr.second))
                {
                    shifter = convert_to_QPoint(curr)-convert_to_QPoint(cur_cor);
                    shifter = shifter/sqrt(shifter.x()*shifter.x() + shifter.y()*shifter.y())*size/2*scale;
                    painter.drawLine(convert_to_QPoint(curr)-shifter,convert_to_QPoint(cur_cor)+shifter);
                }

        }
        if(thereisr)
        {
            painter.setBrush(Qt::red);
            painter.drawEllipse(convert_to_QPoint(reda),size*scale/2,size*scale/2);
        }
        if(thereisg)
        {
            painter.setBrush(Qt::green);
            painter.drawEllipse(convert_to_QPoint(grena),size*scale/2,size*scale/2);
        }

        if(currres.size())
        {

            for(long i = currres.size()-1;i;--i)
            {
                painter.setPen(boldway);
                painter.setBrush(Qt::BrushStyle());
                painter.drawEllipse(convert_to_QPoint(currres[i]),size*scale/2,size*scale/2);

                shifter = convert_to_QPoint(currres[i])-convert_to_QPoint(currres[i-1]);
                shifter = shifter/sqrt(shifter.x()*shifter.x() + shifter.y()*shifter.y())*size/2*scale;
                painter.drawLine(convert_to_QPoint(currres[i])-shifter,convert_to_QPoint(currres[i-1])+shifter);
                painter.setBrush(Qt::blue);
                painter.setPen(lightway);
                size_t nod = grid_net::find_NOD(abs(currres[i].first-currres[i-1].first),abs(currres[i].second-currres[i-1].second));
                r2d2point stepic((currres[i].first-currres[i-1].first)/nod,(currres[i].second-currres[i-1].second)/nod);
                for(r2d2point curr(currres[i-1].first+stepic.first,currres[i-1].second+stepic.second);curr!=currres[i];curr.first+=stepic.first,curr.second+=stepic.second)
                {
                    painter.drawEllipse(convert_to_QPoint(curr),size*scale/2,size*scale/2);
                }
            }
            painter.setPen(boldway);
            painter.setBrush(Qt::BrushStyle());
            painter.drawEllipse(convert_to_QPoint(currres[0]),size*scale/2,size*scale/2);
        }
        painter.end();


    }
    void check()
    {
        if(!grid.isExists(grena))
        {
            thereisg = false;
        }
        if(!grid.isExists(reda))
        {
            thereisr = false;
        }
    }


    void mouseMoveEvent(QMouseEvent* event)
    {
        if(move_mode)
        {
            //shift -= convert_to_QPoint(convert_to_grid_net(event->pos())) - convert_to_QPoint(anchor);
            shift -= event->pos() - move_anchor;move_anchor = event->pos();
#ifdef whall
            shift.setX((shift.x()< left_border)?left_border:shift.x());
            shift.setY((shift.y()< top_border)?top_border:shift.y());
#endif
        }
        update();
    }

    void wheelEvent(QWheelEvent *wevent)
    {
        QPointF oldp = (wevent->posF()+shift)/scale;
        scale += static_cast<double>(wevent->delta())/1200;
        scale = (max_zoom<scale) ? max_zoom : (min_zoom>scale) ? min_zoom : scale;
        shift = oldp*scale - wevent->posF();
#ifdef whall
        shift.setX((shift.x() < left_border)?left_border:shift.x());
        shift.setY((shift.y() < top_border)?top_border:shift.y());
#endif
        bold.setWidthF((scale<1)?wid:wid*scale);
        boldway.setWidthF((scale<1)?wid:wid*scale);
        update();
    }
  };

#endif // THE_MAIN_MAIN_WINDOW_H
