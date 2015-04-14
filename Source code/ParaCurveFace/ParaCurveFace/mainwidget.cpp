#include "mainwidget.h"
#include "ediitwidget.h"
#include "viewerwidget.h"
#include <QHBoxLayout>

MainWidget::MainWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowTitle(tr("Para-curve Face"));

	//初始化左侧编辑界面
	m_editWidget = new EdiitWidget(this);
	m_editWidget->setFixedWidth(m_editWidget->width());
	m_editWidget->setMinimumHeight(m_editWidget->height());

	//初始化右侧3DView
	m_viewer = new ViewerWidget();

	//加入到主窗口中
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(m_editWidget);
	mainLayout->addWidget(m_viewer);

	//更新3DView
	update(false);
}

void MainWidget::update(bool clicked)
{
	(void)clicked;

	//取得Z=kX^2中的k值
	float value = 1.0f;
	if (!m_editWidget->getValue(value))
		return;

	if (m_viewer)
		m_viewer->setValue(value);
}