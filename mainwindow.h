#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <vector>

class QCPAxisRect;
class QCPGraph;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void makePlots();
    void initializeTopBottomWidgets();

private slots:
    void realTimeDataSlot();

private:
    Ui::MainWindow *ui;
    QTimer dataTimer;
    std::vector< QCPAxisRect* > wideAxisRect;
    std::vector< QCPGraph* > mainGraphCos;

    const size_t NUMBER_CHARTS = 6;
};

#endif // MAINWINDOW_H
