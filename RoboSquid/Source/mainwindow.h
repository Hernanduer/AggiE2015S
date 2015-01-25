#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtWebKit/QtWebKit>
#include <QtWebKitWidgets>
#include <QUrl>
#include <QtSql>
#include <QDebug>
#include <QFileInfo>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

#include "parser.h"
#include "rotation.h"
#include "qcustomplot.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    QSqlDatabase RoboSquidDB;
    UI::MainWindow *ui;
    QTimer dataTimer;
    Parser dataParser;
    double AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ, MagX, MagY, MagZ, Temp, Pressure, Depth;
    glm::vec3 setVector;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void setupAcceleration(QCustomPlot* plot);
    void setupPressure(QCustomPlot* plot);
    void setupTemperature(QCustomPlot* plot);
    void setupDepth(QCustomPlot* plot);
    void setupMagnetometer(QCustomPlot* plot);
    void setupGyroscope(QCustomPlot* plot);
    void setupMap(QWebView* map);
    void updateTridata(QCustomPlot* plot, const double&key, const double&offset, const double&x, const double&y = 0, const double&z = 0);

    void into_database();

    const bool DbOpen();
    void DbClose();

    void resizeEvent(QResizeEvent* event);

private slots:
    void realtimeDataSlot(QVector<double> &data);

    void on_setButton_clicked();
    void on_exitButton_clicked();
};

#endif // MAINWINDOW_H
