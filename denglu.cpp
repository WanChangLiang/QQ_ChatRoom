#include "denglu.h"
#include "ui_denglu.h"
#include<QIcon>
#include<QToolButton>
#include"widget.h"
#include <QMessageBox>

denglu::denglu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::denglu)
{
    ui->setupUi(this);
    //设置图标
    //路径：冒号+前缀+路径
    this->setWindowIcon(QIcon(":/image/4.jpeg"));
    //设置名称
    this->setWindowTitle("QQ2024");

    QList<QString> namelist;
    namelist << "小飞侠" <<"摩托怪咖" <<"死宅" << "经济人" <<"精神弱";
    QStringList iconNamelist;
    iconNamelist << "1" <<"2" << "3" <<"5" <<"6";


    QVector<QToolButton *> vector;
    for(int i =0;i<5;i++)
    {
        QToolButton *btn = new QToolButton(this);
        //加载图标
        btn->setIcon(QPixmap(QString(":/image/%1.jpeg").arg(iconNamelist[i])));
        //设置图片大小
        btn->setIconSize(QPixmap(QString(":/image/%1.jpeg").arg(iconNamelist[i])).size());
        //设置网名
        btn->setText(QString("%1").arg(namelist[i]));
        //设置图片和文字同时显示
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        //设为透明
        ui->vlayout->addWidget(btn);
        vector.push_back(btn);
        IsShow.push_back(false);
    }
    for (int i = 0; i < 5; i++)
    {
        connect(vector[i],&QToolButton::clicked,[=](){
            if (IsShow[i])
            {
                QMessageBox::warning(this,"警告","该聊天框已建立，请勿重复建立！");
                return;
            }
            IsShow[i] = true;
            Widget *widget = new Widget(nullptr,vector[i]->text());
            widget->setWindowIcon(vector[i]->icon());
            widget->setWindowTitle(vector[i]->text());
            widget->show();
            //关闭聊天窗口时，将IsShow变为false
            connect(widget,&Widget::closeWidget,this,[=](){
                IsShow[i] = false;
            });

        });

    }
}

denglu::~denglu()
{
    delete ui;
}
