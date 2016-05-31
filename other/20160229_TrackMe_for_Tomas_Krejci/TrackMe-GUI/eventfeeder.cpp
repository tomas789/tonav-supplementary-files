#include "eventfeeder.h"

#include <TrackMe.hpp>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QtGlobal>
#include <cstdlib>

EventFeeder::EventFeeder(QObject *parent) :
  QObject(parent)
{
  // Pass the configuration to the positioner
  positioner_.config() = CONFIG;

  // Synchronize earliest and latest timestamps of all sensors
  connect(this, SIGNAL(newRawEvent(RawEvent)),
          this, SLOT(setLatestTimestamp(RawEvent)));
  connect(this, SIGNAL(newRawEvent(RawEvent)),
          this, SLOT(setBaseTimestamp(RawEvent)));
  connect(this, SIGNAL(restartEvent()),
          this, SLOT(resetValues()));

  // Now we will convert the callbacks provided by the TrackMe library
  // to the Qt signals.

  // Accelerometer events
  positioner_.setRawAccelerometerCallback(
    [&](const RawEvent &e) { emit newRawEvent(e); });
  positioner_.setFilteredAccelerometerCallback(
    [&](const RawEvent &e) { emit newFilteredEvent(e); });
  positioner_.setVelocityCallback(
    [&](const RawEvent &e) { emit newVelocityEvent(e); });
  positioner_.setDisplacementCallback(
    [&](const RawEvent &e) { emit newDisplacementEvent(e); });

  // Gyroscope events
  positioner_.setRawGyroscopeCallback(
    [&](const RawEvent &e) { emit newRawEvent(e); });
  positioner_.setFilteredGyroscopeCallback(
    [&](const RawEvent &e) { emit newFilteredEvent(e); });
  positioner_.setOrientationCallback(
    [&](const OrientationEvent &e) { emit newOrientationEvent(e); });

  // Magnetic field events
  positioner_.setRawMagneticFieldCallback(
    [&](const RawEvent &e) { emit newRawEvent(e); });
  positioner_.setFilteredMagneticFieldCallback(
    [&](const RawEvent &e) { emit newFilteredEvent(e); });
}

/**
 * @brief Start TCP server and wait for a connection.
 */
bool EventFeeder::initializeServer()
{
  // Start the server.
  tcpServer_ = new QTcpServer{this};
  if (!tcpServer_->listen(QHostAddress::Any, TCP_PORT)) {
    int chosen = QMessageBox::critical(nullptr,
                   "Server error",
                   tr("Unable to listen on port %1").arg(TCP_PORT),
                   QMessageBox::Retry | QMessageBox::Abort,
                   QMessageBox::Retry);

    switch (chosen) {
    case QMessageBox::Retry:
      initializeServer();
      break;
    case QMessageBox::Abort:
      std::exit(1);
      break;
    default:;
    }

    return true;
  }

  // Create a list of addresses and ports to be displayed in the status bar.
  QStringList ipAddresses;
  // Find all non-localhost IPv4 addresses
  QList<QHostAddress> allAddresses = QNetworkInterface::allAddresses();
  for (auto address : allAddresses) {
    if (address != QHostAddress::LocalHost && address.toIPv4Address())
      ipAddresses << address.toString();
  }
  QString text =
    QString("Server listening on addresses ") +
    ipAddresses.join(", ") +
    QString(" TCP port ") +
    QString::number(tcpServer_->serverPort());

  qDebug() << text;
  emit serverText(text);

  // Set the connection handler.
  connect(tcpServer_, SIGNAL(newConnection()), this, SLOT(handleConnection()));

  return true;
}

/**
 * @brief Handle an incoming connection request. If there is a communication in
 * progress, reject the request.
 */
void EventFeeder::handleConnection()
{
  // Allow only one client
  if (tcpConnection_ != nullptr) {
    tcpServer_->nextPendingConnection()->close();
    return;
  }

  // On success, set the incoming event handler.
  tcpConnection_ = tcpServer_->nextPendingConnection();
  connect(tcpConnection_, SIGNAL(disconnected()),
          this, SLOT(closeConnection()));
  connect(tcpConnection_, SIGNAL(readyRead()),
          this, SLOT(receiveEvents()));
  connect(tcpConnection_, SIGNAL(error(QAbstractSocket::SocketError)),
          this, SLOT(socketError(QAbstractSocket::SocketError)));
}

/**
 * @brief Reset the newest and the oldest timestamp for each new connection.
 */
void EventFeeder::resetValues()
{
  latestTimestamp_ = std::numeric_limits<RawEvent::TimestampT>::min();
  baseTimestamp_ = std::numeric_limits<RawEvent::TimestampT>::max();
}

/**
 * @brief Handle the incoming events.
 */
void EventFeeder::receiveEvents()
{
  // Wait for enough blocks available
  while (tcpConnection_->bytesAvailable() >= RawEvent::byteArraySize) {
    auto block = tcpConnection_->read(RawEvent::byteArraySize);
    // convert block to vector<char>
    std::vector<char> vblock{block.data(), block.data() + block.size()};
    // read event from byte array
    auto event = RawEvent::fromByteArray(vblock);

    publishEvent(event);
  }
}

/**
 * @brief Simulate real-time reception of events when reading them from a file.
 */
void EventFeeder::openFile(const QString &fileName)
{
  // If there is running another file, request it to stop
  if (fileReader_.joinable()) {
    fileReaderStopReq_ = true;
    fileReader_.join();
    fileReaderStopReq_ = false;
  }

  // If there is a running network communication, show error
  if (tcpConnection_ != nullptr) {
    QMessageBox::information(nullptr,
                 "Program error",
                 "Cannot open file while receiving data from network.");
    return;
  }

  // Open the sensor data file
  sourceFile_.setFileName(fileName);
  if (!sourceFile_.open(QIODevice::ReadOnly)) {
    qDebug() << "Cannot open the file.";
    return;
  }

  // Create a dummy TCP connection
  tcpConnection_ = (QTcpSocket*)1;

  // Run the simulation
  fileReader_ = std::thread{[&, this]() {
    std::vector<char> vblock(RawEvent::byteArraySize);
    RawEvent::TimestampT prev_timestamp = -1;

    // Read events from file
    while (!fileReaderStopReq_ &&
           sourceFile_.read(vblock.data(), RawEvent::byteArraySize) > 0) {
      // read event from byte array
      auto event = RawEvent::fromByteArray(vblock);

      // Break if there is an invalid event
      if (event.type == TrackMe::EventType::UNKNOWN) {
        qDebug() << "The data in file is invalid. Stopping the reading.";
        break;
      }

      // Sleep for time duration between two events
      if (!TrackMe::isSystemEvent(event.type)) {
        if (prev_timestamp != -1) {
          auto dur = std::chrono::duration<RawEvent::ValueT, RawEvent::Ratio>
            {event.timestamp - prev_timestamp};
          std::this_thread::sleep_for(dur);
        }
        prev_timestamp = event.timestamp;
      }

      publishEvent(event);
    }
    sourceFile_.close();
    tcpConnection_ = nullptr;
  }};
}

void EventFeeder::publishEvent(const RawEvent &event) {
    // publish event
    positioner_.receiveEvent(event);

    if (event.type == TrackMe::EventType::STOP) {
      emit stopEvent();
    }
    else if (event.type == TrackMe::EventType::START) {
      emit restartEvent();
      numReceived_ = 1;
    }

    // increment counter
    ++numReceived_;
}

void EventFeeder::socketError(QAbstractSocket::SocketError error)
{
  if (error != QAbstractSocket::RemoteHostClosedError)
    qDebug() << "Socket error: " << error;
}

void EventFeeder::closeConnection()
{
  tcpConnection_->close();
  tcpConnection_->deleteLater();
  tcpConnection_ = nullptr;
}

void EventFeeder::setLatestTimestamp(const RawEvent &e)
{
  // Only one thread accesses the variable, no mutex needed
  // Take maximum
  if (this->latestTimestamp_ < e.timestamp)
    this->latestTimestamp_ = e.timestamp;
}

RawEvent::TimestampT EventFeeder::latestTimestamp() const
{
  return this->latestTimestamp_;
}

RawEvent::ValueT EventFeeder::eventRate() const
{
  auto time = latestTimestamp_ - baseTimestamp_;
  return numReceived_ / TrackMe::timestampToSeconds<RawEvent>(time);
}

void EventFeeder::setBaseTimestamp(const RawEvent &e)
{
  // Only one thread accesses the variable, no mutex needed
  // Take maximum
  if (this->baseTimestamp_ > e.timestamp)
    this->baseTimestamp_ = e.timestamp;
}

RawEvent::TimestampT EventFeeder::baseTimestamp() const
{
  return this->baseTimestamp_;
}

TrackMe::SensorStats<RawEvent::ValueT>
  EventFeeder::accelerometerStats() const
{
  return positioner_.accelerometerStats();
}

TrackMe::SensorStats<RawEvent::ValueT>
  EventFeeder::gyroscopeStats() const
{
  return positioner_.gyroscopeStats();
}

TrackMe::SensorStats<RawEvent::ValueT>
  EventFeeder::magneticFieldStats() const
{
  return positioner_.magneticFieldStats();
}

bool EventFeeder::calibrated() const
{
  return positioner_.calibrated();
}

EventFeeder::~EventFeeder()
{
  if (fileReader_.joinable()) {
    fileReaderStopReq_ = true;
    fileReader_.join();
  }
  if (tcpConnection_)
    tcpConnection_->close();
  positioner_.stop();
}
