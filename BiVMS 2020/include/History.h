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
	//保存项目创建历史信息
	void write_History(QString projectname, QString filepath, QString time);
	//得到项目的历史信息，用于显示
	void get_History(QVector<QString> &projectname, QVector<QString> &projectpath, QVector<QString> &createtime);

	//删除某一项的历史信息
	void delete_History(int index);
private:
	QVector<QString> m_projectname;														//保存历史创建项目名称
	QVector<QString> m_projectpath;														//保存历史创建项目路径
	QVector<QString> m_createtime;														//保存历史创建项目时间
	QString strPath = qApp->applicationDirPath() + "/BiVMSinfo.ini";					//历史信息保存位置（debug下）
	QSettings *m_ini = new QSettings(strPath, QSettings::IniFormat);					//ini成员变量，用来写ini文件
	QDir m_dir;																			//Dir成员变量，用来生成文件夹

private:
	//读取项目的历史信息
	void _read_History();
	
	

};
