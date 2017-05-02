#include "taskdetailswidget.h"
#include "ui_taskdetailswidget.h"
#include <QHostAddress>
#include <QtWidgets>
#include "syncevent.h"
#include <QDebug>

TaskDetailsWidget::TaskDetailsWidget(QHostAddress addr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskDetailsWidget)
{
    ui->setupUi(this);
    ui->idLabel->setText (QString("Host @ ") + addr.toString ());
    autoDeny = false;
    setVisible (false);
}

TaskDetailsWidget::~TaskDetailsWidget()
{
    delete ui;
}

void TaskDetailsWidget::request(QObject *who, QString fileName, qint64 length)
{
    setVisible (true);
    lastRequest = who;
    currentFile = fileName;
    ui->currentFileName->setText (currentFile);
    currentFileLength = length;
    ui->currentFileLength->setText (QString::asprintf ("%lld Byte(s)", currentFileLength));
    ui->porgress->setRange (0, currentFileLength>>10);
    if (autoDeny) {
        noAnswer ();
        return;
    }
    ui->questionLable->setVisible (true);
    ui->answerBox->setVisible (true);
    ui->progressLabel->setVisible (false);
    ui->porgress->setVisible (false);
}

void TaskDetailsWidget::updateProgress(qint64 current)
{
    ui->porgress->setValue (current>>10);
}

//void TaskDetailsWidget::on_answerBox_clicked(QPushButton *button)
//{
//    qDebug() << "Clicked";
//    if (button == ui->answerBox->button (QDialogButtonBox::Yes)) // Yes Button
//        yesAnswer ();
//    else if (button == ui->answerBox->button (QDialogButtonBox::No)) // No Button
//        noAnswer ();
//    else // NoToAll Button
//        noToAllAnswer ();
//    ui->questionLable->setVisible (false);
//    ui->answerBox->setVisible (false);
//}

void TaskDetailsWidget::yesAnswer()
{
    ui->progressLabel->setVisible (true);
    ui->porgress->setVisible (true);
    ui->porgress->setValue (0);
    emit reply (lastRequest, REPLY_COMFIRM);
}

void TaskDetailsWidget::noAnswer()
{
    emit reply (lastRequest, REPLY_REFUSE);
}

void TaskDetailsWidget::noToAllAnswer()
{
    autoDeny = true;
    noAnswer ();
}

void TaskDetailsWidget::on_answerBox_clicked(QAbstractButton *button)
{
    qDebug() << "Clicked";
    if (button == ui->answerBox->button (QDialogButtonBox::Yes)) // Yes Button
        yesAnswer ();
    else if (button == ui->answerBox->button (QDialogButtonBox::No)) // No Button
        noAnswer ();
    else // NoToAll Button
        noToAllAnswer ();
    ui->questionLable->setVisible (false);
    ui->answerBox->setVisible (false);
}
