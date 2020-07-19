#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QListWidgetItem>
#include "custompoint.h"
#include "customline.h"
#include "util.h"
#include "filemgr.h"

#define IMAGE_Z_VALUE -9999

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_actionOpen_Dir_triggered();

    void on_prevButton_clicked();

    void on_nextButton_clicked();

    void on_saveButton_clicked();

    void on_actionExport_triggered();

    void on_actionDelPoint_triggered();

    void on_cancelButton_clicked();

    void on_actionSaveScene_triggered();

    void on_fileSlider_sliderReleased();

    void on_actionEditCancel_triggered();

    void on_presetPutButton_clicked();

    void on_presetSetButton_clicked();

    void on_actionSelectAll_triggered();

private:
    Ui::MainWindow *ui;
    FileMgr *fileMgr;
    QGraphicsScene scene;
    Conf conf;

    void setEnabledControl(bool mode);

    void setupScene(QString img_file, QString json_file, bool export_mode = false);
    void saveScene(QString json_file);
    void clearScene();
    void cancelScene();

    void drawBodyPolygon();

    void addPoint(PointInfo pi);
    void delPoint(CustomPoint *point);
    void repaintPoint(CustomPoint *point);
    void delSelectedPoints();

    int topZ;

    bool keyA_isPressed;

    //現在のscene上の情報等
    QSize curImgSize;
    QList<CustomPoint *> points;
    QList<CustomLine *> lines;
};

#endif // MAINWINDOW_H
