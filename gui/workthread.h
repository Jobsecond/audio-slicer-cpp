#ifndef AUDIO_SLICER_WORKTHREAD_H
#define AUDIO_SLICER_WORKTHREAD_H

#include <QObject>
#include <QThread>
#include <QRunnable>
#include <QString>
#include <QStringList>

class WorkThread : public QObject, public QRunnable {
Q_OBJECT
public:
    WorkThread(QString filename,
               QString out_path,
               double threshold,
               uint64_t min_length,
               uint64_t min_interval,
               uint64_t hop_size,
               uint64_t max_sil_kept);
    void run() override;

private:
    QString m_filename;
    QString m_out_path;
    double m_threshold;
    uint64_t m_min_length;
    uint64_t m_min_interval;
    uint64_t m_hop_size;
    uint64_t m_max_sil_kept;

signals:
    void oneFinished();
    void oneError(const QString &errmsg);
};


#endif //AUDIO_SLICER_WORKTHREAD_H
