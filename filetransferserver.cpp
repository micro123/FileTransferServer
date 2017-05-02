#include "filetransferserver.h"

FileTransferServer::FileTransferServer(QObject *parent):
    QTcpServer (parent)
{

}

FileTransferServer::~FileTransferServer()
{

}

void FileTransferServer::incomingConnection(qintptr handle)
{
    emit newClient (handle);
}
