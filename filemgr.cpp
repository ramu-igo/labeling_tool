#include "filemgr.h"
#include "util.h"
#include <QDebug>

FileMgr::FileMgr(QObject *parent)
    : QObject(parent)
{
}

void FileMgr::setDir(QString dirPath)
{
    imgDirPath = dirPath;
    if (imgDirPath.endsWith('/'))
        imgDirPath.chop(1);//末尾が'/'の場合削除

    //json用フォルダは[画像フォルダ名 + "_json"]とする
    jsonDirPath = imgDirPath + QString("_json");
    util::prepareDir(jsonDirPath);

    //export用フォルダは[画像フォルダ名 + "_export"]とする
    exportDirPath = imgDirPath + QString("_export");
    util::prepareDir(exportDirPath);

    QDir imgDir(imgDirPath);
    imgFileList = imgDir.entryList(QStringList(), QDir::Files);//全ファイル(フォルダは含まない)

    imgFileIndex = 0;//first image

    qDebug() << "****************";
    qDebug() << "img dir    : " << imgDirPath;
    qDebug() << "json dir   : " << jsonDirPath;
    qDebug() << "img count  : " << imgFileList.count();
    qDebug() << "first img  : " << curImgPath();
    qDebug() << "first json : " << curJsonPath();
    qDebug() << "****************";
}

bool FileMgr::hasNext()
{
    return (imgFileIndex + 1 < imgFileList.count());
}

bool FileMgr::hasPrev()
{
    return (imgFileIndex > 0);
}

void FileMgr::next()
{
    if (hasNext())
        imgFileIndex++;
}

void FileMgr::prev()
{
    if (hasPrev())
        imgFileIndex--;
}

void FileMgr::setIndex(int index)
{
    imgFileIndex = index;
}

int FileMgr::curIndex()
{
    return imgFileIndex;
}

int FileMgr::maxIndex()
{
    return imgFileList.size() - 1;
}

QString FileMgr::curImgPath()
{
    if (imgFileList.isEmpty())
        return QString();

    QString file = imgFileList[imgFileIndex];
    return imgDirPath + "/" + file;
}

QString FileMgr::curJsonPath()
{
    if (imgFileList.isEmpty())
        return QString();

    //jsonファイルは対応する画像ファイルと同名で, 拡張子だけ変更
    QString file = imgFileList[imgFileIndex];
    QString base = file.split('.')[0];
    file = base + ".json";
    return jsonDirPath + "/" + file;
}

QString FileMgr::curExportPath()
{
    if (imgFileList.isEmpty())
        return QString();

    //対応する画像ファイルと同名で, exportDirへ
    QString file = imgFileList[imgFileIndex];
    return exportDirPath + "/" + file;
}
