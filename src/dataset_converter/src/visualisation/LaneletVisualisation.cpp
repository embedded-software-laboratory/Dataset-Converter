#include "LaneletVisualisation.h"
#include <QDebug>

LaneletVisualisation::LaneletVisualisation(QGraphicsView *canvas, QObject *parent)
    : QObject(parent), m_canvas(canvas), m_scene(canvas->scene())
{}
QGraphicsView *LaneletVisualisation::canvas() const
{
    return m_canvas;
}

void LaneletVisualisation::visualizeNode(NodeItem *node)
{
    this->m_scene->addItem(node);
    this->m_nodes.push_back(node);
    qDebug() << "[LaneletVisualisation] Node visualized!";
}
void LaneletVisualisation::visualizeWay(WayItem *way)
{
    this->m_scene->addItem(way);
    this->m_ways.push_back(way);
    way->setSelected(true);
    qDebug() << "[LaneletVisualisation] Way visualized!";
}
void LaneletVisualisation::visualizeLanelet(LaneletItem *lanelet)
{
    this->m_scene->addItem(lanelet);
    this->m_lanelets.push_back(lanelet);
    lanelet->updateElement();
    lanelet->setSelected(true);
    qDebug() << "[LaneletVisualisation] Lanelet visualized!";
}
void LaneletVisualisation::selectItem(QGraphicsItem *item)
{
    item->setSelected(true);
    qDebug() << "[LaneletVisualisation] Item selected!";
}
void LaneletVisualisation::addNodeToWay(WayItem *way, NodeItem *node)
{
    way->addNode(node);
    qDebug() << "[LaneletVisualisation] Node added to Way!";
}
void LaneletVisualisation::addWayToLanelet(LaneletItem *lanelet, WayItem *item)
{
    lanelet->setRightWayItem(item);
    qDebug() << "[LaneletVisualisation] Way added to lanelet!";
}

void LaneletVisualisation::removeNode(NodeItem *node)
{
    this->m_scene->removeItem(node);
    this->m_nodes.removeOne(node);
    for (auto parent : node->ways()) {
        parent->removeNode(node);
        emit nodeRemovedFromWay(parent, node);
        qDebug() << "[LaneletVisualisation] Node removed from way!";
    }
    emit nodeRemovedFromVisualisation(node);
    qDebug() << "[LaneletVisualisation] Node removed from visualization!";
}
void LaneletVisualisation::removeWay(WayItem *way)
{
    this->m_scene->removeItem(way);
    this->m_ways.removeOne(way);
    for (auto child : way->nodes()) {
        child->removeParent(way);
        qDebug() << "[LaneletVisualisation] Way removed from node!";
    }
    emit wayRemovedFromVisualisation(way);
    qDebug() << "[LaneletVisualisation] Way removed from visualization!";
}
void LaneletVisualisation::removeLanelet(LaneletItem *lanelet)
{
    this->m_scene->removeItem(lanelet);
    this->m_lanelets.removeOne(lanelet);
    auto rightWay = lanelet->rightWayItem();
    auto leftWay = lanelet->leftWayItem();
    if (rightWay) rightWay->removeFromLanelet(lanelet);
    if (leftWay) leftWay->removeFromLanelet(lanelet);
    qDebug() << "[LaneletVisualisation] Ways removed from lanelet!";

    emit laneletRemovedFromVisualisation(lanelet);
    qDebug() << "[LaneletVisualisation] Lanelet removed from visualization!";
}

void LaneletVisualisation::clearVisualisation()
{
    for (auto node : this->m_nodes) {
        this->m_scene->removeItem(node);
    }
    for (auto way : this->m_ways) {
        this->m_scene->removeItem(way);
    }
    qDebug() << "[LaneletVisualisation] Visualization cleared!";
}
void LaneletVisualisation::splineSelection(qreal splineFactor)
{
    auto itemsSelected = this->m_scene->selectedItems();

    // Get nodes under cursor
    WayItem *selectedWay = nullptr;
    for (auto item : itemsSelected) {
        auto *wayItem = dynamic_cast<WayItem *>(item);
        if (!wayItem) continue;
        selectedWay = wayItem;
        break;
    }
    QList<NodeItem *> nodeItemsUnderCursor;
    for (auto item : itemsSelected) {
        auto *nodeItem = dynamic_cast<NodeItem *>(item);
        if (!nodeItem) continue;
        nodeItemsUnderCursor.push_back(nodeItem);
    }

    if (!selectedWay) return;
    QList<NodeItem *> nodesSelected = selectedWay->nodes();

    if (nodesSelected.size() < 3) return;
    QList<QPointF> positions;
    positions.push_back(nodesSelected.first()->pos());
    for (int i = 1; i < nodesSelected.size() - 1; i++) {
        auto current = nodesSelected.at(i);
        if (nodeItemsUnderCursor.contains(current)) {
            positions.push_back(current->pos());
            continue;
        }
        auto nextPos = nodesSelected.at(i + 1)->pos();
        auto previousPos = nodesSelected.at(i - 1)->pos();
        auto splinePosition = 0.5 * (nextPos - previousPos) + previousPos;
        QPointF newPosition;

        qreal currentFactor = 1.0 - splineFactor;
        newPosition = (splineFactor * splinePosition + currentFactor * current->pos());

        positions.push_back(newPosition);
    }
    positions.push_back(nodesSelected.last()->pos());

    for (int i = 1; i < nodesSelected.size() - 1; i++) {
        auto current = nodesSelected.at(i);
        auto newPosition = positions.at(i);
        current->setPos(newPosition);
        current->itemChange(QGraphicsItem::ItemPositionChange, newPosition);
    }

}

