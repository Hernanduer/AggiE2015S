#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new UI::MainWindow) {
    ui->setupUI(this);
    setVector = glm::vec3();
    DbOpen();

    this->setupAcceleration(ui->accel.plot);
    this->setupAcceleration(ui->accelBig.plot);
    this->setupPressure(ui->press.plot);
    this->setupTemperature(ui->temp.plot);
    this->setupDepth(ui->depth.plot);
    this->setupMagnetometer(ui->mag.plot);
    this->setupMagnetometer(ui->magBig.plot);
    this->setupGyroscope(ui->gyro.plot);
    this->setupGyroscope(ui->gyroBig.plot);
    this->setupMap(ui->webView);
    dataParser.setOutputBox(ui->infoBox);
    //setup a timer that repeatedly calls MainWindow::realtimeDataSlot
    connect(&dataTimer, SIGNAL(timeout()), &dataParser, SLOT(transaction()));
    connect(&dataParser, SIGNAL(done(QVector<double>&)), this, SLOT(realtimeDataSlot(QVector<double>&)));
    dataTimer.start(100); //sets interval for recording data in ms, 1000 = every second data is recorded
    Rotation rot = Rotation();
    rot.setData(0,0,180,0,0,0,0,180,0);
    rot.getHeading();
    rot.matUpdate();

    Sleep(999);
    rot.setData(0,180,0,0,180,0,180,0,0);
    rot.getHeading();
    rot.matUpdate();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    double ratioW = event->size().width() / (double)event->oldSize().width();
    double ratioH = event->size().height() / (double)event->oldSize().height();
    if (ratioW > 0 && ratioH > 0)
        ui->resize(ratioW, ratioH);
}

void MainWindow::realtimeDataSlot(QVector<double> &data) {
    // calculate two new data points:
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
    double key = 0;
#else
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif
    static double lastPointKey = 0;
    // at most add point every 10 ms
    if (key-lastPointKey > 0.01) {
        AccelX   = data[0];
        AccelY   = data[1];
        AccelZ   = data[2];
        GyroX    = data[3];
        GyroY    = data[4];
        GyroZ    = data[5];
        MagX     = data[6];
        MagY     = data[7];
        MagZ     = data[8];
        Temp     = data[9];
        Pressure = data[10];
        Depth    = data[11];

        //Transformations on data
        glm::quat accelRot = glm::quat(0,0,0,0);
        accelRot = glm::rotate(accelRot, (float)atan((float)AccelZ/AccelY), glm::vec3(1.0,0,0));
        accelRot = glm::rotate(accelRot, (float)atan((float)AccelZ/AccelX), glm::vec3(0,1.0,0));
        accelRot = glm::rotate(accelRot, (float)atan((float)AccelY/AccelX), glm::vec3(0,0,1.0));
        glm::quat setRot = glm::quat(setVector);
        ui->infoBox->setPlainText(ui->infoBox->toPlainText());
        ui->infoBox->moveCursor(QTextCursor::End);


        into_database();

        updateTridata(ui->accel.plot, key, 8, AccelX, AccelY, AccelZ);
        updateTridata(ui->accelBig.plot, key, 40, AccelX, AccelY, AccelZ);
        updateTridata(ui->press.plot, key, 8, Pressure);
        updateTridata(ui->temp.plot, key, 8, Temp);
        updateTridata(ui->depth.plot, key, 8, Depth);
        updateTridata(ui->mag.plot, key, 8, MagX, MagY, MagZ);
        updateTridata(ui->magBig.plot, key, 40, MagX, MagY, MagZ);
        updateTridata(ui->gyro.plot, key, 8, GyroX, GyroY, GyroZ);
        updateTridata(ui->gyroBig.plot, key, 40, GyroX, GyroY, GyroZ);

        lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->accel.plot->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
    ui->accel.plot->replot();
    ui->accelBig.plot->xAxis->setRange(key+0.25, 40, Qt::AlignRight);
    ui->accelBig.plot->replot();

    ui->press.plot->xAxis->setRange(key+0.25,8,Qt::AlignRight);
    ui->press.plot->replot();

    ui->temp.plot->xAxis->setRange(key+0.25,8,Qt::AlignRight);
    ui->temp.plot->replot();

    ui->depth.plot->xAxis->setRange(key+0.25,8,Qt::AlignRight);
    ui->depth.plot->replot();

    ui->mag.plot->xAxis->setRange(key+.25,8,Qt::AlignRight);
    ui->mag.plot->replot();
    ui->magBig.plot->xAxis->setRange(key+0.25, 40, Qt::AlignRight);
    ui->magBig.plot->replot();

    ui->gyro.plot->xAxis->setRange(key+.25,8,Qt::AlignRight);
    ui->gyro.plot->replot();
    ui->gyroBig.plot->xAxis->setRange(key+0.25, 40, Qt::AlignRight);
    ui->gyroBig.plot->replot();
}

void MainWindow::updateTridata(QCustomPlot* plot, const double&key, const double&offset, const double&x, const double&y, const double&z) {
    //Add new line data
    plot->graph(0)->addData(key, x);
    if (y != 0 || z != 0) {
        plot->graph(1)->addData(key, y);
        plot->graph(2)->addData(key, z);
    }

    //Add new position for point
    if (y != 0 || z != 0) {
        plot->graph(3)->clearData();
        plot->graph(3)->addData(key, x);
        plot->graph(4)->clearData();
        plot->graph(4)->addData(key, y);
        plot->graph(5)->clearData();
        plot->graph(5)->addData(key, z);
    } else {
        plot->graph(1)->clearData();
        plot->graph(1)->addData(key, x);
    }

    //Remove old data
    plot->graph(0)->removeDataBefore(key-offset);
    if (y != 0 && z != 0) {
        plot->graph(1)->removeDataBefore(key-offset);
        plot->graph(2)->removeDataBefore(key-offset);
    }

    plot->rescaleAxes();
}

void MainWindow::setupAcceleration(QCustomPlot *GraphAccel) {
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  QMessageBox::critical(this, "", "You're using Qt < 4.7, the realtime data demo needs functions that are available with Qt 4.7 to work properly");
#endif

  GraphAccel->addGraph(); // blue line
  GraphAccel->graph(0)->setPen(QPen(Qt::blue));

  GraphAccel->addGraph(); // red line
  GraphAccel->graph(1)->setPen(QPen(Qt::red));


  GraphAccel->addGraph(); //green line
  GraphAccel->graph(2)->setPen(QPen(Qt::green));

  GraphAccel->addGraph(); // blue dot
  GraphAccel->graph(3)->setPen(QPen(Qt::blue));
  GraphAccel->graph(3)->setLineStyle(QCPGraph::lsNone);
  GraphAccel->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);

  GraphAccel->addGraph(); // red dot
  GraphAccel->graph(4)->setPen(QPen(Qt::red));
  GraphAccel->graph(4)->setLineStyle(QCPGraph::lsNone);
  GraphAccel->graph(4)->setScatterStyle(QCPScatterStyle::ssDisc);

  GraphAccel->addGraph(); //green dot
  GraphAccel->graph(5)->setPen(QPen(Qt::green));
  GraphAccel->graph(5)->setLineStyle(QCPGraph::lsNone);
  GraphAccel->graph(5)->setScatterStyle(QCPScatterStyle::ssDisc);

  GraphAccel->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  GraphAccel->xAxis->setDateTimeFormat("hh:mm:ss");
  GraphAccel->xAxis->setAutoTickStep(false);
  GraphAccel->xAxis->setTickStep(2);
  GraphAccel->axisRect()->setupFullAxesBox();


  // make left and bottom axes transfer their ranges to right and top axes:
  connect(GraphAccel->xAxis, SIGNAL(rangeChanged(QCPRange)), GraphAccel->xAxis2, SLOT(setRange(QCPRange)));
  connect(GraphAccel->yAxis, SIGNAL(rangeChanged(QCPRange)), GraphAccel->yAxis2, SLOT(setRange(QCPRange)));
}

void MainWindow::setupPressure(QCustomPlot* GraphPress) {
    GraphPress -> addGraph();
    GraphPress -> graph(0)->setPen(QPen(Qt::red));

    GraphPress -> addGraph();
    GraphPress->graph(1)->setPen(QPen(Qt::red));
    GraphPress->graph(1)->setLineStyle(QCPGraph::lsNone);
    GraphPress->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);

    GraphPress->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    GraphPress->xAxis->setDateTimeFormat("hh:mm:ss");
    GraphPress->xAxis->setAutoTickStep(false);
    GraphPress->xAxis->setTickStep(2);
    GraphPress->axisRect()->setupFullAxesBox();

    connect(GraphPress->xAxis, SIGNAL(rangeChanged(QCPRange)), GraphPress->xAxis2, SLOT(setRange(QCPRange)));
    connect(GraphPress->yAxis, SIGNAL(rangeChanged(QCPRange)), GraphPress->yAxis2, SLOT(setRange(QCPRange)));
}

void MainWindow::setupTemperature(QCustomPlot* GraphTemp) {

    GraphTemp -> addGraph();
    GraphTemp->graph(0)->setPen(QPen(Qt::blue));

    GraphTemp->addGraph();
    GraphTemp->graph(1)->setPen(QPen(Qt::blue));
    GraphTemp->graph(1)->setLineStyle(QCPGraph::lsNone);
    GraphTemp->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);

    GraphTemp->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    GraphTemp->xAxis->setDateTimeFormat("hh:mm:ss");
    GraphTemp->xAxis->setAutoTickStep(false);
    GraphTemp->xAxis->setTickStep(2);
    GraphTemp->axisRect()->setupFullAxesBox();

    connect(GraphTemp->xAxis, SIGNAL(rangeChanged(QCPRange)), GraphTemp->xAxis2, SLOT(setRange(QCPRange)));
    connect(GraphTemp->yAxis, SIGNAL(rangeChanged(QCPRange)), GraphTemp->yAxis2, SLOT(setRange(QCPRange)));


}

void MainWindow::setupDepth(QCustomPlot* GraphDepth) {

    GraphDepth->addGraph();
    GraphDepth->graph(0)->setPen(QPen(Qt::black));

    GraphDepth->addGraph();
    GraphDepth->graph(1)->setPen(QPen(Qt::black));
    GraphDepth->graph(1)->setLineStyle(QCPGraph::lsNone);
    GraphDepth->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);

    GraphDepth->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    GraphDepth->xAxis->setDateTimeFormat("hh:mm:ss");
    GraphDepth->xAxis->setAutoTickStep(false);
    GraphDepth->xAxis->setTickStep(2);
    GraphDepth->axisRect()->setupFullAxesBox();

    connect(GraphDepth->xAxis, SIGNAL(rangeChanged(QCPRange)), GraphDepth->xAxis2, SLOT(setRange(QCPRange)));
    connect(GraphDepth->yAxis, SIGNAL(rangeChanged(QCPRange)), GraphDepth->yAxis2, SLOT(setRange(QCPRange)));


}

void MainWindow::setupMagnetometer(QCustomPlot *GraphMag) {
    GraphMag->addGraph();
    GraphMag->graph(0)->setPen(QPen(Qt::blue));
    GraphMag->addGraph();
    GraphMag->graph(1)->setPen(QPen(Qt::red));
    GraphMag->addGraph();
    GraphMag->graph(2)->setPen(QPen(Qt::green));

    GraphMag->addGraph();
    GraphMag->graph(3)->setPen(QPen(Qt::blue));
    GraphMag->graph(3)->setLineStyle(QCPGraph::lsNone);
    GraphMag->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);
    GraphMag->addGraph();
    GraphMag->graph(4)->setPen(QPen(Qt::red));
    GraphMag->graph(4)->setLineStyle(QCPGraph::lsNone);
    GraphMag->graph(4)->setScatterStyle(QCPScatterStyle::ssDisc);
    GraphMag->addGraph();
    GraphMag->graph(5)->setPen(QPen(Qt::green));
    GraphMag->graph(5)->setLineStyle(QCPGraph::lsNone);
    GraphMag->graph(5)->setScatterStyle(QCPScatterStyle::ssDisc);

    GraphMag->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    GraphMag->xAxis->setDateTimeFormat("hh:mm:ss");
    GraphMag->xAxis->setAutoTickStep(false);
    GraphMag->xAxis->setTickStep(2);
    GraphMag->axisRect()->setupFullAxesBox();

    connect(GraphMag->xAxis, SIGNAL(rangeChanged(QCPRange)), GraphMag->xAxis2, SLOT(setRange(QCPRange)));
    connect(GraphMag->yAxis, SIGNAL(rangeChanged(QCPRange)), GraphMag->yAxis2, SLOT(setRange(QCPRange)));


}

void MainWindow::setupGyroscope(QCustomPlot *GraphGyro) {

    GraphGyro->addGraph();
    GraphGyro->graph(0)->setPen(QPen(Qt::blue));
    GraphGyro->addGraph();
    GraphGyro->graph(1)->setPen(QPen(Qt::red));
    GraphGyro->addGraph();
    GraphGyro->graph(2)->setPen(QPen(Qt::green));

    GraphGyro->addGraph();
    GraphGyro->graph(3)->setPen(QPen(Qt::blue));
    GraphGyro->graph(3)->setLineStyle(QCPGraph::lsNone);
    GraphGyro->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);
    GraphGyro->addGraph();
    GraphGyro->graph(4)->setPen(QPen(Qt::red));
    GraphGyro->graph(4)->setLineStyle(QCPGraph::lsNone);
    GraphGyro->graph(4)->setScatterStyle(QCPScatterStyle::ssDisc);
    GraphGyro->addGraph();
    GraphGyro->graph(5)->setPen(QPen(Qt::green));
    GraphGyro->graph(5)->setLineStyle(QCPGraph::lsNone);
    GraphGyro->graph(5)->setScatterStyle(QCPScatterStyle::ssDisc);


    GraphGyro->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    GraphGyro->xAxis->setDateTimeFormat("hh:mm:ss");
    GraphGyro->xAxis->setAutoTickStep(false);
    GraphGyro->xAxis->setTickStep(2);
    GraphGyro->axisRect()->setupFullAxesBox();

    connect(GraphGyro->xAxis, SIGNAL(rangeChanged(QCPRange)), GraphGyro->xAxis2, SLOT(setRange(QCPRange)));
    connect(GraphGyro->yAxis, SIGNAL(rangeChanged(QCPRange)), GraphGyro->yAxis2, SLOT(setRange(QCPRange)));


}

void MainWindow::setupMap(QWebView* map) {
     map->load(QUrl("https://www.google.com/maps/"));
}

void MainWindow::into_database() {
    if (!RoboSquidDB.open()) return;

    QSqlQuery qry;

    qry.prepare("INSERT INTO RoboSquidData(AccelX,AccelY,AccelZ,MagX,MagY,MagZ,GryoX,GryoY,GryoZ,Pressure,Temperature,Depth) VALUES (:AccelX, :AccelY, :AccelZ, :MagX, :MagY, :MagZ, :GryoX, :GryoY, :GryoZ, :Pressure, :Temperature, :Depth)");

    qry.bindValue(":AccelX",AccelX);
    qry.bindValue(":AccelY",AccelY);
    qry.bindValue(":AccelZ",AccelZ);
    qry.bindValue(":MagX",MagX);
    qry.bindValue(":MagY",MagY);
    qry.bindValue(":MagZ",MagZ);
    qry.bindValue(":GryoX",GyroX);
    qry.bindValue(":GryoY",GyroY);
    qry.bindValue(":GryoZ",GyroZ);
    qry.bindValue(":Pressure",Pressure);
    qry.bindValue(":Temperature",Temp);
    qry.bindValue(":Depth",Depth);

    if (qry.exec()) {
        ui->infoBox->setPlainText(ui->infoBox->toPlainText() + "\nData Stored");
    } else {
        //qDebug() << "NOT Stored!";
    }
}

void MainWindow::on_setButton_clicked() {
    setVector = glm::vec3(AccelX, AccelY, AccelZ);
    ui->infoBox->setPlainText(ui->infoBox->toPlainText() + "\nSetting Baseline");
}

void MainWindow::on_exitButton_clicked() {
    exit(0);
}

const bool MainWindow::DbOpen() {
    RoboSquidDB = QSqlDatabase::addDatabase("QSQLITE");
    RoboSquidDB.setDatabaseName("RoboSquid.db");

    if(RoboSquidDB.open()) {
        ui->infoBox->setPlainText(ui->infoBox->toPlainText() + "\nDatabase Opened");
        return true;
    } else {
        ui->infoBox->setPlainText(ui->infoBox->toPlainText() + "\nFailed to Open Database");
        return false;
    }
}

void MainWindow::DbClose() {
    RoboSquidDB.close();
    RoboSquidDB.removeDatabase(QSqlDatabase::defaultConnection);
}
