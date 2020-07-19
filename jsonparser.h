#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include "util.h"
#include "custompoint.h"

class JsonParser : public QObject
{
    Q_OBJECT
public:
    explicit JsonParser(QObject *parent = 0);

    //各画像ごとのjsonファイルのload/save
    static QSize loadImgSize(QString json_file);
    static QList<PointInfo> loadAllPoints(QString json_file);
    static void saveAllPoints(QString json_file, QSize imgSize, QList<PointInfo> allPoints);

    //preset
    static QList<PointInfo> loadPreset(int presetNo, QSize imgSize);
    static void savePreset(int presetNo, QList<PointInfo> pointList, QSize imgSize);

signals:

public slots:

private:
    static QJsonObject pointInfo2Obj(PointInfo pi);
    static PointInfo pointObj2Info(QJsonObject obj, int personId);
};

#endif // JSONPARSER_H
