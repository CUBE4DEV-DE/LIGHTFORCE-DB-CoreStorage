#include "XConfig.h"

#include <QCryptographicHash>

#include "XFile.h"
#include "XFileBlock.h"

#include "../CoreBase/XDebug.h"

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    28.10.2022
// Context: DefaultNamespace
// Class:   XConfig
// Method:  m_DocumentCache
//----------------------------------------------------------------------------- 

QCache<QString, QMap<QString, QMap<QString, QVariant>> > XConfig::m_DocumentCache(1024);
QMap<QString, QDateTime> XConfig::m_DocumentChanged;
QMutex XConfig::m_Mutex;

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XRegistry
// Method:  XRegistry
// Params:  const QString& filename
// Params:  const QString& group
//----------------------------------------------------------------------------- 

XConfig::XConfig(const QString& filename, const QString& group)
{
	m_FileName = filename;
	m_Group = group;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XConfig
// Method:  ~XConfig
//----------------------------------------------------------------------------- 
XConfig::~XConfig()
{
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XConfig
// Method:  setValue
// Params:  const QString& name
// Params:  const QVariant& value
//----------------------------------------------------------------------------- 

void XConfig::setValue(const QString& name, const QVariant& value)
{
	setValue(m_FileName, m_Group, name,value);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XConfig
// Method:  value
// Params:  const QString& name
// Params:  
//----------------------------------------------------------------------------- 

QVariant XConfig::value(const QString& name)
{
	return value(m_FileName, m_Group);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XRegistry
// Method:  setValue
// Params:  const QString& filename
// Params:  const QString& group
// Params:  const QString& name
// Params:  const QVariant& value
//-----------------------------------------------------------------------------

//QMap<QString, QMap<QString, QVariant>> map;

void XConfig::setValue(const QString& filename, const QString& group, const QString& name, const QVariant& value)
{
	//O_STACK(XConfig, setValue);
	QMutexLocker lock(&m_Mutex);

/*	//QMap<QString, QMap<QString, QVariant>>* documentcache = documentCache(filename);
	//if (documentcache)

	O_PARAM(group);
	O_PARAM(name);
	O_PARAM(value.toString());

	QMap<QString, QMap<QString, QVariant>> map = XFile::readDocumentMap(filename);
	{
		map[group][name] = value;
		

		XFile::writeDocumentMap(filename, map);
	}
	*/

	QFileInfo fileInfo(filename);
	fileInfo.refresh();

	QMap<QString, QMap<QString, QVariant>>* documentcache = Q_NULLPTR;
	if (m_DocumentCache.contains(filename))
	{
		documentcache = m_DocumentCache[filename];
		if (m_DocumentChanged[filename] < fileInfo.lastModified().addSecs(-5))
		{
			*documentcache = XFile::readDocumentMap(filename);
			m_DocumentChanged[filename] = fileInfo.lastModified();
		}
	}
	else
	{
		documentcache = new QMap<QString, QMap<QString, QVariant>>();
		*documentcache = XFile::readDocumentMap(filename);
		m_DocumentCache.insert(filename, documentcache, 1);
		m_DocumentChanged[filename] = fileInfo.lastModified();
	}

	(*documentcache)[group][name] = value;

	XFile::writeDocumentMap(filename, (*documentcache));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XRegistry
// Method:  isValue
// Params:  const QString& datapath
// Params:  const QString& name
// Params:  const QString& value
// Params:  
//----------------------------------------------------------------------------- 
QVariant XConfig::value(const QString& filename, const QString& group, const QString& name)
{
	QMutexLocker lock(&m_Mutex);


	QFileInfo fileInfo(filename);
	fileInfo.refresh();

	QMap<QString, QMap<QString, QVariant>>* documentcache = Q_NULLPTR;
	if (m_DocumentCache.contains(filename))
	{
		documentcache = m_DocumentCache[filename];
		if (m_DocumentChanged[filename] < fileInfo.lastModified().addSecs(-5))
		{
			*documentcache = XFile::readDocumentMap(filename);
			m_DocumentChanged[filename] = fileInfo.lastModified();
		}
	}
	else
	{
		documentcache = new QMap<QString, QMap<QString, QVariant>>();
		*documentcache = XFile::readDocumentMap(filename);
		m_DocumentCache.insert(filename, documentcache, 1);
		m_DocumentChanged[filename] = fileInfo.lastModified();
	}

	if (!documentcache->contains(group))
		return QVariant();

	if (!(*documentcache)[group].contains(name))
		return QVariant();

	if (documentcache)
		return (*documentcache)[group][name];

	return QVariant();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    29.01.2023
// Context: DefaultNamespace
// Class:   XConfig
// Method:  values
// Params:  const QString& filename
// Params:  const QString& group
// Params:  
//----------------------------------------------------------------------------- 

QStringList XConfig::values(const QString& filename, const QString& group)
{
	QMutexLocker lock(&m_Mutex);

	QFileInfo fileInfo(filename);
	fileInfo.refresh();

	QMap<QString, QMap<QString, QVariant>>* documentcache = Q_NULLPTR;
	if (m_DocumentCache.contains(filename))
	{
		documentcache = m_DocumentCache[filename];
		if (m_DocumentChanged[filename] < fileInfo.lastModified().addSecs(-5))
		{
			*documentcache = XFile::readDocumentMap(filename);
			m_DocumentChanged[filename] = fileInfo.lastModified();
		}
	}
	else
	{
		documentcache = new QMap<QString, QMap<QString, QVariant>>();
		*documentcache = XFile::readDocumentMap(filename);
		m_DocumentCache.insert(filename, documentcache, 1);
		m_DocumentChanged[filename] = fileInfo.lastModified();
	}

	if (!documentcache->contains(group))
		return QStringList();

	return (*documentcache)[group].keys();
}
