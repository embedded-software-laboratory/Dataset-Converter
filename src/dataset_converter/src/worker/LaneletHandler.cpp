#include "LaneletHandler.h"

#include <QFileInfo>
#include <QDebug>
#include <QtSvg/QSvgGenerator>
#include <QPainter>
#include <QGraphicsScene>

#include <lanelet2_projection/CPM.h>
#include <lanelet2_io/Io.h>

LaneletHandler::LaneletHandler(QObject *parent)
    : QObject(parent)
{}

void LaneletHandler::parseLanelet(QString laneletFileName, qreal scaleFactor)
{
    this->lanelet_file_name_ = std::move(laneletFileName);
    qInfo("Load lanelet File: %s", qUtf8Printable(this->lanelet_file_name_));

    emit progress(0, 5);

    try {
        lanelet::projection::CpmProjector projector(lanelet::Origin({0.0, 0.0}));
        this->m_map = lanelet::load(lanelet_file_name_.toStdString(), projector);
    }
    catch (const std::exception &e) {
        emit error(e.what());
        return;
    }
    emit progress(1, 5);

    std::map<long, NodeItem *> nodeIdMap;
    std::map<long, WayItem *> wayIdMap;

    // Parse nodes and push to visualisation
    long id = 0;
    for (const auto &node : this->m_map->pointLayer) {
        auto item = new NodeItem();
        item->setPos({node.x() * scaleFactor * 18, (4 - node.y()) * scaleFactor * 18});
        item->setPos(item->pos());
        this->nodes_.push_back(item);
        nodeIdMap[node.id()] = item;
        emit nodeAdded(item);
        id++;
    }
    emit progress(3, 5);

    // Parse ways
    for (const auto &way : this->m_map->lineStringLayer) {
        auto item = new WayItem();
        QString internalType = "unknown";
        if (way.hasAttribute("type")) {
            std::string laneletType = way.attribute("type").value();
            if (laneletType == "virtual") {
                internalType = "virtual";
            }
            else if (laneletType == "line_thin" || "line_thick") {
                if (way.hasAttribute("subtype")) {
                    std::string laneletSubtype = way.attribute("subtype").value();
                    if (laneletSubtype == "dashed") {
                        internalType = "dashed";
                    }
                    else if (laneletSubtype == "solid") {
                        internalType = "solid";
                    }
                }
            }
        }
        item->setWayType(internalType);
        this->ways_.push_back(item);
        wayIdMap[way.id()] = item;
        emit wayAdded(item);

        for (const auto &node : way) {
            item->addNode(nodeIdMap[node.id()]);
        }
        id++;
    }
    emit progress(4, 5);

    // Parse lanelets
    for (const auto &lanelet : this->m_map->laneletLayer) {
        auto item = new LaneletItem();
        item->setLaneletType(lanelet.attribute("cpm_type").value().c_str());
        item->setRightWayItem(wayIdMap[lanelet.rightBound().id()]);
        item->setLeftWayItem(wayIdMap[lanelet.leftBound().id()]);
        this->lanelets_.push_back(item);
        emit laneletAdded(item);
        id++;
    }

    emit progress(5, 5);

    emit loaded();
}

void LaneletHandler::writeLanelet(QString laneletFileName, qreal scaleFactor)
{
    this->lanelet_file_name_ = std::move(laneletFileName);
    qInfo("Write lanelet File: %s", qUtf8Printable(this->lanelet_file_name_));

    std::map<NodeItem *, lanelet::Point3d> nodeIdMap;
    std::map<WayItem *, lanelet::LineString3d> wayIdMap;
    this->m_map = std::make_shared<lanelet::LaneletMap>();
    emit progress(0, 3);
    long id = 1;
    for (auto node : nodes_) {

        auto backendNode =
            lanelet::Point3d(id,
                             node->pos().x() / (scaleFactor * 18.0),
                             4 - (node->pos().y() / (scaleFactor * 18.0)),
                             0);
        this->m_map->add(backendNode);
        nodeIdMap[node] = backendNode;

        id++;
    }
    emit progress(1, 4);
    for (auto way : ways_) {
        if (way->nodes().size() < 2) continue;
        auto backendWay = lanelet::LineString3d(id);
        QString internalWay = way->wayType();
        // Equivalent for both unknown and virtual
        if (internalWay == "solid") {
            backendWay.setAttribute("type", "line_thin");
            backendWay.setAttribute("subtype", "solid");
        }
        else if (internalWay == "dashed") {
            backendWay.setAttribute("type", "line_thin");
            backendWay.setAttribute("subtype", "dashed");
        }
        else if (internalWay == "virtual") {
            backendWay.setAttribute("type", "virtual");
        }

        for (auto node : way->nodes()) {
            backendWay.push_back(nodeIdMap[node]);
        }
        this->m_map->add(backendWay);
        wayIdMap[way] = backendWay;
        id++;
    }
    emit progress(2, 4);
    for (auto lanelet : lanelets_) {
        if (!lanelet->leftWayItem() || !lanelet->rightWayItem()) continue;
        auto backendLanelet = lanelet::Lanelet(id);
        backendLanelet.setAttribute("cpm_type", lanelet->laneletType().toStdString());
        backendLanelet.setAttribute("location", "urban");
        backendLanelet.setAttribute("name", "CPM-Lab");
        backendLanelet.setAttribute("one_way", "yes");
        backendLanelet.setAttribute("region", "de");
        backendLanelet.setAttribute("subtype", "road");
        backendLanelet.setAttribute(lanelet::AttributeName::Subtype, lanelet::AttributeValueString::Road);
        backendLanelet.setLeftBound(wayIdMap[lanelet->leftWayItem()]);
        backendLanelet.setRightBound(wayIdMap[lanelet->rightWayItem()]);
        this->m_map->add(backendLanelet);
        id++;
    }
    emit progress(3, 4);
    lanelet::projection::CpmProjector projector(lanelet::Origin({0.0, 0.0}));
    lanelet::write(lanelet_file_name_.toStdString(), *this->m_map, projector);
    emit progress(4, 4);
    emit stored();
}
void LaneletHandler::addNode(QPointF position)
{
    auto item = new NodeItem();
    item->setPos(position);
    this->nodes_.push_back(item);
    emit nodeAdded(item);
    qDebug() << "[LaneletParser] Node added!";
}
void LaneletHandler::removeNode(NodeItem *node)
{
    int index = -1;
    int counter = -1;
    for (auto element : this->nodes_) {
        counter++;
        if (element != node) continue;
        index = counter;
    }
    this->nodes_.removeAt(index);
    // Free Memory
    delete node;
    qDebug() << "[LaneletParser] Node removed!";
}
void LaneletHandler::addWayWithNode(NodeItem *node)
{
    auto path = new WayItem();
    path->addNode(node);
    this->ways_.push_back(path);
    emit wayAdded(path);
    qDebug() << "[LaneletParser] Path added!";
}
void LaneletHandler::removeWay(WayItem *way)
{
    if (!way->lanelets().empty()) {
        for (auto lanelet : way->lanelets()) {
            auto laneletWay = lanelet->leftWayItem();
            if (laneletWay) laneletWay->removeFromLanelet(lanelet);
            laneletWay = lanelet->rightWayItem();
            if (laneletWay) laneletWay->removeFromLanelet(lanelet);
            this->removeLanelet(lanelet);
        }
    }
    int index = -1;
    int counter = -1;
    for (auto element : this->ways_) {
        counter++;
        if (element != way) continue;
        index = counter;
    }
    this->ways_.removeAt(index);
    // Free Memory
    delete way;
    qDebug() << "[LaneletParser] Path removed!";
}
void LaneletHandler::addLaneletWithPath(WayItem *way)
{
    auto lanelet = new LaneletItem();
    lanelet->setLeftWayItem(way);
    this->lanelets_.push_back(lanelet);
    emit laneletAdded(lanelet);
    qDebug() << "[LaneletParser] Lanelet added!";
}
void LaneletHandler::removeLanelet(LaneletItem *lanelet)
{
    int index = -1;
    int counter = -1;
    for (auto element : this->lanelets_) {
        counter++;
        if (element != lanelet) continue;
        index = counter;
    }
    this->lanelets_.removeAt(index);
    // Free Memory
    delete lanelet;
    qDebug() << "[LaneletParser] Lanelet removed!";
}
void LaneletHandler::writeLaneletImage(const QString &svgFile, QGraphicsScene *scene)
{
    QSvgGenerator svgGen;

    svgGen.setFileName(svgFile);
    svgGen.setSize(QSize(1190, 1057));
    svgGen.setViewBox(QRect(0, 0, 1190, 1057));
    svgGen.setTitle(tr("SVG Generator Example Drawing"));
    svgGen.setDescription(tr("An SVG drawing created by the SVG Generator Example provided with Qt."));

    QPainter painter(&svgGen);
    scene->render(&painter);
}
