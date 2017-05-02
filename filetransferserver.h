#ifndef FILETRANSFERSERVER_H
#define FILETRANSFERSERVER_H

#include <QTcpServer>

class FileTransferServer : public QTcpServer
{
    Q_OBJECT
public:
    FileTransferServer(QObject *parent = 0);
    ~FileTransferServer();

signals:
    void newClient(qintptr handle);

    // QTcpServer interface
protected:
    void incomingConnection(qintptr handle);
};

#endif // FILETRANSFERSERVER_H
