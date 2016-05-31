#include "recorder.h"

Recorder::Recorder(QObject *parent)
  : QObject(parent)
{
}

Recorder::~Recorder()
{
  closeFile();
}

/**
 * @brief Open the file and write the START event.
 */
void Recorder::initFile()
{
  if (file_.isWritable()) {
    recording_ = true;
    RawEvent e{};
    e.type = TrackMe::EventType::START;
    auto ba = e.toByteArray();
    file_.write(ba.data(), RawEvent::byteArraySize);
  }
}

/**
 * @brief Close the file and write the STOP event.
 */
void Recorder::closeFile()
{
  if (file_.isOpen()) {
    RawEvent e{};
    e.type = TrackMe::EventType::STOP;
    auto ba = e.toByteArray();
    file_.write(ba.data(), RawEvent::byteArraySize);
    file_.close();
    recording_ = false;
  }
}

/**
 * @brief Store the recording request. It will wait for the next START event and
 *        than it will record the communication.
 */
void Recorder::request(const EventFeeder *eventFeeder,
                       const QString &path)
{
  eventFeeder_ = eventFeeder;
  file_.setFileName(path);

  if (!file_.open(QIODevice::WriteOnly)) {
    qDebug() << "Cannot open the file for recording.";
    return;
  }

  connect(eventFeeder_, SIGNAL(restartEvent()),
          this, SLOT(receiveStartEvent()));
}

bool Recorder::recording() const
{
  return recording_;
}

/**
 * @brief When the START event is received, start receiving all other events
 *        until the STOP event.
 */
void Recorder::receiveStartEvent()
{
  initFile();
  connect(eventFeeder_, SIGNAL(newRawEvent(RawEvent)),
          this, SLOT(receiveRawEvent(RawEvent)));
  connect(eventFeeder_, SIGNAL(stopEvent()),
          this, SLOT(receiveStopEvent()));
}

void Recorder::receiveRawEvent(const RawEvent &e)
{
  if (file_.isWritable()) {
    auto ba = e.toByteArray();
    file_.write(ba.data(), RawEvent::byteArraySize);
  }
}

/**
 * @brief Stop receiving all event at the end of the recording.
 */
void Recorder::receiveStopEvent()
{
  disconnect(eventFeeder_, SIGNAL(restartEvent()),
          this, SLOT(receiveStartEvent()));
  disconnect(eventFeeder_, SIGNAL(newRawEvent(RawEvent)),
          this, SLOT(receiveRawEvent(RawEvent)));
  disconnect(eventFeeder_, SIGNAL(stopEvent()),
          this, SLOT(receiveStopEvent()));
  closeFile();
}
