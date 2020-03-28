#ifndef WARNDATAHANDLER_H
#define WARNDATAHANDLER_H

#include <QObject>
#include <QMap>

/**
 * @brief for warningDlg
 */
class WarnDataHandler : public QObject
{
    Q_OBJECT
public:
    explicit WarnDataHandler(QObject *parent = 0);

signals:
    void sigPropertySelected(QByteArray data);

public slots:
    void slotPropertySelected(int index);
    void slotPropertyModified(QByteArray data);

};

#endif // WARNDATAHANDLER_H
