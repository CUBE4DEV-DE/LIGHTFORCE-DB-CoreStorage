#pragma once
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
// Date:    19.10.2022
// Context: DefaultNamespace
// Class:   XCrypt
// Method:  XCrypt
//----------------------------------------------------------------------------- 
	
class CORESTORAGE_EXPORT XCrypt
{
public:
	static QByteArray crypt(const QByteArray& src, const QString& key);
	static QByteArray unCrypt(const QByteArray& src, const QString& key); 
private:
	static qint64 hash(const QString& str);
};

 
};
using namespace CoreStorage;
