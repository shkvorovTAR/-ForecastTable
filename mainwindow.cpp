#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(managerFinished(QNetworkReply*)));

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setShowGrid(true);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    QStringList headers;
    headers << "dt" << "temp" << "description";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString url = "http://api.openweathermap.org/data/2.5/forecast?id=" + ui->lineEdit->text() + "&units=metric&APPID=27e8b6674f0f44f79e51e6994b0eb8d6";
    qDebug() << url;
    request.setUrl(QUrl(url));
    manager->get(request);
}

void MainWindow::managerFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
    QJsonArray jsonList = jsonResponse.object()["list"].toArray();

    for(int i=0; i<jsonList.count(); i++) {
        QJsonObject item = jsonList[i].toObject();

        QString dtStr = item["dt_txt"].toString();

        QString tempStr = QString::number(item["main"].toObject()["temp"].toDouble());
        qDebug() << tempStr;
        qDebug() << "-----------";
        QString description = item["weather"].toArray()[0].toObject()["description"].toString();
        qDebug() << description;
        qDebug() << "-----------";

        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(dtStr, 0));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(tempStr, 0));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(description, 0));
    }

    ui->tableWidget->resizeColumnsToContents();
}
