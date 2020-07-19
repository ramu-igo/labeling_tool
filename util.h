#ifndef UTIL_H
#define UTIL_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QFile>
#include <QColor>
#include <QHash>
#include <QDir>
#include <QMessageBox>

#define CONF_JSON "config.json"

struct PointConf
{
    QColor color;
    QStringList connectTo;//接続先tagのリスト
    int order;//conf.json中の並び順(色の優先度)
};

struct Conf
{
    QHash<QString, PointConf> points;//tagをキーとし、pointごとに設定を格納
};

class util
{
public:
    util();
    static QJsonObject readJson(const QString filename);
    static void writeJson(const QString filename, QJsonObject obj);
    static Conf loadConf();

    static bool askMsgBox(const QString &msg, QMessageBox::Icon icon = QMessageBox::Question, QWidget *parent = 0);
    static void prepareDir(QString dirPath);
};

#endif // UTIL_H
