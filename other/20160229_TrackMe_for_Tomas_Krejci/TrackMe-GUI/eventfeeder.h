#ifndef EVENTFEEDER_H
#define EVENTFEEDER_H

#include "config.h"
#include <TrackMe.hpp>

#include <QObject>
#include <QtNetwork>

#include <thread>
#include <atomic>

/**
 * @brief The EventFeeder class is a wrapper for the TrackMe library which
 * converts its callbacks to Qt signals and distributes the events to the
 * corresponding tabs. It extends the library by a TCP server capability
 * used to read events from the network.
 *
 * To explore the implementation of the sensor fusion, read
 * the run() function in the TrackMe.hpp file from the TrackMe library instead.
 */
class EventFeeder : public QObject
{
    Q_OBJECT
public:
    explicit EventFeeder(QObject *parent = 0);
    RawEvent::TimestampT latestTimestamp() const;
    RawEvent::TimestampT baseTimestamp() const;
    RawEvent::ValueT eventRate() const;
    TrackMe::SensorStats<RawEvent::ValueT> accelerometerStats() const;
    TrackMe::SensorStats<RawEvent::ValueT> gyroscopeStats() const;
    TrackMe::SensorStats<RawEvent::ValueT> magneticFieldStats() const;
    bool calibrated() const;
    bool initializeServer();
    void publishEvent(const RawEvent &);
    void openFile(const QString &);
    virtual ~EventFeeder() override;

protected:
    // To synchronize all the tabs, the feeder keeps track of the oldest
    // and newest timestamps received.
    RawEvent::TimestampT latestTimestamp_
      {std::numeric_limits<RawEvent::TimestampT>::min()};
    RawEvent::TimestampT baseTimestamp_
      {std::numeric_limits<RawEvent::TimestampT>::max()};
    std::size_t numReceived_{0};

    std::thread fileReader_;
    QFile sourceFile_;
    std::atomic<bool> fileReaderStopReq_{false};

    // Positioner is the TrackMe library core.
    TrackMe::Tracker<RawEvent> positioner_;

private:
    QTcpServer *tcpServer_{nullptr};
    QTcpSocket *tcpConnection_{nullptr};

signals:
    void newRawEvent(const RawEvent &e);
    void newFilteredEvent(const RawEvent &e);
    void newVelocityEvent(const RawEvent &e);
    void newDisplacementEvent(const RawEvent &e);
    void newOrientationEvent(const OrientationEvent &e);
    void stopEvent();
    void restartEvent();
    void serverText(const QString &text);

protected slots:
    void setLatestTimestamp(const RawEvent &);
    void setBaseTimestamp(const RawEvent &);
    void handleConnection();
    void receiveEvents();
    void socketError(QAbstractSocket::SocketError error);
    void closeConnection();
    void resetValues();
};

#endif // EVENTFEEDER_H
