#include "WayItem.h"
#include "NodeItem.h"
#include "LaneletItem.h"
#include "ColorDefinition.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <utility>

LaneletItem::LaneletItem(QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    setFlag(ItemIsSelectable, true);
    setZValue(5);
}
void LaneletItem::updateElement()
{
    // Reject update if lanelet is invalid
    if (!left_way_item_) return;
    if (!right_way_item_) return;
    if (right_way_item_->nodes().size() < 2) return;
    if (left_way_item_->nodes().size() < 2) return;

    QLineF first(left_way_item_->nodes().front()->pos(), right_way_item_->nodes().front()->pos());
    QLineF second(left_way_item_->nodes().back()->pos(), right_way_item_->nodes().back()->pos());
    QPointF intersectionPoint;
    bool rightInverted = first.intersects(second, &intersectionPoint) != QLineF::BoundedIntersection;

    // Transfer both ways into polygons
    QPolygonF polygon;
    for (auto node: left_way_item_->nodes()) {
        polygon << node->pos();
    }
    if (rightInverted) {
        for (auto elementPtr = right_way_item_->nodes().rbegin(); elementPtr != right_way_item_->nodes().rend();
             elementPtr++) {
            polygon << (*elementPtr)->pos();
        }
    }
    else {
        for (auto node: right_way_item_->nodes()) {
            polygon << node->pos();
        }
    }
    this->setPolygon(polygon);
}
//QVariant LaneletItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
//{
//    if (change == ItemPositionChange && scene()) {
//        if (left_way_item_) this->left_way_item_->ParentPositionUpdated(value.toPointF());
//        if (right_way_item_) this->right_way_item_->ParentPositionUpdated(value.toPointF());
//        return QPointF(0, 0);
//    }
//    return QGraphicsItem::itemChange(change, value);
//}
void LaneletItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    updatePenAndBrush();
    painter->setPen(this->pen());
    painter->setBrush(this->brush());

    // Draw polygon
    painter->drawPolygon(this->polygon());
    drawArrows(painter);
    if (this->isSelected()) drawSelectionIndicator(painter, option);
}
void LaneletItem::updatePenAndBrush()
{
    QPen pen;
    QBrush brush;
    if (isSelected()) {
        QColor greenTransparent = ColorDefinitions::GREEN;
        greenTransparent.setAlpha(50);
        brush = QBrush(greenTransparent);
        pen = QPen(ColorDefinitions::GREEN, 0.5);
    }
    else {
        if (type_ == "entry_point"
            || type_ == "exit_entry_point"
            || type_ == "exit_point") {
            pen = QPen(ColorDefinitions::ORANGE, 0.5);
            QColor orangeTransparent = ColorDefinitions::ORANGE;
            orangeTransparent.setAlpha(50);
            brush = QBrush(orangeTransparent);
        }
        else if (type_ == "scenario") {
            pen = QPen(ColorDefinitions::BLUE, 0.5);
            QColor blueTransparent = ColorDefinitions::BLUE;
            blueTransparent.setAlpha(50);
            brush = QBrush(blueTransparent);
        }
        else if (type_ == "system") {
            pen = QPen(ColorDefinitions::VIOLET, 0.5);
            QColor greenTransparent = ColorDefinitions::VIOLET;
            greenTransparent.setAlpha(50);
            brush = QBrush(greenTransparent);
        }
        else if (type_ == "unknown") {
            pen = QPen(ColorDefinitions::RED, 0.5);
            QColor redTransparent = ColorDefinitions::RED;
            redTransparent.setAlpha(50);
            brush = QBrush(redTransparent);
        }
    }
    setPen(pen);
    setBrush(brush);
}

void LaneletItem::drawSelectionIndicator(QPainter *painter, const QStyleOptionGraphicsItem *option) const
{
    const qreal penWidth = 0; // cosmetic pen

    const QColor foregroundColor = option->palette.windowText().color();
    const QColor backgroundColor( // ensure good contrast against foregroundColor
        foregroundColor.red() > 127 ? 0 : 255,
        foregroundColor.green() > 127 ? 0 : 255,
        foregroundColor.blue() > 127 ? 0 : 255);

    painter->setPen(QPen(backgroundColor, penWidth, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawPolygon(polygon());

    painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawPolygon(polygon());
}

void LaneletItem::drawArrows(QPainter *painter) const
{
    if (!this->left_way_item_) return;
    if (!this->right_way_item_) return;
    if (right_way_item_->nodes().size() < 2) return;
    if (left_way_item_->nodes().size() < 2) return;
    QLineF first(left_way_item_->nodes().front()->pos(), right_way_item_->nodes().front()->pos());
    QLineF second(left_way_item_->nodes().back()->pos(), right_way_item_->nodes().back()->pos());
    QPointF intersectionPoint;
    bool rightInverted = first.intersects(second, &intersectionPoint) != QLineF::BoundedIntersection;

    QLineF
        line(rightInverted ? this->left_way_item_->nodes().last()->pos() : this->left_way_item_->nodes().first()
        ->pos(), this->right_way_item_->nodes().last()->pos());

    QPointF point = line.p1() + 0.5 * (line.p2() - line.p1());
    double angle = atan2(line.dy(), -line.dx()) + M_PI_2;
    double arrowSize = 10;
    QPointF arrowP3 = point + QPointF(sin(angle + M_PI_2) * arrowSize * 2, cos(angle + M_PI_2) * arrowSize * 2);

    QPointF arrowP1 =
        point + QPointF(sin(angle + M_PI * 2.0 / 3.0) * arrowSize, cos(angle + M_PI * 2.0 / 3.0) * arrowSize);
    QPointF arrowP2 = point
        + QPointF(sin(angle + M_PI - M_PI * 2.0 / 3.0) * arrowSize, cos(angle + M_PI - M_PI * 2.0 / 3.0) * arrowSize);

    QPolygonF arrowHead;
    arrowHead << point << arrowP1 << point << arrowP2 << point << arrowP3;

    QPainterPath path;
    QPen pen = this->pen();
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    painter->setPen(pen);
    path.moveTo(point);
    path.addPolygon(arrowHead);
    painter->drawPath(path);

}

QString LaneletItem::laneletType() const
{
    return type_;
}
void LaneletItem::setLaneletType(QString type)
{
    this->type_ = std::move(type);
    this->update();
}
WayItem *LaneletItem::leftWayItem() const
{
    return left_way_item_;
}
void LaneletItem::setLeftWayItem(WayItem *leftWayItem)
{
    if (leftWayItem == right_way_item_) return;
    left_way_item_ = leftWayItem;
    this->updateElement();
    if (leftWayItem) {
        leftWayItem->addToLanelet(this);
    }
}
WayItem *LaneletItem::rightWayItem() const
{
    return right_way_item_;
}
void LaneletItem::setRightWayItem(WayItem *rightWayItem)
{
    if (rightWayItem == left_way_item_) return;
    right_way_item_ = rightWayItem;
    this->updateElement();
    if (rightWayItem) {
        rightWayItem->addToLanelet(this);
    }
}
void LaneletItem::flipDirection()
{
    if (!this->right_way_item_) return;
    if (!this->left_way_item_) return;
    auto store = this->left_way_item_;
    this->left_way_item_ = this->right_way_item_;
    this->right_way_item_ = store;
    this->updateElement();
}
