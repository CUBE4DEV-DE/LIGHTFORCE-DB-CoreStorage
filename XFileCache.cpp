#include "XFileCache.h"

#include "XFileBlock.h"
#include "XFile.h"
#include "XFunctionInject.h"


// 100 Megabytes

QCache<QString, XFileCache> XFileCache::m_TextCache(100*1000);
QCache<QString, XFileCache> XFileCache::m_BinaryCache(100*1000);
QCache<QString, XFileCache> XFileCache::m_DocumentCache(100 * 1000);

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  XCache
//----------------------------------------------------------------------------- 
XFileCache::XFileCache()
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
XFileCache::~XFileCache()
{
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  fill
// Params:  QString& filename
//----------------------------------------------------------------------------- 

void XFileCache::fill(const QString& filename)
{
	m_FileName = filename;
	m_FileInfo.setFile(m_FileName);
	m_FileInfo.refresh();
	m_Text= XFile::read(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  fillBinary
// Params:  QString& filename
//----------------------------------------------------------------------------- 

void XFileCache::fillBinary(const QString& filename)
{
	{
		XFileBlock block(filename);
		m_FileName = filename;
		m_FileInfo.setFile(m_FileName);
		m_FileInfo.refresh();
	}
	m_ByteArray = XFile::readBinary(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XFileCache
// Method:  fillDocument
// Params:  const QString& filename
//-----------------------------------------------------------------------------

void XFileCache::fillDocument(const QString& filename)
{
	m_FileName = filename;
	m_FileInfo.setFile(m_FileName);
	m_FileInfo.refresh();

	if (XInject::hasStringFunction(m_FileInfo.suffix().toLower()))
	{
		QString json = XInject::call(m_FileInfo.suffix().toLower(),QString(XFile::readBinary(filename)));
		XFile::write(filename + ".json", json);
		m_Document = QJsonDocument::fromJson(json.toUtf8());                                   
	}
	else
	{
		m_Document = QJsonDocument::fromJson(XFile::readBinary(filename));
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  isChanged
// Params:  
//----------------------------------------------------------------------------- 
bool XFileCache::isChanged()
{
	XFileBlock block(m_FileName);
	QFileInfo fileInfo(m_FileName);
	fileInfo.refresh();

	if (m_FileInfo.size() != fileInfo.size())
		return true;

	if (m_FileInfo.lastModified() < fileInfo.lastModified().addSecs(-2))
		return true;
	
	return false;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  size
// Params:  
//----------------------------------------------------------------------------- 
qint64 XFileCache::size()
{
	return m_FileInfo.size();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  setMemory
// Params:  qint32 megabyte
//----------------------------------------------------------------------------- 

void XFileCache::setMemory(qint32 megabyte)
{
	m_TextCache.setMaxCost(megabyte* 1000);
	m_BinaryCache.setMaxCost(megabyte * 1000);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  read
// Params:  const QString& filename
// Params:  
//----------------------------------------------------------------------------- 
const QString& XFileCache::read(const QString& filename)
{
	XFileCache* textcache = Q_NULLPTR;
	if (m_TextCache.contains(filename))
	{
		textcache = m_TextCache[filename];
		if (textcache->isChanged())
			textcache->fill(filename);
		return textcache->text();
	}

	textcache = new XFileCache();
	textcache->fill(filename);
	m_TextCache.insert(filename, textcache,textcache->size()/1000);

	return textcache->text();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCache
// Method:  readBinary
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
const QByteArray& XFileCache::readBinary(const QString& filename)
{
	XFileCache* binarycache = Q_NULLPTR;
	if (m_BinaryCache.contains(filename))
	{
		binarycache = m_BinaryCache[filename];
		if (binarycache->isChanged())
			binarycache->fillBinary(filename);
		return binarycache->byteArray();
	}
	
	binarycache = new XFileCache();
	binarycache->fillBinary(filename);
	m_BinaryCache.insert(filename, binarycache, binarycache->size() / 1000);

	return binarycache->byteArray();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XFileCache
// Method:  readDocument
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
const QJsonDocument& XFileCache::readDocument(const QString& filename)
{
	XFileCache* documentcache = Q_NULLPTR;
	if (m_DocumentCache.contains(filename))
	{
		documentcache = m_DocumentCache[filename];
		if (documentcache->isChanged())
			documentcache->fillDocument(filename);
		return documentcache->document();
	}

	documentcache = new XFileCache();
	documentcache->fillDocument(filename);
	m_DocumentCache.insert(filename, documentcache, documentcache->size() / 1000);

	return documentcache->document();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFileCache
// Method:  isChanged
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
bool XFileCache::isChanged(const QString& filename)
{
	XFileCache* textcache = Q_NULLPTR;
	if (m_TextCache.contains(filename))
	{
		textcache = m_TextCache[filename];
		return textcache->isChanged();
	}

	return true;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XFileCache
// Method:  isChangedBinary
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
bool XFileCache::isChangedBinary(const QString& filename)
{
	XFileCache* binarycache = Q_NULLPTR;
	if (m_BinaryCache.contains(filename))
	{
		binarycache = m_BinaryCache[filename];
		return binarycache->isChanged();
	}

	return true;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XFileCache
// Method:  isChangedDocument
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
bool XFileCache::isChangedDocument(const QString& filename)
{
	XFileCache* documentcache = Q_NULLPTR;
	if (m_DocumentCache.contains(filename))
	{
		documentcache = m_DocumentCache[filename];
		return documentcache->isChanged();
	}
	return true;
}
