#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

QT_USE_NAMESPACE

class Settings : public QObject {

    Q_OBJECT
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

public:
    Settings(QObject *parent = 0);
    Settings(QString portName);
    QString name;
    qint32 baudRate;
    QString stringBaudRate;
    QSerialPort::DataBits dataBits;
    QString stringDataBits;
    QSerialPort::Parity parity;
    QString stringParity;
    QSerialPort::StopBits stopBits;
    QString stringStopBits;
    QSerialPort::FlowControl flowControl;
    QString stringFlowControl;
    bool localEchoEnabled;

};

#endif // SETTINGS_H

