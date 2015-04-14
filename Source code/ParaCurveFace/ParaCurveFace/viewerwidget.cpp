#include <QGridLayout>
#include <osg/Material>
#include <osg/LightModel>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include "Paracurve.h"
#include "ViewerWidget.h"

ViewerWidget::ViewerWidget()
	: QWidget()
{
#if QT_VERSION >= 0x050000
	// Qt5 is currently crashing and reporting "Cannot make QOpenGLContext current in a different thread" when the viewer is run multi-threaded, this is regression from Qt4
	osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::ViewerBase::SingleThreaded;
#else
	osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::ViewerBase::CullDrawThreadPerContext;
#endif
	setThreadingModel(threadingModel);

	// disable the default setting of viewer.done() by pressing Escape.
	setKeyEventSetsDone(0);

	m_axis = generateAxis();//Éú³É×ø±êÖá
	QGridLayout* grid = new QGridLayout;
	grid->addWidget(addViewWidget(createGraphicsWindow(0, 0, 100, 100)), 0, 0);
	setLayout(grid);
	setGeometry(100, 100, 800, 600);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
	_timer.start(10);
}

QWidget* ViewerWidget::addViewWidget(osgQt::GraphicsWindowQt* gw)
{
	m_view = new osgViewer::View;
	addView(m_view);

	const osg::GraphicsContext::Traits* traits = gw->getTraits();

	osg::Camera* camera = m_view->getCamera();
	camera->setGraphicsContext(gw);
	camera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0f, 10000.0f);

	m_view->addEventHandler(new osgViewer::StatsHandler);
	m_view->setCameraManipulator(new osgGA::TrackballManipulator);

	return gw->getGLWidget();
}

osgQt::GraphicsWindowQt* ViewerWidget::createGraphicsWindow(int x, int y, int w, int h, const std::string& name, bool windowDecoration)
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowName = name;
	traits->windowDecoration = windowDecoration;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = ds->getNumMultiSamples();

	return new osgQt::GraphicsWindowQt(traits.get());
}

void ViewerWidget::paintEvent(QPaintEvent* event)
{
	(void)event;
	frame();
}

void ViewerWidget::setValue(float value)
{
	redraw(value);
}

void ViewerWidget::redraw(float value)
{
	osg::Group* root = new osg::Group();
	root->addChild(m_axis);

	osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> norms = new osg::Vec3Array;
	if (!Paracurve::sampling(value, points.get(), norms.get()))
		return;

	int numPoints = points->size();
	if (numPoints != norms->size())
		return;

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::Geometry* geometry = new osg::Geometry;

	geometry->setVertexArray(points);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, numPoints));

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0, 1.0, 0, 1.0));
	geometry->setColorArray(colorArray.get());
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	geometry->setNormalArray(norms);
	geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	osg::StateSet* stateSet = geometry->getOrCreateStateSet();
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	geode->addDrawable(geometry);

	osg::ref_ptr<osg::Group> group = new osg::Group;
	group->addChild(geode);
	root->addChild(group);

	osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
	lightModel->setAmbientIntensity(osg::Vec4(0.5, 0.5, 0.5, 0.5));
	lightModel->setTwoSided(true);
	root->getOrCreateStateSet()->setAttribute(lightModel, osg::StateAttribute::ON);

	m_view->setSceneData(root);
}

osg::Group* ViewerWidget::generateAxis()
{
	osg::Group* group = new osg::Group();
	osg::ref_ptr<osg::Vec3Array> pointArray = new osg::Vec3Array;
	pointArray->push_back(osg::Vec3(0, 0, 0));
	pointArray->push_back(osg::Vec3(1, 0, 0));
	pointArray->push_back(osg::Vec3(0, 1, 0));
	pointArray->push_back(osg::Vec3(0, 0, 1));

	double size = 5;

	osg::Vec3 base = pointArray->front();
	for (osg::Vec3Array::iterator itr = pointArray->begin() + 1; itr != pointArray->end(); ++itr)
	{
		std::size_t index = std::distance(pointArray->begin(), itr);//1=x=red, 2=y=green, 3=z=blue
		osg::Vec4 color(static_cast<float>(index == 1),
			static_cast<float>(index == 2),
			static_cast<float>(index == 3), 1.0f);

		osg::Quat rotate;
		rotate.makeRotate(osg::Vec3(0.0f, 0.0f, 1.0f), *itr);

		osg::ref_ptr<osg::Geode> geode = new osg::Geode;

		osg::Material* material = dynamic_cast<osg::Material*>(geode->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
		if (!material)
		{
			material = new osg::Material;
			material->setDataVariance(osg::Object::DYNAMIC);
			geode->getOrCreateStateSet()->setAttribute(material);
		}
		material->setDiffuse(osg::Material::FRONT_AND_BACK, color);

#ifndef OSG_GLES2_AVAILABLE
		geode->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
#endif

		osg::ref_ptr<osg::Cone> cone = new osg::Cone((*itr) * size, size * 0.02, size * 0.05);
		cone->setRotation(rotate);
		osg::ref_ptr<osg::ShapeDrawable> coneDrawable = new osg::ShapeDrawable(cone.get());
		coneDrawable->setColor(color);
		geode->addDrawable(coneDrawable);

		osg::ref_ptr<osg::Cylinder> cylinder = new osg::Cylinder(((*itr) * size) / 2.0, size * 0.006, size);
		cylinder->setRotation(rotate);
		osg::ref_ptr<osg::ShapeDrawable> cylinderDrawable = new osg::ShapeDrawable(cylinder.get());
		cylinderDrawable->setColor(color);
		geode->addDrawable(cylinderDrawable);

		osg::ref_ptr<osg::MatrixTransform> matrixTransform = new osg::MatrixTransform;
		osg::Matrix m = matrixTransform->getMatrix();
		m.makeTranslate(base);
		matrixTransform->setMatrix(m);
		matrixTransform->addChild(geode.get());
		group->addChild(matrixTransform);
	}

	return group;
}
