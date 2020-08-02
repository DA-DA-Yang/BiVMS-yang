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
	//�ж���Ŀ�Ƿ�Ϊ��
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
	//�趨��Ŀ����
	inline void set_projecName(QString projectName) noexcept {
		m_projectName = projectName;
	}
	//�趨������Ա
	inline void set_surveyor(QString surveyor) noexcept {
		m_surveyor = surveyor;
	}
	//�趨�����ص�
	inline void set_location(QString location) noexcept {
		m_location = location;
	}
	//�趨����ʱ��
	inline void set_time(QString time) noexcept {
		m_time = time;
	}
	//�趨�ļ�·��
	inline void set_filePath(QString filePath) noexcept {
		m_filePath = filePath;
	}
	//������Ŀ����
	inline QString get_projecName() noexcept {
		return m_projectName;
	}
	//���ز�����Ա
	inline QString get_surveyor() noexcept {
		return m_surveyor;
	}
	//���ز����ص�
	inline QString get_location() noexcept {
		return m_location;
	}
	//���ز���ʱ��
	inline QString get_time() noexcept {
		return m_time;
	}
	//�����ļ�·��
	inline QString get_filePath() noexcept {
		return m_filePath;
	}
	//���ر궨�����ļ���·��
	inline QString get_clibParameterPath() noexcept {
		return m_clibParameterPath;
	}
	//���������ļ���·��
	inline QString get_dataPath() noexcept {
		return m_dataPath;
	}
	//����ͼ���ļ���·��
	inline QString get_imagePath() noexcept {
		return m_imagePath;
	}

	//�����ļ�·��,�����ļ���
	bool generate_Folder();	
	//д��ĿXML�ļ�������Ϊvms��ʽ			
	void write_XMl();	
	//��ȡXML����Ϣ
	void read_XML();
	//��Ŀģʽ-3Dʵʱ�������ߺ���
	enum Mode{REALTIME, analysis};


private:
	QString m_projectName{};										//��Ŀ����
	QString m_surveyor{};											//������Ա
	QString m_location{};											//�����ص�
	QString m_time{};												//����ʱ��
	QString m_filePath{};											//�ļ�·��
	QString m_clibParameterPath{};								//�궨�����ļ���·��
	QString m_dataPath{};											//�����ļ���·��
	QString m_imagePath{};										//ͼ���ļ���·��
	QDomDocument m_doc;											//дXML		
};
