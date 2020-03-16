#ifndef WARNDATAHANDLER_H
#define WARNDATAHANDLER_H

#include <QObject>
#include <QMap>

class WarnDataHandler : public QObject
{
    Q_OBJECT
public:
    explicit WarnDataHandler(QObject *parent = 0);
    void initPropertyList(void);
    QStringList getPropertyList(void){return mPropertys.keys();}

signals:
    void sigPropertySelected(QByteArray data);

public slots:
    void slotPropertySelected(QString name);
    void slotPropertyModified(QByteArray data);

private:
    QMap<QString,int> mPropertys;
};

#endif // WARNDATAHANDLER_H
