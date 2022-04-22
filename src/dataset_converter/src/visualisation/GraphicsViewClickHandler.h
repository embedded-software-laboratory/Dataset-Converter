#ifndef GRAPHICSVIEWCLICKHANDLER_H
#define GRAPHICSVIEWCLICKHANDLER_H

#include <QGraphicsView>

#include "LaneletVisualisation.h"
#include "src/worker/LaneletHandler.h"

/**
 * Possible tool modes
 */
enum class EditorMode
{
    SELECT = 0, ///< Move and select nodes
    NODE_TOOL = 1, ///< Add new nodes
    WAY_TOOL = 2, ///< Add new ways
    LANELET_TOOL = 3 ///< Add new lanelets
};

class WayItem;

/**
 * Handler that manages all click interaction with the canvas.
 */
class GraphicsViewClickHandler: public QObject
{
Q_OBJECT
private:
    Q_PROPERTY(EditorMode m_mode READ editorMode NOTIFY editorModeChanged WRITE setEditorMode)
    Q_PROPERTY(LaneletItem *m_selectedLanelet READ selectedLanelet NOTIFY selectedLaneletChanged)
    Q_PROPERTY(WayItem *m_selectedWay READ selectedWay NOTIFY selectedPathChanged)
    Q_PROPERTY(NodeItem *m_selectedNode READ selectedNode NOTIFY selectedNodeChanged)
    EditorMode m_mode = EditorMode::SELECT;  ///<Mode will be dictated by the user with a dedicated toolbar

    // Intern access to get elements under mouse
    QGraphicsView *const m_canvas; ///< Canvas

    LaneletItem *m_selectedLanelet = nullptr; ///< Buffer for the selected lanelet item
    WayItem *m_selectedWay = nullptr; ///< Buffer for the selected way item
    NodeItem *m_selectedNode = nullptr; ///< Buffer for the selected node item

    /**
     * Main event interrupt that is called by the system. This will split the calls to the sercive functions.
     * @param object Object triggering the event.
     * @param event Actual event. Only mouse events are used.
     * @return True if used, false otherwise.
     */
    bool eventFilter(QObject *object, QEvent *event) override;

    /**
     * Show the type menu for a way. Change the way type.
     * @param globalPosition Position to show the context menu.
     * @param selectedWay Way to show this for.
     */
    void showMetadataMenuWay(const QPoint &globalPosition, WayItem *selectedWay) const;

    /**
     * Show the type menu for a lanelet. Change the lanelet type.
     * @param globalPosition Position to show the context menu.
     * @param selectedLanelet Lanelet to show this for.
     */
    void showMetadataMenuLanelet(const QPoint &globalPosition, LaneletItem *selectedLanelet) const;

    /**
     * Show the action menu for a node. Option to delete the node.
     * @param globalPosition Position to show the context menu.
     * @param selectedNode Node to show this for.
     */
    void showActionMenuNode(const QPoint &globalPosition, NodeItem *selectedNode);

    /**
     * Show the action menu for a way. Option to delete the way.
     * @param globalPosition Position to show the context menu.
     * @param selectedWay Way to show this for.
     */
    void showActionMenuWay(const QPoint &globalPosition, WayItem *selectedWay);

    /**
     * Show the action menu for a lanelet. Option to delete or flip the lanelet.
     * @param globalPosition Position to show the context menu.
     * @param selectedLanelet Lanelet to show this for.
     */
    void showActionMenuLanelet(const QPoint &globalPosition, LaneletItem *selectedLanelet);

    /**
     * Show context menu with type menus if a way or lanelet is selection.
     * @param globalPosition Position to show the context menu.
     * @param selectedWay Way to show this for.
     * @param selectedLanelet Lanelet to show this for.
     * @return True if used, false otherwise.
     */
    bool handleRightClickSelectTool(const QPoint &globalPosition,
                                    WayItem *selectedPath,
                                    LaneletItem *selectedLanelet) const;

    /**
     * Show context menu with action menus if a way is selection.
     * @param selectedWay Way to show this for.
     * @return True if used, false otherwise.
     */
    bool handleRightClickWayTool(const QPoint &globalPosition, WayItem *selectedWay);

    /**
     * Handles extension of a way with a new node or selects a path under the courser.
     * @param nodeUnderCursor Possible node to add to the path.
     * @param wayUnderCursor Possible way to select.
     * @param selectedWay Already selected way to add node to.
     * @return True if used, false otherwise.
     */
    bool handleLeftClickWayTool(NodeItem *nodeUnderCursor, WayItem *wayUnderCursor, WayItem *selectedWay);

    /**
     * Show context menu with action menus if a node is selection.
     * @param selectedNode Node to show this for.
     * @return True if used, false otherwise.
     */
    bool handleRightClickNode(const QPoint &globalPosition, NodeItem *selectedNode);

    /**
     * Handles adding of a new node or selects a node under the courser.
     * @param scenePosition Possible new position of a node.
     * @param nodeUnderCursor Possible node to select.
     * @param selectedNode Already selected node.
     * @return True if used, false otherwise.
     */
    bool handleLeftClickNode(QPointF &scenePosition, NodeItem *nodeUnderCursor, const NodeItem *selectedNode);

    /**
     * Show context menu with action menus if a lanelet is selection.
     * @param selectedLanelet Lanelet to show this for.
     * @return True if used, false otherwise.
     */
    bool handleRightClickLanelet(const QPoint &globalPosition, LaneletItem *selectedLanelet);

    /**
     * Handles adding of a new lanelet or selects a existing lanelet under the courser.
     * @param wayUnderCursor Possible way to add to lanelet.
     * @param laneletUnderCursor Possible lanelet to select.
     * @param selectedLanelet Already selected lanelet to add way to.
     * @return True if used, false otherwise.
     */
    bool handleLeftClickLanelet(WayItem *wayUnderCursor,
                                LaneletItem *laneletUnderCursor,
                                LaneletItem *selectedLanelet);

    /**
     * Set the new selected lanelet and emits signals.
     * @param selectedLanelet New selected lanelet.
     */
    void setSelectedLanelet(LaneletItem *selectedLanelet);

    /**
     * Set the new selected way and emits signals.
     * @param selectedWay New selected way.
     */
    void setSelectedPath(WayItem *selectedWay);

    /**
     * Set the new selected node and emits signals.
     * @param selectedNode New selected node.
     */
    void setSelectedNode(NodeItem *selectedNode);

private slots:
    /**
     * Handles changes in the selected items to always limit the selection to single item.
     */
    void handleSceneSelectionChange();

public:
    /**
     * Creates the click handler and modifies the canvas to send events.
     * @param view Canvas that should be used to get events.
     */
    explicit GraphicsViewClickHandler(QGraphicsView *view);

    /**
     * Getter for the current selected edit.
     * @return Selected edit mode.
     */
    [[nodiscard]] EditorMode editorMode() const;

    /**
     * Getter for the selected lanelet.
     * @return Selected lanelet.
     */
    [[nodiscard]] LaneletItem *selectedLanelet() const;

    /**
     * Getter for the selected way.
     * @return Selected way.
     */
    [[nodiscard]] WayItem *selectedWay() const;

    /**
     * Getter for the selected node.
     * @return Selected node.
     */
    [[nodiscard]] NodeItem *selectedNode() const;


public slots:
    /**
     * Updates the selected editor mode when the use changes the selected tool.
     * @param mode New editor mode.
     */
    void setEditorMode(EditorMode mode);
    /**
     * Interface to trigger node deletion from the context menu.
     */
    void actionToRemoveNodeTriggered();
    /**
     * Interface to trigger way deletion from the context menu.
     */
    void actionToRemoveWayTriggered();
    /**
     * Interface to trigger lanelet deletion from the context menu.
     */
    void actionToRemoveLaneletTriggered();

    /**
     * Interface to trigger lanelet flip from the context menu.
     */
    void actionToFlipLaneletTriggered();

signals:
    /**
     * Triggered of the selected editor mode is changed.
     * @param mode New editor mode.
     */
    void editorModeChanged(EditorMode mode);

    // Signals to emit requested action
    /**
     * User request a new to be added at the given position.
     * @param position Position to add a new node.
     */
    void requestAddNode(QPointF position);

    /**
     * User requests to remove a node.
     * @param node Node to remove.
     */
    void requestRemoveNode(NodeItem *node);

    /**
     * User requests to add a new way with the given node as starting point.
     * @param node Node to add to the new way.
     */
    void requestNewWay(NodeItem *node);

    /**
     * User requests to remove a way.
     * @param way Way to be removed.
     */
    void requestRemoveWay(WayItem *way);

    /**
     * User requests a node to be added to a way.
     * @param way Way to add the node to.
     * @param node Node that should be added to the way.
     */
    void requestAddNodeToWay(WayItem *way, NodeItem *node);

    /**
     * User requests a new lanelet with the given way as a starting point.
     * @param way Way to add to the new lanelet.
     */
    void requestNewLanelet(WayItem *way);

    /**
     * User request the removal of a lanelet.
     * @param lanelet Lanelet to remove.
     */
    void requestRemoveLanelet(LaneletItem *lanelet);

    /**
     * User requests to add a way to the given lanelet.
     * @param lanelet Lanelet to add way to.
     * @param way Way to add to the lanelet.
     */
    void requestAddWayToLanelet(LaneletItem *lanelet, WayItem *way);

    /**
     * Triggered of the selected lanelet changed. Used to disable the context menu if no is selected.
     * @param lanelet Lanelet that is selected, maybe nullptr.
     */
    void selectedLaneletChanged(LaneletItem *lanelet);
    /**
     * Triggered of the selected way changed. Used to disable the context menu if no is selected.
     * @param way Way that is selected, maybe nullptr.
     */
    void selectedPathChanged(WayItem *way);

    /**
     * Triggered of the selected node changed. Used to disable the context menu if no is selected.
     * @param node Node that is selected, maybe nullptr.
     */
    void selectedNodeChanged(NodeItem *node);
};

#endif //GRAPHICSVIEWCLICKHANDLER_H
