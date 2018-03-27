#include "resource.h"
#include <stdlib.h>
#include <time.h>

Resource::Resource(QWidget *parent) :
    QWidget(parent),
    head(new CpuStat[5]),
    tail(new CpuStat[5]),
    memScence(new QGraphicsScene),
    memSeries(new QPieSeries),
    memChart(new QChart),
    swapScence(new QGraphicsScene),
    swapSeries(new QPieSeries),
    swapChart(new QChart),
    cpuScence(new QGraphicsScene),
    cpuChart(new QChart),
    splineSeries(new QSplineSeries)
{
    maxSize = 81;
    maxX = 80;
    maxY = 100;

    cpuChart->legend()->hide();
    cpuChart->addSeries(splineSeries);
    cpuChart->createDefaultAxes();
    cpuChart->axisX()->setRange(0, maxX);
    cpuChart->axisY()->setRange(0, maxY);
    cpuChart->setGeometry(0, 0, 900, 300);
    cpuScence->addItem(cpuChart);

    memChart->addSeries(memSeries);
    memChart->legend()->setAlignment(Qt::AlignRight);
    memChart->setTheme(QChart::ChartThemeLight);
    memChart->legend()->setFont(QFont("Arial", 12));
    memChart->setGeometry(0, 0, 300, 300);
    memScence->addItem(memChart);

    swapChart->addSeries(swapSeries);
    swapChart->legend()->setAlignment(Qt::AlignRight);
    swapChart->setTheme(QChart::ChartThemeLight);
    swapChart->legend()->setFont(QFont("Arial", 12));
    swapChart->setGeometry(0, 0, 300, 300);
    swapScence->addItem(swapChart);
}

void Resource::get_cpu_stat(CpuStat *arry)
{
    QFile file;
    QByteArray tmp;
    QString line;
    double x;

    file.setFileName("/proc/stat");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"error: can't open /proc/meminfo";
        return;
    }

    arry->total = 0;
    arry->idle = 0;
    tmp = file.readLine();
    file.close();
    line = QString(tmp);
    for(int j=1; j<=7; ++j){
        x = line.section(" ", j+1, j+1).toDouble();
        arry->total += x;
        if(j == 4) arry->idle = x;
    }
}

void Resource::cal_cpu_rate()
{
    get_cpu_stat(head);
    sleep(101);
    get_cpu_stat(tail);

    if(tail->total-head->total < 1e-3) return;
    cpu_usage = 1 - (tail->idle-head->idle)/(tail->total-head->total);
    cpu_usage *= 100;
}

void Resource::update_cpu_rate()
{
    cal_cpu_rate();
    data.push_front(cpu_usage);

    while (data.size() > maxSize)
        data.removeLast();

    splineSeries->clear();
    int dx = maxX / (maxSize-1);
    for (int i = 0; i < data.size(); ++i)
        splineSeries->append(i*dx, data.at(i));
}

void Resource::update_memory()
{
    QFile file;
    QString line;
    QString display;
    QByteArray tmp;
    int count = 0;
    int pos;

    file.setFileName("/proc/meminfo");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"error: can't open /proc/meminfo";
        return;
    }

    while (count < 4) {
        tmp = file.readLine();
        line = QString(tmp);
        if((pos = line.indexOf("MemTotal")) != -1){
            display = line.mid(pos+9, line.length()-12);
            display = display.trimmed();
            memTotal = (int) display.toULong()/1024;
            ++count;
        }
        else if((pos = line.indexOf("MemAvailable")) != -1){
            display = line.mid(pos+13, line.length()-16);
            display = display.trimmed();
            memAvaiable = (int) display.toULong()/1024;
            ++count;
        }
        else if((pos = line.indexOf("SwapTotal")) != -1){
            display = line.mid(pos+10, line.length()-13);
            display = display.trimmed();
            swapTotal = (int) display.toULong()/1024;
            ++count;
        }
        else if((pos = line.indexOf("SwapFree")) != -1){
            display = line.mid(pos+9, line.length()-12);
            display = display.trimmed();
            swapFree = (int) display.toULong()/1024;
            ++count;
        }
        else continue;
    }

    file.close();

    memSeries->clear();
    memSeries->append("used", memTotal-memAvaiable);
    memSeries->append("free", memAvaiable);

    swapSeries->clear();
    swapSeries->append("used", swapTotal-swapFree);
    swapSeries->append("free", swapFree);
}

int Resource::get_memTotal()
{
    return memTotal;
}

int Resource::get_memAvaiable()
{
    return memAvaiable;
}

int Resource::get_swapTotal()
{
    return swapTotal;
}

int Resource::get_swapFree()
{
    return swapFree;
}

double Resource::get_cpu_usage()
{
    return cpu_usage;
}

QGraphicsScene *Resource::get_cpuScence()
{
    return cpuScence;
}

QGraphicsScene *Resource::get_memScence()
{
    return memScence;
}

QGraphicsScene *Resource::get_swapScence()
{
    return swapScence;
}

void Resource::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
