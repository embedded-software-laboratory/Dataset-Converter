#ifndef LANELETHANDLER_H
#define LANELETHANDLER_H

#include <memory>

#include <QObject>

#include <lanelet2_core/LaneletMap.h>

#include "visualisation/graphics_items/NodeItem.h"
#include "visualisation/graphics_items/WayItem.h"
#include "visualisation/graphics_items/LaneletItem.h"

/**
 * Worker class that loads or stores lanelet 2 maps. All updates in the interface are always mirrored here.
 */
class LaneletHandler: public QObject
{
Q_OBJECT
private:
    QString lanelet_file_name_; ///< Lanelet map that should be loaded or stored
    lanelet::LaneletMapPtr m_map = nullptr; ///< Lanelet map instance that is ever stored to or loaded from

    QList<NodeItem *> nodes_; ///< All nodes that appeared or should appear in the map
    QList<WayItem *> ways_; ///< All ways that appeared or should appear in the map
    QList<LaneletItem *> lanelets_; ///< All lanelets that appeared or should appear in the map

public:
    /**
     * Creates the handler..
     * @param parent Possible parent or qt pointer destruction.
     */
    explicit LaneletHandler(QObject *parent = nullptr);

public slots:
    /**
     * Starts the parsing of a lanelet map.
     * @param laneletFileName File to parse from.
     * @param scaleFactor Scale factor matching meter to pixels
     */
    void parseLanelet(QString laneletFileName, qreal scaleFactor);

    /**
     * Starts the storing of a lanelet map.
     * @param laneletFileName File to wrote to.
     * @param scaleFactor Scale factor matching meter to pixels
     */
    void writeLanelet(QString laneletFileName, qreal scaleFactor);

    /**
     * Starts the export process to store the current lanelet map into a svg file.
     * @param svgFile SVG file path to store into.
     * @param scene Scene to grab the data from.
     */
    void writeLaneletImage(const QString &svgFile, QGraphicsScene *scene);

    /**
     * Add a node for later storing.
     * @param position Position to add the node.
     */
    void addNode(QPointF position);

    /**
     * Remove a node from buffer.
     * @param node Node to remove.
     */
    void removeNode(NodeItem *node);

    /**
     * Add a way for later storing.
     * @param node Node to start way with.
     */
    void addWayWithNode(NodeItem *node);

    /**
     * Remove a way from buffer.
     * @param way Way to remove.
     */
    void removeWay(WayItem *way);

    /**
     * Add a lanelet for later storing.
     * @param way Way to start lanelet with.
     */
    void addLaneletWithPath(WayItem *way);

    /**
     * Remove a lanelet from buffer.
     * @param lanelet Lanelet to remove.
     */
    void removeLanelet(LaneletItem *lanelet);

signals:
    /**
     * Indicator for the current progress. Send periodically from the worker to the main thread.
     * @param currentTask Current progress.
     * @param numberOfTasks Number of tasks ahead.
     */
    void progress(qint16 currentTask, qint16 numberOfTasks);

    /**
     * Indicates error. Aborts process.
     * @param message Error message.
     */
    void error(QString message);
    /**
     * Loading is finished and no errors occurred.
     */
    void loaded();
    /**
     * Storing is finished and no errors occurred.
     */
    void stored();

    /**
     * Node added.
     * @param node New node.
     */
    void nodeAdded(NodeItem *node);
    /**
     * Way added.
     * @param way New way.
     */
    void wayAdded(WayItem *way);

    /**
     * Lanelet added.
     * @param lanelet New lanelet.
     */
    void laneletAdded(LaneletItem *lanelet);
};

#endif // LANELETHANDLER_H
