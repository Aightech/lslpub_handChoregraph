/**
 * \file mainwindow.cpp
 * \brief TODO.
 * \author Alexis Devillard
 * \version 1.0
 * \date 03 march 2019
 */
#include "include/mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Thumb")<< tr("Index")<< tr("Middle")<< tr("Ring")<< tr("Pinky"));
    for(int j=0; j< 5; j++)
        ui->tableWidget->setColumnWidth(j,100);



    ui->comboBox->addItem("Left_Hand",0);
    ui->comboBox->addItem("Right_Hand",1);

    //createLSLStream();

    ui->lineEdit->setText("choregraphies/01.cry");
    if(!openFile())
    {
        ui->tableWidget->setRowCount(4);
        for(int i=0; i< 4; i++)
        {
            std::vector<float> v(m_nbJoints);
            m_choregraphy.push_back(v);
            for(unsigned j =0; j<m_nbJoints; j++)
            {
                m_choregraphy[i][j]=0;
            }
            for(int j=0; j< 5; j++)
            {
                QTableWidgetItem *item = new QTableWidgetItem("0");
                item->setCheckState(Qt::CheckState::Unchecked);
                ui->tableWidget->setItem(i, j, item);
            }
        }
    }


    connect(ui->tableWidget, SIGNAL (cellChanged(int, int)), this, SLOT (updateTable(int,int)));
    connect(ui->pushButton_open, SIGNAL (released()), this, SLOT (openFile()));
    connect(ui->pushButton_addStep, SIGNAL (released()), this, SLOT (addStep()));
    connect(ui->pushButton_save, SIGNAL (released()), this, SLOT (saveFile()));
    connect(ui->pushButton_play, SIGNAL (released()), this, SLOT (startLSLStream()));
    connect(ui->spinBox, SIGNAL (valueChanged(int)), this, SLOT (update()));
    connect(ui->comboBox, SIGNAL (currentIndexChanged(int)), this, SLOT (createLSLStream()));
    QObject::connect(&m_timer, &QTimer::timeout, this, &MainWindow::sendingData);
    m_timer.setInterval(ui->spinBox->value());
    m_timer.start();


}

bool MainWindow::openFile()
{
    std::ifstream source;                    // build a read-Stream
    source.open(ui->lineEdit->text().toStdString(), std::ios_base::in);  // open data
    if (!source)
    {// if it does not work
        std::cout << "Can't open file: " << ui->lineEdit->text().toStdString() << std::endl;
        return false;
    }
    m_choregraphy.clear();
    ui->tableWidget->clear();
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Thumb")<< tr("Index")<< tr("Middle")<< tr("Ring")<< tr("Pinky"));

    std::cout << "Reading file: " << ui->lineEdit->text().toStdString() << " ..."<< std::endl;
    unsigned i=0;
    for(std::string line; std::getline(source, line); )   //read stream line by line
    {
        std::istringstream in(line);      //make a stream for the line itself
        std::vector<float> v(m_nbJoints);
        m_choregraphy.push_back(v);
        for(unsigned j =0; j<m_nbJoints; j++)
        {
            in >> m_choregraphy[i][j];
            std::cout << m_choregraphy[i][j] << " ";
        }
        std::cout << std::endl;
        ui->tableWidget->setRowCount(static_cast<int>(i+1));
        for(unsigned j=0; j< 5; j++)
        {
            bool state = ((m_choregraphy[i][j*3]>20)?true:false);
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(static_cast<double>(m_choregraphy[i][j*3])));
            item->setCheckState((state)?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
            ui->tableWidget->setItem(i, j, item);
        }

        i++;
    }

    return true;
}

bool MainWindow::saveFile()
{
    std::ofstream myfile (ui->lineEdit->text().toStdString(),std::ios::trunc);
      if (myfile.is_open())
      {
          for(unsigned i=0; i< m_choregraphy.size(); i++)
          {
              for(unsigned j=0; j< m_choregraphy[i].size(); j++)
              {
                  myfile << m_choregraphy[i][j] << " ";
              }
              myfile << "\n";
          }
        myfile.close();
        return true;
      }
      else
          std::cout << "Unable to open file";
    return false;
}

void MainWindow::update()
{
    m_timer.setInterval(ui->spinBox->value());

}

void MainWindow::updateTable(int i, int j)
{
    //std::cout << i << " " << j << " : " << ui->tableWidget->item(i,j) <<  std::endl;
    //ui->tableWidget->item(i,j)->setCheckState(Qt::CheckState::Checked);

    if(ui->tableWidget->item(i,j)->checkState()==Qt::CheckState::Checked)
        ui->tableWidget->item(i,j)->setText("70");
    else
        ui->tableWidget->item(i,j)->setText("0");
    for(int k = 0; k < 3; k++)
        m_choregraphy[i][3*j+k]=ui->tableWidget->item(i,j)->text().toInt();

}

void MainWindow::startLSLStream()
{
    try
    {
        if(m_outlet==nullptr)
            createLSLStream();
        // send it
        m_sendingInd = m_choregraphy.size();

    }
    catch (std::exception& e)
    { std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl; }

}

void MainWindow::createLSLStream()
{
    try
    {
        std::string name = (ui->comboBox->currentIndex()==0)?"Left_Hand":"Right_Hand";
        lsl::stream_info info(name, "hand_choregraphy", m_nbJoints, lsl::IRREGULAR_RATE,lsl::cf_float32);
        if(m_outlet!=nullptr)
            delete m_outlet;
        m_outlet = new lsl::stream_outlet(info);
    }
    catch (std::exception& e)
    { std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl; }

}

void MainWindow::sendingData()
{
    if(m_sendingInd>0)
    {
        m_outlet->push_sample(m_choregraphy[m_choregraphy.size()-m_sendingInd]);
        std::cout << "Sent phase " << m_choregraphy.size()-m_sendingInd  << "\xd"<< std::flush;
        m_sendingInd--;
    }
}

void MainWindow::addStep()
{
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    std::vector<float> v(m_nbJoints);
    m_choregraphy.push_back(v);
    for(unsigned j =0; j<m_nbJoints; j++)
    {
        m_choregraphy[m_choregraphy.size()-1][j]=0;
    }
    for(unsigned j=0; j< 5; j++)
    {
        bool state = false;
        QTableWidgetItem *item = new QTableWidgetItem("0");
        item->setCheckState((state)?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, j, item);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
