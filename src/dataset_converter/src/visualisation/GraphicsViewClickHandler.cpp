#include "GraphicsViewClickHandler.h"

#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include <qmath.h>
#include <QGestureEvent>
#include <QDebug>
#include <QtWidgets/QMenu>

#include "visualisation/graphics_items/NodeItem.h"
#include "visualisation/graphics_items/WayItem.h"

GraphicsViewClickHandler::GraphicsViewClickHandler(QGraphicsView *view)
    : QObject(view), m_canvas(view)
{
    this->m_canvas->viewport()->installEventFilter(this);
    this->m_canvas->setMouseTracking(true);

    connect(this->m_canvas->scene(),
            &QGraphicsScene::selectionChanged,
            this,
            &GraphicsViewClickHandler::handleSceneSelectionChange);
}

// Pointer type need to be a pointer
template<typename PointerType, typename ListType>
PointerType GetFirstItemInList(QList<ListType> list)
{
    for (auto item : list) {
        // Cast
        auto *itemOfCorrectType = dynamic_cast<PointerType>(item);
        // Check if valid
        if (!itemOfCorrectType) continue;
        // Return found item
        return itemOfCorrectType;
    }
    // No item found
    return nullptr;
}

bool GraphicsViewClickHandler::eventFilter(QObject *object, QEvent *event)
{
    // Only use mouse event
    if (event->type() != QEvent::MouseButtonPress) return false;
    auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);

    // Get metadata and selected items
    auto scenePosition = this->m_canvas->mapToScene(mouseEvent->pos());
    auto globalPosition = mouseEvent->globalPos();

    auto itemsUnderCursor = this->m_canvas->scene()->items({scenePosition.x() - 1, scenePosition.y() - 1, 2, 2});
    auto itemsSelected = this->m_canvas->scene()->selectedItems();

    auto nodeUnderCursor = GetFirstItemInList<NodeItem *, QGraphicsItem *>(itemsUnderCursor);
    auto pathUnderCursor = GetFirstItemInList<WayItem *, QGraphicsItem *>(itemsUnderCursor);
    auto laneletUnderCursor = GetFirstItemInList<LaneletItem *, QGraphicsItem *>(itemsUnderCursor);
    auto selectedNode = GetFirstItemInList<NodeItem *, QGraphicsItem *>(itemsSelected);
    auto selectedWay = GetFirstItemInList<WayItem *, QGraphicsItem *>(itemsSelected);
    auto selectedLanelet = GetFirstItemInList<LaneletItem *, QGraphicsItem *>(itemsSelected);

    switch (this->m_mode) {
        case EditorMode::SELECT:
            // No left click as this is handled by the native QGraphicsView
            if (mouseEvent->button() == Qt::RightButton) {
                return handleRightClickSelectTool(globalPosition, selectedWay, selectedLanelet);
            }
            break;
        case EditorMode::NODE_TOOL:
            if (mouseEvent->button() == Qt::LeftButton) {
                return handleLeftClickNode(scenePosition, nodeUnderCursor, selectedNode);
            }
            else if (mouseEvent->button() == Qt::RightButton) {
                return handleRightClickNode(globalPosition, selectedNode);
            }
            break;
        case EditorMode::WAY_TOOL:
            if (mouseEvent->button() == Qt::LeftButton) {
                return handleLeftClickWayTool(nodeUnderCursor, pathUnderCursor, selectedWay);
            }
            else if (mouseEvent->button() == Qt::RightButton) {
                return handleRightClickWayTool(globalPosition, selectedWay);
            }
            break;
        case EditorMode::LANELET_TOOL:
            if (mouseEvent->button() == Qt::LeftButton) {
                return handleLeftClickLanelet(pathUnderCursor, laneletUnderCursor, selectedLanelet);
            }
            else if (mouseEvent->button() == Qt::RightButton) {
                return handleRightClickLanelet(globalPosition, selectedLanelet);
            }
            break;
    }

    // Let QGraphicsView handle all unexpected cases
    Q_UNUSED(object)
    return false;
}
bool GraphicsViewClickHandler::handleRightClickLanelet(const QPoint &globalPosition, LaneletItem *selectedLanelet)
{
    if (!selectedLanelet) return true;
    showActionMenuLanelet(globalPosition, selectedLanelet);
    return true;
}
void GraphicsViewClickHandler::showActionMenuLanelet(const QPoint &globalPosition, LaneletItem *selectedLanelet)
{
    QMenu menu(m_canvas);
    QAction actionVirtual(QIcon(":resources/icons/trash.svg"), "Delete Lanelet", &menu);
    connect(&actionVirtual, &QAction::triggered, this, [this, selectedLanelet]()
    {
        emit requestRemoveLanelet(selectedLanelet);
        qDebug() << "[GraphicsViewClickHandler] Request remove lanelet!";
    });
    menu.addAction(&actionVirtual);

    QAction actionFlip(QIcon(":resources/icons/flip.svg"), "Flip Direction", &menu);
    connect(&actionFlip, &QAction::triggered, this, [selectedLanelet]()
    {
        selectedLanelet->flipDirection();
        qDebug() << "[GraphicsViewClickHandler] Flip lanelet!";
    });
    menu.addAction(&actionFlip);

    QRect mask = menu.rect();
    mask.setSize(menu.sizeHint());

    QPainterPath path;
    path.addRoundedRect(mask, 6, 6);
    menu.setMask(path.toFillPolygon().toPolygon());

    menu.exec(globalPosition);
}
bool GraphicsViewClickHandler::handleLeftClickLanelet(WayItem *wayUnderCursor,
                                                      LaneletItem *laneletUnderCursor,
                                                      LaneletItem *selectedLanelet)
{
    if (laneletUnderCursor && !wayUnderCursor) {
        m_canvas->scene()->clearSelection();
        laneletUnderCursor->setSelected(true);
        return true;
    }
    if (!laneletUnderCursor && !wayUnderCursor) {
        m_canvas->scene()->clearSelection();
        return true;
    }
    if (!wayUnderCursor) return true;
    if (!selectedLanelet) {
        wayUnderCursor->setSelected(true);
        emit requestNewLanelet(wayUnderCursor);
        return true;
    }
    if (selectedLanelet->rightWayItem() && selectedLanelet->rightWayItem()) return true;
    wayUnderCursor->setSelected(true);
    emit requestAddWayToLanelet(selectedLanelet, wayUnderCursor);
    return true;
}
bool GraphicsViewClickHandler::handleLeftClickNode(QPointF &scenePosition,
                                                   NodeItem *nodeUnderCursor,
                                                   const NodeItem *selectedNode)
{
    if (nodeUnderCursor) {
        m_canvas->scene()->clearSelection();
        nodeUnderCursor->setSelected(true);
        return true;
    }
    if (selectedNode) {
        m_canvas->scene()->clearSelection();
        return true;
    }emit
    requestAddNode(scenePosition);
    qDebug() << "[GraphicsViewClickHandler] Request add node!";
    return true;
}
bool GraphicsViewClickHandler::handleRightClickNode(const QPoint &globalPosition, NodeItem *selectedNode)
{
    if (!selectedNode) return true;
    showActionMenuNode(globalPosition, selectedNode);
    return true;
}
void GraphicsViewClickHandler::showActionMenuNode(const QPoint &globalPosition, NodeItem *selectedNode)
{
    QMenu menu(m_canvas);
    QAction actionVirtual(QIcon(":resources/icons/trash.svg"), "Delete Node", &menu);
    connect(&actionVirtual, &QAction::triggered, this, [this, selectedNode]()
    {
        emit requestRemoveNode(selectedNode);
        qDebug() << "[GraphicsViewClickHandler] Request remove node!";
    });
    menu.addAction(&actionVirtual);

    QRect mask = menu.rect();
    mask.setSize(menu.sizeHint());

    QPainterPath path;
    path.addRoundedRect(mask, 6, 6);
    menu.setMask(path.toFillPolygon().toPolygon());

    menu.exec(globalPosition);
}
bool GraphicsViewClickHandler::handleLeftClickWayTool(NodeItem *nodeUnderCursor,
                                                      WayItem *wayUnderCursor,
                                                      WayItem *selectedWay)
{
    if (wayUnderCursor && !nodeUnderCursor) {
        m_canvas->scene()->clearSelection();
        wayUnderCursor->setSelected(true);
        return true;
    }
    if (!wayUnderCursor && !nodeUnderCursor) {
        m_canvas->scene()->clearSelection();
        return true;
    }
    if (!nodeUnderCursor) return true;
    if (!selectedWay) {
        nodeUnderCursor->setSelected(true);
        emit requestNewWay(nodeUnderCursor);
        return true;
    }
    nodeUnderCursor->setSelected(true);emit
    requestAddNodeToWay(selectedWay, nodeUnderCursor);
    return true;
}
bool GraphicsViewClickHandler::handleRightClickWayTool(const QPoint &globalPosition, WayItem *selectedWay)
{
    if (!selectedWay) return true;
    showActionMenuWay(globalPosition, selectedWay);
    return true;
}
bool GraphicsViewClickHandler::handleRightClickSelectTool(const QPoint &globalPosition,
                                                          WayItem *selectedPath,
                                                          LaneletItem *selectedLanelet) const
{
    if (selectedPath) {
        showMetadataMenuWay(globalPosition, selectedPath);
        return true;
    }
    if (selectedLanelet) {
        showMetadataMenuLanelet(globalPosition, selectedLanelet);
        return true;
    }
    return false;
}
void GraphicsViewClickHandler::showActionMenuWay(const QPoint &globalPosition, WayItem *selectedWay)
{
    QMenu menu(m_canvas);
    QAction actionVirtual(QIcon(":resources/icons/trash.svg"), "Delete Path", &menu);
    connect(&actionVirtual, &QAction::triggered, this, [this, selectedWay]()
    {
        emit requestRemoveWay(selectedWay);
        qDebug() << "[GraphicsViewClickHandler] Request remove path!";
    });
    menu.addAction(&actionVirtual);

    QRect mask = menu.rect();
    mask.setSize(menu.sizeHint());

    QPainterPath path;
    path.addRoundedRect(mask, 6, 6);
    menu.setMask(path.toFillPolygon().toPolygon());

    menu.exec(globalPosition);
}
void GraphicsViewClickHandler::showMetadataMenuLanelet(const QPoint &globalPosition,
                                                       LaneletItem *selectedLanelet) const
{
    QMenu menu(m_canvas);
    QAction actionSystem("System", &menu);
    actionSystem.setCheckable(true);
    actionSystem.setChecked(selectedLanelet->laneletType() == "system");
    connect(&actionSystem, &QAction::triggered, &actionSystem, [selectedLanelet]()
    {
        selectedLanelet->setLaneletType("system");
    });
    menu.addAction(&actionSystem);

    QAction actionScenario("Scenario", &menu);
    actionScenario.setCheckable(true);
    actionScenario.setChecked(selectedLanelet->laneletType() == "scenario");
    connect(&actionScenario, &QAction::triggered, &actionScenario, [selectedLanelet]()
    {
        selectedLanelet->setLaneletType("scenario");
    });
    menu.addAction(&actionScenario);

    QAction actionExit("Exit", &menu);
    actionExit.setCheckable(true);
    actionExit.setChecked(selectedLanelet->laneletType() == "exit_point");
    connect(&actionExit, &QAction::triggered, &actionExit, [selectedLanelet]()
    {
        selectedLanelet->setLaneletType("exit_point");
    });
    menu.addAction(&actionExit);

    QAction actionEntry("Entry", &menu);
    actionEntry.setCheckable(true);
    actionEntry.setChecked(selectedLanelet->laneletType() == "entry_point");
    connect(&actionEntry, &QAction::triggered, &actionEntry, [selectedLanelet]()
    {
        selectedLanelet->setLaneletType("entry_point");
    });
    menu.addAction(&actionEntry);

    QAction actionEntryOrExit("Entry or Exit", &menu);
    actionEntryOrExit.setCheckable(true);
    actionEntryOrExit.setChecked(selectedLanelet->laneletType() == "exit_entry_point");
    connect(&actionEntryOrExit, &QAction::triggered, &actionEntryOrExit, [selectedLanelet]()
    {
        selectedLanelet->setLaneletType("exit_entry_point");
    });
    menu.addAction(&actionEntryOrExit);

    QRect mask = menu.rect();
    mask.setSize(menu.sizeHint());

    QPainterPath path;
    path.addRoundedRect(mask, 6, 6);
    menu.setMask(path.toFillPolygon().toPolygon());

    menu.exec(globalPosition);
}
void GraphicsViewClickHandler::showMetadataMenuWay(const QPoint &globalPosition, WayItem *selectedWay) const
{
    QMenu menu(m_canvas);
    QAction actionVirtual("Virtual", &menu);
    actionVirtual.setCheckable(true);
    actionVirtual.setChecked(selectedWay->wayType() == "virtual");
    connect(&actionVirtual, &QAction::triggered, &actionVirtual, [selectedWay]()
    {
        selectedWay->setWayType("virtual");
    });
    menu.addAction(&actionVirtual);

    QAction actionSolid("Solid", &menu);
    actionSolid.setCheckable(true);
    actionSolid.setChecked(selectedWay->wayType() == "solid");
    connect(&actionSolid, &QAction::triggered, &actionSolid, [selectedWay]()
    {
        selectedWay->setWayType("solid");
    });
    menu.addAction(&actionSolid);

    QAction actionDashed("Dashed", &menu);
    actionDashed.setCheckable(true);
    actionDashed.setChecked(selectedWay->wayType() == "dashed");
    connect(&actionDashed, &QAction::triggered, &actionDashed, [selectedWay]()
    {
        selectedWay->setWayType("dashed");
    });
    menu.addAction(&actionDashed);

    QRect mask = menu.rect();
    mask.setSize(menu.sizeHint());

    QPainterPath path;
    path.addRoundedRect(mask, 6, 6);
    menu.setMask(path.toFillPolygon().toPolygon());

    menu.exec(globalPosition);
}
EditorMode GraphicsViewClickHandler::editorMode() const
{
    return this->m_mode;
}
void GraphicsViewClickHandler::setEditorMode(EditorMode mode)
{
    this->m_mode = mode;
    this->m_canvas->scene()->clearSelection();
    emit this->editorModeChanged(mode);
}
LaneletItem *GraphicsViewClickHandler::selectedLanelet() const
{
    return m_selectedLanelet;
}
WayItem *GraphicsViewClickHandler::selectedWay() const
{
    return m_selectedWay;
}
NodeItem *GraphicsViewClickHandler::selectedNode() const
{
    return m_selectedNode;
}
void GraphicsViewClickHandler::handleSceneSelectionChange()
{
    auto itemsSelected = this->m_canvas->scene()->selectedItems();
    auto selectedNode = GetFirstItemInList<NodeItem *, QGraphicsItem *>(itemsSelected);
    auto selectedWay = GetFirstItemInList<WayItem *, QGraphicsItem *>(itemsSelected);
    auto selectedLanelet = GetFirstItemInList<LaneletItem *, QGraphicsItem *>(itemsSelected);

    setSelectedNode(selectedNode);
    setSelectedPath(selectedWay);
    setSelectedLanelet(selectedLanelet);
}
void GraphicsViewClickHandler::setSelectedLanelet(LaneletItem *selectedLanelet)
{
    if (selectedLanelet == this->m_selectedLanelet) return;
    this->m_selectedLanelet = selectedLanelet;
    if (selectedLanelet) selectedLanelet->setSelected(true);
    emit this->selectedLaneletChanged(selectedLanelet);
    qDebug() << "[GraphicsViewClickHandler] New lanelet selected!";
}
void GraphicsViewClickHandler::setSelectedPath(WayItem *selectedWay)
{
    if (selectedWay == this->m_selectedWay) return;
    this->m_selectedWay = selectedWay;
    if (selectedWay) selectedWay->setSelected(true);
    emit this->selectedPathChanged(selectedWay);
    qDebug() << "[GraphicsViewClickHandler] New way selected!";
}
void GraphicsViewClickHandler::setSelectedNode(NodeItem *selectedNode)
{
    if (selectedNode == this->m_selectedNode) return;
    this->m_selectedNode = selectedNode;
    if (selectedNode) selectedNode->setSelected(true);
    emit this->selectedNodeChanged(selectedNode);
    qDebug() << "[GraphicsViewClickHandler] New node selected!";
}

void GraphicsViewClickHandler::actionToRemoveNodeTriggered()
{
    if (this->m_selectedNode)emit requestRemoveNode(this->m_selectedNode);
}
void GraphicsViewClickHandler::actionToRemoveWayTriggered()
{
    if (this->m_selectedWay) emit requestRemoveWay(this->m_selectedWay);
}
void GraphicsViewClickHandler::actionToRemoveLaneletTriggered()
{
    if (this->m_selectedLanelet) emit requestRemoveLanelet(this->m_selectedLanelet);
}
void GraphicsViewClickHandler::actionToFlipLaneletTriggered()
{
    if (this->m_selectedLanelet) this->m_selectedLanelet->flipDirection();
}

