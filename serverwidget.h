#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QWidget>

#include "filetransferserver.h"
#include "taskthread.h"
#include "syncevent.h"
#include "taskdetailswidget.h"

namespace Ui {
class ServerWidget;
}

class QVBoxLayout;

class ServerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ServerWidget(QWidget *parent = 0);
    ~ServerWidget();

private slots:
    void on_switchButton_clicked();

    void on_new_client(qintptr handle);

    void on_user_reply(QObject *to, int ans);
private:
    Ui::ServerWidget *ui;
    bool mServerIsRunning;
    FileTransferServer *mServer;
    QMap<TaskThread*, TaskDetailsWidget*> mClientsMap;
    QVBoxLayout *taskLayout;

    void synchronization(SyncEvent* event);
    // QObject interface
public:
    bool event(QEvent *event) override;
};

#endif // SERVERWIDGET_H
