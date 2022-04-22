#include "WayItem.h"
#include "NodeItem.h"
#include "LaneletItem.h"
#include "ColorDefinition.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QDebug>
#include <utility>

WayItem::WayItem(QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
{
    setFlag(ItemIsMovable, true);
    setFlag(ItemIsSelectable, true);
    setFlag(ItemSendsGeometryChanges, true);
    setZValue(6);
}
QPainterPath WayItem::shape() const
{
    QPainterPath ret;
    ret.addPolygon(m_selection_polygon);
    return ret;
}
QRectF WayItem::boundingRect() const
{
    return m_selection_polygon.boundingRect();
}
void WayItem::UpdateBoundingBoxPolygon()
{
    if (m_nodes.size() < 2) return;
    double selectionOffset = 2;
    QPolygonF leftLine;
    QPolygonF rightLine;
    for (int i = 0; i < m_nodes.size() - 1; i++) {
        QLineF line(m_nodes.at(i)->pos(), m_nodes.at(i + 1)->pos());
        qreal radAngle = line.angle() * M_PI / 180;
        qreal dx = selectionOffset * sin(radAngle);
        qreal dy = selectionOffset * cos(radAngle);
        QPointF offset1 = QPointF(dx, dy);
        QPointF offset2 = QPointF(-dx, -dy);
        leftLine << line.p1() + offset1
                 << line.p2() + offset1;
        rightLine << line.p1() + offset2
                  << line.p2() + offset2;
    }
    QPolygonF nPolygon;
    nPolygon << leftLine;
    for (auto elementPtr = rightLine.rbegin(); elementPtr != rightLine.rend(); elementPtr++) {
        nPolygon << (*elementPtr);
    }

    m_selection_polygon = nPolygon;
    update();
}
void WayItem::addNode(NodeItem *item)
{
    if (m_nodes.contains(item)) return;

    QPainterPath path = this->path();
    // Prevent line from 0, 0
    if (this->m_nodes.isEmpty()) {
        path.moveTo(item->pos());
    }
    path.lineTo(item->pos());
    setPath(path);
    this->m_nodes.push_back(item);
    item->addParent(this);
    UpdateBoundingBoxPolygon();
    updateLanelets();
}
void WayItem::removeNode(NodeItem *item)
{
    if (!m_nodes.contains(item)) return;
    if (m_nodes.empty()) return;

    // Copy path and skip item that is to be removed
    QPainterPath newPath;
    QPainterPath path = this->path();

    newPath.moveTo(path.elementAt(0));

    int counter = -1;
    int index = -1;
    for (auto element : this->m_nodes) {
        counter++;
        if (item == element) {
            index = counter;
            continue;
        }
        newPath.lineTo(element->pos());
    }
    this->setPath(newPath);

    // Remove item
    this->m_nodes.removeAt(index);
    UpdateBoundingBoxPolygon();
    updateLanelets();
}

void WayItem::nodeUpdated(NodeItem *item)
{
    if (!m_nodes.contains(item)) return;

    QPainterPath path = this->path();
    // Prevent line from 0, 0
    int counter = -1;
    for (auto element : this->m_nodes) {
        counter++;
        if (item != element) continue;
        path.setElementPositionAt(counter, item->x(), item->y());
        break;
    }
    this->setPath(path);
    UpdateBoundingBoxPolygon();
    updateLanelets();
}
QVariant WayItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        auto shift = value.toPointF() - this->m_system_position;
        for (auto child : this->m_nodes) {
            child->parentPositionShifted(shift);
        }
        this->m_system_position = value.toPointF();
        return QPointF(0, 0);
    }
    return QGraphicsItem::itemChange(change, value);
}
void WayItem::updateLanelets()
{
    for (auto parent : m_lanelets) {
        parent->updateElement();
    }
}
void WayItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Update main pen and brush
    updatePenAndBrush();
    painter->setPen(this->pen());
    painter->setBrush(this->brush());

    // Draw line
    painter->drawPath(this->path());

    drawArrows(painter);

    if (this->isSelected()) drawSelectionIndicator(painter, option);
}
void WayItem::drawArrows(QPainter *painter) const
{// Draw arrows
    for (auto i = 1; i < m_nodes.size(); i++) {
        auto last = m_nodes.at(i);
        auto preLast = m_nodes.at(i - 1);

        QLineF line;
        line.setLine(preLast->x(), preLast->y(), last->x(), last->y());
        line.setLength(0.5 * line.length());

        double angle = atan2(line.dy(), -line.dx());
        double arrowSize = 3;
        QPointF arrowP1 = line.p2() + QPointF(sin(angle + M_PI * 2.0 / 3.0) * arrowSize,
                                              cos(angle + M_PI * 2.0 / 3.0) * arrowSize);
        QPointF arrowP2 = line.p2() + QPointF(sin(angle + M_PI - M_PI * 2.0 / 3.0) * arrowSize,
                                              cos(angle + M_PI - M_PI * 2.0 / 3.0) * arrowSize);

        QPolygonF arrowHead;
        arrowHead << line.p2() << arrowP1 << line.p2() << arrowP2 << line.p2();

        QPainterPath path;
        path.moveTo(line.p2());
        path.addPolygon(arrowHead);
        painter->drawPath(path);
    }
}
void WayItem::drawSelectionIndicator(QPainter *painter, const QStyleOptionGraphicsItem *option) const
{
    const qreal penWidth = 0; // cosmetic pen

    const QColor foregroundColor = option->palette.windowText().color();
    const QColor backgroundColor( // ensure good contrast against foregroundColor
        foregroundColor.red() > 127 ? 0 : 255,
        foregroundColor.green() > 127 ? 0 : 255,
        foregroundColor.blue() > 127 ? 0 : 255);

    painter->setPen(QPen(backgroundColor, penWidth, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawPolygon(m_selection_polygon);

    painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawPolygon(m_selection_polygon);
}
void WayItem::updatePenAndBrush()
{
    QPen pen;
    if (isSelected()) {
        pen = QPen(ColorDefinitions::GREEN, 1);
    }
    else {
        if (m_type == "solid") {
            pen = QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        }
        else if (m_type == "dashed") {
            pen = QPen(Qt::white, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
            pen.setDashPattern({10, 10});
        }
        else if (m_type == "virtual") {
            pen = QPen(ColorDefinitions::LIGHT_BLUE, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
            pen.setDashPattern({2, 2});
        }
        else if (m_type == "unknown") {
            pen = QPen(ColorDefinitions::RED, 1);
        }
    }
    setPen(pen);
}
const QList<NodeItem *> &WayItem::nodes() const
{
    return m_nodes;
}
QString WayItem::wayType() const
{
    return m_type;
}
void WayItem::setWayType(QString type)
{
    this->m_type = std::move(type);
    this->update();
}

const QList<LaneletItem *> &WayItem::lanelets() const
{
    return m_lanelets;
}
void WayItem::addToLanelet(LaneletItem *parent)
{
    this->m_lanelets.push_back(parent);
}
void WayItem::removeFromLanelet(LaneletItem *parent)
{
    int index = -1;
    int counter = -1;
    for (auto element : this->m_lanelets) {
        counter++;
        if (element != parent) continue;
        index = counter;
    }
    this->m_lanelets.removeAt(index);
}
void WayItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Reset before the next shift
    this->m_system_position = {0, 0};
    QGraphicsItem::mouseReleaseEvent(event);
}
