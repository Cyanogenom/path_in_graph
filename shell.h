#ifndef SHELL_H
#define SHELL_H

#include"the_main_main_window.h"
#include<QPushButton>
#include<QLineEdit>
#include<QLabel>
#include<QVBoxLayout>
#include<QCheckBox>
#include<QValidator>
#include<QRegExp>
#include<QRegExpValidator>
#include"coder.h"
#include<QFile>
#include<QFileDialog>
#include<QMessageBox>
#include"graph.h"
#include<QCloseEvent>
class spinforcost:public QLineEdit
{
    Q_OBJECT
public:
    spinforcost():QLineEdit()
    {
        exp.setPattern("^([0-9]+|[0-9]+\\.[0-9]+)($|\\/([0-9]+|[0-9]+\\.[0-9]+)$)");
        val.setRegExp(exp);
        setValidator(&val);
        connect(this,SIGNAL(textChanged(QString)),this,SLOT(r()));
    }
public slots:
    void r()
    {
        if(hasAcceptableInput())
        {
            exp.indexIn(text());
            double down, up = exp.cap(1).toDouble();
            if(!exp.cap(3).isEmpty())
                down = exp.cap(3).toDouble();
            else
                down = 1;
            emit newNum(udrob(up)/udrob(down));
        }
    }

signals:
    void keypressed(QKeyEvent*);
    void keyreleased(QKeyEvent*);
    void newNum(cost_t);
private:
    QRegExpValidator val;
    QRegExp exp;
    void keyPressEvent(QKeyEvent*event)
    {
        emit keypressed(event);
        if(event->key() == Qt::Key_Control)
        {
            QPalette buff = this->palette();
            buff.setBrush(QPalette::Active,QPalette::Base,Qt::lightGray);
            this->setPalette(buff);
            update();
        }
        QLineEdit::keyPressEvent(event);

    }
    void keyReleaseEvent(QKeyEvent*event)
    {
        emit keyreleased(event);
        if(event->key() == Qt::Key_Control)
        {
            QPalette buff = this->palette();
            buff.setBrush(QPalette::Active,QPalette::Base,Qt::white);
            this->setPalette(buff);
            update();
        }
        QLineEdit::keyReleaseEvent(event);
    }
};

class shell:public QWidget
{
    Q_OBJECT
public:
    shell(QString filename = QString()):QWidget(),filename(filename)
    {

        btn_new = new QPushButton("Создать");
        btn_open = new QPushButton("Открыть");
        btn_save = new QPushButton("Сохранить");
        btn_saveas = new QPushButton("Сохранить как");
        paint = new Paint_view(grid);
        defer = new spinforcost();

        defer_set = new QPushButton("Рассчитать кратчайший путь");
        defer_header = new QLabel("");
        defer_header->setFixedHeight(27);
        QVBoxLayout * vbox = new QVBoxLayout;
        QHBoxLayout * tools = new QHBoxLayout;
        tools->addWidget(btn_new);
        tools->addWidget(btn_open);
        tools->addWidget(btn_save);
        tools->addWidget(btn_saveas);

        vbox->addLayout(tools);
        vbox->addWidget(defer_header);
        QFrame * frame  = new QFrame();
        frame->setFrameStyle(QFrame::Box | QFrame::Plain);
        QVBoxLayout * buff = new QVBoxLayout;
        buff->setSpacing(0);
        buff->setMargin(0);
        buff->addWidget(paint);
        frame->setLayout(buff);
        vbox->addWidget(frame,1);
        frame->setLineWidth(1);

        defer_set->setDisabled(1);
        vbox->addWidget(defer);
        vbox->addWidget(defer_set);
        defer_set->setFocusPolicy(Qt::NoFocus);
        btn_new->setFocusPolicy(Qt::NoFocus);
        btn_open->setFocusPolicy(Qt::NoFocus);
        btn_save->setFocusPolicy(Qt::NoFocus);
        btn_saveas->setFocusPolicy(Qt::NoFocus);
        connect(defer,SIGNAL(keypressed(QKeyEvent*)),paint,SLOT(keyPressEvent(QKeyEvent*)));
        connect(defer,SIGNAL(keyreleased(QKeyEvent*)),paint,SLOT(keyReleaseEvent(QKeyEvent*)));
        connect(paint,SIGNAL(selected(cost_t,QString,r2d2point,r2d2point)),this,SLOT(selected(cost_t,QString,r2d2point,r2d2point)));
        connect(paint,SIGNAL(changed()),this,SLOT(clear_res()));
        connect(defer_set,SIGNAL(clicked()),this,SLOT(calc()));
        connect(defer,SIGNAL(newNum(cost_t)),paint,SLOT(setDefaultWeight(cost_t)));
        connect(paint,SIGNAL(deselected()),defer_header,SLOT(clear()));
        connect(paint,SIGNAL(changed()),defer_header,SLOT(clear()));

        connect(btn_save,SIGNAL(clicked()),this,SLOT(save()));
        connect(btn_saveas,SIGNAL(clicked()),this,SLOT(saveas()));
        connect(btn_new,SIGNAL(clicked()),this,SLOT(new_pressed()));
        connect(btn_open,SIGNAL(clicked()),this,SLOT(open()));
        connect(paint,SIGNAL(deselected(bool)),defer_set,SLOT(setDisabled(bool)));
        setLayout(vbox);

        need_rewrite_graph = true;
        need_resave = false;
        retitle();
    }
public slots:
    void open(QString filen = QString())
    {
        if(iegova_savior("Открытие файла графа") != QMessageBox::Cancel)
        {
            if(filen.isEmpty())
                filen = QFileDialog::getOpenFileName(this,"Выберите файл для открытия",QString(),"graph files(*.graph)");

            if(!filen.isEmpty())
            {
                QFile file(filen);
                if(file.open(QIODevice::ReadOnly))
                {

                    bool ok;
                    QByteArray arr = decoder(file.readAll(),&ok);
                    if(ok)
                    {
                        paint->fromarray(arr);
                        filename = filen;
                        need_resave = false;
                        retitle();
                    }
                    else
                    {
                        QMessageBox::QMessageBox::critical(this,"Невозможно открыть файл","Зафиксированно повреждение файла");
                    }
                }
                else
                {
                    QMessageBox::warning(this,"Ошибка чтения","Не удалось открыть файл!");
                }
            }
        }
    }

private:
    grid_net grid;
    graph gra;
    QHash<QPair<r2d2point,r2d2point>,graph::task_t> results;
    Paint_view * paint;
    QPushButton* defer_set;
    spinforcost* defer;
    QLabel * defer_header;
    bool need_rewrite_graph;
    QString filename;
    bool need_resave;
    QPushButton *btn_save,*btn_saveas,*btn_new,*btn_open;

    QPair<r2d2point,r2d2point> genkey(r2d2point a,r2d2point b)
    {
        return (a.first<b.first || (a.first == b.first && a.second < b.second))?QPair<r2d2point,r2d2point>(a,b):QPair<r2d2point,r2d2point>(b,a);
        }
private slots:
    void selected(cost_t,QString mess,r2d2point a,r2d2point b)
        {
        QPair<r2d2point,r2d2point> key = genkey(a,b);
        defer_set->setEnabled(1);
        if(results.contains(key) && results[key].second.isInf())
        {
            mess += "\nИ между ними в принципе нет пути";
        }
        else
        {
            mess += "\nКратчайший путь между ними ";
            mess+=(results.contains(key))?(" равен "+QString(results[key].second)):QString("пока не был просчитан");
            if(results.contains(key))
                paint->setWay(results[genkey(a,b)].first);
        }
        defer_header->setText(mess);
        //defer->setText(QString(cost));
    }
    void clear_res()
    {
        results.clear();
        need_rewrite_graph = true;
        need_resave = true;
        retitle();
    }
    void retitle()
    {
        setWindowTitle(((filename.isNull())?QString("Untitled"):filename)+((need_resave)?QString("*"):QString()));
    }
    void calc()
    {
        if(!results.contains(genkey(paint->start(),paint->stop())))
        {
            grareset();
            if(gra.thereIsWay(paint->start(),paint->stop()))
            {
                QHash<id_t,graph::task_t> res = gra.calc(paint->start());
                for(id_t& curr: res.keys())
                {
                    results[genkey(curr,paint->start())] = res[curr];
                }
                for(id_t& curr: gra.get_vertexes())
                    if(! results.contains(genkey(curr,paint->start())))
                        results[genkey(curr,paint->start())] = graph::task_t(QVector<id_t>(),number(1,0));
            }
            else
            {
                results[genkey(paint->stop(),paint->start())] = graph::task_t(QVector<id_t>(),number(1,0));
            }
        }
        //paint->setWay(results[genkey(paint->stop(),paint->start())].first);
        paint->calc_line();
    }
    void saveas()
    {
        QString name = QFileDialog::getSaveFileName(this,"Выберите файл для сохранения",QString(),"graph files(*.graph)");
        if(!name.isEmpty())
        {
            filename = name;
            save();
        }
    }
    void save()
    {
        if(filename.isEmpty())
        {
            saveas();
        }
        else
        {
            QFile file(filename);
            if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
            {
                QMessageBox::warning(this,"Ошибка записи","Не удалось открыть файл!");
                return;
            }
            //qDebug()<<gra.toStr();
            grareset();
            file.write(coder(gra.toStr().toStdString().c_str()));
            file.close();
            need_resave = false;
            retitle();
        }
    }
    void new_pressed()
    {
        if(iegova_savior("Создание нового графа") != QMessageBox::Cancel)
        {
            paint->clear();
            filename = QString();
            need_resave = false;
            retitle();
        }
    }

    QMessageBox::StandardButton iegova_savior(QString tit)
    {
        if(need_resave)
        {
            QMessageBox::StandardButton res = QMessageBox::question(this,
                                                                    tit,
                                                                    "Несохранённые данные будут утеряны!\nЖелаете сохранить?",
                                                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                                                    QMessageBox::Cancel);
            if(res == QMessageBox::Yes)
                save();
            return res;
        }
        return QMessageBox::Yes;
    }
    void closeEvent(QCloseEvent*event)
    {
        if(iegova_savior("Подтверждение закрытия") != QMessageBox::Cancel)
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }

    void grareset()
    {
        if(need_rewrite_graph)
        {
            need_rewrite_graph = false;
            grid.gengra(gra);
        }
    }
};

#endif // SHELL_H
