//
// Created by uniautonom on 09.03.20.
//

#include "NavigationMarkerItem.h"

NavigationMarkerItem::NavigationMarkerItem(std::shared_ptr<NavigationMarker> navMarker, QGraphicsItem *parent) :
        QGraphicsEllipseItem(parent), navMarker(navMarker) {

    int x = navMarker->getLocation()->x;
    int y = navMarker->getLocation()->y;

    QBrush brush;
    QColor color;
    switch (navMarker->getNavType()) {
        case eNavType::Intermediate:
            color = QColor(qRgb(140, 215, 255));
            break;
        case eNavType::Final:
            color = QColor(qRgb(0, 42, 255));
            break;
    }

    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    setBrush(brush);

    QPen pen = QPen(QColor(Qt::GlobalColor::black));
    pen.setWidth(10);
    setPen(pen);

    int circle_segment = 60;
    setStartAngle((90-circle_segment/2)*16);
    setSpanAngle(circle_segment*16);

    int posX = x - width / 2;
    int posY = y - height / 2;
    setRect(posX, posY, width, height);

    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void NavigationMarkerItem::setToolTipMarker(){
    QString tip("Navigation marker: ");
    tip.append(std::to_string(navMarker->getId()).c_str());
    tip.append("\n You can drag this marker onto other lanes or parking slots to change its navigation target");
    setToolTip(tip);
}

void NavigationMarkerItem::setENavType(eNavType type) {
    if(type == Intermediate){
        QBrush brush;
        QColor color = QColor(qRgb(140, 215, 255));
        brush.setColor(color);
        brush.setStyle(Qt::SolidPattern);
        this->setBrush(brush);
        navMarker->setNavType(Intermediate);
    } else if(type == Final){
        QBrush brush;
        QColor color = QColor(qRgb(0, 42, 255));
        brush.setColor(color);
        brush.setStyle(Qt::SolidPattern);
        this->setBrush(brush);
        navMarker->setNavType(Final);
    }
}

NavigationMarkerItem::~NavigationMarkerItem() {
}

std::shared_ptr<NavigationMarker> NavigationMarkerItem::getNavMarker() {
    return navMarker;
}

void NavigationMarkerItem::highlight() {
    QPen pen = QPen(QColor(Qt::GlobalColor::red));
    pen.setWidth(40);
    setPen(pen);
}

void NavigationMarkerItem::unHighlight() {
    QPen pen = QPen(QColor(Qt::GlobalColor::black));
    pen.setWidth(10);
    setPen(pen);
}

void NavigationMarkerItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(event->button() == Qt::LeftButton){
            startPos = event->pos();
            x = this->pos().x();
            y = this->pos().y();
          }
    QGraphicsItem::mousePressEvent(event);
}

void NavigationMarkerItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton){ //event->button() == Qt::LeftButton
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= 10){
            executeDrag = true;
        }
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void NavigationMarkerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    changed = false;
    if(executeDrag){
        QList<QGraphicsItem *> list = scene()->items(event->scenePos());

        for(int i = 0; i < list.size(); i++){
            LaneItem * laneItem = dynamic_cast<LaneItem *>(list.at(i));
            if(laneItem != nullptr){
                changed = true;
                getNavMarker()->setPassableElement(laneItem->lane);
                getNavMarker()->setLocation(event->scenePos().x(), event->scenePos().y());
                break;
            }
            SpecialMarkingItem * markingItem = dynamic_cast<SpecialMarkingItem *>(list.at(i));
            if(markingItem != nullptr && markingItem->getM()->isParkingType()){
                changed = true;
                getNavMarker()->setPassableElement(markingItem->getM());
                getNavMarker()->setLocation(event->scenePos().x(), event->scenePos().y());
                break;
            }
        }
        ItemSignalController::getInstance().sendMarkerMoved(this);
        QGraphicsItem::mouseReleaseEvent(event);

        executeDrag = false;
    }
}

bool NavigationMarkerItem::isChanged() const {
    return changed;
}

int NavigationMarkerItem::getX() const {
    return x;
}

int NavigationMarkerItem::getY() const {
    return y;
}
