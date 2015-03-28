
#include "MasterThread.h"

QT_USE_NAMESPACE

MasterThread::MasterThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false) {}

MasterThread::~MasterThread() {
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void MasterThread::transaction(const QString &portName, int waitTimeout, const QString &request) {
    mutex.lock();
    this->portName = portName;
    this->waitTimeout = waitTimeout;
    this->request = request;
    mutex.unlock();
    if (!isRunning())
        start();
    else
        cond.wakeOne();
}

void MasterThread::run() {
    bool currentPortNameChanged = false;
    mutex.lock();
    QString currentPortName;
    if (currentPortName != portName) {
        currentPortName = portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = waitTimeout;
    QString currentRequest = request;
    mutex.unlock();
    QSerialPort serial(portName);
    while (!quit) {
        //qDebug() << "Reading Port: " << currentPortName;
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Can't open %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

            if (!serial.setBaudRate(QSerialPort::Baud57600)) {
                emit error(tr("Can't set baud rate 57600 baud to port %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

            if (!serial.setDataBits(QSerialPort::Data8)) {
                emit error(tr("Can't set 8 data bits to port %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

            if (!serial.setParity(QSerialPort::NoParity)) {
                emit error(tr("Can't set no patity to port %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

            if (!serial.setStopBits(QSerialPort::OneStop)) {
                emit error(tr("Can't set 1 stop bit to port %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

            if (!serial.setFlowControl(QSerialPort::NoFlowControl)) {
                emit error(tr("Can't set no flow control to port %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }
        }


        // if this part can be put on a timed loop, it would perform the same role as the serialInput tutorial.

        //Write Request - Commented out due to it messing up arduino
        //QByteArray requestData = currentRequest.toLocal8Bit();
        //serial.write(requestData);
        //if (serial.waitForBytesWritten(waitTimeout)) {
            // read response
            if (serial.waitForReadyRead(currentWaitTimeout)) {
                QByteArray responseData = serial.readAll();
                while (serial.waitForReadyRead(10))
                    responseData += serial.readAll();

                QString response(responseData);
                emit this->response(response);  //this is where the data is taken from the serial stream and passed to
                                                //the show response function. response is the current data.
            } else {
                emit timeout(tr("Wait read response timeout %1")
                             .arg(QTime::currentTime().toString()));
            }
        //} else {
        //    emit timeout(tr("Wait write request timeout %1")
        //                 .arg(QTime::currentTime().toString()));
        //    qDebug() << "Timed Out Writing: " << QTime::currentTime().toString();
        //}
        mutex.lock();
        cond.wait(&mutex);
        if (currentPortName != portName) {
            currentPortName = portName;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = waitTimeout;
        currentRequest = request;
        mutex.unlock();
    }
}
