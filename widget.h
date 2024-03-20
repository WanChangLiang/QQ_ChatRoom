#ifndef WIDGET_H
#define WIDGET_H
#include <QUdpSocket>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent,QString name);
    void closeEvent(QCloseEvent *);
    enum Msgtype{Msg,UserEnter,UserLeft};   //枚举，分别代表普通信息，用户进入，用户离开
    void sndMsg(Msgtype tpye);      //广播UDP消息
    QString getName();      //获取名字
    QString getMsg();       //获取消息
    void userEnter(QString username);       //处理用户进入
    void userLeft(QString username,QString time);       //处理用户退出
    void ReceiveMessage();      //接收消息
    ~Widget();
signals:
    void closeWidget();

private:
    Ui::Widget *ui;
    QString myname;
    quint16 port;
    QUdpSocket *udpSocket;
};
#endif // WIDGET_H
