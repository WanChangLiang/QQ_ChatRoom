#include "widget.h"
#include "ui_widget.h"
#include<QDataStream>
#include<QMessageBox>
#include<QDateTime>
#include <QColorDialog>
#include <QFileDialog>

Widget::Widget(QWidget *parent,QString name)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    myname = name;
    this->port = 9999;      //初始化端口
    this->udpSocket = new QUdpSocket(this);     //创建套接字
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);

    connect(udpSocket,&QUdpSocket::readyRead,this,&Widget::ReceiveMessage);     //监听信号

    connect(ui->sendBtn,&QPushButton::clicked,[=](){        //链接发送按钮
        sndMsg(Msg);
    });

    //新用户进入
        sndMsg(UserEnter);

    connect(ui->exitBtn,&QPushButton::clicked,[=]()     //链接退出按钮
    {
        this->close();
    });
        connect(ui->fonCbx,&QFontComboBox::currentFontChanged,[=](const QFont &font){   //改变字体
        ui->msgTextEdit->setFontFamily(font.toString());
        ui->msgTextEdit->setFocus();
    });
    void (QComboBox::* sizebtn)(const QString &text) = &QComboBox::currentTextChanged;
        connect(ui->sizeCbx,sizebtn,[=](const QString &text){                           //链接改变文字大小
        ui->msgTextEdit->setFontPointSize(text.toDouble());
        ui->msgTextEdit->setFocus();
    });
    connect(ui->boldBtn,&QToolButton::clicked,[=](bool checked){
            if(checked)
            {
                ui->msgTextEdit->setFontWeight(QFont::Bold);                            //链接加粗按钮
            }
            else
            {
             ui->msgTextEdit->setFontWeight(QFont::Normal);
            }
        });
    /*connect(ui->italicTbtn->&QToolButton::clicked,[=](bool checked){                //链接倾斜（因为没有设置按键，所以本部分代码注释
        ui->msgTextEdit->setFontItalic(checked);
        ui->msgTextEdit->setFocus();
    });*/

    connect(ui->underlineBtn,&QToolButton::clicked,[=](bool checked){               //链接下划线
        ui->msgTextEdit->setFontUnderline(checked);
        ui->msgTextEdit->setFocus();
    });

    connect(ui->clearBtn,&QToolButton::clicked,[=](){                   //链接清空按钮
        ui->msgBrowser->clear();
    });

    connect(ui->colorBtn,&QToolButton::clicked,this,[=](){
        QColor color = QColorDialog::getColor(color,this);
        ui->msgTextEdit->setTextColor(color);
    });

    connect(ui->saveBtn,&QToolButton::clicked,[=](){            //链接保存按钮
        if(ui->msgBrowser->toPlainText() == "")
        {
            QMessageBox::warning(this,"警告","保存内容不能为空！");
            return;
        }
        QString filename = QFileDialog::getSaveFileName(this,"保存聊天记录","聊天记录","(*.txt)");
        if(!filename.isEmpty())
        {
            QFile file(filename);
            file.open(QIODevice::WriteOnly | QFile::Text);
            QTextStream stream(&file);
            stream<<ui->msgBrowser->toPlainText();
            file.close();
        }
    });
}

void Widget::closeEvent(QCloseEvent *)
{
    emit this->closeWidget();
    sndMsg(UserLeft);
    udpSocket->close();
    udpSocket->destroyed();
}

QString Widget::getName()
{
    return this->myname;
}

QString Widget::getMsg()
{
    QString msg = ui->msgTextEdit->toHtml();
    ui->msgTextEdit->clear();
    ui->msgTextEdit->setFocus();
    return msg;
}
void Widget::ReceiveMessage()
{
    qint64 size = udpSocket->pendingDatagramSize(); //qint64格式转换为int
    int mysize = static_cast<int>(size);
    QByteArray *array = new QByteArray(mysize,0);
    udpSocket->readDatagram((*array).data(),size);
    QDataStream stream(array,QIODevice::ReadOnly);
    int mytype;
    QString name,msg;       //用户名 聊天消息
    QString time = QDateTime::currentDateTime().toString(("yyyy-MM-dd hh:mm:ss"));
    stream>>mytype;
    switch (mytype) {
    case Msg:
        stream>>name>>msg;
        ui->msgBrowser->setTextColor(QColor(Qt::blue));
        ui->msgBrowser->append("["+name+"]"+time);
        ui->msgBrowser->append(msg);
        break;
    case UserLeft:
        stream>>name;
        userLeft(name,time);
        break;
    case UserEnter:
        stream>>name;
        userEnter(name);
        break;
    }


}
void Widget::sndMsg(Widget::Msgtype type)
{
    QByteArray array;
    QDataStream stream(&array,QIODevice::WriteOnly);
    stream<<type<<this->getName();
    switch (type) {
    case Msg:
        if(ui->msgTextEdit->toPlainText()=="")
        {
            QMessageBox::warning(this,"警告","发送的聊天内容不能为空！");
            return;
        }
        stream <<this->getMsg();
        break;
    case UserEnter:
        break;
    case UserLeft:
        break;
    }
    //书写报文
    udpSocket->writeDatagram(array.data(),array.size(),QHostAddress::Broadcast,this->port);
}

void Widget::userEnter(QString username)
{
    bool IsEmpty = ui->tableWidget->findItems(username,Qt::MatchExactly).isEmpty();
    if(IsEmpty)
    {
        QTableWidgetItem *table = new QTableWidgetItem(username);
        ui->tableWidget->insertRow(0);
        ui->tableWidget->setItem(0,0,table);
        ui->msgBrowser->setTextColor(QColor(Qt::gray));
        ui->msgBrowser->append(username+"已上线");
        ui->userNumLbl->setText(QString("在线人数：%1人").arg(ui->tableWidget->rowCount()));
        sndMsg(UserEnter);
    }
}

void Widget::userLeft(QString username,QString time)
{
    bool IsEmpty = ui->tableWidget->findItems(username,Qt::MatchExactly).isEmpty();

    if(!IsEmpty)
    {
        int row = ui->tableWidget->findItems(username,Qt::MatchExactly).first()->row();
        ui->tableWidget->removeRow(row);
        ui->msgBrowser->append(QString("%1用户于%2离开").arg(username).arg(time));
        ui->userNumLbl->setText(QString("在线人数：%1人").arg(ui->tableWidget->rowCount()));
    }
}

Widget::~Widget()
{
    delete ui;
}
