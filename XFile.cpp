#include "XFile.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QtConcurrent>
#include <QTextStream>

#include "XFileBlock.h"
#include "XFileCache.h"
#include "XCrypt.h"
#include "XFileAlias.h"
#include "XLogInject.h"

#include "../CoreBase/XDebug.h"

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    03.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  m_Futures
//----------------------------------------------------------------------------- 

QList<QFuture<void> > XFile::m_Futures;

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  alias
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

// This C++ function, named "alias", is a member of the "XFile" class.It takes a 
// "filename" parameter as a QString reference and simply returns the result of the
// "alias" method from the "XFileAlias" class, which is called with the same 
// "filename" parameter.

QString XFile::alias(const QString& filename)
{
	return XFileAlias::alias(filename);
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  alias
// Params:  const QString& filename
// Params:  const QString& alias
//----------------------------------------------------------------------------- 

// This "alias" method is part of the "XFile" class and takes two QString references 
// as parameters : "filename" and "alias".Initially,  it checks if both strings are empty,
// and if so, invokes the "clear" method from the "XFileAlias" class,
// potentially clearing all aliases.If only "filename" is empty, it again calls the "clear" method,
// this time with the "alias" parameter, possibly to remove a specific alias.Finally,
// if both strings are not empty, it calls the "insert" method of the "XFileAlias" 
// class to create or update an alias with the provided "alias" and "filename".

void XFile::alias(const QString& filename, const QString& alias)
{

	if (filename.isEmpty()&&alias.isEmpty())
		XFileAlias::clear();
	else if (filename.isEmpty())
		XFileAlias::clear(alias);

	if (!filename.isEmpty() && !alias.isEmpty())
		XFileAlias::insert(alias,filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readFile
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

// This "read" function, part of the "XFile" class, takes a filename as an input
// parameter and attempts to read the content of the file with that name. Initially,
// it checks if a trace for "FILE" is active using "XInject::isTrace", logging 
// a trace message if true. Then, it tries to open and read the specified file,
// logging a warning and returning an empty QString if the file cannot be opened
// or if any other error occurs during the read operation. If the file is successfully opened
// , it reads the entire content using a QTextStream and returns it as a QString.
// Throughout the function, it uses an "alias" method to possibly resolve the filename 
// to an alias before accessing the file, and "FileNameJSON" method to 
// format the filename for logging.

QString XFile::read(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE","CoreStorage", "XFile", "read", "Trace", "Ok", FileNameJSON(filename));
	}


	XFileBlock block(alias(filename));

	try
	{
		QFile file(alias(filename));

		if (!file.open(QIODevice::ReadOnly))
		{
			X_WARNINGJSON("CoreStorage", "XFile", "read", "Open", "Failed", FileNameJSON(filename));
			return QString();
		}

		QTextStream fileread(&file);
		fileread.setAutoDetectUnicode(true);
		return fileread.readAll();
	}
	catch(...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "read", "Read", "Failed",FileNameJSON(filename));
		return QString();
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readCache
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

// This function, named "readCache", is part of the "XFile" class and takes a
// "filename" parameter as a QString reference. Initially, it checks if a trace
// for "FILE" is active using "XInject::isTrace", and if true, logs a trace message.
// Then, it calls the "read" method from the "XFileCache" class, passing an alias
// of the "filename" (resolved through the "alias" method of the "XFile" class) 
// as the parameter, and returns its result.

const QString& XFile::readCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return XFileCache::read(alias(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeFile
// Params:  const QString& filename
// Params:  const QString& content
// Params:  
//----------------------------------------------------------------------------- 

// This source code defines a method `XFile::write` in C++ that takes three
// parameters: a filename (as a QString), content (as a QString), and a 
// boolean flag named "run". The method first checks if a trace for "FILE" 
// is active using the `XInject::isTrace` method and logs a message if it is. 
// Next, based on the value of the "run" parameter, it writes the content to 
// the specified file in two different ways : 

// 1. If "run" is true, it starts a concurrent operation(potentially running 
// in a separate thread) to write the content to the file.It tries to create 
// necessary paths and open the file for writing, logging a warning message 
// if any of these operations fail.It writes the content to the file using 
// a QTextStream, which is set to automatically detect Unicode encoding.

// 2. If "run" is false, it performs a similar series of operations but within
//  the main thread, returning true if the write operation was successful and 
// false otherwise, logging a warning message if the operation fails at any step.

bool XFile::write(const QString& filename, const QString& content, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "write", "Trace", "Ok", FileNameJSON(filename));
	}

	if (run)
	{
		concurrent([filename,content]
			{
				XFileBlock block(alias(filename),true);

				try {

					mkFilePath(filename);

					QFile file(alias(filename));
					if (!file.open(QIODevice::WriteOnly))
					{
						X_WARNINGJSON("CoreStorage", "XFile", "write", "Open", "Failed", FileNameJSON(filename));
						return;
					}
					{
						QTextStream filewrite(&file);
						filewrite.setAutoDetectUnicode(true);
						filewrite << content;
					}
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "write", "Write", "Failed", FileNameJSON(filename));
				}
			});

		return true;
	}
	else
	{
		XFileBlock block(alias(filename), true);
		
		try
		{
			mkFilePath(filename);

			QFile file(alias(filename));
			if (!file.open(QIODevice::WriteOnly))
			{
				X_WARNINGJSON("CoreStorage", "XFile", "write", "Open", "Failed", FileNameJSON(filename));
				return false;
			}

			{
				QTextStream filewrite(&file);
				filewrite.setAutoDetectUnicode(true);
				filewrite << content;
			}

			return true;
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "write", "Write", "Failed", FileNameJSON(filename));
			return false;
		}
	}

}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  append
// Params:  const QString& filename
// Params:  const QString& content
// Params:  bool run
//----------------------------------------------------------------------------- 

// This script defines a method named `append` within the `XFile` class that
// takes four arguments: a filename (as a QString), content (as a QString), 
// a boolean named "run", and a boolean named "ignorelog".
// 
// 1. Initially, it checks whether a trace is active for "FILE" using 
// `XInject::isTrace("FILE")` and if "ignorelog" is false.
// If so, it logs a trace message using `X_LOGJSON`.
// 
// 2. Then, the script checks the "run" argument.If "run" is true,
// it executes a concurrent block where it attempts to append the provided
// "content" to the file specified by "filename" in a separate thread.
// During this process, it creates an `XFileBlock` and opens the file in append mode,
// then writes the "content" to it using a `QTextStream`. If it encounters any 
// errors(either in opening the file or during the write operation), and "ignorelog" 
// is false, it logs a warning message using `X_WARNINGJSON`. The concurrent
// operation doesn't return any value.
// 
// 3. If "run" is false, the script executes a similar block of code but not
// in a separate thread.It tries to append "content" to the file and logs
// warning messages if necessary(same as in the concurrent block).It returns `true`
// if the operation succeeds and `false` otherwise.

// Throughout the code, a series of operations(such as aliasing the filename 
// and creating the file path) are performed to manage the file before the 
// writing operation.

bool XFile::append(const QString& filename, const QString& content, bool run, bool ignorelog)
{
	if (XInject::isTrace("FILE")&&!ignorelog)
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "append", "Trace", "Ok", FileNameJSON(filename));
	}

	if (run)
	{
		concurrent([filename, content,ignorelog]
			{
				XFileBlock block(alias(filename), true);

				try {

					mkFilePath(filename);

					QFile file(alias(filename));
					if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
					{
						if (!ignorelog)
						{
							X_WARNINGJSON("CoreStorage", "XFile", "append", "Open", "Failed", FileNameJSON(filename));
						}
						return;
					}

					{
						QTextStream filewrite(&file);
						filewrite.setAutoDetectUnicode(true);
						filewrite << content;
					}
				}
				catch (...)
				{
					if (!ignorelog)
					{
						X_WARNINGJSON("CoreStorage", "XFile", "append", "Write", "Failed", FileNameJSON(filename));
					}
				}
			});

		return true;
	}
	else
	{
		XFileBlock block(alias(filename), true);

		try {

			mkFilePath(filename);

			QFile file(alias(filename));
			if (!file.open(QIODevice::WriteOnly| QIODevice::Append))
			{
				if (!ignorelog)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "append", "Open", "Failed", FileNameJSON(filename));
				}
				return false;
			}


			{
				QTextStream filewrite(&file);
				filewrite.setAutoDetectUnicode(true);
				filewrite << content;
			}

			return true;

		}
		catch (...)
		{
			if (!ignorelog)
			{
				X_WARNINGJSON("CoreStorage", "XFile", "append", "Write", "Failed", FileNameJSON(filename));
			}
			return false;
		}

	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readBinary
// Params:  const QString& filename
//----------------------------------------------------------------------------- 


QByteArray XFile::readBinary(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readBinary", "Trace", "Ok", FileNameJSON(filename));
	}

	XFileBlock block(alias(filename));

	try
	{
		QFile file(alias(filename));
		if (!file.open(QIODevice::ReadOnly))
		{
			X_WARNINGJSON("CoreStorage", "XFile", "readBinary", "Open", "Failed",FileNameJSON(filename));
			return QByteArray();
		}

		return file.readAll();
	}
	catch (...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "readBinary", "Read", "Failed", FileNameJSON(filename));
	}

	return QByteArray();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readBinaryCache
// Params:  const QString& filename
//----------------------------------------------------------------------------- 

const QByteArray& XFile::readBinaryCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readBinaryCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return XFileCache::readBinary(alias(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeBinary
// Params:  const QString& filename
// Params:  const QByteArray& data
//----------------------------------------------------------------------------- 

bool XFile::writeBinary(const QString& filename, const QByteArray& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeBinary", "Trace", "Ok", FileNameJSON(filename));
	}

	if(run)
	{
		concurrent([filename, data]
			{
				XFileBlock block(alias(filename), true);

				try
				{
					mkFilePath(filename);
					QFile file(alias(filename));
					if (!file.open(QIODevice::WriteOnly))
					{
						X_WARNINGJSON("CoreStorage", "XFile", "writeBinary", "Open", "Failed", FileNameJSON(filename));
						return;
					}
					file.write(data);
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "writeBinary", "Write", "Failed", FileNameJSON(filename));
				}

			});

		return true;
	}
	else
	{
		XFileBlock block(alias(filename), true);

		try
		{
			mkFilePath(filename);

			QFile file(alias(filename));
			if (!file.open(QIODevice::WriteOnly))
			{
				X_WARNINGJSON("CoreStorage", "XFile", "writeBinary", "Open", "Failed", FileNameJSON(filename));
				return false;
			}

			file.write(data);

			return true;
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "writeBinary", "Write", "Failed", FileNameJSON(filename));
			return false;
		}

	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  appendBinary
// Params:  const QString& filename
// Params:  const QByteArray& data
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 

bool XFile::appendBinary(const QString& filename, const QByteArray& data, bool run, bool ignorelog)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "appendBinary", "Trace", "Ok", FileNameJSON(filename));
	}

	if (run)
	{
		concurrent([filename, data,ignorelog]
			{
				XFileBlock block(alias(filename), true);

				try
				{

					mkFilePath(filename);

					QFile file(alias(filename));
					if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
					{
						if (!ignorelog)
						{
							X_WARNINGJSON("CoreStorage", "XFile", "appendBinary", "Open", "Failed", FileNameJSON(filename));
						}
						return;
					}
					file.write(data);
				}
				catch (...)
				{
					if (!ignorelog)
					{
						X_WARNINGJSON("CoreStorage", "XFile", "writeBinary", "Write", "Failed", FileNameJSON(filename));
					}
				}
			});

		return true;
	}
	else
	{
		XFileBlock block(alias(filename), true);
		try
		{
			mkFilePath(filename);

			QFile file(alias(filename));

			if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
			{
				if (!ignorelog)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "appendBinary", "Open", "Failed", FileNameJSON(filename));
				}
				return false;
			}

			file.write(data);
			return true;

		}
		catch (...)
		{
			if (!ignorelog)
			{
				X_WARNINGJSON("CoreStorage", "XFile", "appendBinary", "Write", "Failed", FileNameJSON(filename));
			}
			return false;
		}

	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readDocument
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

QJsonDocument XFile::readDocument(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readDocument", "Trace", "Ok", FileNameJSON(filename));
	}

	return QJsonDocument::fromJson(readBinary(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readDocumentCache
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 
const QJsonDocument& XFile::readDocumentCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readDocumentCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return XFileCache::readDocument(alias(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeDocument
// Params:  const QString& filename
// Params:  const QJsonDocument& data
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 
bool XFile::writeDocument(const QString& filename, const QJsonDocument& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeDocument", "Trace", "Ok", FileNameJSON(filename));
	}

	return writeBinary(filename,data.toJson(),run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    28.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readDocumentMap
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
QMap<QString, QMap<QString, QVariant>> XFile::readDocumentMap(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readDocumentMap", "Trace", "Ok", FileNameJSON(filename));
	}

	//O_STACK(XFile, readDocumentMap);

	const QJsonDocument& doc = XFile::readDocument(filename);
	QMap<QString, QMap<QString, QVariant>> map;

	if (doc.isEmpty())
		return map;

	if (doc.isObject())
	{
		const QJsonObject& rootobject = doc.object();
		for (const QString& key : rootobject.keys())
		{
			//O_PARAM(key);
			if (rootobject[key].isObject())
			{
				QJsonObject jsonobj = rootobject[key].toObject();
				for (const QString& childkey : jsonobj.keys())
				{
					map[key][childkey] = jsonobj.value(childkey).toVariant();
					//O_PARAM(childkey);
					//O_PARAM(map[key][childkey].toString());
				}
			}
		}
	}
	return map;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    28.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readDocumentMapCache
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
QMap<QString, QMap<QString, QVariant>> XFile::readDocumentMapCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readDocumentMapCache", "Trace", "Ok", FileNameJSON(filename));
	}

	const QJsonDocument& doc = XFile::readDocumentCache(filename);
	QMap<QString,QMap<QString,QVariant>> map;

	if (doc.isEmpty())
		return map;

	if (doc.isObject())
	{
		const QJsonObject& rootobject=doc.object();
		for (const QString& key:rootobject.keys())
		{
			if (rootobject[key].isObject())
			{
				QJsonObject jsonobj = rootobject[key].toObject();
				for (const QString& childkey : jsonobj.keys())
				{
					map[key][childkey]=jsonobj.value(childkey).toVariant();
				}
			}
		}
	}
	return map;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    28.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeDocumentMapMap
// Params:  const QString& filename
// Params:  const QMap<QString, QMap<QString, QVariant>>& data
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XFile::writeDocumentMap(const QString& filename, const QMap<QString, QMap<QString, QVariant>>& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeDocumentMap", "Trace", "Ok", FileNameJSON(filename));
	}

	//O_STACK(XFile,writeDocumentMap)

	QJsonObject rootobj;
	for (const QString& key : data.keys())
	{
		//O_PARAM(key);
		QJsonObject jsonobj;
		for (const QString& childkey:data[key].keys())
		{
			//O_PARAM(childkey);
			//O_PARAM(data[key][childkey].toString());
			jsonobj.insert(childkey, data[key][childkey].toJsonValue());
		}
		rootobj.insert(key, jsonobj);
	}

	QJsonDocument doc(rootobj);
	return writeDocument(filename, doc, run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readList
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 
QStringList XFile::readList(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readList", "Trace", "Ok", FileNameJSON(filename));
	}

	return read<QStringList>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readListCache
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 
QStringList XFile::readListCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readListCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return readCache<QStringList>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeList
// Params:  const QString& filename
// Params:  const QStringList& data
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XFile::writeList(const QString& filename, const QStringList& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeList", "Trace", "Ok", FileNameJSON(filename));
	}

	return write<QStringList>(filename, data, run);
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readMap
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

QMap<QString,QString> XFile::readMap(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readMap", "Trace", "Ok", FileNameJSON(filename));
	}

	return read<QMap<QString, QString>>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readMapCache
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

QMap<QString,QString> XFile::readMapCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readMapCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return readCache<QMap<QString, QString>>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeMap
// Params:  const QString& filename
// Params:  const QMap<QString, QString>& data
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XFile::writeMap(const QString& filename, const QMap<QString, QString>& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeMap", "Trace", "Ok", FileNameJSON(filename));
	}

	return write<QMap<QString, QString>>(filename, data, run);
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readSet
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
QSet<QString> XFile::readSet(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readSet", "Trace", "Ok", FileNameJSON(filename));
	}

	return read<QSet<QString>>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  readSetCache
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
QSet<QString> XFile::readSetCache(const QString& filename)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "readSetCache", "Trace", "Ok", FileNameJSON(filename));
	}

	return readCache<QSet<QString>>(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  writeSet
// Params:  const QString& filename
// Params:  const QSet<QString>& data
// Params:  bool run
//----------------------------------------------------------------------------- 
bool XFile::writeSet(const QString& filename, const QSet<QString>& data, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "writeSet", "Trace", "Ok", FileNameJSON(filename));
	}

	return write<QSet<QString>>(filename, data, run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  copyFile
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
//----------------------------------------------------------------------------- 

bool XFile::copy(const QString& srcfilename, const QString& dstfilename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "copy", "Trace", "Ok", FileNamesJSON(srcfilename,dstfilename));
	}

	//O_STACK(XFile, copy);
	//O_PARAM(srcfilename);
	//O_PARAM(dstfilename);

	if (srcfilename.isEmpty())
		return false;

	QFileInfo srcinfo(alias(srcfilename));
	srcinfo.refresh();
	QFileInfo dstinfo(alias(dstfilename));
	dstinfo.refresh();

	if (srcinfo.lastModified() == dstinfo.lastModified() &&
		srcinfo.birthTime() == dstinfo.birthTime() &&
		srcinfo.size() == dstinfo.size())
	{
		X_WARNINGJSON("CoreStorage", "XFile", "copy", "Equal", "Skipped", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (!exists(srcfilename))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "copy", "Source", "Missing", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (run)
	{

		concurrent([srcfilename, dstfilename]
			{
				XFileBlock srcblock(alias(srcfilename));
				XFileBlock dstblock(alias(dstfilename),true);

				try {
					mkFilePath(dstfilename);
					QFile::copy(alias(srcfilename), alias(dstfilename));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "copy", "Copy","Failed",FileNamesJSON(srcfilename,dstfilename));
				}

			});

		return true;
	}
	else
	{
		XFileBlock srcblock(alias(srcfilename));
		XFileBlock dstblock(alias(dstfilename),true);

		try {
			mkFilePath(dstfilename);
			return QFile::copy(alias(srcfilename), alias(dstfilename));
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "copy", "Copy", "Failed", FileNamesJSON(srcfilename, dstfilename));
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    12.01.2023
// Context: DefaultNamespace
// Class:   XFile
// Method:  rename
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 
bool XFile::rename(const QString& srcfilename, const QString& dstfilename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "rename", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	XFileBlock srcblock(alias(srcfilename));
	XFileBlock dstblock(alias(dstfilename), true);

	if (!exists(alias(srcfilename)))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "rename", "Source", "Missing",FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	try {
		return QFile::rename(alias(srcfilename), alias(dstfilename));
	}
	catch (...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "rename", "Rename", "Failed", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    12.01.2023
// Context: DefaultNamespace
// Class:   XFile
// Method:  move
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 
bool XFile::move(const QString& srcfilename, const QString& dstfilename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "move", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	if (srcfilename.isEmpty())
		return false;

	QFileInfo srcinfo(alias(srcfilename));
	srcinfo.refresh();
	QFileInfo dstinfo(alias(dstfilename));
	dstinfo.refresh();

	if (srcinfo.lastModified() == dstinfo.lastModified() &&
		srcinfo.birthTime() == dstinfo.birthTime() &&
		srcinfo.size() == dstinfo.size())
		return false;

	if (!exists(alias(srcfilename)))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "move", "Source", "Missing", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (run)
	{
		
		concurrent([srcfilename, dstfilename]
			{
				XFileBlock srcblock(alias(srcfilename));
				XFileBlock dstblock(alias(dstfilename), true);

				try {
					mkFilePath(dstfilename);
					QFile::copy(alias(srcfilename), alias(dstfilename));
					QFile::remove(alias(srcfilename));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "move", "Moving", "Failed", FileNamesJSON(srcfilename, dstfilename));
				}
			});

		return true;
	}
	else
	{
		XFileBlock srcblock(alias(srcfilename));
		XFileBlock dstblock(alias(dstfilename), true);

		try
		{
			mkFilePath(dstfilename);

			QFile::copy(alias(srcfilename), alias(dstfilename));
			QFile::remove(alias(srcfilename));
			return true;
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "move", "Moving", "Failed", FileNamesJSON(srcfilename, dstfilename));
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  removeFile
// Params:  const QString& filename
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 
bool XFile::remove(const QString& filename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "remove", "Trace", "Ok", FileNameJSON(filename));
	}

	//O_STACK(XFile, remove);
	//O_PARAM(filename);
	if (!exists(alias(filename)))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "remove", "Filename", "Missing", FileNameJSON(filename));
		return false;
	}

	if (run)
	{

		concurrent([filename]
			{
				XFileBlock srcblock(alias(filename),true);

				try 
				{
					XFileAlias::remove(alias(filename));
					QFile::remove(alias(filename));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "remove", "Remove", "Failed", FileNameJSON(filename));
				}
			});

		return true;
	}
	else
	{
		XFileBlock srcblock(alias(filename),true);

		try {
			XFileAlias::remove(alias(filename));
			return QFile::remove(alias(filename));
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "remove", "Remove", "Failed", FileNameJSON(filename));
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  remove
// Params:  const QStringList& filenames
// Params:  bool run
//----------------------------------------------------------------------------- 

bool XFile::remove(const QStringList& filenames, bool run)
{
	bool ret = true;
	for (const QString& filename : filenames)
		if (!XFile::remove(filename, run))
			ret = false;
	return ret;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    13.01.2023
// Context: DefaultNamespace
// Class:   XFile
// Method:  unique
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
QString XFile::unique(const QString& filename)
{

	if (!QFile::exists(filename))
		return filename;

	QFileInfo fileinfo(filename);

	QString renamefilename(fileinfo.baseName());

	qint32 pos = renamefilename.lastIndexOf('_');
	if (pos >= 0)
		renamefilename = renamefilename.mid(0, pos-1);

	QString basefilename= renamefilename;

	qint32 number=1;
	while (QFile::exists(fileinfo.path()+QDir::separator()+ renamefilename+"."+fileinfo.suffix()))
	{
		renamefilename = basefilename + QString("_%1").arg(number++, 4, 10, QChar('0'));
	}

	return fileinfo.path() + QDir::separator() + renamefilename + "." + fileinfo.suffix();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  exists
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

bool XFile::exists(const QString& filename)
{
	return QFile::exists(alias(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  equal
// Params:  const QString& filename
// Params:  const QString& filename2
// Params:  
//----------------------------------------------------------------------------- 

bool XFile::equal(const QString& filename, const QString& filename2)
{
	QFileInfo fileinfo(alias(filename));
	fileinfo.refresh();
	QFileInfo fileinfo2(alias(filename2));
	fileinfo2.refresh();

	if (fileinfo.size() != fileinfo2.size())
		return false;

	if (fileinfo.lastModified() == fileinfo2.lastModified() &&
		fileinfo.birthTime() == fileinfo2.birthTime())
		return true;

	return XFile::readBinary(filename)==XFile::readBinary(filename2);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  contains
// Params:  const QString& filename
// Params:  const QString& search
// Params:  Qt::CaseSensitivity sensitivity
//----------------------------------------------------------------------------- 

bool XFile::contains(const QString& filename, const QString& search, Qt::CaseSensitivity sensitivity)
{
	return XFile::read(filename).contains(search, sensitivity);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  contains
// Params:  const QString& filename
// Params:  const QRegularExpression& search
//----------------------------------------------------------------------------- 

bool XFile::contains(const QString& filename, const QRegularExpression& search)
{
	return XFile::read(filename).contains(search);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  compressFile
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
//----------------------------------------------------------------------------- 
bool XFile::compress(const QString& srcfilename, const QString& dstfilename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "compress", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	if (!exists(srcfilename))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "compress", "Source", "Missing", FileNamesJSON(srcfilename,dstfilename));
		return false;
	}

	if (run)
	{
		concurrent([srcfilename, dstfilename]
			{
				try 
				{
					writeBinary(dstfilename, qCompress(readBinary(srcfilename)));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "compress", "Compress", "Failed", FileNamesJSON(srcfilename, dstfilename));
				}
			});

		return true;
	}
	else
	{
		try 
		{
			return writeBinary(dstfilename, qCompress(readBinary(srcfilename)));
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "compress", "Compress", "Failed", FileNamesJSON(srcfilename, dstfilename));
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  unCompressFile
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
//----------------------------------------------------------------------------- 

bool XFile::unCompress(const QString& srcfilename, const QString& dstfilename, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "unCompress", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	if (!exists(srcfilename))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "unCompress", "Source", "Missing", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (run)
	{
		
		concurrent([srcfilename,dstfilename]
			{
				try
				{
					writeBinary(dstfilename, qUncompress(readBinary(srcfilename)));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "unCompress", "UnCompress", "Failed", FileNamesJSON(srcfilename, dstfilename));
				}
			});

		return true;
	}
	else
	{
		try
		{
			return writeBinary(dstfilename, qUncompress(readBinary(srcfilename)));
		}
		catch (...)
		{
			X_WARNINGJSON("CoreStorage", "XFile", "unCompress", "UnCompress", "Failed", FileNamesJSON(srcfilename, dstfilename));
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  compressFile
// Params:  const QString& srcfilename
//----------------------------------------------------------------------------- 
bool XFile::compress(const QString& srcfilename, bool run)
{
	return compress(srcfilename, srcfilename+QString(".z"),run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  unCompressFile
// Params:  const QString& srcfilename
// Params:  
//----------------------------------------------------------------------------- 
bool XFile::unCompress(const QString& srcfilename, bool run)
{
	return unCompress(srcfilename , QString(srcfilename).remove(".z"),run);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  crypt
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
// Params:  const QString& key
// Params:  bool run
// Params:  
//----------------------------------------------------------------------------- 

bool XFile::crypt(const QString& srcfilename, const QString& dstfilename, const QString& key, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "crypt", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	if (!exists(srcfilename))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "crypt", "Source", "Missing", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (run)
	{
		concurrent([srcfilename, dstfilename, key]
			{
				try
				{
					XFile::writeBinary(dstfilename, XCrypt::crypt(XFile::readBinary(srcfilename), key));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "crypt", "Crypt", "Failed", FileNamesJSON(srcfilename, dstfilename));
				}
			});

		return true;
	}

	try
	{
		return XFile::writeBinary(dstfilename,XCrypt::crypt(XFile::readBinary(srcfilename),key));
	}
	catch (...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "crypt", "Crypt", "Failed", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  unCrypt
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
// Params:  const QString& key
// Params:  bool run
//----------------------------------------------------------------------------- 

bool XFile::unCrypt(const QString& srcfilename, const QString& dstfilename, const QString& key, bool run)
{
	if (XInject::isTrace("FILE"))
	{
		X_LOGJSON("STORAGE", "CoreStorage", "XFile", "unCrypt", "Trace", "Ok", FileNamesJSON(srcfilename, dstfilename));
	}

	if (!exists(srcfilename))
	{
		X_WARNINGJSON("CoreStorage", "XFile", "unCrypt", "Source", "Missing", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}

	if (run)
	{

		concurrent([srcfilename, dstfilename,key]
			{
				try
				{
					XFile::writeBinary(dstfilename, XCrypt::unCrypt(XFile::readBinary(srcfilename), key));
				}
				catch (...)
				{
					X_WARNINGJSON("CoreStorage", "XFile", "unCrypt", "UnCrypt", "Failed", FileNamesJSON(srcfilename, dstfilename));
				}

			});

		return true;
	}

	try
	{
		return XFile::writeBinary(dstfilename, XCrypt::unCrypt(XFile::readBinary(srcfilename), key));
	}
	catch (...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "unCrypt", "UnCrypt", "Failed", FileNamesJSON(srcfilename, dstfilename));
		return false;
	}
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    03.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  wait
//----------------------------------------------------------------------------- 

void XFile::wait()
{
	for (auto future : m_Futures) 
	{
		future.waitForFinished();
	}
	m_Futures.clear();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.02.2023
// Context: DefaultNamespace
// Class:   XFile
// Method:  FileNameJSON
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

QJsonObject XFile::FileNameJSON(const QString& filename)
{
	QJsonObject obj;

	QString aliasfilename(alias(filename));

	if (aliasfilename == filename)
	{
		obj["Filename"] = filename;
	}
	else
	{
		obj["FilenameAlias"] = filename;
		obj["Filename"] = aliasfilename;
	}
	
	return obj;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.02.2023
// Context: DefaultNamespace
// Class:   XFile
// Method:  FileNamesJSON
// Params:  const QString& srcfilename
// Params:  const QString& dstfilename
// Params:  
//----------------------------------------------------------------------------- 

QJsonObject XFile::FileNamesJSON(const QString& srcfilename, const QString& dstfilename)
{
	QJsonObject obj;

	QString srcaliasfilename(alias(srcfilename));

	if (srcaliasfilename == srcfilename)
	{
		obj["SrcFilename"] = srcfilename;
	}
	else
	{
		obj["SrcFilenameAlias"] = srcfilename;
		obj["SrcFilename"] = srcaliasfilename;
	}

	QString dstaliasfilename(alias(dstfilename));
	if (dstaliasfilename == dstfilename)
	{
		obj["DstFilename"] = dstfilename;
	}
	else
	{
		obj["DstFilenameAlias"] = dstfilename;
		obj["DstFilename"] = dstaliasfilename;
	}

	return obj;
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  append
// Params:  QFuture<void>& future
//----------------------------------------------------------------------------- 
void XFile::append(QFuture<void> future)
{
	m_Futures.append(future);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XFile
// Method:  mkPath
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 

// This code defines a function named "mkFilePath" in the "XFile" class, which
// takes a string parameter "filename" and returns a boolean value. 

// It tries to create a file path using the given "filename" with the 
// following steps :

// 1. It retrieves file information using "alias(filename)" and stores it
//  in the "info" object.

// 2. It attempts to create a path using the "mkpath" method on a "QDir"
//  object created with the path from the "info" object.

// 3. If the path is successfully created, it returns true.

// If any exception occurs during this process(caught by the catch block),
// it logs a warning message with the "X_WARNINGJSON" macro, which includes
// details like "CoreStorage", "XFile", "mkFilePath", and the filename.
// In the case of an exception, it returns false.

bool XFile::mkFilePath(const QString& filename)
{
	try
	{
		QFileInfo info(alias(filename));
		return QDir(info.path()).mkpath(".");
	}
	catch(...)
	{
		X_WARNINGJSON("CoreStorage", "XFile", "mkFilePath", "MakeFilePath","Failed", FileNameJSON(filename));
	}

	return false;
}

