#ifndef TASKDETAILSWIDGET_H
#define TASKDETAILSWIDGET_H

#include <QWidget>

class QAbstractButton;
class QHostAddress;

namespace Ui {
class TaskDetailsWidget;
}

class TaskDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskDetailsWidget(QHostAddress addr, QWidget *parent = 0);
    ~TaskDetailsWidget();

    void request(QObject *who, QString fileName, qint64 length);
    void updateProgress(qint64 current);
signals:
    void reply(QObject *to, int reply);

private slots:
    void on_answerBox_clicked(QAbstractButton *button);

private:
    Ui::TaskDetailsWidget *ui;
    QString currentFile;
    qint64 currentFileLength;
    QObject *lastRequest;
    bool autoDeny;

    void yesAnswer();
    void noAnswer();
    void noToAllAnswer();
};

#endif // TASKDETAILSWIDGET_H
