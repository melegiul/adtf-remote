#ifndef UNIAUTONOM_REMOTEGUI_QTOOLTIPPER_H
#define UNIAUTONOM_REMOTEGUI_QTOOLTIPPER_H
#include <QObject>
#include <QAbstractItemView>
#include <QHelpEvent>
#include <QToolTip>

class QToolTipper : public QObject
{
Q_OBJECT
public:
    explicit QToolTipper(QObject* parent = NULL) : QObject(parent) {}

protected:
    bool eventFilter(QObject* obj, QEvent* event);
};
#endif //UNIAUTONOM_REMOTEGUI_QTOOLTIPPER_H
