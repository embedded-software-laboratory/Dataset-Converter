#ifndef EXTENDEDOBJECTITEM_H
#define EXTENDEDOBJECTITEM_H

#include <QGraphicsItem>
#include <cpm_scenario/ExtendedObject.h>

/**
 * QGraphicsItem that is representing the individual dynamic objects in a scenario.
 */
class ExtendedObjectItem: public QGraphicsRectItem
{
private:
    // Graphical elements
    QLine velocity_arrow_; ///< QGraphics item that indicates the velocity as an arrow.

    // Fixed data
    const cpm_scenario::ExtendedObjectPtr extended_object_; ///< Object to link this visual item to.
    const double scale_factor_; ///< Scaling factor that should be used to transform the linked item to the canvas.

    // Dynamic data
    long current_frame_ = -1; ///< Frame number of the linked item that should be rendered.

    /**
     * Main update function that will update all graphical elements.
     */
    void UpdateObject();

    /**
     * Updates pen and brush of the polygon depending on the type and the selection status.
     */
    void UpdatePenAndBrush();

    /**
     * Will update the velocity arrow according to the provided velocity values.
     * @param vx Velocity component in x direction.
     * @param vy Velocity component in y direction.
     */
    void UpdateVelocityArrow(qreal vx, qreal vy);

    /**
     * Will update the size of the rectangle.
     * @param length Length of the rectangle.
     * @param width  Width of the rectangle.
     */
    void UpdateDimensions(qreal length, qreal width);

    /**
     * Will update the text displayed on hovering the object with the mouse. All values from the state may appear.
     * @param state Current state that should be displayed in the description box.
     */
    void UpdateToolTipText(const std::shared_ptr<cpm_scenario::ObjectState> &state);

    /**
     * Shifts and rotates the rectangle.
     * @param x Position component in x direction.
     * @param y Position component in y direction.
     * @param orientation Orientation angle of box.
     */
    void UpdatePositionAndOrientation(qreal x, qreal y, qreal orientation);

public:
    /**
     * Creates an visualisation element linked to the provided object using the scale factor.
     * @param parent Possible parent element this is relative to.
     * @param extendedObject Object this is linked to.
     * @param scaleFactor Scale factor to apply during linking.
     */
    ExtendedObjectItem(QGraphicsItem *parent,
                       cpm_scenario::ExtendedObjectPtr extendedObject,
                       double scaleFactor = 1.0);

    /**
     * Will render this object.
     * @param painter QPainter to render with.
     * @param option Options to follow.
     * @param widget QWidget to paint on.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    /**
     * Updates the object to use the current frame of the linked object. The update will be rendered independent of this
     * on the next render call.
     * @param currentFrame Frame to show on next render call.
     */
    void SetCurrentFrame(long currentFrame);
};

#endif //EXTENDEDOBJECTITEM_H
