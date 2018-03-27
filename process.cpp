#include "process.h"

Process::Process():
    total(0),
    running(0),
    process_model(new QStandardItemModel())
{
    process_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("process name")));
    process_model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("pid")));
    process_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("status")));
    process_model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("ppid")));
    process_model->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("memory")));
    process_model->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("priority")));
}

void Process::updateTable()
{
    total = 0;
    running = 0;

    // read dirlist in /proc
    QDir dir("/proc");
    dirList = dir.entryList();
    for(int i = 0; i < dirList.size(); ++i){
        pid = dirList.at(i);                     // process id
        if(is_not_digital(pid)) continue;
        file.setFileName("/proc/"+pid+"/stat");
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "open file /proc/" << pid << "/stat failed.\n";
            return;
        }

        QByteArray line = file.readLine();
        file.close();
        QString tmp = QString(line);
        int x1 = tmp.indexOf("(");
        int x2 = tmp.indexOf(")");
        process_name = tmp.mid(x1+1, x2-x1-1);              // process_name
        convert_state(tmp.section(" ", 2, 2));              // status
        ppid = tmp.section(" ", 3, 3);                      // ppid
        priority = tmp.section(" ", 17, 17);                // priority
        memory = tmp.section(" ", 22, 22);                  // memory

        process_model->setItem(total,0,new QStandardItem(process_name));
        process_model->setItem(total,1,new QStandardItem(pid));
        process_model->setItem(total,2,new QStandardItem(status));
        process_model->setItem(total,3,new QStandardItem(ppid));
        process_model->setItem(total,4,new QStandardItem(QString::number(memory.toULong()/1024/1024)+"MB"));
        process_model->setItem(total,5,new QStandardItem(priority));
        ++total;
    }

    for(int i = 0; i < dirList.size(); ++i){
        pid = dirList.at(i);                     // process id
        if(is_not_digital(pid)) continue;
        file.setFileName("/proc/"+pid+"/statm");
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "open file /proc/" << pid << "/stat failed.\n";
            return;
        }

        QByteArray line = file.readLine();
        file.close();
        QString tmp = QString(line);
        memory = tmp.section(" ", 22, 22);                  // memory

        process_model->setItem(total,0,new QStandardItem(process_name));
        process_model->setItem(total,1,new QStandardItem(pid));
        process_model->setItem(total,2,new QStandardItem(status));
        process_model->setItem(total,3,new QStandardItem(ppid));
        process_model->setItem(total,4,new QStandardItem(QString::number(memory.toULong()/1024/1024)+"MB"));
        process_model->setItem(total,5,new QStandardItem(priority));
        ++total;
    }

}

int Process::get_total()
{
    return total;
}

int Process::get_running()
{
    return running;
}

QStandardItemModel *Process::get_model()
{
    return process_model;
}


bool Process::is_not_digital(QString s)
{
    QByteArray tmp = s.toLatin1();
    const char* c = tmp.data();

    while(*c && *c>='0' && *c<='9') ++c;
    if(*c) return true;                     // s is not digital
    else return false;                      // s is a digital
}

void Process::convert_state(QString s)
{
    switch (s.at(0).toLatin1()) {
    case 'R':
        status = "Running";
        ++running;
        break;
    case 'S':
        status = "Sleeping";
        break;
    case 'D':
        status = "Waiting";
        break;
    case 'Z':
        status = "Zombie";
        break;
    case 'T':
        status = "Stopped";
        break;
    default:
        break;
    }
}
