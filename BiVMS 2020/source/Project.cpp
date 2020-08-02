#include "Project.h"

Project::Project(QObject *parent)
	: QObject(parent)
{
}

Project::~Project()
{
}

//������Ŀ�ļ���
bool Project::generate_Folder()
{
	QDir dir;
	QString filename;
	dir.cd(m_filePath);
	filename = m_filePath + "/" + m_projectName;
	if (dir.exists(filename))
	{
		MyMessage information("�ļ��Ѵ��ڣ�");
		return false;
	}
	dir.mkdir(m_projectName);
	dir.cd(filename);
	dir.mkdir(m_filePath);
	dir.mkdir(QString::fromLocal8Bit("�궨����"));
	m_clibParameterPath = filename + "/" + QString::fromLocal8Bit("�궨����");
	dir.mkdir(QString::fromLocal8Bit("����"));
	m_dataPath = filename + "/" + QString::fromLocal8Bit("����");
	dir.mkdir(QString::fromLocal8Bit("ͼ��"));
	m_imagePath = filename + "/" + QString::fromLocal8Bit("ͼ��");
	m_filePath = filename;
	return true;
}

//дXML
void Project::write_XMl()
{
	QString filename = m_filePath + "/" + m_projectName + ".vms";
	QString strHeader("version=\"1.0\" encoding=\"UTF-8\"");
	m_doc.appendChild(m_doc.createProcessingInstruction("xml", strHeader));
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		return;
	}
	QDomElement rootFile = m_doc.createElement("Info");
	m_doc.appendChild(rootFile);
	QDomElement rootProjectName = m_doc.createElement("projectname");
	rootFile.appendChild(rootProjectName);
	QDomText textProjectName = m_doc.createTextNode(m_projectName);
	rootProjectName.appendChild(textProjectName);
	QDomElement rootSurveyor = m_doc.createElement("surveyor");
	rootFile.appendChild(rootSurveyor);
	QDomText textSurveyor = m_doc.createTextNode(m_surveyor);
	rootSurveyor.appendChild(textSurveyor);
	QDomElement rootLocation = m_doc.createElement("location");
	rootFile.appendChild(rootLocation);
	QDomText textLocation = m_doc.createTextNode(m_location);
	rootLocation.appendChild(textLocation);
	QDomElement rootTime = m_doc.createElement("time");
	rootFile.appendChild(rootTime);
	QDomText textTime = m_doc.createTextNode(m_time);
	rootTime.appendChild(textTime);
	QString xml = m_doc.toString();
	QTextStream txtOutput(&file);
	txtOutput << xml;
	file.close();
}

//��ȡXML
void Project::read_XML()
{
	QString name;
	QDomDocument doc;
	QString m_filePath = QFileDialog::getOpenFileName(nullptr,
		("���ĵ��ļ�"), "./",
		tr("(*.vms)"));
	QFile file(m_filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	QDomElement rootElem = doc.documentElement();
	QDomNode rootNode = rootElem.firstChild();
	while (!rootNode.isNull())
	{
		QDomElement fileElem = rootNode.toElement();
		if (!fileElem.isNull()){
			name = fileElem.tagName();
			if (name == "projectname"){
				for (QDomNode childNode = fileElem.firstChild(); !childNode.isNull();
					childNode = childNode.nextSibling())
				{
					QDomText childText = childNode.toText();
					m_projectName = childText.data();
				}
			}
			else if (name == "surveyor"){
				for (QDomNode childNode = fileElem.firstChild(); !childNode.isNull();
					childNode = childNode.nextSibling()){
					QDomText childText = childNode.toText();
					m_surveyor = childText.data();
					
				}
			}
			else if (name == "location"){
				for (QDomNode childNode = fileElem.firstChild(); !childNode.isNull();
					childNode = childNode.nextSibling()){
					QDomText childText = childNode.toText();
					m_location = childText.data();
					
				}
			}
			else if (name == "time"){
				for (QDomNode childNode = fileElem.firstChild(); !childNode.isNull();
					childNode = childNode.nextSibling()){
					QDomText childText = childNode.toText();
					m_time = childText.data();
				}
			}

		}
		rootNode = rootNode.nextSibling();
	}
	file.close();
}

