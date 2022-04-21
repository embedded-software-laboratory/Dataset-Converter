#ifndef LANELETITEM_H
#define LANELETITEM_H

#include <QGraphicsItem>

#include "WayItem.h"

class WayItem;

/**
 * Graphical representation of a lanelets from two ways.
 */
class LaneletItem: public QGraphicsPolygonItem
{
private:
    WayItem *left_way_item_ = nullptr; ///< Left way visual item that is a child of this lanelet
    WayItem *right_way_item_ = nullptr; ///< Left way visual item that is a child of this lanelet
    QString type_ = "unknown"; ///< Internal lanelet type of this lanelet

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
     * Updates pen and brush of the polygon depending on the type and the selection status.
     */
    void updatePenAndBrush();

public:
    /**
     * Creates a lanelet item, make it selectable and moves it in the correct z index.
     * @param parent Possible parent item.
     */
    explicit LaneletItem(QGraphicsItem *parent = nullptr);

    /**
     * Will render this object.
     * @param painter QPainter to render with.
     * @param option Options to follow.
     * @param widget QWidget to paint on.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

//    /**
//     * Will update child elements if this element is moved.
//     * @param change Type of change.
//     * @param value Value of change.
//     * @return QGraphicsItem response after filtering desired change.
//     */
//    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    /**
     * Getter for the lanelet type.
     * @return Lanelet type.
     */
    [[nodiscard]] QString laneletType() const;

    /**
     * Getter for the left way item.
     * @return Left way item.
     */
    [[nodiscard]] WayItem *leftWayItem() const;

    /**
     * Getter for the right way item.
     * @return Right way item.
     */
    [[nodiscard]] WayItem *rightWayItem() const;

public slots:
    /**
     * Setter for the lanelet type.
     * @param type New lanelet type.
     */
    void setLaneletType(QString type);

    /**
     * Setter for the left way item.
     * @param leftWayItem New left way item.
     */
    void setLeftWayItem(WayItem *leftWayItem);

    /**
     * Setter for the right way item.
     * @param rightWayItem New right way item.
     */
    void setRightWayItem(WayItem *rightWayItem);

    /**
     * Updates element after changed to it of its child. Requires both ways and a lanelet type to be set.
     */
    void updateElement();

    /**
     * Flips the direction by switching right and left way.
     */
    void flipDirection();
};

#endif //LANELETITEM_H
