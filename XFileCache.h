#pragma once

#include "CoreStorageGlobal.h"

#include <QFileInfo>
#include <QSystemSemaphore>
#include <QJsonDocument>

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  XCache
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

