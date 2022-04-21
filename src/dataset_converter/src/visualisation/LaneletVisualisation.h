#ifndef LANELETVISUALISATION_H
#define LANELETVISUALISATION_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsView>

#include "visualisation/graphics_items/NodeItem.h"
#include "visualisation/graphics_items/WayItem.h"
#include "visualisation/graphics_items/LaneletItem.h"

/**
 * Handler for all lanelet related visualisation tasks.
 */
class LaneletVisualisation: public QObject
{
Q_OBJECT
private:
    // Widgets
    QGraphicsView * const m_canvas = nullptr; ///< Canvas to render on
    QGraphicsScene * const m_scene = nullptr; ///< Scene where all object live in

    // Dynamic scene elements all elements in this list are visualised
    QList<NodeItem *> m_nodes; ///< All rendered nodes
    QList<WayItem *> m_ways; ///< All rendered ways
    QList<LaneletItem *> m_lanelets; ///< All rendered lanelets

public:
    /**
     * Initialises the service by setting up scene and canvas.
     * @param canvas Canvas to draw on.
     * @param parent Possible parent or qt pointer destruction.
     */
    explicit LaneletVisualisation(QGraphicsView *canvas, QObject *parent = nullptr);

    /**
     * Getter for the canvas.
     * @return Canvas rendered on.
     */
    [[nodiscard]] QGraphicsView *canvas() const;

public slots:

    /**
     * Interface to request a external created node to the render service.
     * @param node Node to add.
     */
    void visualizeNode(NodeItem *node);
    /**
     * Interface to request a external created way to the render service.
     * @param way Way to add.
     */
    void visualizeWay(WayItem *way);
    /**
     * Interface to request a external created lanelet to the render service.
     * @param lanelet Lanelet to add.
     */
    void visualizeLanelet(LaneletItem *lanelet);

    /**
     * Request the service to link a node to a way.
     * @param way Way to add node to.
     * @param node Node to add to the way.
     */
    void addNodeToWay(WayItem *way, NodeItem *node);
    /**
     * Request the service to link a way to a lanelet.
     * @param way Way to add to the lanelet.
     * @param lanelet Lanelet to add to the way to.
     */
    void addWayToLanelet(LaneletItem *lanelet, WayItem *item);

    /**
     * Request the service to remove the node from the canvas.
     * @param node Node to remove.
     */
    void removeNode(NodeItem *node);

    /**
     * Request the service to remove the way from the canvas.
     * @param way Way to remove.
     */
    void removeWay(WayItem *way);

    /**
     * Request the service to remove the lanelet from the canvas.
     * @param lanelet Lanelet to remove.
     */
    void removeLanelet(LaneletItem *lanelet);

    /**
     * Select an item. Implemented to make sure that item are added before they are selected. The Qt event queue makes
     * sure of that.
     * @param item Item to select.
     */
    void selectItem(QGraphicsItem *item);

    /**
     * Small service function smoothing a selected way.
     * @param splineFactor Factor to smooth.
     */
    void splineSelection(qreal splineFactor);

    /**
     * Removed all lanelet elements from the visualisation.
     */
    void clearVisualisation();

signals:

    /**
     * Signal acknowledging the request to remove a node.
     * @param node Node that was removed.
     */
    void nodeRemovedFromVisualisation(NodeItem *node);
    /**
     * Signal acknowledging the request to remove a way.
     * @param way Way that was removed.
     */
    void wayRemovedFromVisualisation(WayItem *way);
    /**
     * Signal acknowledging the request to remove a lanelet.
     * @param lanelet Lanelet that was removed.
     */
    void laneletRemovedFromVisualisation(LaneletItem *lanelet);
    /**
     * Signal acknowledging the request to remove a node from a way.
     * @param way Way that the node was removed from.
     * @param node Node that was removed.
     */
    void nodeRemovedFromWay(WayItem *way, NodeItem *node);
};

#endif //LANELETVISUALISATION_H
