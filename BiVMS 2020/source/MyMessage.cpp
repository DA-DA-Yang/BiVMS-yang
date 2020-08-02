#include "MyMessage.h"

MyMessage::MyMessage(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

MyMessage::MyMessage(const char* message,bool autoClose,QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	if (autoClose)
	{
		ui.pushButton_OK->setVisible(false);
		ui.pushButton_CANCLE->setVisible(false);
		ui.label_message->setText(QString::fromLocal8Bit(message));
		QTimer::singleShot(1000, this, SLOT(accept()));
		this->exec();
	}
	else
	{
		ui.label_message->setText(QString::fromLocal8Bit(message));
		connect(ui.pushButton_OK, &QPushButton::clicked, this, &MyMessage::accept);
		connect(ui.pushButton_CANCLE, &QPushButton::clicked, this, &MyMessage::reject);
	}
	
}
MyMessage::~MyMessage()
{
}

