#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui>
#include <QtWidgets/QWidget>
#include "ui_mainwidget.h"

class ViewerWidget;
class EdiitWidget;
class MainWidget : public QWidget
{
	Q_OBJECT

public:
	MainWidget(QWidget *parent = 0);

public Q_SLOTS:
	void update(bool clicked);

private:
	Ui::MainWidgetClass ui;

private:
	ViewerWidget* m_viewer;
	EdiitWidget* m_editWidget;
};

#endif // MAINWIDGET_H
