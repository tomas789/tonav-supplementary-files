#ifndef INFOTAB_H
#define INFOTAB_H

#include "trackmetab.h"

#include <QGridLayout>
#include <QLabel>

/**
 * @brief For each sensor, the tab shows the same information. This information
 *        is wrapped in this widget.
 */
class StatsWidget : public QWidget
{
  Q_OBJECT
public:
  StatsWidget(QWidget *parent = 0) :
    QWidget(parent)
  {
    auto layout = new QGridLayout;

    layout->addWidget(new QLabel("Number of samples:"), 1,1);
    size_ = new QLabel;
    layout->addWidget(size_, 1,2);

    layout->addWidget(new QLabel("Event rate:"), 2,1);
    rate_ = new QLabel;
    layout->addWidget(rate_, 2,2);

    layout->addWidget(new QLabel("Mean:"), 3,1);
    mean_ = new QLabel;
    layout->addWidget(mean_, 3,2);

    layout->addWidget(new QLabel("Mean magnitude:"), 4,1);
    mean_magnitude_ = new QLabel;
    layout->addWidget(mean_magnitude_, 4,2);

    layout->addWidget(new QLabel("Magnitude variance:"), 5,1);
    variance_ = new QLabel;
    layout->addWidget(variance_, 5,2);

    layout->addWidget(new QLabel("Magnitude squares:"), 6,1);
    squares_ = new QLabel;
    layout->addWidget(squares_, 6,2);

    this->setLayout(layout);
  }

  template <typename Stats>
  void assign(const Stats &stats)
  {
    size_->setText(QString::number(stats.size));
    rate_->setText(QString::number(stats.rate));
    mean_->setText(vectorToString(stats.mean));
    mean_magnitude_->setText(QString::number(stats.mean_magnitude));
    variance_->setText(QString::number(stats.variance));
    squares_->setText(QString::number(stats.squares));
  }

private:

  template <typename Vector>
  QString vectorToString(const Vector &v)
  {
    return QString("[")
    + QString::number(v[0]) + ", "
    + QString::number(v[1]) + ", "
    + QString::number(v[2])
    + "]";
  }

  QLabel *size_;
  QLabel *rate_;
  QLabel *mean_;
  QLabel *mean_magnitude_;
  QLabel *variance_;
  QLabel *squares_;
};

/**
 * @brief The InfoTab class provides various statistics about individual
 *        filters.
 */
class InfoTab : public TrackMeTab
{
  Q_OBJECT
public:
    explicit InfoTab(QWidget *parent = 0);

protected:
    StatsWidget *accStats_;
    StatsWidget *gyroStats_;
    StatsWidget *magStats_;
    QLabel *calibrated_;

public slots:
    void replot() override;
};

#endif // INFOTAB_H
