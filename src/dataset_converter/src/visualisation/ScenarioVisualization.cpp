#include "ScenarioVisualization.h"

#include <QApplication>

ScenarioVisualization::ScenarioVisualization(QGraphicsView *canvas, QObject *parent)
    : QObject(parent), m_canvas(canvas), m_scene(new QGraphicsScene(this))
{
    // Add Scene and set general behaviour
    this->m_canvas->setScene(this->m_scene);

    // Loading Lab Background
    QPixmap labBackgroundPixmap(":resources/lab_background.png");
    this->m_scaleFactor = labBackgroundPixmap.width() / 81.0;
    this->m_labWidth = labBackgroundPixmap.width();
    this->m_labHeight = labBackgroundPixmap.height();

    // Set base size of Scene and add clipping Parent
    this->m_scene->setSceneRect(-100, -100, this->m_labWidth + 200,
                                this->m_labHeight + 200);

    // Define needed pens and brushes
    QBrush brushWindowColor(this->m_canvas->palette().color(QPalette::Window));
    QPen penHighlightColor(this->m_canvas->palette().color(QPalette::Highlight));
    QBrush brushHighlightColor(this->m_canvas->palette().color(QPalette::Highlight));
    QBrush brushHighlightColorPattern(this->m_canvas->palette().color(QPalette::Highlight),
                                      Qt::FDiagPattern);
    QFont sceneFont = this->m_canvas->font();
    sceneFont.setPointSize(20);

    // Make background follow palette
    this->m_scene->setBackgroundBrush(brushWindowColor);

    // Add pattern to border
    auto patternRectangle = this->m_scene->addRect(
        -10, -10, this->m_labWidth + 20, this->m_labHeight + 20, Qt::NoPen, brushHighlightColorPattern);
    patternRectangle->setZValue(0);

    // Add black rectangle to make pattern only appear at border
    auto blackRectangle = this->m_scene->addRect(0, 0, this->m_labWidth, this->m_labHeight, Qt::NoPen,
                                                 brushWindowColor);
    blackRectangle->setZValue(1);

    // Add Bounding rectangle
    this->m_borderItem = this->m_scene->addRect(0, 0, m_labWidth, m_labHeight);
    this->m_borderItem->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    this->m_borderItem->setZValue(2);

    // Add Lab background Image
    this->m_labBackgroundItem = this->m_scene->addPixmap(labBackgroundPixmap);
    //this->m_labBackgroundItem->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    this->m_labBackgroundItem->setParentItem(this->m_borderItem);
    this->m_labBackgroundItem->setZValue(1);

    // Add dummy scenario background
    this->m_scenarioBackgroundItem = this->m_scene->addPixmap(QPixmap());
    this->m_scenarioBackgroundItem->setTransformationMode(Qt::SmoothTransformation);
    this->m_scenarioBackgroundItem->setParentItem(this->m_borderItem);
    this->m_scenarioBackgroundItem->setZValue(2);

    // Draw line around bound rectangle
    this->m_scene->addRect(0, 0, m_labWidth, m_labHeight, penHighlightColor, Qt::NoBrush);

    // Draw line around clipping rectangle with 10 m space
    auto scenarioClippingElement = this->m_scene->addRect(
        10 * this->m_scaleFactor, 10 * this->m_scaleFactor,
        this->m_labWidth - 20 * this->m_scaleFactor, this->m_labHeight - 20 * this->m_scaleFactor,
        penHighlightColor, Qt::NoBrush);
    scenarioClippingElement->setZValue(10);
    scenarioClippingElement->setFlag(QGraphicsItem::ItemClipsChildrenToShape);

    // Add container that will contain all dynamic elements
    this->m_scenarioForegroundItem = this->m_scene->addRect(0, 0, this->m_labWidth, this->m_labHeight);
    this->m_scenarioForegroundItem->setZValue(9);
    this->m_scenarioForegroundItem->setParentItem(scenarioClippingElement);

    // Add indicator of current shift position
    this->m_shiftIndicatorItem = this->m_scene->addEllipse(
        this->m_scenarioShiftX - 5, this->m_scenarioShiftY - 5, 10, 10, Qt::NoPen,
        brushHighlightColor);
    this->m_shiftIndicatorItem->setZValue(5);

    // Add title to scene
    this->m_scenarioTitleItem =
        this->m_scene->addSimpleText("No scenario loaded!", sceneFont);
    this->m_scenarioTitleItem->setPos(0, -60);
    this->m_scenarioTitleItem->setBrush(QBrush(Qt::white));
    this->m_scenarioTitleItem->setZValue(-76);

    // Add frame number to scene
    this->m_frameItem = this->m_scene->addSimpleText("", sceneFont);
    this->m_frameItem->setPos(0, -35);
    this->m_frameItem->setBrush(QBrush(Qt::white));
    this->m_frameItem->setZValue(-75);

    // Make slider update frame
    connect(this, &ScenarioVisualization::frameChanged, this,
            &ScenarioVisualization::updateDynamicObjects);
    connect(this, &ScenarioVisualization::scenarioShiftXChanged, this,
            &ScenarioVisualization::applyShiftToDynamicObjects);
    connect(this, &ScenarioVisualization::scenarioShiftYChanged, this,
            &ScenarioVisualization::applyShiftToDynamicObjects);
    connect(this, &ScenarioVisualization::scenarioRotationChanged, this,
            &ScenarioVisualization::applyShiftToDynamicObjects);

    // Make scenario update visualization
    connect(this, &ScenarioVisualization::scenarioChanged, this,
            &ScenarioVisualization::clearDynamicObjects);
    connect(this, &ScenarioVisualization::scenarioChanged, this,
            &ScenarioVisualization::updateScenarioBackgroundImage);
    connect(this, &ScenarioVisualization::scenarioChanged, this,
            &ScenarioVisualization::updateScenarioMetaData);
}

void ScenarioVisualization::updateScenarioBackgroundImage(cpm_scenario::ScenarioPtr scenario)
{
    if (!scenario) {
        this->m_scenarioBackgroundItem->setPixmap(QPixmap(":resources/no-scenario-loaded.png"));
        return;
    }
    // Scale image
    QPixmap pixmap(scenario->GetBackgroundImageSourcePath().c_str());
    qreal scenarioScaleFactor = scenario->GetBackgroundImageScaleFactor();
    qreal combinedScaleFactor = this->m_scaleFactor * 1.0 / scenarioScaleFactor;
    int width = std::floor(static_cast<qreal>(pixmap.width()) * combinedScaleFactor);
    int height = std::floor(static_cast<qreal>(pixmap.height()) * combinedScaleFactor);

    // Display image
    this->m_scenarioBackgroundItem->setPixmap(
        pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));

}
void ScenarioVisualization::updateScenarioMetaData(cpm_scenario::ScenarioPtr scenario)
{
    if (!scenario) {
        this->m_scenarioTitleItem->setText("No scenario selected");
        return;
    }
    this->m_scenarioTitleItem->setText("Scenario: " + QString::fromStdString(scenario->GetName()));
}
void ScenarioVisualization::updateDynamicObjects(qint64 frame)
{
    this->m_frameItem->setText("Frame: " + QString::number(frame));
    for (const auto &object : this->m_scenario->GetObjects()) {
        // Object is not in the scene but was created -> clean up graphic element
        if (!object->IsInScene(frame)
            && this->m_idToDynamicElementMap.contains(object->GetId())) {
            // Found object -> clean up
            QGraphicsItem *bodyGraphicsItem = this->m_idToDynamicElementMap[object->GetId()];
            this->m_scene->removeItem(bodyGraphicsItem);
            this->m_idToDynamicElementMap.remove(object->GetId());
        }
        // Object is not in the scene -> skip
        if (!object->IsInScene(frame)) continue;

        // Object is in scene but no object was created
        if (!this->m_idToDynamicElementMap.contains(object->GetId())) {
            auto *newItem = new ExtendedObjectItem(this->m_scenarioForegroundItem, object, this->m_scaleFactor);
            this->m_idToDynamicElementMap[object->GetId()] = newItem;
        }

        // Update object
        auto item = this->m_idToDynamicElementMap[object->GetId()];
        //item->SetExtendedObject(object);
        item->SetCurrentFrame(frame);
        //item->UpdateObject();

    }

    this->applyShiftToDynamicObjects();
}
void ScenarioVisualization::clearDynamicObjects()
{
    for (auto child : this->m_idToDynamicElementMap) {
        this->m_scene->removeItem(child);
    }
    this->m_idToDynamicElementMap.clear();
}

void ScenarioVisualization::applyShiftToDynamicObjects()
{
    this->m_shiftIndicatorItem->setPos(this->m_scenarioShiftX, this->m_scenarioShiftY);
    this->m_scenarioBackgroundItem->setPos(this->m_scenarioShiftX, this->m_scenarioShiftY);
    //this->m_scenarioBackgroundItem->setTransformOriginPoint(this->m_scenarioShiftX, this->m_scenarioShiftY);
    this->m_scenarioBackgroundItem->setRotation(this->m_scenarioRotation);

    this->m_scenarioForegroundItem->setPos(this->m_scenarioShiftX, this->m_scenarioShiftY);
    //this->m_scenarioForegroundItem->setTransformOriginPoint(this->m_scenarioShiftX, this->m_scenarioShiftY);
    this->m_scenarioForegroundItem->setRotation(this->m_scenarioRotation);
}

cpm_scenario::ScenarioPtr ScenarioVisualization::scenario() const
{
    return this->m_scenario;
}
void ScenarioVisualization::setScenario(cpm_scenario::ScenarioPtr scenario)
{
    this->m_scenario = scenario;
    emit this->scenarioChanged(scenario);
}
qint64 ScenarioVisualization::frame() const
{
    return this->m_frame;
}
void ScenarioVisualization::setFrame(qint64 frame)
{
    this->m_frame = frame;
    emit this->frameChanged(frame);
}
qreal ScenarioVisualization::scenarioTransparency() const
{
    return this->m_scenarioTransparency;
}
void ScenarioVisualization::setScenarioTransparency(qreal transparency)
{
    this->m_scenarioTransparency = transparency;
    this->m_scenarioBackgroundItem->setOpacity(transparency);
    this->m_scene->update();
    emit this->scenarioTransparencyChanged(transparency);
}
void ScenarioVisualization::setScenarioShiftX(qreal shiftX)
{
    this->m_scenarioShiftX = shiftX * this->m_scaleFactor;
    emit this->scenarioShiftXChanged(this->m_scenarioShiftX);
}
qreal ScenarioVisualization::scenarioShiftX() const
{
    return this->m_scenarioShiftX;
}
void ScenarioVisualization::setScenarioShiftY(qreal shiftY)
{
    this->m_scenarioShiftY = shiftY * this->m_scaleFactor;
    emit this->scenarioShiftYChanged(this->m_scenarioShiftY);
}
qreal ScenarioVisualization::scenarioShiftY() const
{
    return this->m_scenarioShiftY;
}
void ScenarioVisualization::setScenarioRotation(qreal rotation)
{
    this->m_scenarioRotation = rotation;
    emit this->scenarioRotationChanged(m_scenarioRotation);
}
qreal ScenarioVisualization::scenarioRotation() const
{
    return this->m_scenarioRotation;
}
qreal ScenarioVisualization::labTransparency() const
{
    return this->m_labTransparency;
}
void ScenarioVisualization::setLabTransparency(qreal transparency)
{
    this->m_labTransparency = transparency;
    this->m_labBackgroundItem->setOpacity(transparency);
    this->m_scene->update();
    emit this->labTransparencyChanged(transparency);
}
qreal ScenarioVisualization::scaleFactor() const
{
    return this->m_scaleFactor;
}

