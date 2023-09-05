#include "XFileAlias.h"

#include <QReadLocker>
#include <QWriteLocker>

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFileAlias
// Method:  m_Mutex
//----------------------------------------------------------------------------- 

QReadWriteLock XFileAlias::m_Lock;

QMap<QString, QString> XFileAlias::m_Alias;
QMap<QString, QSet<QString> > XFileAlias::m_Filenames;

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  XCache
//----------------------------------------------------------------------------- 
XFileAlias::XFileAlias()
{
}
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  ~XCache
//----------------------------------------------------------------------------- 
XFileAlias::~XFileAlias()
{
}
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFileAlias
// Method:  alias
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 
const QString& XFileAlias::alias(const QString& filename)
{
	QReadLocker lock(&m_Lock);

	if (m_Alias.contains(filename))
		return m_Alias[filename];

	return filename;
}
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFileAlias
// Method:  insert
// Params:  const QString& alias
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
void XFileAlias::insert(const QString& alias, const QString& filename)
{
	clear(alias);

	{
		QWriteLocker lock(&m_Lock);
		m_Alias[alias] = filename;
		m_Filenames[filename] << alias;
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFileAlias
// Method:  remove
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

void XFileAlias::remove(const QString& filename)
{
	QWriteLocker lock(&m_Lock);
	if (m_Filenames.contains(filename))
	{
		for(const QString&alias: m_Filenames[filename].values())
		{
			m_Alias.remove(alias);
		}
		
		m_Filenames.remove(filename);
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFileAlias
// Method:  clear
// Params:  const QString& alias
//----------------------------------------------------------------------------- 
void XFileAlias::clear(const QString& alias)
{
	QWriteLocker lock(&m_Lock);
	if (m_Alias.contains(alias))
	{
		QString filename=m_Alias[alias];
		m_Alias.remove(alias);

		m_Filenames[filename].remove(alias);
		if (m_Filenames[filename].isEmpty())
			m_Filenames.remove(filename);
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFileAlias
// Method:  clear
//----------------------------------------------------------------------------- 
void XFileAlias::clear()
{
	QWriteLocker lock(&m_Lock);
	m_Alias.clear();
	m_Filenames.clear();
}

