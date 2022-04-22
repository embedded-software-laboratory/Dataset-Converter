#ifndef GRAPHICSVIEWZOOMHANDLER_H
#define GRAPHICSVIEWZOOMHANDLER_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QKeyEvent>

/**
 * Handler that manages all zoom interaction with the canvas.
 *
 * This class was developed by Pavel Strakhov an posted on StackOverflow at October 1st 2013.
 * https://stackoverflow.com/questions/19113532/qgraphicsview-zooming-in-and-out-under-mouse-position-using-mouse-wheel
 * Only minor changes were made to make class compliant to the Qt code formatting.
 */
class GraphicsViewZoomHandler: public QObject
{
Q_OBJECT
private:
    QGraphicsView *const m_canvas; ///< Graphics view that should be zoomed.

    QPointF m_target_scene_position; ///< Position of the scene that should be zoomed.

    QPointF m_target_viewport_position; ///< Position of the viewport that should be zoomed.

    const Qt::KeyboardModifiers m_modifiers; ///< Key that needs to be pressed to trigger zoom.

    const double m_zoom_base_factor; ///< Factor that make the zoom.

    /**
     * @brief eventFilter Will be triggered by all events from the parent graphics view.
     * @param object Contains a pointer to the element that the event was triggered in.
     * @param event Actually event that was triggered.
     * @return If true is returned the event will not be processed any more.
     */
    bool eventFilter(QObject *object, QEvent *event) override;

public:
    /**
     * @brief GraphicsViewZoom Will create a new event filter that adds the zoom function to the graphics view.
     * @param view GraphicsView to add zoom function.
     */
    explicit GraphicsViewZoomHandler(QGraphicsView *view);

    /**
     * @brief zoom Will zoom the graphics view to the given zoom factor.
     * @param factor New zoom factor.
     */
    void zoom(double factor);

signals:
    /**
     * @brief zoomed Signal emitted if the zoom process is complete.
     */
    void zoomed();
};

#endif //GRAPHICSVIEWZOOMHANDLER_H
