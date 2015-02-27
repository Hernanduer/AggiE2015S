/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWebKitWidgets/QWebView>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

namespace UI {
    struct WidgetParam {
        double x, y, w, h;
        QRect q() {
            return QRect(round(x), round(y), round(w), round(h));
        }
        void r(const double&ratioW, const double&ratioH, const bool&label = false) {
            this->x *= ratioW;
            this->y *= ratioH;
            if (!label) {
                this->w *= ratioW;
                this->h *= ratioH;
            }
        }
    };

    struct Plot {
        double x, y, w, h, labelW, labelH;
        QCustomPlot *plot;
        std::string name;
        QRect rect, titleRect;
        QLabel *label;
        Plot()
            : name(""), plot(NULL), label(NULL) {}
        Plot(QWidget *window, const std::string&name, const int&x, const int&y, const int&w, const int&h)
            : name(name), x(x), y(y), w(w), h(h) {
            //setup the plot
            this->plot = new QCustomPlot(window);
            this->plot->setObjectName(QString::fromStdString("Plot_"+this->name));

            //setup the label, set the text first so we can get its pixel size after
            this->label = new QLabel(window);
            this->label->setObjectName(QString::fromStdString("Label_"+this->name));
            this->label->setText(QString::fromStdString(this->name));

            this->move(x,y,w,h);
        }
        void move(const double&x, const double&y, double w = 0, double h = 0) {
            if (w == 0) w = this->w; else this->w = w;
            if (h == 0) h = this->h; else this->h = h;
            this->x = x;
            this->y = y;
            //move and size the plot
            this->rect = QRect(round(x),round(y),round(w),round(h));
            this->plot->setGeometry(this->rect);

            //get the label's text width and height so we can calculate the middle point and center it below the graphs
            this->labelW = this->label->fontMetrics().boundingRect(this->label->text()).width();
            this->labelH = this->label->fontMetrics().boundingRect(this->label->text()).height();

            //set the text's location on screen
            this->titleRect = QRect(round(this->x + (this->w/2) - (this->labelW/2)), round(this->y - this->labelH), this->labelW, this->labelH);
            this->label->setGeometry(this->titleRect);
        }
    };
    struct RelatedLabel {
        double x, y, w, h;
        bool below, center;
        std::string name, text;
        const QWidget *relatedTo;
        QRect rect;
        QLabel *label;
        RelatedLabel()
            : name(""), relatedTo(NULL), label(NULL) {}
        RelatedLabel(QWidget *window, const QWidget *relatedTo, const std::string&name, const std::string&text, const bool&below = false, const bool&center = true)
            : name(name), text(text), relatedTo(relatedTo), below(below), center(center) {
            //setup the label, set the text first so we can get its pixel size after
            this->label = new QLabel(window);
            this->label->setObjectName(QString::fromStdString("Label_"+this->name));
            this->label->setText(QString::fromStdString(this->text));

            if (!below)
                this->move(this->relatedTo->x(), this->relatedTo->y());
            else
                this->move(this->relatedTo->x(), this->relatedTo->y() + this->relatedTo->height());
        }
        void move(const double&x, const double&y) {
            this->x = x;
            this->y = y;

            this->w = this->label->fontMetrics().boundingRect(this->label->text()).width();
            this->h = this->label->fontMetrics().boundingRect(this->label->text()).height();
            double c = (this->relatedTo->width()/2) - (this->w/2); //center
            double a = this->h; //above
            this->rect = QRect(round(this->x + ((center) ? c : 0)), round(this->y - ((below) ? 0 : a)), this->w, this->h);
            this->label->setGeometry(this->rect);
        }
    };

    class MainWindow {
    public:
        Plot accel, press, temp, depth, mag, gyro, accelBig, gyroBig, magBig;
        WidgetParam webViewP, batteryBarP, infoBoxP, exitButtonP, setButtonP, hlLblP, locationTextP, unitTextP, signalTextP;
        RelatedLabel batteryBarText;

        QWidget *centralWidget, *summaryTab, *accelTab, *gyroTab, *magTab;
        QLabel *hlLbl, *locationText, *unitText, *signalText;
        QTabWidget *tabControl;
        QWebView *webView;
        QTextEdit *infoBox;
        QProgressBar *batteryBar;
        QPushButton *exitButton, *setButton;
        QMenuBar *menuBar;
        QMenu *menuVisual;
        QToolBar *mainToolBar;
        QStatusBar *statusBar;
        double ratioW, ratioH;

        void setupUI(QMainWindow *MainWindow) {
            if (MainWindow->objectName().isEmpty())
                MainWindow->setObjectName(QStringLiteral("MainWindow"));
            MainWindow->resize(1200, 870);
            MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Vessel Data Observer", 0));

            centralWidget = new QWidget(MainWindow);
            centralWidget->setObjectName(QStringLiteral("centralWidget"));

            tabControl = new QTabWidget(MainWindow);
            tabControl->setObjectName(QStringLiteral("tabControl"));

            //Stuff inside Summary Tab
            summaryTab = new QWidget();
            summaryTab->setObjectName(QStringLiteral("summaryTab"));
            int finalSelected = tabControl->addTab(summaryTab, "Summary");

            accel = Plot(summaryTab, "Acceleration", 30, 430, 270, 180);
            gyro = Plot(summaryTab, "Gyroscope", 300, 430, 270, 180);
            mag = Plot(summaryTab, "Magnetometer", 570, 430, 270, 180);

            temp = Plot(summaryTab, "Temperature", 30, 620, 270, 180);
            press = Plot(summaryTab, "Pressure", 300, 620, 270, 180);
            depth = Plot(summaryTab, "Depth", 570, 620, 270, 180);

            //image file is located in the project release folder and must be copied to a build folder to display
            QString hlPath=QGuiApplication::applicationDirPath()+"/hrg_logor.png";
            hlLbl = new QLabel(summaryTab);
            QPixmap huffLogo(hlPath);
            if (!huffLogo.isNull())
                hlLbl->setPixmap(huffLogo.scaled(338,225,Qt::KeepAspectRatio,Qt::SmoothTransformation));
            else
                hlLbl->setText("No Image Found at \n "+hlPath);
            hlLbl->setObjectName(QStringLiteral("huffLogo"));
            hlLblP={850, 10, 338, 225};
            hlLbl->setGeometry(hlLblP.q());

            webView = new QWebView(summaryTab);
            webView->setObjectName(QStringLiteral("webView"));
            webViewP = {0, 0, 840, 400};
            webView->setGeometry(webViewP.q());
            webView->setUrl(QUrl(QStringLiteral("about:blank")));

            locationText = new QLabel(summaryTab);
            locationText->setText("Last Position: 00.000N, 00.000W");
            locationText->setObjectName(QStringLiteral("locationText"));
            locationTextP = {850, 240, 335, 15};
            locationText->setGeometry(locationTextP.q());

            unitText = new QLabel(summaryTab);
            unitText->setText("Connected Units: 0");
            unitText->setObjectName(QStringLiteral("unitText"));
            unitTextP = {850, 255, 335, 15};
            unitText->setGeometry(unitTextP.q());

            signalText = new QLabel(summaryTab);
            signalText->setText("Signal Strength from Base: 0dBm");
            signalText->setObjectName(QStringLiteral("signalText"));
            signalTextP = {850, 270, 335, 15};
            signalText->setGeometry(signalTextP.q());

            infoBox = new QTextEdit(summaryTab);
            infoBox->setObjectName(QStringLiteral("infoBox"));
            infoBoxP = {850, 500, 335, 300};
            infoBox->setGeometry(infoBoxP.q());

            batteryBar = new QProgressBar(summaryTab);
            batteryBar->setObjectName(QStringLiteral("batteryBar"));
            batteryBarP = {850, 300, 200, 10};
            batteryBar->setGeometry(batteryBarP.q());
            batteryBar->setValue(16);

            batteryBarText = RelatedLabel(summaryTab, batteryBar, "batteryBar", "Battery", false, false);

            setButton = new QPushButton(summaryTab);
            setButton->setObjectName(QStringLiteral("setButton"));
            setButtonP = {850, 470, 75, 23};
            setButton->setGeometry(setButtonP.q());
            setButton->setText("Set Baseline");

            exitButton = new QPushButton(summaryTab);
            exitButton->setObjectName(QStringLiteral("exitButton"));
            exitButtonP = {1110, 470, 75, 23};
            exitButton->setGeometry(exitButtonP.q());
            exitButton->setText("Exit Program");

            //Individual Graph Tabs
            //Acceleration
            accelTab = new QWidget();
            accelTab->setObjectName(QStringLiteral("accelTab"));
            tabControl->addTab(accelTab, "Acceleration");

            accelBig = Plot(accelTab, "", 200, 10, 1000, 800);

            //Gyroscope
            gyroTab = new QWidget();
            gyroTab->setObjectName(QStringLiteral("gyroTab"));
            tabControl->addTab(gyroTab, "Gyroscope");

            gyroBig = Plot(gyroTab, "", 200, 10, 1000, 800);

            //Magnetometer
            magTab = new QWidget();
            magTab->setObjectName(QStringLiteral("magTab"));
            tabControl->addTab(magTab, "Magnetometer");

            magBig = Plot(magTab, "", 200, 10, 1000, 800);


            menuBar = new QMenuBar(MainWindow);
            menuBar->setObjectName(QStringLiteral("menuBar"));
            menuBar->setGeometry(QRect(0, 0, 1384, 21));




            menuVisual = new QMenu(menuBar);
            menuVisual->setObjectName(QStringLiteral("menuVisual"));
            mainToolBar = new QToolBar(MainWindow);
            mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
            statusBar = new QStatusBar(MainWindow);
            statusBar->setObjectName(QStringLiteral("statusBar"));

            MainWindow->setCentralWidget(tabControl);
            MainWindow->setMenuBar(menuBar);
            MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
            MainWindow->setStatusBar(statusBar);

            menuBar->addAction(menuVisual->menuAction());

            tabControl->setCurrentIndex(finalSelected);

            QMetaObject::connectSlotsByName(MainWindow);
        }
        void inline presize(Plot&p) {
            p.move(p.x*ratioW, p.y*ratioH, p.w*ratioW, p.h*ratioH);
        }
        void inline lresize(RelatedLabel&p) {
            p.move(p.x*ratioW, p.y*ratioH);
        }
        void inline wresize(QWidget*w, WidgetParam&p, const bool&label = false) {
            p.r(ratioW, ratioH, label);
            w->setGeometry(p.q());
        }
        void inline lresize(QLabel* l, WidgetParam&p, const bool&label=false) {
            p.r(ratioW, ratioH, label);
            l->setGeometry(p.q());
        }


        void resize(const double&ratioW, const double&ratioH) {
            this->ratioW = ratioW;
            this->ratioH = ratioH;

            presize(accel);
            presize(gyro);
            presize(mag);
            presize(temp);
            presize(press);
            presize(depth);
            presize(accelBig);
            presize(gyroBig);
            presize(magBig);

            wresize(webView, webViewP);
            wresize(batteryBar, batteryBarP);
            wresize(infoBox, infoBoxP);
            wresize(setButton, setButtonP);
            wresize(exitButton, exitButtonP);

            lresize(unitText, unitTextP);
            lresize(signalText, signalTextP);
            lresize(locationText, locationTextP);
            lresize(hlLbl, hlLblP);

            //RelatedLabel's are reliant on the transforms of another widget, so make sure they are processed last and,
            //if reliant on another label, in the proper order
            lresize(batteryBarText);
        }
    };
}

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
