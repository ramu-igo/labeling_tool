#ifndef FILEMGR_H
#define FILEMGR_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QFileDialog>

class FileMgr : public QObject
{
    Q_OBJECT
public:
    explicit FileMgr(QObject *parent = 0);

    void setDir(QString dirPath);

    bool hasNext();
    bool hasPrev();
    void next();
    void prev();

    void setIndex(int index);
    int curIndex();
    int maxIndex();

    QString curImgPath();
    QString curJsonPath();
    QString curExportPath();

signals:

public slots:

private:
    QString imgDirPath, jsonDirPath, exportDirPath;
    QStringList imgFileList;
    int imgFileIndex;
};

#endif // FILEMGR_H
