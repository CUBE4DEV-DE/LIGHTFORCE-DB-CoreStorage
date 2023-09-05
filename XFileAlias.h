#pragma once
#include <QFileInfo>
#include <QReadWriteLock>

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  XCache
//----------------------------------------------------------------------------- 
class XFileAlias
{
public:
	XFileAlias();
	virtual ~XFileAlias();

	static const QString& alias(const QString& filename);

	static void insert(const QString& alias, const QString& filename);

	static void remove(const QString& filename);
	static void clear(const QString& alias);


	static void clear();
private:
	static QReadWriteLock m_Lock;
	static QMap<QString, QString> m_Alias;
	static QMap<QString, QSet<QString> > m_Filenames;
};

