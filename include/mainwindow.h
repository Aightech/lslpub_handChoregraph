#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**
 * \file mainwindow.h
 * \brief TODO.
 * \author Alexis Devillard
 * \version 1.0
 * \date 03 march 2019
 */

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
    void updateChoregraphy(int,int);
    void quickTableChange(int,int);
    void startLSLStream();
    void createLSLStream(int i);
    void addStep();
    void rmStep();
    void clearStep();
    void desactivate();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void sendingData();


private:
    void enableGUI(bool);

    Ui::MainWindow *ui;
    unsigned m_nbJoints=3*5;
    std::vector<std::vector<float>> m_choregraphy;
    lsl::stream_outlet* m_outlet[2] = {nullptr,nullptr};
    QTimer m_timer;
    unsigned m_sendingInd=0;
};

#endif // MAINWINDOW_H
