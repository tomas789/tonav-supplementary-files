#include "config.h"

Configuration<> CONFIG;
int PLOT_REFRESH_INTERVAL;
qreal PLOT_INTERVAL;
qreal SVG_EXPORT_WIDTH;
qreal SVG_EXPORT_HEIGHT;
int TCP_PORT;

// Initialize the configuration constants.
void config()
{
  qRegisterMetaType<PlotEvent>("PlotEvent");
  qRegisterMetaType<OrientationEvent>("OrientationEvent");
  qRegisterMetaType<RawEvent>("RawEvent");

  // Set configuration variables and default values
  CONFIG.loadFile("trackme_config.txt");

  // Port 50000 by default
  TCP_PORT = CONFIG.ul("TCP_PORT", 50000);
  // 100 ms default
  PLOT_REFRESH_INTERVAL = CONFIG.ul("PLOT_REFRESH_INTERVAL", 100);
  // 5000 ms default
  PLOT_INTERVAL = CONFIG.d("PLOT_INTERVAL", 5000);
  PLOT_INTERVAL *= 0.001 / TrackMe::timestampToSeconds<PlotEvent>(1);
  // 300 mm default
  SVG_EXPORT_WIDTH = CONFIG.d("SVG_EXPORT_WIDTH", 300);
  // 200 mm default
  SVG_EXPORT_HEIGHT = CONFIG.d("SVG_EXPORT_HEIGHT", 200);
}
