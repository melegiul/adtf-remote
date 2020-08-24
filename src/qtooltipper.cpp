#include "qtooltipper.h"

bool QToolTipper::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QAbstractItemView* view = qobject_cast<QAbstractItemView*>(obj->parent());
        if (!view)
        {
            return false;
        }

        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
        QPoint pos = helpEvent->pos();
        QModelIndex index = view->indexAt(pos);
        if (!index.isValid())
            return false;

        QString itemText = view->model()->data(index).toString();
        QString itemTooltip = view->model()->data(index, Qt::ToolTipRole).toString();

        QFontMetrics fm(view->font());
        int itemTextWidth = fm.width(itemText);
        QRect rect = view->visualRect(index);
        int rectWidth = rect.width();

        if ((itemTextWidth > rectWidth) && !itemTooltip.isEmpty())
        {
            QToolTip::showText(helpEvent->globalPos(), itemTooltip, view, rect);
        }
        else
        {
            QToolTip::hideText();
        }
        return true;
    }
    return false;
}

