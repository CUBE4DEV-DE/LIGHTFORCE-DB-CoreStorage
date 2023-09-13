#pragma once
#include <QFileInfo>
#include <QReadWriteLock>

namespace CoreStorage
{

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  XCache
// Description:
//	The XFileAlias class is defined the CoreStorage-Namespace.
//	This class has several static methods that allow you to set and update
//	file aliases, remove file aliases, and clear the aliases entirely.

//	The alias method retrieves the file QString representation for a given alias.

//	The insert method sets the alias for a given file path.

//	The remove method removes the entry with a given file path.

//	The clear method removes either the file path or the alias
//	association from the cache entirely.

//	The m_Lock attribute is a QReadWriteLock object that ensures thread
//	safety while accessing the m_Alias and m_Filenames attributes,
//	which are of type QMap<QString, QString> and
//	QMap<QString, QSet<QString> > respectively.

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

};
using namespace CoreStorage;

