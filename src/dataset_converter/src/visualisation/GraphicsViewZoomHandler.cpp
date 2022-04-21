#include "GraphicsViewZoomHandler.h"

#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include <qmath.h>
#include <QGestureEvent>
#include <QDebug>


GraphicsViewZoomHandler::GraphicsViewZoomHandler(QGraphicsView *view)
    : QObject(view), m_canvas(view), m_modifiers(Qt::ControlModifier), m_zoom_base_factor(1.0015)
{
    this->m_canvas->viewport()->installEventFilter(this);
    this->m_canvas->setMouseTracking(true);
}

void GraphicsViewZoomHandler::zoom(double factor)
{
    this->m_canvas->scale(factor, factor);
    this->m_canvas->centerOn(m_target_scene_position);
    QPointF deltaViewportPos = m_target_viewport_position - QPointF(this->m_canvas->viewport()->width() / 2.0,
                                                                    this->m_canvas->viewport()->height() / 2.0);
    QPointF viewportCenter = this->m_canvas->mapFromScene(m_target_scene_position) - deltaViewportPos;
    this->m_canvas->centerOn(this->m_canvas->mapToScene(viewportCenter.toPoint()));
    emit zoomed();
}

bool GraphicsViewZoomHandler::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseMove) {
        auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
        QPointF delta = m_target_viewport_position - mouseEvent->pos();
        if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
            m_target_viewport_position = mouseEvent->pos();
            m_target_scene_position = this->m_canvas->mapToScene(mouseEvent->pos());
        }
    }
    else if (event->type() == QEvent::Wheel) {
        auto *wheelEvent = dynamic_cast<QWheelEvent *>(event);
        if (QApplication::keyboardModifiers() == m_modifiers) {
            double angle = wheelEvent->angleDelta().y();
            double factor = qPow(this->m_zoom_base_factor, angle);
            zoom(factor);
            return true;
        }
    }
    Q_UNUSED(object)
    return false;
}
