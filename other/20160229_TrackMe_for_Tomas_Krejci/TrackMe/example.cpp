#include <iostream>
#include <vector>

#include "TrackMe.hpp"
#include "AndroidEvent.hpp"
using namespace TrackMe;

void printDisplacement(const AndroidEvent &event)
{
  using ArrayMatrix::operator<<;
  std::cout << "Displacement in time "
            << event.timestamp
            << " is [" << event.value << "].\n";
}

int main(/*int argc, char ** argv*/)
{
  // initialization
  Tracker<AndroidEvent> tracker;
  // try to read a configuration file
  tracker.config().loadFile("trackme_config.txt");
  // set a callback for printing the displacement on the standard output
  tracker.setDisplacementCallback(printDisplacement);

  // load events from the standard input
  std::vector<char> buffer(AndroidEvent::byteArraySize);
  while (std::cin.read(buffer.data(), AndroidEvent::byteArraySize)) {
    auto event = AndroidEvent::fromByteArray(buffer);
    tracker.receiveEvent(event);
  }

  return 0;
}
