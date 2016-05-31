#ifndef UTILITIES_H
#define UTILITIES_H

#include <TrackMe.hpp>

#include <algorithm>

template <typename Container, typename TimestampT>
void erase_old_events(Container &c, TimestampT timestamp)
{
  typedef typename Container::value_type EventType;
  EventType time;
  time.timestamp = timestamp;
  c.erase(c.begin(),
          std::lower_bound(c.begin(),
                           c.end(),
                           time,
                           TrackMe::compareTimestamps<EventType>));
}

#endif // UTILITIES_H
