#include "mainwindow.h"

#include "tabs/rawsensorstab.h"
#include "tabs/filteredsensorstab.h"
#include "tabs/orientationtab.h"
#include "tabs/infotab.h"
#include "tabs/velocitytab.h"
#include "tabs/displacementtab.h"

#include <QHBoxLayout>
#include <QFileDialog>
#include <QStatusBar>
#include <QTabBar>
#include <QMenuBar>

/**
 * @brief Initialize the individual tabs.
 */
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  eventFeeder_ = new EventFeeder(this);

  auto tabWidget = new QTabWidget;
  connect(eventFeeder_, SIGNAL(serverText(QString)),
          statusBar(), SLOT(showMessage(QString)));
  // wait for the first packet and then start the status bar refreshing
  connect(eventFeeder_, SIGNAL(restartEvent()),
          this, SLOT(startStatusBarTimer()));

  auto rawSensorsTab = new RawSensorsTab;
  rawSensorsTab->setEventFeeder(eventFeeder_);
  connect(eventFeeder_, SIGNAL(newRawEvent(RawEvent)),
          rawSensorsTab, SLOT(addRawEvent(RawEvent)));
  connect(eventFeeder_, SIGNAL(restartEvent()),
          rawSensorsTab, SLOT(restart()));
  connect(eventFeeder_, SIGNAL(stopEvent()),
          rawSensorsTab, SLOT(stop()));
  connect(this, SIGNAL(exportPlots(QString)),
          rawSensorsTab, SLOT(exportPlots(QString)));

  auto filteredSensorsTab = new FilteredSensorsTab;
  filteredSensorsTab->setEventFeeder(eventFeeder_);
  connect(eventFeeder_, SIGNAL(newFilteredEvent(RawEvent)),
          filteredSensorsTab, SLOT(addRawEvent(RawEvent)));
  connect(eventFeeder_, SIGNAL(restartEvent()),
          filteredSensorsTab, SLOT(restart()));
  connect(eventFeeder_, SIGNAL(stopEvent()),
          filteredSensorsTab, SLOT(stop()));
  connect(this, SIGNAL(exportPlots(QString)),
          filteredSensorsTab, SLOT(exportPlots(QString)));

  auto orientationTab = new OrientationTab;
  orientationTab->setEventFeeder(eventFeeder_);
  connect(eventFeeder_, SIGNAL(newOrientationEvent(OrientationEvent)),
          orientationTab, SLOT(addOrientationEvent(OrientationEvent)));
  connect(eventFeeder_, SIGNAL(restartEvent()),
          orientationTab, SLOT(restart()));
  connect(eventFeeder_, SIGNAL(stopEvent()),
          orientationTab, SLOT(stop()));
  connect(this, SIGNAL(exportPlots(QString)),
          orientationTab, SLOT(exportPlots(QString)));

  auto velocityTab = new VelocityTab;
  velocityTab->setEventFeeder(eventFeeder_);
  connect(eventFeeder_, SIGNAL(newVelocityEvent(RawEvent)),
          velocityTab, SLOT(addVelocityEvent(RawEvent)));
  connect(eventFeeder_, SIGNAL(restartEvent()),
          velocityTab, SLOT(restart()));
  connect(eventFeeder_, SIGNAL(stopEvent()),
          velocityTab, SLOT(stop()));
  connect(this, SIGNAL(exportPlots(QString)),
          velocityTab, SLOT(exportPlots(QString)));

  auto displacementTab = new DisplacementTab;
  displacementTab->setEventFeeder(eventFeeder_);
  connect(eventFeeder_, SIGNAL(newDisplacementEvent(RawEvent)),
          displacementTab, SLOT(addDisplacementEvent(RawEvent)));
  connect(eventFeeder_, SIGNAL(restartEvent()),
          displacementTab, SLOT(restart()));
  connect(eventFeeder_, SIGNAL(stopEvent()),
          displacementTab, SLOT(stop()));
  connect(this, SIGNAL(exportPlots(QString)),
          displacementTab, SLOT(exportPlots(QString)));

  auto infoTab = new InfoTab;
  infoTab->setEventFeeder(eventFeeder_);
  connect(eventFeeder_, SIGNAL(restartEvent()),
          infoTab, SLOT(restart()));
  connect(eventFeeder_, SIGNAL(stopEvent()),
          infoTab, SLOT(stop()));

  tabWidget->addTab(rawSensorsTab, "Raw sensor data");
  tabWidget->addTab(filteredSensorsTab, "Filtered sensor data");
  tabWidget->addTab(velocityTab, "Velocity");
  tabWidget->addTab(displacementTab, "Displacement");
  tabWidget->addTab(orientationTab, "Orientation");
  tabWidget->addTab(infoTab, "Statistics");

  setCentralWidget(tabWidget);
  eventFeeder_->initializeServer();

  createMenu();
}

/**
 * @brief Creates and assigns signals to menu actions.
 */
void MainWindow::createMenu()
{
  auto fileMenu = menuBar()->addMenu("&File");
  auto openFileA = new QAction("Open file", this);
  connect(openFileA, SIGNAL(triggered()), this, SLOT(openFile()));
  fileMenu->addAction(openFileA);
  auto exportPlots = new QAction("Export plots to SVG", this);
  connect(exportPlots, SIGNAL(triggered()), this, SLOT(exportPlotsRequest()));
  fileMenu->addAction(exportPlots);
  auto recordNext = new QAction("Record next connection", this);
  connect(recordNext, SIGNAL(triggered()), this, SLOT(recordRequest()));
  fileMenu->addAction(recordNext);
}

/**
 * @brief Starts a timer for refreshing the status bar.
 */
void MainWindow::startStatusBarTimer()
{
  startTimer(PLOT_REFRESH_INTERVAL);
}

/**
 * @brief Slot for the "export plots" button.
 */
void MainWindow::exportPlotsRequest()
{
  QString path = QFileDialog::getExistingDirectory(this, "Select Folder");
  if (path != QString::null)
    emit exportPlots(path);
}

/**
 * @brief Slot for the "record connection" button. Stores the request to
 *        record the next connection.
 */
void MainWindow::recordRequest()
{
  auto path = QFileDialog::getSaveFileName(this, "Destination File",
                                           QString(),
                                           "Sensor data file (*.sns)");
  if (path != QString::null)
    recorder_.request(eventFeeder_, path);
}

/**
 * @brief Slot for the "open file" button. The file path is chosen and
 *        passed to the event feeder.
 */
void MainWindow::openFile()
{
  // Choose the sensor data file
  auto path = QFileDialog::getOpenFileName(this, "Select sensor file",
                                           QString(),
                                           "Sensor data file (*.sns)");
  if (path != QString::null)
    eventFeeder_->openFile(path);
}

/**
 * @brief Refresh the status bar on timer event.
 */
void MainWindow::timerEvent(QTimerEvent *)
{
  QString message;

  if (recorder_.recording())
    message += "RECORDING, ";

  message += QString("Event rate: ") +
             QString::number(eventFeeder_->eventRate()) +
             QString(" events/sec");

  statusBar()->showMessage(message);
}
