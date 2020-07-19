#include "jsonparser.h"
#include <QDebug>

#define PRESET_JSON "preset.json"

JsonParser::JsonParser(QObject *parent) : QObject(parent)
{
}

QSize JsonParser::loadImgSize(QString json_file)
{
    QJsonObject json = util::readJson(json_file);
    if (json.isEmpty())
        return QSize(-1, -1);

    QJsonObject infoObj = json["info"].toObject();
    QSize imgSize = QSize(infoObj["img_width"].toInt(),
                       infoObj["img_height"].toInt());
    return imgSize;
}

QList<PointInfo> JsonParser::loadAllPoints(QString json_file)
{
    QList<PointInfo> allPoints;
    QJsonObject json = util::readJson(json_file);
    QJsonObject peopleObj = json["people"].toObject();

    foreach (QString key, peopleObj.keys()) {
        int personId = key.toInt();

        QJsonArray pointArray = peopleObj[key].toArray();
        for (int i=0; i<pointArray.size(); i++) {
            QJsonObject pObj = pointArray[i].toObject();
            PointInfo pi = pointObj2Info(pObj, personId);
            allPoints << pi;
        }
    }
    return allPoints;
}

void JsonParser::saveAllPoints(QString json_file, QSize imgSize, QList<PointInfo> allPoints)
{
    if (json_file.isEmpty())
        return;

    QJsonObject saveObj, infoObj, peopleObj;

    //prepare info
    infoObj["img_height"] = QJsonValue(imgSize.height());
    infoObj["img_width"] = QJsonValue(imgSize.width());

    //prepare people
    QHash<int, QJsonArray> personArrays;
    foreach (PointInfo pi, allPoints) {
        if (!personArrays.contains(pi.personId))
            personArrays.insert(pi.personId, QJsonArray());

        QJsonObject pointObj = pointInfo2Obj(pi);
        personArrays[pi.personId].append(QJsonValue(pointObj));
    }
    foreach (int personId, personArrays.keys()) {
        peopleObj[QString::number(personId)] = QJsonValue(personArrays[personId]);
    }

    saveObj["info"] = QJsonValue(infoObj);
    saveObj["people"] = QJsonValue(peopleObj);

    util::writeJson(json_file, saveObj);
}

QList<PointInfo> JsonParser::loadPreset(int presetNo, QSize imgSize)
{
    QList<PointInfo> presetPoints;
    QJsonObject json = util::readJson(PRESET_JSON);
    if (json.isEmpty())
        return presetPoints;

    QJsonObject presetObj = json["preset"].toObject();
    if (presetObj.isEmpty() || !presetObj.contains(QString::number(presetNo)))
        return presetPoints;

    /*
     * presetObjは、{"preset No" : [preset points]}の形
     * [preset points]はそれぞれperson1人分
     * preset point中のx, yはimgSizeに対する相対値
     */
    QJsonArray pointsArray = presetObj[QString::number(presetNo)].toArray();
    for (int i=0; i<pointsArray.size(); i++) {
        QJsonObject obj = pointsArray[i].toObject();
        int personId = 0;//ダミー
        PointInfo pi;
        pi.personId = personId;
        pi.tag = obj["tag"].toString();
        pi.pos = QPoint(obj["x"].toDouble() * imgSize.width(),
                        obj["y"].toDouble() * imgSize.height());
        if (obj["r"].isDouble())
            pi.r = obj["r"].toDouble() * imgSize.height();
        presetPoints << pi;
    }
    return presetPoints;
}

void JsonParser::savePreset(int presetNo, QList<PointInfo> pointList, QSize imgSize)
{
    QJsonObject presetObj;
    QJsonObject json = util::readJson(PRESET_JSON);
    if (!json.isEmpty()) {
        presetObj = json["preset"].toObject();
    } else {
        qDebug() << "--> create new json.";
    }

    QString presetKey = QString::number(presetNo);
    if (presetObj.contains(presetKey))
        presetObj.remove(presetKey);

    QJsonArray presetPoints;
    foreach (PointInfo pi, pointList) {
        //画像サイズに対する相対座標に変換
        double rel_x, rel_y, rel_r;
        rel_x = (double)pi.pos.x() / (double)imgSize.width();
        rel_y = (double)pi.pos.y() / (double)imgSize.height();
        rel_r = (double)pi.r / (double)imgSize.height();
        QJsonObject pointObj;
        pointObj["tag"] = QJsonValue(pi.tag);
        pointObj["x"] = QJsonValue(rel_x);
        pointObj["y"] = QJsonValue(rel_y);
        pointObj["r"] = QJsonValue(rel_r);
        presetPoints.append(QJsonValue(pointObj));
    }

    presetObj[presetKey] = QJsonValue(presetPoints);
    QJsonObject saveObj;
    saveObj["preset"] = QJsonValue(presetObj);
    util::writeJson(PRESET_JSON, saveObj);
}

QJsonObject JsonParser::pointInfo2Obj(PointInfo pi)
{
    QJsonObject pointObj;
    pointObj["tag"] = QJsonValue(pi.tag);
    pointObj["x"] = QJsonValue(pi.pos.x());
    pointObj["y"] = QJsonValue(pi.pos.y());
    pointObj["r"] = QJsonValue(pi.r);
    return pointObj;
}

PointInfo JsonParser::pointObj2Info(QJsonObject obj, int personId)
{
    PointInfo pi;
    pi.personId = personId;
    pi.tag = obj["tag"].toString();
    pi.pos = QPoint(obj["x"].toInt(), obj["y"].toInt());
    if (obj["r"].isDouble())
        pi.r = obj["r"].toInt();
    return pi;
}
