#include "NodeItem.h"
#include "WayItem.h"
#include "ColorDefinition.h"

#include <QApplication>
#include <QPainter>
#include <QPalette>

NodeItem::NodeItem(QGraphicsItem *parent) : QGraphicsItem(parent) {
  setFlag(ItemIsMovable, true);
  setFlag(ItemIsSelectable, true);
  setFlag(ItemSendsGeometryChanges, true);
  setZValue(7);
}

QRectF NodeItem::boundingRect() const {
  if (!this->isSelected()) {
    return {-2, -2, 4, 4};
  } else {
    return {-3, -3, 6, 6};
  }
}
void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  QRectF rect = boundingRect();

  QPen pen;
  QBrush brush;
  if (this->isSelected()) {
    pen = QPen(Qt::transparent, 0);
    brush = QBrush(ColorDefinitions::GREEN);

  } else {
    pen = QPen(Qt::transparent, 0);
    brush = QBrush(QApplication::palette().color(QPalette::Highlight));
  }
  painter->setPen(pen);
  painter->setBrush(brush);
  painter->drawEllipse(rect);

  auto font = QApplication::font();
  font.setPointSizeF(2);
  QString text = QString("%1, %2").arg(this->pos().x()).arg(this->pos().y());
  QFontMetrics fm(font);
  int width = fm.horizontalAdvance(text);
  int height = fm.height();

  painter->setFont(font);
  painter->setPen(Qt::white);

  if (this->isSelected()) {
    painter->drawText(- static_cast<int>(width / 2.0), static_cast<int>(height / 2.0), text);
  }
}
QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value) {

  // Do custom update and afterwards the normal update
  if (change == ItemPositionChange && scene()) {
    for (auto parent : this->m_parents) {
        parent->nodeUpdated(this);
    }
  }

  // Snap to grid.
  // TODO Add this feature to the editor
//  if (change == ItemPositionChange && scene()) {
//    QPointF gridPosition = value.toPointF();
//    int gridSize = 10;
//    gridPosition.setX(round(gridPosition.x() / gridSize) * gridSize);
//    gridPosition.setY(round(gridPosition.y() / gridSize) * gridSize);
//    return gridPosition;
//  }

  // Normal update
  return QGraphicsItem::itemChange(change, value);
}

void NodeItem::parentPositionShifted(QPointF shift) {
  this->moveBy(shift.x(), shift.y());
}

const QList<WayItem *> &NodeItem::ways() const {
  return m_parents;
}
//void NodeItem::setParents(const QList<WayItem *> &parents) {
//    m_parents = parents;
//}
void NodeItem::addParent(WayItem *parent) {
  this->m_parents.push_back(parent);
}
void NodeItem::removeParent(WayItem *parent) {
  int index = -1;
  int counter = -1;
  for (auto element : this->m_parents) {
    counter++;
    if (element != parent) continue;
    index = counter;
  }
  this->m_parents.removeAt(index);
}
