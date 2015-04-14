#ifndef EDIITWIDGET_H
#define EDIITWIDGET_H

#include <QWidget>
#include "ui_ediitwidget.h"

class EdiitWidget : public QWidget
{
	Q_OBJECT

public:
	EdiitWidget(QWidget *parent = 0);
	bool getValue(float& value);

private:
	Ui::EdiitWidget ui;
};

#endif // EDIITWIDGET_H
