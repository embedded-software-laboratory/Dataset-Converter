#ifndef WAYITEM_H
#define WAYITEM_H

#include <QGraphicsItem>
#include <cpm_scenario/Scenario.h>

class NodeItem;
class LaneletItem;

/**
 * Graphical representation of the ways making up a lanelets left and right border.
 */
class WayItem: public QGraphicsPathItem
{
private:
    QList<NodeItem *> m_nodes; ///< Child nodes
    QList<LaneletItem *> m_lanelets; ///< Parent lanelets
    QString m_type = "unknown"; ///< Internal way type
    QPointF m_system_position; ///< Buffer to calculate relative motion
    QPolygonF m_selection_polygon; ///< Polygon to calculate the selection box of a complex shape


    /**
     * Updates pen and brush of the polygon depending on the type and the selection status.
     */
    void updatePenAndBrush();

    /**
     * Draws the dashed line around the element if it it selected.
     * @param painter QPainter to paint with.
     * @param option Option to respect while painting.
     */
    void drawSelectionIndicator(QPainter *painter, const QStyleOptionGraphicsItem *option) const;

    /**
     * Draws direction arrow that shows the direction of the lanelet.
     * @param painter QPainter to paint with.
     */
    void drawArrows(QPainter *painter) const;

    /**
     * Updates the lanelets if this item is modified or moved.
     */
    void updateLanelets();

    /**
     * Update the selection polygon to fit the way.
     */
    void UpdateBoundingBoxPolygon();


public:
    /**
     * Creates a way item, make it selectable, movable and moves it in the correct z index.
     * @param parent Possible parent item.
     */
    explicit WayItem(QGraphicsItem *parent = nullptr);

    /**
     * Overrides this function to make the selection bounding box show up correctly.
     * @return New shape that is the bounding box.
     */
    [[nodiscard]] QPainterPath shape() const override;

    /**
     * Overrides this function to make the selection bounding box show up correctly.
     * @return New updated bounding box rectangle.
     */
    [[nodiscard]] QRectF boundingRect() const override;

    /**
     * Will render this object.
     * @param painter QPainter to render with.
     * @param option Options to follow.
     * @param widget QWidget to paint on.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    /**
     * Will update child elements if this element is moved.
     * @param change Type of change.
     * @param value Value of change.
     * @return QGraphicsItem response after filtering desired change.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    /**
     * Will reset the stored position for the shift calculation.
     * @param event Information about the mouse movement.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * Getter for the type of way.
     * @return Type of way.
     */
    [[nodiscard]] QString wayType() const;

    /**
     * Getter for node that make up this way.
     * @return Nodes of this way.
     */
    [[nodiscard]] const QList<NodeItem *> &nodes() const;

    /**
     * Getter for the lanelets this is part of.
     * @return Lanelets using this way.
     */
    [[nodiscard]] const QList<LaneletItem *> &lanelets() const;

public slots:
    /**
     * Setter for the type of way.
     * @param type New type
     */
    void setWayType(QString type);

    /**
     * Adds a node to the end of the way.
     * @param item New node.
     */
    void addNode(NodeItem *item);

    /**
     * Removes a node from this way.
     * @param item Node to be removed.
     */
    void removeNode(NodeItem *item);

    /**
     * A node was updates so update this way as well.
     * @param item Node that was updated.
     */
    void nodeUpdated(NodeItem *item);

    /**
     * Add this way to a lanelet.
     * @param parent Lanelet this is to be added to.
     */
    void addToLanelet(LaneletItem *parent);

    /**
     * This way was removed from a lanelet.
     * @param parent Lanelet that this was removed from.
     */
    void removeFromLanelet(LaneletItem *parent);
};

#endif //WAYITEM_H
