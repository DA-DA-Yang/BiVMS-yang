/***************************************************************************
**  BiVMS 2020                                                            **
**  A Dialog to creat Project infomation                                   **

****************************************************************************
**           Author: CHENG HUANG                                          **
**          Contact: huangcheng@shu.edu.cn                                **
**             Date: 27.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/

#pragma once
#include <QDialog>
#include "ui_NewProjectDlg.h"
#include <QFileDialog>
#include <QFile>     
#include <QtXml\QtXml>  
#include <QtXml\QDomDocument> 
#include "MyMessage.h"
#include "project.h"
#include "Communication.h"

class NewProjectDlg : public QDialog
{
	Q_OBJECT

public:
	NewProjectDlg(QWidget *parent = Q_NULLPTR);
	~NewProjectDlg();

	//������Ŀ�๲��ָ��
	inline void copyProjectPtr(Project* ptr) noexcept { m_project = ptr; }
	//�ж���Ŀ�Ƿ񴴽��ɹ�
	inline bool isCreateProject() noexcept { return m_isCreateProject; }
	inline void setCommunication(Communication* communication) noexcept
	{
		m_communicationMessage = communication;
		connect(m_communicationMessage, &Communication::connected, this, &NewProjectDlg::_slot_get_Connect);
	}

private:
	///��Ӧ�Ի���İ�ť-����
	//����ͨ��
	void _slot_get_Connect();
	//������վͨ��
	void _slot_pBt_Connect();
	//��ȡ��ǰʱ��
	void _slot_get_CurrentTime();
	//������Ŀ·��
	void _slot_choose_FilePath();
	//ȷ�ϲ������ļ��е�
	void _slot_Ok();
	//ȡ�����˳���Ŀ����
	void _slot_Cancel();
	
private:
	//��ʼ������
	void _init_Infomation();
	//�õ��������Ϣ
	void _get_Infomation();
	//���������Ϣ������Ŀ��
	void _send_Infomation();

private:
	QString m_projectName{};										//��Ŀ����
	QString m_surveyor{};											//������Ա
	QString m_location{};											//�����ص�
	QString m_time{};												//����ʱ��
	QString m_filePath{};											//�ļ�·��
	bool m_isConnect = false;									//�ж��豸�Ƿ�����
	bool m_isCreateProject = false;								//�ж���Ŀ�Ƿ񴴽�
	History *m_history = nullptr;								//������ʷ��Ա����
	//std::shared_ptr<Project> m_project = nullptr;				//��Ŀ��Ϣ��Ա����
	Project *m_project = nullptr;								//��Ŀ��Ϣ��Ա����
	Communication* m_communicationMessage = nullptr;                   //ͨ��
private:
	Ui::NewProjectDlg ui;

};
