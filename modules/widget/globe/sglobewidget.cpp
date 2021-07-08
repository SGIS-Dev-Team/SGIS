
#include "sglobewidget.h"


#include "osgGA/TrackballManipulator"
#include "osg/ShapeDrawable"
#include "osg/Material"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include "osgEarth/Map"
#include "osgGA/EventHandler"
#include "QOpenGLWidget"
#include "osgGA/StateSetManipulator"
#include "osgEarthUtil/AutoClipPlaneHandler"
#include "osgEarth/Viewpoint"
#include "osgViewer/ViewerEventHandlers"
#include <osgDB/ReadFile>
#include <osgEarth/MapNode>
#include <osgEarthDrivers/cache_filesystem/FileSystemCache>
#include <osgEarth/ImageLayer>
#include <osgEarthFeatures/Feature>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#include <osgEarth/ModelLayer>
#include <osgEarthDrivers/bing/BingOptions>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarthAnnotation/CircleNode>

SGlobeWidget::SGlobeWidget(QWidget* parent, Qt::WindowFlags f): QOpenGLWidget(parent, f)
{
    _initialize();
}

SGlobeWidget::~SGlobeWidget()
{

}

void SGlobeWidget::paintGL()
{
    mpViewer->frame();
}

void SGlobeWidget::resizeGL(int width, int height)
{
    mpGraphicsWindow->getEventQueue()->windowResize(this->x() * mdScaleX, this->y() * mdScaleY, width * mdScaleX, height * mdScaleY);
    mpGraphicsWindow->resized(this->x()*mdScaleX, this->y() * mdScaleY, width * mdScaleX, height * mdScaleY);
    osg::Camera* camera = mpViewer->getCamera();
    camera->setViewport(0, 0, this->width() * mdScaleX, this->height()* mdScaleY);
}

void SGlobeWidget::initializeGL()
{
//    osg::Geode* geode = dynamic_cast<osg::Geode*>(mpViewer->getSceneData());
//    osg::StateSet* stateSet = geode->getOrCreateStateSet();
//    osg::Material* material = new osg::Material;
//    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
//    stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
//    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
}

void SGlobeWidget::mouseMoveEvent(QMouseEvent* event)
{
    mpGraphicsWindow->getEventQueue()->mouseMotion(event->x() * mdScaleX, event->y() * mdScaleY);
}

void SGlobeWidget::mousePressEvent(QMouseEvent* event)
{
    unsigned int button = 0;
    switch (event->button())
    {
    case Qt::LeftButton:
        button = 1;
        break;
    case Qt::MiddleButton:
        button = 2;
        break;
    case Qt::RightButton:
        button = 3;
        break;
    default:
        break;
    }
    mpGraphicsWindow->getEventQueue()->mouseButtonPress(event->x() * mdScaleX, event->y()*mdScaleY, button);
}

void SGlobeWidget::mouseReleaseEvent(QMouseEvent* event)
{
    unsigned int button = 0;
    switch (event->button())
    {
    case Qt::LeftButton:
        button = 1;
        break;
    case Qt::MiddleButton:
        button = 2;
        break;
    case Qt::RightButton:
        button = 3;
        break;
    default:
        break;
    }
    mpGraphicsWindow->getEventQueue()->mouseButtonRelease(event->x()*mdScaleX, event->y()*mdScaleY, button);
}

void SGlobeWidget::wheelEvent(QWheelEvent* event)
{
    int delta = event->delta();
    osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?
                                                     osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;
    mpGraphicsWindow->getEventQueue()->mouseScroll(motion);
}

void SGlobeWidget::_initialize()
{
    //启动鼠标跟踪
    this->setMouseTracking(true);

    mdScaleX = mdScaleY = 1;
    mpGraphicsWindow = new osgViewer::GraphicsWindowEmbedded(this->x(), this->y(), this->width(), this->height());
    mpViewer = new osgViewer::Viewer;

    osgEarth::ProfileOptions profileOpts;

    //地图配置：设置缓存目录
    osgEarth::Drivers::FileSystemCacheOptions cacheOpts;
    QString cacheDir = "D:/OsgEarth_Cache/";
    cacheOpts.rootPath() = cacheDir.toStdString();

    //地图选项
    osgEarth::MapOptions mapOpts;
    mapOpts.cache() = cacheOpts;
    mapOpts.profile() = profileOpts;

    //创建地图节点
    osg::ref_ptr<osgEarth::Map> map = new osgEarth::Map(mapOpts);
    osg::ref_ptr<osgEarth::MapNode> mapNode = new osgEarth::MapNode(map);

    //读取世界地图贴图并添加图层
    osgEarth::Drivers::GDALOptions gdal;
    gdal.url() = "D:/RS_Files/world.tif";
    osg::ref_ptr<osgEarth::ImageLayer> layer = new osgEarth::ImageLayer("BlueMarble", gdal);
    map->addLayer(layer);

//    osgEarth::Drivers::BingOptions bing;
//    bing.apiKey() = "BFWXVZlX1xqkqiKfU9Hw~I9tzaIT_-0QA8gUzddAcCg~ArueLfchkgn_aQKoae45kpwUI0E1h8X1mMPhDogt4mPxaiCnEBnT2KeDT5LT6ZQk";
//    osg::ref_ptr<osgEarth::ImageLayer> bingLayer = new osgEarth::ImageLayer("Bing Online Map", bing);
//    map->addLayer(bingLayer);

    //设置相机
    osg::Camera* pCamera = new osg::Camera;
    pCamera->setViewport(0, 0, this->width(), this->height());
    pCamera->setGraphicsContext(mpGraphicsWindow);
    mpViewer->setCamera(pCamera);
    mpViewer->setSceneData(mapNode);
    mpViewer->setCameraManipulator(new osgEarth::Util::EarthManipulator());
    mpViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
    mpViewer->realize();

}

void SGlobeWidget::_initializeConnections()
{

}

bool SGlobeWidget::event(QEvent* event)
{
    bool handled = QOpenGLWidget::event(event);
    this->update();
    return handled;
}

void SGlobeWidget::drawLine(const std::vector<double> coors)
{
    auto mapNode = dynamic_cast<osgEarth::MapNode*>(mpViewer->getSceneData());
    const osgEarth::SpatialReference* geoSRS = mapNode->getMapSRS()->getGeographicSRS();

    osg::Group* pAnnoGroup = new osg::Group();
    mapNode->addChild(pAnnoGroup);

    osgEarth::Util::Geometry* geom = new osgEarth::Util::Polygon();
    geom->push_back(-160., -30.);
    geom->push_back(150., -20.);
    geom->push_back(160., -45.);
    geom->push_back(-150., -40.);
    osgEarth::Util::Style geomStyle;

    osgEarth::Features::Feature* feature = new osgEarth::Features::Feature(geom, geoSRS);
    feature->geoInterp() = osgEarth::GEOINTERP_RHUMB_LINE;

    geomStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Symbology::Color::Lime;
    geomStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 3.0f;
    geomStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellationSize()->set(75000, osgEarth::Symbology::Units::METERS);
    geomStyle.getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset()->enabled() = true;

    osgEarth::Annotation::FeatureNode* gnode = new osgEarth::Annotation::FeatureNode(feature, geomStyle);
    pAnnoGroup->addChild(gnode);

    osgEarth::Util::Style circleStyle;
    circleStyle.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(osgEarth::Symbology::Color::Cyan, 0.5);
    circleStyle.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
    circleStyle.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;

    osgEarth::Annotation::CircleNode* circle = new osgEarth::Annotation::CircleNode();
    circle->set(
        osgEarth::GeoPoint(geoSRS, -90.25, 29.98, 1000., osgEarth::ALTMODE_RELATIVE),
        osgEarth::Distance(300, osgEarth::Units::KILOMETERS),
        circleStyle,
        osgEarth::Angle(-45.0, osgEarth::Units::DEGREES),
        osgEarth::Angle(45.0, osgEarth::Units::DEGREES),
        true);

    pAnnoGroup->addChild(circle);
}

