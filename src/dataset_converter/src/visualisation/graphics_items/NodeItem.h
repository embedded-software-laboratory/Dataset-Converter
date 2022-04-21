#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsItem>

class WayItem;

/**
 * Graphical representation of the nodes making up a way.
 */
class NodeItem: public QGraphicsItem
{
private:
    QList<WayItem *> m_parents; ///< All ways that this item is a child of
    QPointF m_system_position; ///< Buffer to calculate relative motion

public:
    /**
     * Creates a node item, make it selectable, movable and moves it in the correct z index.
     * @param parent Possible parent item.
     */
    explicit NodeItem(QGraphicsItem *parent = nullptr);

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
     * Will update parent elements if this element is moved.
     * @param change Type of change.
     * @param value Value of change.
     * @return QGraphicsItem response after filtering desired change.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    /**
     * Getter for all parent nodes.
     * @return Parent nodes.
     */
    [[nodiscard]] const QList<WayItem *> &ways() const;

public slots:
    /**
     * Updates the node position of the parent way is moved. This is propagated up to the way.
     * @param shift Amount of movement.
     */
    void parentPositionShifted(QPointF shift);

    /**
     * Adds a new parent to be updated on moves.
     * @param parent New parent.
     */
    void addParent(WayItem *parent);

    /**
     * Removes an parent from the update list.
     * @param parent Parent to be removed.
     */
    void removeParent(WayItem *parent);
};

#endif //NODEITEM_H
