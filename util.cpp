#include "util.h"
#include <QDebug>

util::util()
{
}

QJsonObject util::readJson(const QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << filename << ": read error.";
        return QJsonObject();//empty
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc(QJsonDocument::fromJson(data, &err));
    if (err.error != QJsonParseError::NoError) {
        qDebug() << err.errorString();
        return QJsonObject();//empty
    }

    return doc.object();
}

void util::writeJson(const QString filename, QJsonObject obj)
{
    QJsonDocument doc(obj);
    QByteArray data(doc.toJson());

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << filename << ": write error.";
        return;
    }
    file.write(data);
    file.close();
}

Conf util::loadConf()
{
    Conf conf;
    QJsonObject confObj = readJson(CONF_JSON);
    QJsonArray pointsArray = confObj["points"].toArray();
    for (int i=0; i<pointsArray.size(); i++) {
        QJsonObject pointConfObj = pointsArray[i].toObject();
        QString tag = pointConfObj["tag"].toString();
        QJsonArray connectAr = pointConfObj["connectTo"].toArray();
        QJsonArray rgbAr = pointConfObj["rgb"].toArray();

        PointConf pc;
        pc.order = i;
        foreach (QJsonValue val, connectAr)
            pc.connectTo << val.toString();
        pc.color = QColor(rgbAr[0].toInt(), rgbAr[1].toInt(), rgbAr[2].toInt());

        conf.points.insert(tag, pc);
    }

    return conf;
}

bool util::askMsgBox(const QString &msg, QMessageBox::Icon icon, QWidget *parent)
{
    QMessageBox msgBox(parent);
    msgBox.setIcon(icon);
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    return (msgBox.exec() == QMessageBox::Ok);
}

void util::prepareDir(QString dirPath)
{
    if (!QDir(dirPath).exists())
        QDir().mkpath(dirPath);
}
