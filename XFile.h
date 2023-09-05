#pragma once
//#DOC

#include "CoreStorageGlobal.h"

#include <QString>
#include <QFuture>
#include <QList>
#include <QtConcurrent>
#include <QRegularExpression>

namespace CoreStorage
{

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  XFile
//----------------------------------------------------------------------------- 
	
class CORESTORAGE_EXPORT XFile
{
public:
	static void alias(const QString& filename, const QString& alias);

	// QString
	static QString read(const QString& filename);
	static const QString& readCache(const QString& filename);
	static bool write(const QString& filename, const QString& content,bool run=false);
	static bool append(const QString& filename, const QString& content, bool run = false,bool ignorelog=false);

	// QByteArray
	static QByteArray readBinary(const QString& filename);
	static const QByteArray& readBinaryCache(const QString& filename);
	static bool writeBinary(const QString& filename, const QByteArray& data, bool run = false);
	static bool appendBinary(const QString& filename, const QByteArray& data, bool run = false, bool ignorelog = false);

	// QJsonDocument
	static QJsonDocument readDocument(const QString& filename);
	static const QJsonDocument& readDocumentCache(const QString& filename);
	static bool writeDocument(const QString& filename, const QJsonDocument& data, bool run = false);

	static QMap<QString,QMap<QString,QVariant>> readDocumentMap(const QString& filename);
	static QMap<QString,QMap<QString,QVariant>> readDocumentMapCache(const QString& filename);
	static bool writeDocumentMap(const QString& filename, const QMap<QString,QMap<QString,QVariant>>& data, bool run = false);

	// QStringList
	static QStringList readList(const QString& filename);
	static QStringList readListCache(const QString& filename);
	static bool writeList(const QString& filename, const QStringList& data, bool run = false);

	// QMap<QString,QString> 
	static QMap<QString,QString> readMap(const QString& filename);
	static QMap<QString, QString> readMapCache(const QString& filename);
	static bool writeMap(const QString& filename, const QMap<QString,QString>& data, bool run = false);

	// QSet<QString>
	static QSet<QString> readSet(const QString& filename);
	static QSet<QString> readSetCache(const QString& filename);
	static bool writeSet(const QString& filename, const QSet<QString>& data, bool run = false);

	// Basic
	static bool copy(const QString& srcfilename, const QString& dstfilename, bool run = false);
	static bool rename(const QString& srcfilename, const QString& dstfilename, bool run = false);
	static bool move(const QString& srcfilename, const QString& dstfilename, bool run = false);
	static bool remove(const QString& filename, bool run = false);
	static bool remove(const QStringList& filenames, bool run = false);

	static QString unique(const QString& filename);


	// Test
	static bool exists(const QString& filename);
	static bool equal(const QString& filename, const QString& filename2);
	static bool contains(const QString& filename, const QString& search,Qt::CaseSensitivity sensitivity=Qt::CaseInsensitive);
	static bool contains(const QString& filename, const QRegularExpression& search);

	// Compress
	static bool compress(const QString& srcfilename, const QString& dstfilename, bool run = false);
	static bool unCompress(const QString& srcfilename, const QString& dstfilename, bool run = false);
	static bool compress(const QString& srcfilename, bool run = false);
	static bool unCompress(const QString& srcfilename, bool run = false);

	// Crypt
	static bool crypt(const QString& srcfilename, const QString& dstfilename, const QString& key, bool run = false);
	static bool unCrypt(const QString& srcfilename, const QString& dstfilename, const QString& key, bool run = false);

	// Path
	static bool mkFilePath(const QString& filename);

	// Concurrent
	static void wait();

	template<class T> static bool write(const QString& filename, const T& data, bool run=false);
	template<class T> static T read(const QString& filename);
	template<class T> static T readCache(const QString& filename);

private:

	static QJsonObject FileNameJSON(const QString& filename);
	static QJsonObject FileNamesJSON(const QString& srcfilename, const QString& dstfilename);
	static QList<QFuture<void> > m_Futures;
	
	template <class Function, class ...Args>
	static auto concurrent(Function&& f, Args &&...args)
	{
		append(QtConcurrent::run(std::forward<Function>(f),std::forward<Args>(args)...));
	}

	static QString alias(const QString& filename);
	static void append(QFuture<void> future);

	QCache<QString, QByteArray> m_BinaryCache;
	QCache<QString, QByteArray> m_TextCache;
};

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  write
// Params:  const QString& filename
// Params:  const T& data
// Params:  bool run
//----------------------------------------------------------------------------- 
template <class T>
bool XFile::write(const QString& filename, const T& data, bool run)
{
	QByteArray binary;
	{
		QDataStream stream(binary);
		stream.setVersion(QDataStream::Qt_6_4);
		stream << data;
	}
	return writeBinary(filename, qCompress(binary),run);
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  read
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
template <class T>
T XFile::read(const QString& filename)
{
	QByteArray binary = readBinary(filename);
	if (binary.isEmpty()) return T();
	QDataStream stream(qUncompress(binary));
	stream.setVersion(QDataStream::Qt_6_4);
	T data;
	stream >> data;
	return data;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readCache
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

template <class T>
T XFile::readCache(const QString& filename)
{
	QByteArray binary = readBinaryCache(filename);
	if (binary.isEmpty()) return T();
	QDataStream stream(qUncompress(binary));
	stream.setVersion(QDataStream::Qt_6_4);
	T data;
	stream >> data;
	return data;
}


};
using namespace CoreStorage;

