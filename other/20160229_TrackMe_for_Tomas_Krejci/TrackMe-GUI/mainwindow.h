#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include "eventfeeder.h"
#include "recorder.h"

#include <QMainWindow>

/**
 * @brief The MainWindow class contains a tabbed widget with various
 * visualizations of the sensor data. The tabs can be found in the tabs folder.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

protected:
    void timerEvent(QTimerEvent *) override;
    EventFeeder *eventFeeder_;
    Recorder recorder_;
    void createMenu();

signals:
    void exportPlots(const QString &path);

protected slots:
    void openFile();
    void exportPlotsRequest();
    void startStatusBarTimer();
    void recordRequest();
};

#endif // MAINWINDOW_H
