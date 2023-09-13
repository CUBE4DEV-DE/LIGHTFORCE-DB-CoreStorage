#pragma once



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
// Description:
//	This XFile file contains a CoreStorage namespace, which is a collection of
//	functions for managing files and their contents including reading and writing
//	different types of data, such as raw Text(QString), UNICODE-Text,
//	QJsonDocument, QByteArray, QStringList, QMap<QString, QString>,
//	and QSet<QString>.The functions also allow for compressing
//	and encrypting data, as well as copying, renaming, moving, and deleting files.
//	There is also a concurrent function for executing a given function in parallel.
//	The XFile class includes static methods to perform these various functions.
//	The file has a template that can be used as a base to create your file and
//	Description is left as blank, allowing developers to provide context.
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
// Description:
//			This is a method of the XFile class which is designed to write
//			data to a file specified by the filename parameter. The T parameter
//			is a template type, which should match the type of the data to be
//			written to the file. The data parameter represents the actual data
//			to be written to the file. The run parameter is a boolean value that
//			determines whether the written data should be run as a program. If
//			run is set to true, then the written data will be executed as a
//			program. Additionally, the T type should be serializable or supply
//			an overload for the << operator.
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
// Description:
//	The read method is static a member function of the XFile class.
//	This template method accepts a QString parameter called filename which
//	is the path and name of the file that is to be read. It returns an
//	object of type T, which must be specified when calling this method.
//	This method reads the contents of a file with the specified filename
//	and returns it as an object of type T.The read method can be further
//	customized for various file formats.When used, the filename should
//	contain the full file path or be a relative path from the
//	current working directory.
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
// Description:
//	This is a method of the XFile class which is designed to read
//	cache data from a file specified by the filename parameter.
//	The method returns a value of type T, which is the template 
//	specified by the developer. The T type should match the type
//	of the data to be read from the file. The QString parameter is
//	used to specify the file name.
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

