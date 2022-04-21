#include "ExtendedObjectItem.h"
#include "ColorDefinition.h"

#include <QPen>
#include <QBrush>
#include <QGraphicsScene>
#include <QPainter>

#include <cpm_scenario/ExtendedObject.h>
#include <QtMath>
#include <utility>

void ExtendedObjectItem::SetCurrentFrame(long currentFrame)
{
    current_frame_ = currentFrame;
    // QGraphics items are not part of the signal slot principle of Qt those this update is triggered manually.
    if (this->current_frame_ > -1) this->UpdateObject();
}
ExtendedObjectItem::ExtendedObjectItem(QGraphicsItem *parent,
                                       cpm_scenario::ExtendedObjectPtr extendedObject,
                                       double scaleFactor)
    : QGraphicsRectItem(parent), extended_object_(std::move(extendedObject)), scale_factor_(scaleFactor)
{}

void ExtendedObjectItem::UpdateObject()
{
    // If object is invalid do not update
    if (current_frame_ < 0) return;
    if (scale_factor_ == 0.0) return;

    if (!extended_object_) return;

    // Buffer frame for an eventual frame overwrite
    long frame = this->current_frame_;
    // Scenario with planning problem was loaded, keep the object visible for more than a single frame,
    if (extended_object_->GetStates().size() == 2) {
        frame = extended_object_->GetFirstFrame();
        if (abs(current_frame_ - extended_object_->GetFirstFrame())
            > abs(current_frame_ - extended_object_->GetLastFrame())) {
            frame = extended_object_->GetLastFrame();
        }
    }
    if (!extended_object_->ContainsState(current_frame_)) return;

    // Get meta data from current state
    auto state = this->extended_object_->GetStates().at(frame);
    qreal x = state->GetPosition().x() * this->scale_factor_;
    qreal y = state->GetPosition().y() * this->scale_factor_;
    qreal vx = state->GetVelocity().x() * this->scale_factor_;
    qreal vy = state->GetVelocity().y() * this->scale_factor_;
    qreal orientation = qRadiansToDegrees(state->GetOrientation());

    qreal length = extended_object_->GetDimension().x() * this->scale_factor_;
    qreal width = extended_object_->GetDimension().y() * this->scale_factor_;

    // Update visualisation
    UpdatePenAndBrush();
    UpdatePositionAndOrientation(x, y, orientation);
    UpdateDimensions(length, width);
    UpdateToolTipText(state);
    UpdateVelocityArrow(vx, vy);
}
void ExtendedObjectItem::UpdatePositionAndOrientation(qreal x, qreal y, qreal orientation)
{
    setPos({x, y});
    setRotation(orientation);
}
void ExtendedObjectItem::UpdateToolTipText(const std::shared_ptr<cpm_scenario::ObjectState> &state)
{
    setToolTip(QString::fromStdString("<html>") +
        "Object: " + QString::number(extended_object_->GetId()) + "<br>" +
        "Type: " + QString(extended_object_->GetTypeString().c_str()) + "<br>" +
        "Position: " + QString::asprintf("%0.2f", state->GetPosition().x()) + "m "
                   + QString::asprintf("%0.2f", state->GetPosition().y()) + "m <br>" +
        "Velocity: " + QString::asprintf("%0.2f", state->GetVelocity().x()) + "m/s "
                   + QString::asprintf("%0.2f", state->GetPosition().y()) + "m/s <br>" +
        "Size: " + QString::asprintf("%0.2f", extended_object_->GetDimension().x()) + "m "
                   + QString::asprintf("%0.2f", extended_object_->GetDimension().y()) + "m <br>" +
        "</html>");
}
void ExtendedObjectItem::UpdateDimensions(qreal length, qreal width)
{
    setRect(0 - length / 2.0, 0 - width / 2.0, length, width);
}
void ExtendedObjectItem::UpdateVelocityArrow(qreal vx, qreal vy)
{
    this->velocity_arrow_.setLine(0, 0, static_cast<int>(vx), static_cast<int>(vy));
}
void ExtendedObjectItem::UpdatePenAndBrush()
{
    auto type = extended_object_->GetType();
    QBrush brush;
    QPen pen;
    switch (type) {
        case cpm_scenario::ExtendedObjectType::CAR:brush = QBrush(ColorDefinitions::LIGHT_GREEN);
            pen = QPen(ColorDefinitions::GREEN);
            break;
        case cpm_scenario::ExtendedObjectType::PEDESTRIAN:
        case cpm_scenario::ExtendedObjectType::BICYCLE_MOTORCYCLES:
        case cpm_scenario::ExtendedObjectType::TRUCK_BUS:
        case cpm_scenario::ExtendedObjectType::VAN:
        case cpm_scenario::ExtendedObjectType::TRAILER:
        case cpm_scenario::ExtendedObjectType::UNKNOWN:brush = QBrush(ColorDefinitions::LILA);
            pen = QPen(ColorDefinitions::VIOLET);
            break;
    }
    QColor brushColor = brush.color();
    brushColor.setAlpha(200);
    brush.setColor(brushColor);

    setPen(pen);
    setBrush(brush);
}
void ExtendedObjectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Paint rectangle
    QGraphicsRectItem::paint(painter, option, widget);

    // Draw velocity arrow
    painter->save();

    painter->setBrush(brush());
    painter->setPen(pen());
    painter->rotate(-rotation());
    painter->drawLine(velocity_arrow_);

    painter->restore();
}
