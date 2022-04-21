#ifndef SCENARIOVISUALIZATION_H
#define SCENARIOVISUALIZATION_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QList>
#include <cpm_scenario/Scenario.h>
#include <QLabel>
#include <QSlider>
#include <QListWidget>

#include <QGraphicsPixmapItem>
#include <QPixmap>

#include "graphics_items/ExtendedObjectItem.h"

/**
 * Visualisation service for the scenario elements.
 */
class ScenarioVisualization: public QObject
{
Q_OBJECT
private:
    Q_PROPERTY(cpm_scenario::ScenarioPtr m_scenario READ scenario NOTIFY scenarioChanged WRITE setScenario)
    Q_PROPERTY(qint64 m_frame READ frame NOTIFY frameChanged WRITE setFrame)
    Q_PROPERTY(qreal
                   m_scenarioTransparency READ scenarioTransparency NOTIFY scenarioTransparencyChanged WRITE setScenarioTransparency)
    Q_PROPERTY(qreal m_labTransparency READ labTransparency NOTIFY labTransparencyChanged WRITE setLabTransparency)
    Q_PROPERTY(qreal m_scenarioShiftX READ scenarioShiftX NOTIFY scenarioShiftXChanged WRITE setScenarioShiftX)
    Q_PROPERTY(qreal m_scenarioShiftY READ scenarioShiftY NOTIFY scenarioShiftYChanged WRITE setScenarioShiftY)
    Q_PROPERTY(qreal
                   m_scenarioRotation READ scenarioRotation NOTIFY scenarioTransparencyChanged WRITE setScenarioRotation)

    // Public properties
    cpm_scenario::ScenarioPtr m_scenario; ///< Scenario linked to the visualisation
    qint64 m_frame = 0; ///< Currently visualised frame

    // Transformation properties
    qreal m_scenarioShiftX = 0.0; ///< Current shift in x direction
    qreal m_scenarioShiftY = 0.0; ///< Current shift in y direction
    qreal m_scenarioRotation = 0.0; ///< Current rotation

    // Display properties
    qreal m_scenarioTransparency = 0.0; ///< Transparency of the scenario layer
    qreal m_labTransparency = 0.0; ///< Transparency of the background layer

    // Widgets
    QGraphicsView *const m_canvas = nullptr; ///< Canvas to render on

    // Static scene elements
    QGraphicsScene *const m_scene = nullptr; ///< Scene where all objects live in

    QGraphicsSimpleTextItem *m_scenarioTitleItem = nullptr; ///< Scenario title in visualisation
    QGraphicsSimpleTextItem *m_frameItem = nullptr; ///< Current frame in visualisation
    QGraphicsEllipseItem *m_shiftIndicatorItem = nullptr; ///< Bubble indication the transformation origin.

    QGraphicsPixmapItem *m_scenarioBackgroundItem = nullptr; ///< Scenario background image
    QGraphicsRectItem *m_scenarioForegroundItem = nullptr; ///< Scenario dynamic element

    QGraphicsRectItem *m_borderItem = nullptr; ///< Lan border line
    QGraphicsPixmapItem *m_labBackgroundItem = nullptr; ///< Lab background

    // Dynamic scene elements
    QMap<qint64, ExtendedObjectItem *>
        m_idToDynamicElementMap; ///< Map from id to linked extended object for quick access

    qreal m_scaleFactor = 1.0; ///< Current scale factor from scenario to pixel values
    qreal m_labWidth = 0.0; ///< Lab background width in pixels
    qreal m_labHeight = 0.0; ///< Lab background height in pixels

private slots:
    /**
     * Updates background if new scenario is loaded.
     * @param scenario New scenario.
     */
    void updateScenarioBackgroundImage(cpm_scenario::ScenarioPtr scenario);
    /**
     * Updates metadata such as the title if a new scenario is loaded.
     * @param scenario New scenario.
     */
    void updateScenarioMetaData(cpm_scenario::ScenarioPtr scenario);
    /**
     * Updates all dynamic elements to show match there object states to the frame.
     * @param frame Frame to show
     */
    void updateDynamicObjects(qint64 frame);

    /**
     * Clear all dynamic objects.
     */
    void clearDynamicObjects();

    /**
     * Updates the shift of the scenario layer.
     */
    void applyShiftToDynamicObjects();

public:
    /**
     * Creates visualisation service and creates all static elements of the visualisation.
     * @param canvas Canvas to render on.
     * @param parent Possible parent or qt pointer destruction.
     */
    explicit ScenarioVisualization(QGraphicsView *canvas, QObject *parent = nullptr);

    /**
     * Getter for the currently displayed scenario.
     * @return Currently displayed scenario.
     */
    [[nodiscard]] cpm_scenario::ScenarioPtr scenario() const;

    /**
     * Getter for the currently displayed frame.
     * @return Currently displayed frame.
     */
    [[nodiscard]] qint64 frame() const;

    /**
     * Getter for the currently displayed scenario transparency.
     * @return Currently displayed scenario transparency.
     */
    [[nodiscard]] qreal scenarioTransparency() const;

    /**
     * Getter for the currently displayed lab transparency.
     * @return Currently displayed lab transparency.
     */
    [[nodiscard]] qreal labTransparency() const;

    /**
     * Getter for the currently displayed shift in x direction.
     * @return Currently displayed shift in x direction.
     */
    [[nodiscard]] qreal scenarioShiftX() const;
    /**
     * Getter for the currently displayed shift in y direction.
     * @return Currently displayed shift in y direction.
     */
    [[nodiscard]] qreal scenarioShiftY() const;
    /**
     * Getter for the currently displayed rotation.
     * @return Currently displayed rotation.
     */
    [[nodiscard]] qreal scenarioRotation() const;

    /**
     * Getter for the background to scenario scale factor.
     * @return Background to scenario scale factor.
     */
    [[nodiscard]] qreal scaleFactor() const;

public slots:
    /**
     * Setter for the current scenario. Will clear all objects and load a new background.
     * @param scenario New scenario.
     */
    void setScenario(cpm_scenario::ScenarioPtr scenario);

    /**
     * Setter for the current displayed frame. Will update the visualisation to match.
     * @param frame New frame.
     */
    void setFrame(qint64 frame);

    /**
     * Setter for the scenario layer transparency.
     * @param transparency Transparency of scenario.
     */
    void setScenarioTransparency(qreal transparency);
    /**
     * Setter for the background layer transparency.
     * @param transparency Transparency of background.
     */
    void setLabTransparency(qreal transparency);

    /**
     * Setter for the scenario shift in x direction.
     * @param shiftX Shift in x direction.
     */
    void setScenarioShiftX(qreal shiftX);

    /**
     * Setter for the scenario shift in y direction.
     * @param shiftX Shift in y direction.
     */
    void setScenarioShiftY(qreal shiftY);

    /**
     * Setter for the scenario rotation.
     * @param rotation Rotation of the scenario.
     */
    void setScenarioRotation(qreal rotation);

signals:
    /**
     * Emitted if the displayed scenario changed.
     * @param scenario New scenario.
     */
    void scenarioChanged(cpm_scenario::ScenarioPtr scenario);
    /**
     * Emitted if the displayed frame changed.
     * @param frame New frame.
     */
    void frameChanged(qint64 frame);
    /**
     * Emitted if the scenario transparency changed.
     * @param transparency New transparency.
     */
    void scenarioTransparencyChanged(qreal transparency);
    /**
     * Emitted if the lab transparency changed.
     * @param transparency New transparency.
     */
    void labTransparencyChanged(qreal transparency);
    /**
     * Emitted if the displayed shift in x direction changed.
     * @param shiftX New shift in x direction.
     */
    void scenarioShiftXChanged(qreal shiftX);
    /**
     * Emitted if the displayed shift in y direction changed.
     * @param shiftY New shift in y direction.
     */
    void scenarioShiftYChanged(qreal shiftY);
    /**
     * Emitted if the displayed rotation changed.
     * @param rotation New rotation.
     */
    void scenarioRotationChanged(qreal rotation);
};

#endif // SCENARIOVISUALIZATION_H
