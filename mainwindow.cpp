#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializeTopBottomWidgets();
    makePlots();

    connect(ui->stopButton, &QPushButton::toggled, this, &MainWindow::stop);
}

MainWindow::~MainWindow()
{
    delete ui;

    for(size_t i = 0; i < NUMBER_CHARTS; i++)
    {
        delete wideAxisRect.at(i);
    }
}

void MainWindow::makePlots()
{
    // configure axis rect:
    ui->customPlot->plotLayout()->clear(); // clear default axis rect so we can start from scratch

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(0, 0, 0));
    plotGradient.setColorAt(1, QColor(0, 0, 0));

    for(size_t i = 0; i < NUMBER_CHARTS; i++)
    {
        wideAxisRect.push_back(new QCPAxisRect(ui->customPlot));
        wideAxisRect.at(i)->setMinimumMargins(QMargins(0,0,0,0));
        wideAxisRect.at(i)->setMinimumSize(QSize(240, 40));
        wideAxisRect.at(i)->setMaximumSize(QSize(240, 40));
        wideAxisRect.at(i)->setBackground(plotGradient);
    }

    QCPLayoutGrid *subLayout = new QCPLayoutGrid;

    for(size_t i = 0; i < wideAxisRect.size(); i++)
    {
        subLayout->addElement(i, 0, wideAxisRect.at(i));
    }

    ui->customPlot->plotLayout()->addElement(0, 0, subLayout);

    for(size_t i = 0; i < wideAxisRect.size(); i++)
    {
        mainGraphCos.push_back(ui->customPlot->addGraph(wideAxisRect.at(i)->axis(QCPAxis::atBottom),
                                                        wideAxisRect.at(i)->axis(QCPAxis::atLeft)));
        mainGraphCos.at(i)->setPen(QPen(QColor(0, 198, 24)));
        mainGraphCos.at(i)->valueAxis()->setRange(-1.2, 1.2);
        mainGraphCos.at(i)->keyAxis()->setVisible(false);
        mainGraphCos.at(i)->valueAxis()->setVisible(false);
    }

    for(size_t i = 0; i < NUMBER_CHARTS; i++)
    {
        connect(mainGraphCos.at(i)->keyAxis(), SIGNAL(rangeChanged(QCPRange)), mainGraphCos.at(i)->keyAxis(), SLOT(setRange(QCPRange)));
        connect(mainGraphCos.at(i)->valueAxis(), SIGNAL(rangeChanged(QCPRange)), mainGraphCos.at(i)->valueAxis(), SLOT(setRange(QCPRange)));
    }
    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realTimeDataSlot()));
    dataTimer.start(8); // Interval 0 means to refresh as fast as possible
}

void MainWindow::initializeTopBottomWidgets()
{
    QPalette paletTop, paletBottom;
    paletTop.setBrush(ui->topWidget->backgroundRole(), QBrush(QPixmap(":/images/top.png")));
    ui->topWidget->setPalette(paletTop);
    ui->topWidget->setAutoFillBackground(true);

    paletBottom.setBrush(ui->bottomWidget->backgroundRole(), QBrush(QPixmap(":/images/bottom.png")));
    ui->bottomWidget->setPalette(paletBottom);
    ui->bottomWidget->setAutoFillBackground(true);
}

void MainWindow::realTimeDataSlot()
{
    static double key;
    key += 0.01;

    static QTime time(QTime::currentTime());
    // calculate two new data points:
    double timeInSec = (time.elapsed()/1000.0); // time elapsed since start of demo, in seconds

    qDebug() << "realTimeDataSlot() " << key;

    // add data to lines:
    for(const auto& mainGraph : mainGraphCos)
    {
        mainGraph->addData(key, qSin(key/0.4364));
    }

    // remove the old data points from a graph to prevent memory overflow
    for(int i = 0; i < ui->customPlot->graphCount(); i++)
    {
        ui->customPlot->graph(i)->data()->removeBefore(key - 8);
    }

    // make key axis range scroll with the data (at a constant range size of 8):
    for(const auto& mainGraph : mainGraphCos)
    {
        mainGraph->keyAxis()->setRange(key, 8, Qt::AlignRight);
    }

    ui->customPlot->replot();

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (timeInSec - lastFpsKey > 1) // average fps over 1 seconds
    {
      ui->statusBar->showMessage(
            QString("%1 FPS, Total Data points: %2")
            .arg(frameCount/(timeInSec - lastFpsKey), 0, 'f', 0)
            .arg(mainGraphCos.at(0)->data()->size())
            , 0);
      qDebug() << "size() " << mainGraphCos.at(0)->data()->size();
      lastFpsKey = timeInSec;
      frameCount = 0;
    }
}

void MainWindow::stop(bool checked)
{
    if(checked)
    {
        dataTimer.stop();
        return;
    }

    dataTimer.start(8);
}
