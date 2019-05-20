/**
 * \file mainwindow.cpp
 * \brief TODO.
 * \author Alexis Devillard
 * \version 1.0
 * \date 03 march 2019
 */
#include "include/mainwindow.h"
#include "ui_mainwindow.h"
//#include "GL/freeglut.h"
#include "cmath"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap pixmap("logo.png");

    QIcon ButtonIcon(pixmap);
    ui->button->setIcon(ButtonIcon);
    //ui->button->setMaximumWidth(50);
    //ui->button->setIconSize(QSize(60,60));

    //set up table widget
    ui->tableWidget->setColumnCount(7);

    //set up stream combobox
    ui->comboBox->addItem("Left_Hand_Command",0);
    ui->comboBox->addItem("Right_Hand_Command",1);
    ui->comboBox->addItem("Both hands",1);

    //createLSLStream();

    //set up lineedit file path and try to open a choregraphy
    ui->lineEdit->setText("choregraphies/01.cry");
    openFile();

    connect(ui->tableWidget, SIGNAL (cellClicked(int, int)), this, SLOT (quickTableChange(int,int)));
    connect(ui->tableWidget, SIGNAL (cellChanged(int, int)), this, SLOT (updateChoregraphy(int,int)));

    connect(ui->pushButton_open, SIGNAL (released()), this, SLOT (openFile()));
    connect(ui->pushButton_addStep, SIGNAL (released()), this, SLOT (addStep()));
    connect(ui->pushButton_rmStep, SIGNAL (released()), this, SLOT (rmStep()));
    connect(ui->pushButton_clearStep, SIGNAL (released()), this, SLOT (clearStep()));
    connect(ui->pushButton_desactivate, SIGNAL(released()), this, SLOT(desactivate()));

    connect(ui->pushButton_save, SIGNAL (released()), this, SLOT (saveFile()));
    connect(ui->pushButton_play, SIGNAL (released()), this, SLOT (startLSLStream()));
    connect(ui->spinBox, SIGNAL (valueChanged(int)), this, SLOT (update()));
    //connect(ui->comboBox, SIGNAL (currentIndexChanged(int)), this, SLOT (createLSLStream(int)));

    //use this time out to send order at the right pace.
    QObject::connect(&m_timer, &QTimer::timeout, this, &MainWindow::sendingData);
    m_timer.setInterval(ui->spinBox->value());
    m_timer.start();


}

/**
 * @brief MainWindow::openFile try to open the file at the path writen in the lineedit widget
 * @return true if successfully open, false elseway.
 */
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
    m_times.clear();
    ui->tableWidget->clear();

    //display the opened choregraphy
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Palm") << tr("Thumb")<< tr("Index")<< tr("Middle")<< tr("Ring")<< tr("Pinky") << tr("Time"));
    std::cout << "Reading file: " << ui->lineEdit->text().toStdString() << " ..."<< std::endl;
    unsigned i=0;
    for(std::string line; std::getline(source, line); )   //read stream line by line
    {
        std::istringstream in(line);      //make a stream for the line itself
        std::vector<float> v(m_nbJoints);
        m_choregraphy.push_back(v); // add a new step to the choregraphy
        for(unsigned j =0; j<m_nbJoints; j++)//store the commands
            in >> m_choregraphy[i][j];
        double t;
        in >> t;
        m_times.push_back(t);
            //std::cout << m_choregraphy[i][j] << " ";

        //create the table item of the new command
        //palm
        ui->tableWidget->setRowCount(static_cast<int>(i+1));
        bool state = ((m_choregraphy[i][0]>20)?true:false);
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(static_cast<double>(m_choregraphy[i][0])));
        item->setCheckState((state)?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
        ui->tableWidget->setItem(static_cast<int>(i), 0, item);
        //thumb
        state = ((m_choregraphy[i][2]>20)?true:false);
        item = new QTableWidgetItem(QString::number(static_cast<double>(m_choregraphy[i][2])));
        item->setCheckState((state)?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
        ui->tableWidget->setItem(static_cast<int>(i), 1, item);
        //the rest of the fingers
        for(unsigned j=1; j< 5; j++)
        {
            bool state = ((m_choregraphy[i][j*3]>20)?true:false);
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(static_cast<double>(m_choregraphy[i][j*3])));
            item->setCheckState((state)?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
            ui->tableWidget->setItem(static_cast<int>(i), j+1, item);
        }
        item = new QTableWidgetItem(QString::number(static_cast<double>(m_times[i])));
        ui->tableWidget->setItem(static_cast<int>(i), 6, item);
        i++;
    }

    return true;
}
/**
 * @brief MainWindow::saveFile Save the choregraphy into a text file.
 * @return true if successfully open, false elseway.
 */
bool MainWindow::saveFile()
{
    std::ofstream myfile (ui->lineEdit->text().toStdString(),std::ios::trunc);
    std::cout << "Saving to " << ui->lineEdit->text().toStdString() << std::endl;
      if (myfile.is_open())
      {
          for(unsigned i=0; i< m_choregraphy.size(); i++)
          {
              for(unsigned j=0; j< m_choregraphy[i].size(); j++)
              {
                  myfile << (std::isnan(m_choregraphy[i][j])?0:m_choregraphy[i][j]) << " ";
              }
              myfile << m_times[i];
              myfile << "\n";
          }
        myfile.close();
        return true;
      }
      else
          std::cout << "Unable to open file";
    return false;
}

/**
 * @brief MainWindow::update Update the different variables with the GUI data.
 */
void MainWindow::update()
{
    m_timer.setInterval(ui->spinBox->value());
}

/**
 * @brief MainWindow::updateTable Update the choregraphy array when the GUI table has been modified.
 * @param i row/step
 * @param j finger
 */
void MainWindow::updateChoregraphy(int i, int j)
{
    if(j>1 && j < 6)//if other than the thump finger
        for(int k = 0; k < 3; k++)
        {
            m_choregraphy[i][3*(j-1)+k]=ui->tableWidget->item(i,j)->text().toFloat();
            //²²std::cout <<  i << " " << 3*(j-1)+k << " : " <<ui->tableWidget->item(i,j)->text().toStdString() << " " << m_choregraphy[i][3*(j-1)+k] << std::endl;
        }
    else if (j==6)
    {
            m_times[i] = ui->tableWidget->item(i,j)->text().toFloat();
    }

    else if(j==0)//palm
        m_choregraphy[i][0]=ui->tableWidget->item(i,j)->text().toFloat();

    else if (j==1)//thumb
        for(int k = 1; k < 3; k++)
            m_choregraphy[i][k]=ui->tableWidget->item(i,j)->text().toFloat();
}

/**
 * @brief MainWindow::quickTableChange Change the command value thank the the checkbox.
 * @param i row/step
 * @param j finger
 */
void MainWindow::quickTableChange(int i, int j)
{
    if(ui->tableWidget->item(i,j)->checkState()==Qt::CheckState::Checked)
        ui->tableWidget->item(i,j)->setText("70");
    else
        ui->tableWidget->item(i,j)->setText("0");
    updateChoregraphy(i,j);
}

/**
 * @brief MainWindow::startLSLStream Slot use to start a choregraphy. If the stream was not yet created it is then created.
 */
void MainWindow::startLSLStream()
{
    if(m_sendingInd==0 && m_choregraphy.size()*ui->spinBox_loop->value()!=0)
    {
        try
        {
            std::cout << m_sendingInd << std::endl;
            //Increase the playing token by the number of step that have to be sent.
            m_sendingInd = m_choregraphy.size()*ui->spinBox_loop->value();
            ui->pushButton_play->setText("Stop");
            enableGUI(false);
            createLSLStream(ui->comboBox->currentIndex());

        }
        catch (std::exception& e)
        { std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl; }
    }
    else
    {
        m_sendingInd = 0;
        ui->pushButton_play->setText("Play");
        enableGUI(true);
    }

}

/**
 * @brief MainWindow::createLSLStream create an LSL stream for the hand movement commands.
 * @param i 0 for the left hand, 1, for the right and 2 for both hands.
 */
void MainWindow::createLSLStream(int i)
{
    try
    {
        if(i==2)//recurssive call to create both hand stream
        {
            createLSLStream(0);
            createLSLStream(1);
        }
        else
        {
            std::cout << "Creating LSL stream ... \xd" << std::flush;
            std::string name = (i==0)?"Left_Hand_Command":"Right_Hand_Command";
            lsl::stream_info info(name, "hand_choregraphy", m_nbJoints, lsl::IRREGULAR_RATE,lsl::cf_float32);

            if(m_outlet[i]!=nullptr)
            {
                delete m_outlet[i];
            }
            m_outlet[i] = new lsl::stream_outlet(info);
            std::cout << "LSL streams created." << std::endl;
        }
    }
    catch (std::exception& e)
    { std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl; }

}

/**
 * @brief MainWindow::sendingData Publish data in the actives command lsl streams.
 */
void MainWindow::sendingData()
{
    if(m_sendingInd>0)
    {
        int ind = ui->comboBox->currentIndex();
        //transform in int to ensure the right funtionning of %
        int a = static_cast<int>(-m_sendingInd);
        int b = static_cast<int>(m_choregraphy.size());
        //implemente the true mathematical modulo
        unsigned step =static_cast<unsigned>((a%b+b)%b);
        std::cout << "Sent phase " << step << std::endl;
        for(int i =0; i<m_choregraphy[step].size(); i++)
            std::cout << m_choregraphy[step][i] << " ";
        std::cout << std::endl;

        if(m_outlet[0]!=nullptr && ind != 1)
            m_outlet[0]->push_sample(m_choregraphy[step]);
        if(m_outlet[1]!=nullptr && ind != 0)
            m_outlet[1]->push_sample(m_choregraphy[step]);
        m_timer.setInterval(m_times[step]);
        m_sendingInd--;
        //reset the GUI if no more step to send
        if(m_sendingInd==0)
        {
            enableGUI(true);

            if(m_outlet[0]!=nullptr)
                delete m_outlet[0];
            if(m_outlet[1]!=nullptr)
                delete m_outlet[1];
            m_outlet[0]=nullptr;
            m_outlet[1]=nullptr;
        }
     }
}

/**
 * @brief MainWindow::addStep Add a step to the choregraphy.
 */
void MainWindow::addStep()
{
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    std::vector<float> v(m_nbJoints);
    m_choregraphy.push_back(v);
    for(unsigned j =0; j<m_nbJoints; j++)
    {
        m_choregraphy[m_choregraphy.size()-1][j]=0;
    }
    for(unsigned j=0; j< 6; j++)
    {
        bool state = false;
        QTableWidgetItem *item = new QTableWidgetItem("0");
        item->setCheckState((state)?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, j, item);
    }
}

/**
 * @brief MainWindow::addStep Add a step to the choregraphy.
 */
void MainWindow::rmStep()
{
    int row = ui->tableWidget->currentRow();

    if(row>-1)
    {
        m_choregraphy.erase(m_choregraphy.begin()+row);
        ui->tableWidget->removeRow(row);
    }

}

/**
 * @brief MainWindow::addStep Add a step to the choregraphy.
 */
void MainWindow::clearStep()
{
    m_choregraphy.clear();
    ui->tableWidget->clear();
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Palm") << tr("Thumb")<< tr("Index")<< tr("Middle")<< tr("Ring")<< tr("Pinky"));
    //ui->tableWidget->setHorizontalHeader()

    ui->tableWidget->setRowCount(0);

}


/**
 * @brief MainWindow::addStep Add a step to the choregraphy.
 */
void MainWindow::desactivate()
{

    int col = ui->tableWidget->currentColumn();

    if(col>-1)
    {
        for(int i = 0; i< m_choregraphy.size(); i++)
        {
            ui->tableWidget->item(i,col)->setText("nan");
            ui->tableWidget->item(i,col)->setCheckState(Qt::CheckState::Unchecked);
            //m_choregraphy[i][col] = std::nanf("");
            //std::cout <<  "desactivate " << i << " " << col << " : " << m_choregraphy[i][col] << std::endl;

        }
    }
}

void MainWindow::enableGUI(bool en)
{
    ui->comboBox->setEnabled(en);
    ui->tableWidget->setEnabled(en);
    ui->pushButton_open->setEnabled(en);
    ui->pushButton_save->setEnabled(en);
    ui->pushButton_addStep->setEnabled(en);
    ui->pushButton_rmStep->setEnabled(en);
    ui->pushButton_clearStep->setEnabled(en);
    ui->lineEdit->setEnabled(en);
    ui->pushButton_play->setText(en?"Play":"Stop");
}

MainWindow::~MainWindow()
{
    delete ui;
}
