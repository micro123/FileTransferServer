#ifndef TASKTHREAD_H
#define TASKTHREAD_H

#include <QTcpSocket>

class SyncEvent;
class QSaveFile;

class TaskThread : public QTcpSocket
{
    Q_OBJECT
public:
    TaskThread(qintptr handle, QObject *eventWatvher);
    ~TaskThread();

private slots:
    void onDisconnect();

    void onReadyRead();
private:
    QThread* mThread;
    QThread* mConstructionThread;
    QObject* eventWatcher;

    bool isTransfering;
    bool hasFileInfo;
    qint64 headerLength;
    qint64 fileLength;
    qint64 fileReceivedLength;
    QString fileName;
    QSaveFile *saveFile;
    QDataStream *mDS;

    void sendEventToWatcher(QEvent* e);
    void synchronization(SyncEvent* event);
    void readHead();
    void transferFile();
    void finishFile();
    // QObject interface
public:
    bool event(QEvent *event) override;
};

#endif // TASKTHREAD_H
