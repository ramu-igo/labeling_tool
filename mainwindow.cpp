#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphics_view_zoom.h"
#include "jsonparser.h"
#include <QDebug>
#include <QPainter>
#include <QThread>
#include <QTimer>
#include <QGraphicsSceneMouseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fileMgr(new FileMgr(this)),
    topZ(0),
    keyA_isPressed(false)
{
    ui->setupUi(this);
    Graphics_view_zoom *zoom = new Graphics_view_zoom(ui->graphicsView);
    zoom->set_modifiers(Qt::NoModifier);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);//ドラッグでスクロール

    ui->graphicsView->setScene(&scene);

    conf = util::loadConf();

    QStringList tags = conf.points.keys();
    tags.sort();
    ui->listWidget->addItems(tags);

    setEnabledControl(false);

    //ホイールイベントをview_zoomに送る前に、this->eventFilterでチェック(pointのサイズ調整のため)
    qApp->installEventFilter(this);

    QTimer::singleShot(500, this, SLOT(on_actionOpen_Dir_triggered()));
}

MainWindow::~MainWindow()
{
    if (!fileMgr->curJsonPath().isEmpty()) {
        //初期状態でなければ、現状のsceneを保存
        saveScene(fileMgr->curJsonPath());
    }

    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Wheel && !scene.selectedItems().isEmpty()) {
        QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
        foreach (QGraphicsItem *item, scene.selectedItems()) {
            CustomPoint *selectedPoint = (CustomPoint *)item;
            selectedPoint->recvWheelEvent(wheel_event);

            //接続ポリゴンの再描画
            foreach (CustomLine *line, lines) {
                if (line->isConnectedTo(selectedPoint)) {
                    line->drawPolygon();
                }
            }
        }
        return true;
    }
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *key_event = static_cast<QKeyEvent*>(event);
        if (key_event->key() == Qt::Key_U  //"UP"
            && !key_event->isAutoRepeat()
            && !scene.selectedItems().isEmpty()) {
            foreach (QGraphicsItem *item, scene.selectedItems()) {
                CustomPoint *selectedPoint = (CustomPoint *)item;
                repaintPoint(selectedPoint);
            }
            return true;
        }

        if (key_event->key() == Qt::Key_A //"add"
            && !key_event->isAutoRepeat()) {
            keyA_isPressed = true;
        }
    }

    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *key_event = static_cast<QKeyEvent*>(event);
        if (key_event->key() == Qt::Key_A //"add"
            && !key_event->isAutoRepeat()) {
            keyA_isPressed = false;
        }
    }

    if (event->type() == QEvent::GraphicsSceneMouseDoubleClick ||
        (event->type() == QEvent::GraphicsSceneMousePress && keyA_isPressed)
       ) {
        //ダブルクリックした場所に、選択中tagでpointを追加
        //"A"押しながらシングルクリックでも可
        if (ui->listWidget->selectedItems().count() != 1)
            return false;

        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
        QPointF _p = mouseEvent->scenePos();

        PointInfo pi;
        pi.personId = ui->personSpinBox->value();
        pi.tag = ui->listWidget->selectedItems()[0]->text();
        pi.pos = QPoint(_p.x(), _p.y());
        pi.r = 12;
        addPoint(pi);

        return true;
    }

    return false;
}

void MainWindow::setupScene(QString img_file, QString json_file, bool export_mode)
{
    ui->filenameLabel->setText(img_file);
    ui->personSpinBox->setValue(1);//初期状態はperson1
    ui->fileSlider->setValue(fileMgr->curIndex());
    ui->countLabel->setText(QString("%1 / %2").arg(fileMgr->curIndex()+1).arg(fileMgr->maxIndex()+1));

    QPixmap pixmap;
    if (export_mode) {
        //編集済みの座標データから画像サイズだけ取得し、黒背景画像を生成
        QSize imgSize = JsonParser::loadImgSize(json_file);
        if (!imgSize.isValid())
            return;
        pixmap = QPixmap(imgSize);
        pixmap.fill(QColor(255, 255, 255));//背景の白画像
    } else {
        //通常の編集モード
        QImage image(img_file);
        pixmap = QPixmap::fromImage(image);
    }
    curImgSize = pixmap.size();
    QGraphicsPixmapItem *image_item = new QGraphicsPixmapItem(pixmap);
    scene.addItem(image_item);
    image_item->setZValue(IMAGE_Z_VALUE);

    QList<PointInfo> allPoints = JsonParser::loadAllPoints(json_file);
    foreach (PointInfo pi, allPoints) {
        pi.isExport = export_mode;
        addPoint(pi);
    }

    if (export_mode) {
        drawBodyPolygon();
    }
}

void MainWindow::saveScene(QString json_file)
{
    QList<PointInfo> allPoints;
    foreach (CustomPoint *point, points) {
        PointInfo pi;
        pi.personId = point->personId();
        pi.tag = point->tag();
        pi.pos = point->center();
        pi.r = point->info.r;
        allPoints << pi;
    }
    JsonParser::saveAllPoints(json_file, curImgSize, allPoints);
}

void MainWindow::clearScene()
{
    lines.clear();
    points.clear();
    scene.clear();
    topZ = 0;
}

void MainWindow::cancelScene()
{
    clearScene();
    setupScene(fileMgr->curImgPath(), fileMgr->curJsonPath());
}

void MainWindow::drawBodyPolygon()
{
    /*
     * エクスポート時にbody部分を塗るための処理
     * TODO: ここで使用する点の種類も設定ファイルで指定可能にしたい
     */

    QMap<int, CustomPoint *> pointMap;

    int needPoint = 4;
    int pointCount = 0;
    foreach (CustomPoint *point, points) {
        if (point->tag() == "L-sho") {
            pointMap.insert(0, point);
            pointCount++;
        } else if (point->tag() == "L-hip") {
            pointMap.insert(1, point);
            pointCount++;
        } else if (point->tag() == "R-hip") {
            pointMap.insert(2, point);
            pointCount++;
        } else if (point->tag() == "R-sho") {
            pointMap.insert(3, point);
            pointCount++;
        }
    }

    if (pointCount != needPoint)
        return;

    //z値は接続点で最小のものに合わせる
    QPolygonF _poly;
    qreal _z = pointMap[0]->zValue();
    foreach (CustomPoint *point, pointMap.values()) {
        _poly << point->center();
        if (point->zValue() < _z)
            _z = point->zValue();
    }

    QGraphicsPolygonItem *polygon = new QGraphicsPolygonItem();
    polygon->setPolygon(_poly);
    polygon->setZValue(_z);
    polygon->setPen(QPen(Qt::NoPen));
    polygon->setBrush(QBrush(skinColor));
    scene.addItem(polygon);
}

void MainWindow::addPoint(PointInfo pi)
{   
    if (!conf.points.contains(pi.tag)) {
        qDebug() << "unknown point !";
        return;
    }

    pi.color = conf.points[pi.tag].color;
    pi.order = conf.points[pi.tag].order;
    pi.imgSize = curImgSize;
    pi.z = topZ;
    topZ++;
    CustomPoint *point = new CustomPoint(&scene, pi);
    points << point;

    foreach (QString connectTo, conf.points[pi.tag].connectTo) {
        //->接続先が現在のscene上にあれば、接続 (同じ人物に属する場合に限る)
        foreach (CustomPoint *_p, points) {
            if (point->personId() == _p->personId() &&
                connectTo == _p->tag()) {
                lines << new CustomLine(point, _p);
            }
        }
    }
}

void MainWindow::delPoint(CustomPoint *point)
{
    qDebug() << "DEL : " << point->personId() << point->tag();
    //対象のpointをつないでいるlineがあれば先に削除
    foreach (CustomLine *line, lines) {
        if (line->isConnectedTo(point)) {
            line->clearPolygon();
            lines.removeOne(line);
            scene.removeItem(line);
        }
    }

    points.removeOne(point);
    scene.removeItem(point);
}

void MainWindow::repaintPoint(CustomPoint *point)
{
    //指定の点を一度消し、同じ情報で再描画 --> 最前面に移動
    qDebug() << "REPAINT : " << point->personId() << point->tag();
    PointInfo pi = point->info;
    pi.pos = point->center();
    delPoint(point);
    addPoint(pi);
}

void MainWindow::delSelectedPoints()
{
    foreach (QGraphicsItem *item, scene.selectedItems()) {
        CustomPoint *selectedPoint = (CustomPoint *)item;
        delPoint(selectedPoint);
    }
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    //ダブルクリックしたtagで新しい点を追加 (personはspinboxで選択中のID)
    PointInfo pi;
    pi.personId = ui->personSpinBox->value();
    pi.tag = item->text();
    pi.pos = QPoint(curImgSize.width()/2, curImgSize.height()/2);
    addPoint(pi);
}

void MainWindow::on_actionOpen_Dir_triggered()
{
    QString path = QFileDialog::getExistingDirectory(this, "Open Dir");
    if (path.isEmpty())
        return;//cancel

    if (!ui->buttonGroupBox->isEnabled()) {
        //初回
        setEnabledControl(true);
    } else {
        //2回目以降
        saveScene(fileMgr->curJsonPath());
        clearScene();
    }

    fileMgr->setDir(path);
    ui->fileSlider->setMaximum(fileMgr->maxIndex());

    setupScene(fileMgr->curImgPath(), fileMgr->curJsonPath());
}

void MainWindow::on_prevButton_clicked()
{
    if (!fileMgr->hasPrev())
        return;

    saveScene(fileMgr->curJsonPath());//現在のを保存
    clearScene();

    fileMgr->prev();
    setupScene(fileMgr->curImgPath(), fileMgr->curJsonPath());
}

void MainWindow::on_nextButton_clicked()
{
    if (!fileMgr->hasNext())
        return;

    saveScene(fileMgr->curJsonPath());//現在のを保存
    clearScene();

    fileMgr->next();
    setupScene(fileMgr->curImgPath(), fileMgr->curJsonPath());
}

void MainWindow::on_saveButton_clicked()
{
    saveScene(fileMgr->curJsonPath());
}

void MainWindow::on_actionDelPoint_triggered()
{
    delSelectedPoints();
}

void MainWindow::on_cancelButton_clicked()
{
    cancelScene();
}

void MainWindow::on_actionSaveScene_triggered()
{
    on_saveButton_clicked();
}

void MainWindow::setEnabledControl(bool mode)
{
    ui->listWidget->setEnabled(mode);
    ui->buttonGroupBox->setEnabled(mode);
    ui->personSpinBox->setEnabled(mode);
    ui->graphicsView->setEnabled(mode);
    ui->fileSlider->setEnabled(mode);
}

void MainWindow::on_actionExport_triggered()
{
    if (util::askMsgBox("エクスポートを実行しますか?") == false)
        return;

    qDebug() << "Export start...";
    setEnabledControl(false);
    saveScene(fileMgr->curJsonPath());
    fileMgr->setIndex(0);//first image

    while (true) {
        clearScene();
        setupScene(fileMgr->curImgPath(), fileMgr->curJsonPath(), true);

        QPixmap exportImage(curImgSize);
        QPainter painter(&exportImage);
        QRect rect = QRect(QPoint(0, 0), curImgSize);
        scene.render(&painter, rect, rect);
        painter.end();
        exportImage.save(fileMgr->curExportPath());

        if (fileMgr->hasNext())
            fileMgr->next();
        else
            break;

        this->repaint();
    }

    cancelScene();
    setEnabledControl(true);
    qDebug() << "Export finish !";
}

void MainWindow::on_fileSlider_sliderReleased()
{
    int newIndex = ui->fileSlider->value();
    if (newIndex == fileMgr->curIndex())
        return;

    saveScene(fileMgr->curJsonPath());//現在のを保存
    clearScene();

    fileMgr->setIndex(newIndex);//スライダー移動先の画像へ移動
    setupScene(fileMgr->curImgPath(), fileMgr->curJsonPath());
}

void MainWindow::on_actionEditCancel_triggered()
{
    cancelScene();
}

void MainWindow::on_presetPutButton_clicked()
{
    //登録済みのpreset pointsを読み出し、person-spinboxで指定中の
    //personに属する点として、scene上に配置する
    int presetNo = ui->presetSpinBox->value();
    QList<PointInfo> presetPoints = JsonParser::loadPreset(presetNo, curImgSize);

    int targetPersonId = ui->personSpinBox->value();
    foreach (PointInfo pi, presetPoints) {
        pi.personId = targetPersonId;
        addPoint(pi);
    }
}

void MainWindow::on_presetSetButton_clicked()
{
    //現在のscene上の点で、person-spinboxで指定中のpersonに属する点たちを
    //1setのpresetとして登録。 presetNoはpreset-spinboxの値
    int targetPersonId = ui->personSpinBox->value();
    int presetNo = ui->presetSpinBox->value();

    QString msg = QString("Person%1の各点をプリセット%2に登録します\r\n"
                          "よろしいですか?").arg(targetPersonId).arg(presetNo);
    if (util::askMsgBox(msg) == false)
        return;

    QList<PointInfo> presetPoints;
    foreach (CustomPoint *point, points) {
        if (point->personId() == targetPersonId) {
            PointInfo pi;
            pi.personId = point->personId();
            pi.tag = point->tag();
            pi.pos = point->center();
            pi.r = point->info.r;
            presetPoints << pi;
        }
    }

    JsonParser::savePreset(presetNo, presetPoints, curImgSize);
}

void MainWindow::on_actionSelectAll_triggered()
{
    //tag選択中なら、そのtagの点のみ全選択
    QString target_tag = "";
    if (ui->listWidget->selectedItems().count() == 1)
        target_tag = ui->listWidget->selectedItems()[0]->text();

    foreach (CustomPoint *point, points) {
        if (!target_tag.isEmpty() && point->tag() != target_tag)
            continue;

        point->setSelected(true);
    }
}
