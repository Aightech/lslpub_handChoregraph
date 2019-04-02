#include "include/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setRowCount(4);
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Thumb")<< tr("Index")<< tr("Middle")<< tr("Ring")<< tr("Pinky"));
    for(int j=0; j< 5; j++)
        ui->tableWidget->setColumnWidth(j,100);

    for(int i=0; i< 4; i++)
    {
        for(int j=0; j< 5; j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem("0");
            item->setCheckState(Qt::CheckState::Unchecked);
            ui->tableWidget->setItem(i, j, item);
        }
    }

    ui->comboBox->addItem("Left_Hand",0);
    ui->comboBox->addItem("Right_Hand",1);

    ui->lineEdit->setText("choregraphies/01.cry");

}

MainWindow::~MainWindow()
{
    delete ui;
}
