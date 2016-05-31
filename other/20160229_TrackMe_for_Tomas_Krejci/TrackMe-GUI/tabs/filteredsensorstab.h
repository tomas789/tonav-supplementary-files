#ifndef FILTEREDSENSORSTAB_H
#define FILTEREDSENSORSTAB_H

#include "rawsensorstab.h"

/**
 * @brief The FilteredSensorsTab class provides a tab very similar to the raw
 *        sensors tab, but with various filters applied.
 */
class FilteredSensorsTab : public RawSensorsTab
{
  Q_OBJECT
public:
  explicit FilteredSensorsTab(QWidget *parent = 0);

public slots:
  void exportPlots(const QString &path);
};

#endif // FILTEREDSENSORSTAB_H
