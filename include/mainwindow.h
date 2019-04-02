#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
 #include <QTime>
#include <vector>
#include <lsl_cpp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QtCore/QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    bool openFile();
    bool saveFile();
    void update();
    void startLSLStream();
    void createLSLStream();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void sendingData();


private:
    Ui::MainWindow *ui;
    unsigned m_nbJoints=3*5;
    std::vector<std::vector<float>> m_choregraphy;
    lsl::stream_outlet* m_outlet=nullptr;
    QTimer m_timer;
    unsigned m_sendingInd=0;
};

#endif // MAINWINDOW_H
