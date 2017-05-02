#include "taskthread.h"
#include <QApplication>
#include <QThread>
#include <QDebug>
#include <QSaveFile>
#include <QDataStream>

#include "syncevent.h"

TaskThread::TaskThread(qintptr handle, QObject *eventWatvher):
    QTcpSocket(nullptr),
    eventWatcher(eventWatvher),
    isTransfering(false),
    hasFileInfo(false)
{
    setSocketDescriptor (handle);
    mConstructionThread = QThread::currentThread ();
    mThread = new QThread;
    saveFile = NULL;
    headerLength = 0;
    mThread->start ();
    moveToThread (mThread);
    connect (this, &TaskThread::disconnected, this, &TaskThread::onDisconnect);
    connect (this, &TaskThread::readyRead, this, &TaskThread::onReadyRead);
    mDS = new QDataStream();
    mDS->setByteOrder (QDataStream::BigEndian);
    mDS->setVersion (QDataStream::Qt_5_0);
    mDS->setDevice (this);
}

TaskThread::~TaskThread()
{
    delete mThread;
    delete mDS;
}

void TaskThread::onDisconnect()
{
    moveToThread (mConstructionThread);
    mThread->quit ();
    sendEventToWatcher (new SyncEvent(this, TYPE_INVAILD, REPLY_INVAILD));
}

void TaskThread::onReadyRead()
{
//    sendEventToWatcher (new SyncEvent(this, SyncEvent::TYPE_REQUEST,
//                                      SyncEvent::REPLY_INVAILD,
//                                      "FileName.suffix",
//                                      1231224ul,
//                                      0ul,
//                                      "Hello Server"));
//    qDebug() << readAll ();
//    qDebug() << "onReadyRead";
    if (!hasFileInfo)
        readHead();
    if (isTransfering)
        transferFile();
}

void TaskThread::sendEventToWatcher(QEvent *e)
{
    if (!eventWatcher)
        return;
    qApp->postEvent (eventWatcher, e, Qt::HighEventPriority);
}

void TaskThread::synchronization(SyncEvent *event)
{
    qDebug() << "synchronization" << event->TYPE << event->REPLY;
    if (event->TYPE == TYPE_REPLY) {
        Header h;
        h.TYPE = TYPE_REPLY;
        h.REPLY = event->REPLY;
        switch (event->REPLY) {
        case REPLY_COMFIRM:
            isTransfering = true;
            break;
        default:
            hasFileInfo = false;
            isTransfering = false;
            headerLength = 0;
//            finishFile ();
            break;
        }
        write (headerData (h));
        flush ();
    }
}

void TaskThread::readHead()
{
    qDebug() << "readHead";
    if (!headerLength) {
        if (bytesAvailable () < (qint64)sizeof(qint64))
            return;
        (*mDS) >> headerLength;
    }
    if (bytesAvailable () >= headerLength){
        Header header;
        (*mDS) >> header;
        fileLength = header.Length;
        fileName = header.FileName;
        fileReceivedLength = 0;
        hasFileInfo = true;
        sendEventToWatcher (new SyncEvent(this, TYPE_REQUEST, REPLY_INVAILD, fileName, fileLength));
        qDebug() << header.TYPE << header.FileName << header.Length;
    }
}

void TaskThread::transferFile()
{
    qDebug() << "TransferFile";
    if (!saveFile) {
        saveFile = new QSaveFile("/home/tang/Receive/" + fileName);
        saveFile->open (QIODevice::WriteOnly);
    }

//    QByteArray t;
//    while (fileLength > fileReceivedLength) {
//        t = read (fileLength - fileReceivedLength > 64*1024 ? 64*1024 :
//                                                                fileLength - fileReceivedLength);
//        fileReceivedLength += t.size ();
//        saveFile->write (t);
//        saveFile->flush ();
//    }
    fileReceivedLength += saveFile->write (read(fileLength - fileReceivedLength > 0x80000 ?
                                                    0x80000 : fileLength - fileReceivedLength));
    saveFile->waitForBytesWritten (30000);
    sendEventToWatcher (new SyncEvent(this, TYPE_UPDATE_PROGRESS, REPLY_INVAILD,
                                      fileName, fileLength, fileReceivedLength));

    if (fileLength == fileReceivedLength) {
        saveFile->commit ();
        saveFile->deleteLater ();
        headerLength = 0;
        hasFileInfo = false;
        saveFile = NULL;
        finishFile ();
        qDebug() << "Received";
    }
}

void TaskThread::finishFile()
{
    hasFileInfo = false;
    headerLength = 0;
    isTransfering = false;
    Header header;
    header.TYPE = TYPE_FINISH;
    header.REPLY = REPLY_INVAILD;
    header.FileName = fileName;
    header.Length = fileReceivedLength;
    write (headerData (header));
    flush ();
}


bool TaskThread::event(QEvent *event)
{
    if (event->type () != SyncEvent::Sync_Event)
        return QTcpSocket::event (event);
    synchronization (dynamic_cast<SyncEvent*>(event));
    return true;
}
