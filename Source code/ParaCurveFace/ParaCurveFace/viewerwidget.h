#pragma once

#include <QTimer>
#include <QApplication>
#include <osgViewer/CompositeViewer>
#include <osgQt/GraphicsWindowQt>

class __declspec(dllexport) ViewerWidget : public QWidget, public osgViewer::CompositeViewer
{
public:
	ViewerWidget();

	QWidget* addViewWidget(osgQt::GraphicsWindowQt* gw);

	osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h, const std::string& name = "", bool windowDecoration = false);

	virtual void paintEvent(QPaintEvent* event);

	virtual osg::Group* generateAxis();//生成坐标轴节点

	virtual void setValue(float value);//设置Z=kX^2中的k值

protected:
	virtual void redraw(float value);//根据Z=kX^2中的k值重新采样并显示

	QTimer _timer;
	osg::ref_ptr<osg::Group> m_axis;
	osgViewer::View* m_view;
};