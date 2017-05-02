#include "serverwidget.h"
#include "ui_serverwidget.h"

#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QVBoxLayout>

ServerWidget::ServerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerWidget),
    mServerIsRunning(false)
{
    ui->setupUi(this);
    mServer = new FileTransferServer(this);
    taskLayout = new QVBoxLayout(ui->taskList);
    ui->taskList->setLayout (taskLayout);
            taskLayout->addItem (new QSpacerItem(0,0,
                                                 QSizePolicy::Expanding, QSizePolicy::Expanding));
    connect (mServer, &FileTransferServer::newClient, this, &ServerWidget::on_new_client);
}

ServerWidget::~ServerWidget()
{
    delete ui;
}

void ServerWidget::on_switchButton_clicked()
{
    if (!mServerIsRunning) {
        mServer->listen (QHostAddress::Any, ui->portSpinBox->value ());
    } else {
        mServer->close ();
    }
    mServerIsRunning = !mServerIsRunning;
    ui->switchButton->setText (mServerIsRunning ? "停止监听" : "开始监听");
    ui->statusLabel->setText (mServerIsRunning ? "服务运行中" : "服务已停止");
    ui->portSpinBox->setEnabled (!mServerIsRunning);
}

void ServerWidget::on_new_client(qintptr handle)
{
    TaskThread *client = new TaskThread(handle, this);
    TaskDetailsWidget *widget = new TaskDetailsWidget(client->peerAddress (), this);
    connect (widget, &TaskDetailsWidget::reply, this, &ServerWidget::on_user_reply);
    taskLayout->insertWidget (0, widget, 1);
    //mClientsMap.append (qMakePair(client, widget));
    mClientsMap.insert (client, widget);
}

void ServerWidget::on_user_reply(QObject *to, int ans)
{
    qDebug() << "REPLY --> " << ans;
    qApp->postEvent (to, new SyncEvent(this, TYPE_REPLY,
                                       (ans == REPLY_COMFIRM) ? REPLY_COMFIRM : REPLY_REFUSE));
}

void ServerWidget::synchronization(SyncEvent *event)
{
//    QMessageBox::StandardButton btn =
//            QMessageBox::question (this, "是否接收文件",
//                                   event->FileName + ", size = " + QString::number (event->Length));
//    qApp->postEvent (event->MAKER, new SyncEvent(this, TYPE_REPLY,
//                                                 btn == QMessageBox::Yes ? REPLY_COMFIRM : REPLY_REFUSE,
//                                                 "", 0ul, 0ul, "Ok"));
    if (event->TYPE == TYPE_REQUEST)
        mClientsMap.value ((TaskThread*)event->MAKER)
                ->request (event->MAKER, event->FileName, event->Length);
    else if (event->TYPE == TYPE_UPDATE_PROGRESS)
        mClientsMap.value ((TaskThread*)event->MAKER)
                ->updateProgress (event->PROGRESS);
    else if (event->TYPE == TYPE_INVAILD && event->REPLY == REPLY_INVAILD) { // client disconnect
        TaskDetailsWidget *w = mClientsMap.value ((TaskThread*)event->MAKER);
        w->setVisible (false);
        taskLayout->removeWidget (w);
        w->deleteLater ();
        mClientsMap.remove ((TaskThread*)event->MAKER);
        event->MAKER->deleteLater ();
    }


}


bool ServerWidget::event(QEvent *event)
{
    if (event->type () != SyncEvent::Sync_Event)
        return QWidget::event (event);
    else {
        synchronization (dynamic_cast<SyncEvent*>(event));
        return true;
    }
}
