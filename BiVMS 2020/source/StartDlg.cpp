#include "StartDlg.h"

StartDlg::StartDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
}

StartDlg::~StartDlg()
{
}
