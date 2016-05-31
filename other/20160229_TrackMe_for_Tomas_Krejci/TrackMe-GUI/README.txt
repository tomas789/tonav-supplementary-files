The TrackMe-GUI is a front-end for the TrackMe library. It binds to the
callbacks provided by the library and feeds it with raw sensor events. The
library returns the output using these callbacks. The GUI is written in the
C++ programming language and utilizes the Qt and Qwt libraries.

The application is able to receive sensor events from a TCP client or a file.
It is compatible with the TrackMe Sensor application for the Android
smartphones.

It is also capable of recording the communication with the client to a file
for further use and exporting all the visible plots to SVG files.

The binary file should be distributed with a sample configuration
file trackme_config.hpp. This file can be also found in the source code
folder.

--- DEPENDENCIES ---
Qt >= 4.8
Qwt >= 6.0

The TrackMe library and the TrackMe-GUI source code folders must have the same
parent folder.
For instance:
/src/TrackMe-GUI
/src/TrackMe

--- BUILD (Windows and Linux) ---
$ qmake
$ make

--- RUN ---
$ ./bin/TrackMe-GUI # Linux
> bin/TrackMe-GUI.exe # Windows
