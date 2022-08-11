#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include  <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QCryptographicHash> //加密
#include <QMessageBox>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_createroom_clicked()
{
    m_cur_opt = create_room;
    Req_login("create_room");

}

void MainWindow::Req_login()
{
    //取出用户名密码
    QString username = ui ->username->text();
    QString password = ui ->password->text();

    //构造某个算法的签名生成器
    QCryptographicHash md5_hash(QCryptographicHash::Md5);

    //添加原始数据到生成器中
    md5_hash.addData(password.toUtf8());
    //调用生成函数得到数字签名
    QString md5_password(md5_hash.result().toHex());

    //拼登录请求(Json格式)
    //构建一个json对象 填入用户名和密码的加密值
    QJsonObject json;
    json["username"] = username;
    json["password"] = md5_password;

    //将json对象放到doc中
    QJsonDocument json_doc(json);

    QString output = json_doc.toJson();

    //发给登录服务器(发http -> qt网络访问库)
    QNetworkRequest req(QUrl("http://192.168.81.128/login/"));
    //QNetworkRequest req(QUrl("http://10.3.113.175:7777/login/"));
    req.setHeader(QNetworkRequest::ContentLengthHeader, output.size());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_reply = m_access_mng.post(req, output.toUtf8());
    //异步处理服务器的回复(connect 服务器回复和回复的处理函数)
    connect(m_reply, SIGNAL(finished()), this, SLOT(proc_login_reply()));
}

void MainWindow::Req_login(QString _opt)
{
    //取出用户名密码
    QString username = ui ->username->text();
    QString password = ui ->password->text();

    //构造某个算法的签名生成器
    QCryptographicHash md5_hash(QCryptographicHash::Md5);

    //添加原始数据到生成器中
    md5_hash.addData(password.toUtf8());
    //调用生成函数得到数字签名
    QString md5_password(md5_hash.result().toHex());

    //拼登录请求(Json格式)
    //构建一个json对象 填入用户名和密码的加密值
    QJsonObject json;
    json["username"] = username;
    json["password"] = md5_password;

    // 将参数填入 opt 键
    json["opt"] = _opt;

    //将json对象放到doc中
    QJsonDocument json_doc(json);

    QString output = json_doc.toJson();

    //发给登录服务器(发http -> qt网络访问库)
    QNetworkRequest req(QUrl("http://192.168.81.128/login/"));
    //QNetworkRequest req(QUrl("http://10.3.113.175:7777/login/"));
    req.setHeader(QNetworkRequest::ContentLengthHeader, output.size());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_reply = m_access_mng.post(req, output.toUtf8());
    //异步处理服务器的回复(connect 服务器回复和回复的处理函数)
    connect(m_reply, SIGNAL(finished()), this, SLOT(proc_login_reply()));
}

void MainWindow::Req_login(QString _opt, QString _room_no)
{
    //取出用户名密码
    QString username = ui ->username->text();
    QString password = ui ->password->text();

    //构造某个算法的签名生成器
    QCryptographicHash md5_hash(QCryptographicHash::Md5);

    //添加原始数据到生成器中
    md5_hash.addData(password.toUtf8());
    //调用生成函数得到数字签名
    QString md5_password(md5_hash.result().toHex());

    //拼登录请求(Json格式)
    //构建一个json对象 填入用户名和密码的加密值
    QJsonObject json;
    json["username"] = username;
    json["password"] = md5_password;

    // 将参数填入 opt 键
    json["opt"] = _opt;
    json["room_no"] = _room_no;
    //将json对象放到doc中
    QJsonDocument json_doc(json);

    QString output = json_doc.toJson();

    //发给登录服务器(发http -> qt网络访问库)
    QNetworkRequest req(QUrl("http://192.168.81.128/login/"));
    //QNetworkRequest req(QUrl("http://10.3.113.175:7777/login/"));
    req.setHeader(QNetworkRequest::ContentLengthHeader, output.size());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_reply = m_access_mng.post(req, output.toUtf8());
    //异步处理服务器的回复(connect 服务器回复和回复的处理函数)
    connect(m_reply, SIGNAL(finished()), this, SLOT(proc_login_reply()));
}

void MainWindow::proc_login_reply()
{


    //判断登录结果
    QJsonDocument json_doc = QJsonDocument::fromJson(m_reply -> readAll());

    QJsonObject json = json_doc.object();

    if("OK" == json["login_result"].toString())
    {
        if(create_room == m_cur_opt)
        {

            QProcess pro;
            QStringList argv;
            int room_no = json["room_no"].toInt();

            QMessageBox::information(this, "房间号", "房间号" + QString::number(room_no));
            argv.push_back(json["server_IP"].toString());
            argv.push_back(json["server_PORT"].toString());
            pro.startDetached("client.exe", argv, ".");

            qApp ->exit();
        }
        else if(follow_room == m_cur_opt)
        {
            //qDebug() << json["ss"].toString();

            if("OK" == json["follow_result"].toString())
            {
                QProcess pro;
                QStringList argv;
                QString room_no = ui ->room_no-> text();
                argv.push_back(json["server_IP"].toString());
                argv.push_back(json["server _PORT"].toString());
                pro.startDetached("client.exe", argv, ".");

                qApp->exit();
            }
            else{
                QMessageBox::information(this, "跟房失败", "房间不存在");
            }
        }
    }
    else{
        QMessageBox::information(this, "登录失败", "用户名或者密码错误");
    }
}

void MainWindow::on_register_2_clicked()
{
    //启动浏览器 并访问注册界面
    QDesktopServices::openUrl(QUrl("http://192.168.81.128"));
}

void MainWindow::on_follow_clicked()
{
    m_cur_opt = follow_room;
    Req_login("follow_room", ui->room_no->text());
}
