#ifndef MASTERTHREAD_H
#define MASTERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <QDebug>

class MasterThread : public QThread {
    Q_OBJECT

    QString portName;
    QString request;
    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;

public:
    MasterThread(QObject *parent = 0);
    ~MasterThread();

    void transaction(const QString &portName, int waitTimeout, const QString &request);
    void run();

signals:
    void response(const QString &s);
    void error(const QString &s);
    void timeout(const QString &s);
};

#endif // MASTERTHREAD_H
