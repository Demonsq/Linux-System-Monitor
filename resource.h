#ifndef RESOURCE_H
#define RESOURCE_H

#include <QtCharts>
#include <QTimer>
#include <QWidget>

struct CpuStat{
    double total;
    double idle;
};

class Resource: public QWidget
{
public:
    Resource(QWidget *parent = 0);
    void update_cpu_rate();
    void update_memory();
    int get_memTotal();
    int get_memAvaiable();
    int get_swapTotal();
    int get_swapFree();
    double get_cpu_usage();
    QGraphicsScene *get_cpuScence();
    QGraphicsScene *get_memScence();
    QGraphicsScene *get_swapScence();

private:
    void get_cpu_stat(CpuStat *arry);
    void cal_cpu_rate();
    void sleep(unsigned int msec);

private:    
    CpuStat *head;
    CpuStat *tail;

    int memTotal;
    int memAvaiable;
    int swapTotal;
    int swapFree;

    // cpu view
    int maxSize;
    int maxX;
    int maxY;
    QList<double> data;
    double cpu_usage;

    QSplineSeries *splineSeries;
    QGraphicsScene *cpuScence;
    QChart *cpuChart;

    // memory view
    QGraphicsScene *memScence;
    QPieSeries *memSeries;
//    QPieSlice *memSlice;
    QChart *memChart;

    // swap view
    QGraphicsScene *swapScence;
    QPieSeries *swapSeries;
//    QPieSlice *swapSlice;
    QChart *swapChart;
};

#endif // RESOURCE_H
