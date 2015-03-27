#include "Parser.h"

#include <QtSerialPort/QSerialPortInfo>

QT_USE_NAMESPACE



Parser::Parser(QObject *parent)
    : currentPort(0) {
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        this->settings.push_back(new Settings(info.portName()));
    this->thread = new MasterThread(this);
    lastPort = -1;
    connect(this->thread, SIGNAL(response(QString)), this, SLOT(Parse(QString)));
    connect(this->thread, SIGNAL(timeout(QString)), this, SLOT(findPort()));
}

void Parser::Parse(const QString&data) {
    if (currentPort != lastPort) {
        lastPort = currentPort;
        infoBox->setPlainText(infoBox->toPlainText() + "\nReading Port: " + settings[currentPort]->name);
        infoBox->moveCursor(QTextCursor::End);
    }
    recentResponse = true;
    std::string strData = data.toUtf8().constData();
    const char* part = strtok(&strData[0],",");
    while (part != NULL) {
        parsedData.push_back(std::stod(std::string(part)));
        part = strtok(NULL,",");
    }
    showParsedData(parsedData);
}

void Parser::showParsedData(QVector<double> P) {
    //qDebug() << "First Term: " << P[0];
    //qDebug() << "Second Term: " << P[1];
    //qDebug() << "Third Term: " << P[2];
    //qDebug() << "Fourth Term: " << P[3];
    //qDebug() << "Fifth Term: " << P[4];
    //qDebug() << "Sixth Term: " << P[5];
    //qDebug() << "Size: " << P.size();
    emit done(P);

    parsedData.clear();
}

void Parser::setOutputBox(QTextEdit* textbox) {
    this->infoBox = textbox;
}

void Parser::transaction() {
    if (this->settings.size() > 0)
        thread->transaction(settings[currentPort]->name,50,"1");
}

void Parser::findPort() {
    if (recentResponse) {
        recentResponse = false;
        return;
    }
    lastPort = -1;
    currentPort = (currentPort + 1) % settings.size();
    infoBox->setPlainText(infoBox->toPlainText() + "\nScanning Port: " + settings[currentPort]->name);
    infoBox->moveCursor(QTextCursor::End);
}
