#include "settings.h"

Settings::Settings(QObject* parent) {
}
Settings::Settings(QString portName) {
    name = portName;
    this->baudRate = 57600;
    stringBaudRate = "BAUD: 57600";
    dataBits = QSerialPort::Data8;
    parity = QSerialPort::NoParity;
    stopBits = QSerialPort::OneAndHalfStop;
    flowControl = QSerialPort::NoFlowControl;
    qDebug() << "Available Port: " << name << "\n";
}
