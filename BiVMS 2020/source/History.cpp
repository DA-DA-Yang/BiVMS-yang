#include "History.h"


History::History(QObject *parent)
	: QObject(parent)
{
}

History::~History()
{
}

//读取项目的历史信息
void History::_read_History()
{
	m_dir.cd(strPath);
	if (!m_dir.exists(strPath)) {
		return;
	}
	m_ini->beginGroup("projectname");
	QStringList str1 = m_ini->allKeys();
	foreach(QString key, str1) {
		m_projectname.push_back(m_ini->value(key).toString());
	}
	m_ini->endGroup();
	m_ini->beginGroup("filepath");
	QStringList str2 = m_ini->allKeys();
	foreach(QString key, str2) {
		m_projectpath.push_back(m_ini->value(key).toString());
	}
	m_ini->endGroup();
	m_ini->beginGroup("creattime");
	QStringList str3 = m_ini->allKeys();
	foreach(QString key, str3) {
		m_createtime.push_back(m_ini->value(key).toString());
	}
	m_ini->endGroup();
}

//保存项目创建历史信息
void History::write_History(QString projectname, QString filepath, QString time)
{
	_read_History();
	int size = m_projectname.size();
	if (size < 20) {
		m_projectname.push_back(projectname);
		m_projectpath.push_back(filepath);
		m_createtime.push_back(time);
	}
	else {
		m_projectname.erase(m_projectname.begin());
		m_projectpath.erase(m_projectpath.begin());
		m_createtime.erase(m_createtime.begin());
		m_projectname.push_back(projectname);
		m_projectpath.push_back(filepath);
		m_createtime.push_back(time);
	}
	for (int i = 0; i < m_projectname.size(); ++i) {
		if (i < 10) {
			m_ini->setValue("projectname/0" + QString::number(i), m_projectname[i]);
			m_ini->setValue("filepath/0" + QString::number(i), m_projectpath[i]);
			m_ini->setValue("creattime/0" + QString::number(i), m_createtime[i]);
		}
		else{
			m_ini->setValue("projectname/" + QString::number(i), m_projectname[i]);
			m_ini->setValue("filepath/" + QString::number(i), m_projectpath[i]);
			m_ini->setValue("creattime/" + QString::number(i), m_createtime[i]);
		}
	}
}

//得到项目的历史信息
void History::get_History(QVector<QString> &projectname, QVector<QString> &projectpath, QVector<QString> &createtime)
{
	_read_History();
	projectname = m_projectname;
	projectpath = m_projectpath;
	createtime = m_createtime;

}

void History::delete_History(int index)
{
	m_projectname.erase(m_projectname.begin() + index);
	m_projectpath.erase(m_projectpath.begin() + index);
	m_createtime.erase(m_createtime.begin() + index);
	//QFile::remove(strPath);

	m_ini->clear();
	//m_ini->remove(strPath);
	for (int i = 0; i < m_projectname.size(); ++i) {
		if (i < 10) {
			m_ini->setValue("projectname/0" + QString::number(i), m_projectname[i]);
			m_ini->setValue("filepath/0" + QString::number(i), m_projectpath[i]);
			m_ini->setValue("creattime/0" + QString::number(i), m_createtime[i]);
		}
		else {
			m_ini->setValue("projectname/" + QString::number(i), m_projectname[i]);
			m_ini->setValue("filepath/" + QString::number(i), m_projectpath[i]);
			m_ini->setValue("creattime/" + QString::number(i), m_createtime[i]);
		}
	}
}



