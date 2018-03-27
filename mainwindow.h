#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QMessageBox>
#include <QMainWindow>
#include <QStandardItemModel>
#include "process.h"
#include "resource.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    void bindSelected(QString s);

public:
    explicit MainWindow(QWidget *parent = 0);
    void get_systemInfo();
    void get_cpuInfo();
    void update_processes();
    void update_resources();
    ~MainWindow();

private slots:
    void on_btn_kill_clicked();
    void updateAll();

private:
    Ui::MainWindow *ui;

    Process proc;
    Resource res;
    QTimer *timer;
    QString selectedPid;

};

#endif // MAINWINDOW_H
