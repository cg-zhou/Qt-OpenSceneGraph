#include "ediitwidget.h"
#include <QMessageBox>

EdiitWidget::EdiitWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.lineEdit->setText("1");
	connect(ui.pushButton, SIGNAL(clicked(bool)), parent, SLOT(update(bool)));
}

bool EdiitWidget::getValue(float& value)
{
	QString str = ui.lineEdit->text();
	bool isValid = false;
	value = str.toFloat(&isValid);
	if (!isValid)
	{
		QMessageBox::warning(NULL, "Warning", "It's not a valid number, please input again.");
		return false;
	}

	return true;
}
