#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("System Monitor");
    ui->tabWidget->setCurrentIndex(0);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateAll()));
    timer->start(1000);

    ui->process_table->setModel(proc.get_model());
    ui->process_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //均分列
    ui->process_table->verticalHeader()->hide();
    ui->process_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->process_table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->process_table->setFocusPolicy(Qt::NoFocus);
    ui->process_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // initial all page
    get_systemInfo();
    updateAll();

    ui->memGraphics->setScene(res.get_memScence());
    ui->swapGraphics->setScene(res.get_swapScence());
    ui->cpuGraphics->setScene(res.get_cpuScence());
}

MainWindow::~MainWindow()
{
    delete ui;
}

// get system information
void MainWindow::get_systemInfo()
{
    QFile file;
    QString tmp;
    QString display;
    QByteArray line;
    int head;
    int tail;

    // hostname
    file.setFileName("/proc/sys/kernel/hostname");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"error: can't open /proc/sys/kernel/hostname";
        return;
    }
    line = file.readLine();
    file.close();
    tmp = QString(line);
    display = tmp.mid(0, tmp.length()-1);
    ui->hostname->setText(display);

    // kernel version && gcc version
    file.setFileName("/proc/version");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"error: can't open /proc/version";
        return;
    }
    line = file.readLine();
    file.close();
    tmp = QString(line);
    head = tmp.indexOf("Linux version");
    tail = tmp.indexOf("(");
    display = tmp.mid(head+14, tail-head-14);
    ui->kernelVersion->setText(display);

    head = tmp.indexOf("gcc version");
    tail = tmp.indexOf(" )");
    display = QString(tmp.mid(head+12,tail-head-12));
    ui->gccVersion->setText(display);
}

// get cpu information
void MainWindow::get_cpuInfo()
{
    QFile file;
    QString line;
    QString display;
    QByteArray tmp;
    int count = 0;
    int pos;

    file.setFileName("/proc/cpuinfo");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"error: can't open /proc/cpuinfo";
        return;
    }

    while (count < 5) {
        tmp = file.readLine();
        line = QString(tmp);
        if((pos = line.indexOf("model name")) != -1){
            display = line.mid(pos+12, line.length()-13);
            ui->processor->setText(display);
            ++count;
        }
        else if((pos = line.indexOf("cpu cores")) != -1){
            display = line.mid(pos+11, line.length()-12);
            ui->coresNum->setText(display);
            ++count;
        }
        else if((pos = line.indexOf("cpu MHz")) != -1){
            display = line.mid(pos+10, line.length()-11);
            ui->frequency->setText(display);
            ++count;
        }
        else if((pos = line.indexOf("cache size")) != -1){
            display = line.mid(pos+12, line.length()-13);
            ui->cacheSize->setText(display);
            ++count;
        }
        else if((pos = line.indexOf("address sizes")) != -1){
            display = line.mid(pos+15, line.length()-16);
            ui->addressSize->setText(display);
            ++count;
        }
        else continue;
    }
    file.close();

    file.setFileName("/proc/uptime");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"error: can't open /proc/cpuinfo";
        return;
    }

    tmp = file.readLine();
    file.close();
    line = QString(tmp);
    line = line.section(" ", 0, 0);
    count = (int) line.toDouble();
    QTime system_runtime(0, 0, 0, 0);
    ui->system_runtime->setText(system_runtime.addSecs(count).toString("hh:mm:ss"));
}

// update process ui
void MainWindow::update_processes()
{
    QStandardItem *item = proc.get_model()->item(ui->process_table->currentIndex().row(),1);
    if(item)
        selectedPid = item->data(Qt::DisplayRole).toString();
    else
        selectedPid = "";
    proc.updateTable();
    bindSelected(selectedPid);      // bind selected process
    ui->total_processes->setText(QString::number(proc.get_total()));
    ui->running_processes->setText(QString::number(proc.get_running()));
}

// update resource ui
void MainWindow::update_resources()
{
    res.update_memory();
    res.update_cpu_rate();

    ui->mem_total->setNum(res.get_memTotal());
    ui->mem_available->setNum(res.get_memAvaiable());
    ui->swap_total->setNum(res.get_swapTotal());
    ui->swap_free->setNum(res.get_swapFree());
    ui->cpu_usage_rate->setText(QString::number(res.get_cpu_usage(), 'f', 1));
}

// real-time update
void MainWindow::updateAll()
{
    get_cpuInfo();
    update_processes();
    update_resources();
}

// kill process
void MainWindow::on_btn_kill_clicked()
{
    QMessageBox box;

    if(selectedPid == "") return;

    box.setWindowTitle("kill process");
    box.setText("Are you sure to kill process " + selectedPid.toLatin1() + "?");
    box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    if(box.exec() == QMessageBox::Yes){
        int res = system("kill "+selectedPid.toLatin1());
        if(res < 0)
            ui->statusBar->showMessage("kill failed.",3000);
        else
            ui->statusBar->showMessage("process "+selectedPid.toLatin1()+" has been killed.",3000);
    }
}

// bind selected
void MainWindow::bindSelected(QString text)
{
    QList<QStandardItem *> list = proc.get_model()->findItems(text, Qt::MatchExactly,1);
    if(list.isEmpty()){
        return;
    }
    ui->process_table->selectRow(list.at(0)->row());
}
