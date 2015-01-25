#ifndef PARSER_H
#define PARSER_H

#include "masterthread.h"
#include "settings.h"
#include <QVector>
#include <iterator>
#include <QTextStream>
#include <QDebug>
#include <QObject>
#include <QTimer>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTextEdit>




class Parser : public QObject{

    Q_OBJECT

    QVector<QString> SerialPorts;
    QVector <char> dataString;
    QVector<double> parsedData;
    QVector<Settings*> settings;
    MasterThread* thread;
    int currentPort, lastPort;
    bool recentResponse;
    QTextEdit*infoBox;



private slots:
    void findPort();
    void Parse(const QString &Data);
    void showParsedData(QVector<double> P);
    void transaction();

signals:
    void done(QVector<double> &Parsed);

public:
    Parser(QObject *parent = 0);
    void start();
    void setOutputBox(QTextEdit* textbox);
};

#endif // PARSER_H
