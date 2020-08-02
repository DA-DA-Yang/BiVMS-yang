/***************************************************************************
**  BiVMS 2020                                                            **
**  record project infomation                                             **

****************************************************************************
**           Author: CHENG HUANG                                          **
**          Contact: huangcheng@shu.edu.cn                                **
**             Date: 27.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/

#pragma once
#include <QObject>
#include <QDialog>
#include <QFileDialog>
#include <QFile>     
#include <QtXml\QtXml>  
#include <QtXml\QDomDocument> 
#include "MyMessage.h"
#include "History.h"

class Project : public QObject
{
	Q_OBJECT

public:
	Project(QObject *parent);
	~Project();
	
public:
	//判断项目是否为空
	inline bool isEmpty() noexcept {
		if (m_projectName.isEmpty() || m_filePath.isEmpty())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	//设定项目名称
	inline void set_projecName(QString projectName) noexcept {
		m_projectName = projectName;
	}
	//设定测量人员
	inline void set_surveyor(QString surveyor) noexcept {
		m_surveyor = surveyor;
	}
	//设定测量地点
	inline void set_location(QString location) noexcept {
		m_location = location;
	}
	//设定测量时间
	inline void set_time(QString time) noexcept {
		m_time = time;
	}
	//设定文件路径
	inline void set_filePath(QString filePath) noexcept {
		m_filePath = filePath;
	}
	//返回项目名称
	inline QString get_projecName() noexcept {
		return m_projectName;
	}
	//返回测量人员
	inline QString get_surveyor() noexcept {
		return m_surveyor;
	}
	//返回测量地点
	inline QString get_location() noexcept {
		return m_location;
	}
	//返回测量时间
	inline QString get_time() noexcept {
		return m_time;
	}
	//返回文件路径
	inline QString get_filePath() noexcept {
		return m_filePath;
	}
	//返回标定参数文件夹路径
	inline QString get_clibParameterPath() noexcept {
		return m_clibParameterPath;
	}
	//返回数据文件夹路径
	inline QString get_dataPath() noexcept {
		return m_dataPath;
	}
	//返回图像文件夹路径
	inline QString get_imagePath() noexcept {
		return m_imagePath;
	}

	//生成文件路径,创建文件夹
	bool generate_Folder();	
	//写项目XML文件，保存为vms格式			
	void write_XMl();	
	//读取XML的信息
	void read_XML();
	//项目模式-3D实时测量或者后处理
	enum Mode{REALTIME, analysis};


private:
	QString m_projectName{};										//项目名称
	QString m_surveyor{};											//测量人员
	QString m_location{};											//测量地点
	QString m_time{};												//测量时间
	QString m_filePath{};											//文件路径
	QString m_clibParameterPath{};								//标定参数文件夹路径
	QString m_dataPath{};											//数据文件夹路径
	QString m_imagePath{};										//图像文件夹路径
	QDomDocument m_doc;											//写XML		
};
