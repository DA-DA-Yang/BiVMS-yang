/***************************************************************************
**  BiVMS 2020                                                            **
**  record history infomation											  **

****************************************************************************
**           Author: CHENG HUANG                                          **
**          Contact: huangcheng@shu.edu.cn                                **
**             Date: 27.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/

#pragma once
#include <QObject>
#include <QSettings>
#include <QWidget>
#include <QDir>
#include <Qapplication>

class History : public QObject
{
	Q_OBJECT
	
public:
	History(QObject *parent);
	~History();

public:
	//������Ŀ������ʷ��Ϣ
	void write_History(QString projectname, QString filepath, QString time);
	//�õ���Ŀ����ʷ��Ϣ��������ʾ
	void get_History(QVector<QString> &projectname, QVector<QString> &projectpath, QVector<QString> &createtime);

	//ɾ��ĳһ�����ʷ��Ϣ
	void delete_History(int index);
private:
	QVector<QString> m_projectname;														//������ʷ������Ŀ����
	QVector<QString> m_projectpath;														//������ʷ������Ŀ·��
	QVector<QString> m_createtime;														//������ʷ������Ŀʱ��
	QString strPath = qApp->applicationDirPath() + "/BiVMSinfo.ini";					//��ʷ��Ϣ����λ�ã�debug�£�
	QSettings *m_ini = new QSettings(strPath, QSettings::IniFormat);					//ini��Ա����������дini�ļ�
	QDir m_dir;																			//Dir��Ա���������������ļ���

private:
	//��ȡ��Ŀ����ʷ��Ϣ
	void _read_History();
	
	

};
