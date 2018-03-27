#ifndef PROCESS_H
#define PROCESS_H

#include <QDir>
#include <QDebug>
#include <QStandardItemModel>

class Process
{
private:
    int total;                              // total processes
    int running;                            // running processes

    QString process_name;
    QString pid;
    QString status;
    QString ppid;
    QString memory;
    QString cpu_time;       // TODO
    QString priority;

    QFile file;
    QStringList dirList;                    // all dirs in /proc
    QStandardItemModel *process_model;

public:
    Process();
    void updateTable();                     // update processes

    int get_total();
    int get_running();
    bool is_not_digital(QString s);
    void convert_state(QString s);
    QStandardItemModel *get_model();        // get QStandardItemModel
};

#endif // PROCESS_H
