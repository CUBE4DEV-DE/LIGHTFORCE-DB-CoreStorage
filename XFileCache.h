#pragma once

#include "CoreStorageGlobal.h"

#include <QFileInfo>
#include <QSystemSemaphore>
#include <QJsonDocument>

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
//	There are multiple methods in this file, each with its own specific task
//	to optimize access to the contents of files.
//	fill fills an XFileCache instance with the contents of a text file.
//	fillBinary fills an XFileCache instance with the contents of a binary file.
//	fillDocument fills an XFileCache instance with a QJsonDocument instance that
//	contains the contents of a JSON or binary file.
//	isChanged checks if the contents of the file have changed since the
//	last read.size returns the size of the contents of the file.
//	The read, readBinary, and readDocument methods access the files in the
//	cache and reload the contents of the file if necessary.Caching enables
//	faster file access since the contents of the file do not have to be read
//	from the storage drive every time.
//----------------------------------------------------------------------------- 
class CORESTORAGE_EXPORT XFileCache
{
	public:
	XFileCache();
	virtual ~XFileCache();

	void fill(const QString& filename);
	void fillBinary(const QString& filename);
	void fillDocument(const QString& filename);
	bool isChanged();
	qint64 size();
	const QString& text() { return m_Text;  }
	const QByteArray& byteArray() { return m_ByteArray; }
	const QJsonDocument& document() { return m_Document; }

	static void setMemory(qint32 megabyte);

	static const QString& read(const QString& filename);
	static const QByteArray& readBinary(const QString& filename);
	static const QJsonDocument& readDocument(const QString& filename);

	static bool isChanged(const QString& filename);
	static bool isChangedBinary(const QString& filename);
	static bool isChangedDocument(const QString& filename);

	private:
	QFileInfo m_FileInfo;
	QByteArray m_ByteArray;
	QJsonDocument m_Document;
	QString m_Text;
	QString m_FileName;
	QString m_Path;

	static QCache<QString, XFileCache> m_TextCache;
	static QCache<QString, XFileCache> m_BinaryCache;
	static QCache<QString, XFileCache> m_DocumentCache;
};


};
using namespace CoreStorage;

