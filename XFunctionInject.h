#pragma once

#include "CoreStorageGlobal.h"
#include <QMap>
#include <QMultiMap>
#include <qset.h>

namespace CoreStorage
{
	typedef std::function<QString(const QString& content)> XFunctionString;

	//-----------------------------------------------------------------------------
	// Author:  Tobias Post
	// Company: CUBE4DEV GmbH
	// Date:    16.02.2023
	// Context: DefaultNamespace
	// Class:   XFunctionInject
	// Method:  XFunctionInject
	//----------------------------------------------------------------------------- 

	template <class FUNCTION> class XFunctionInject
	{
	public:
		void inject(const QString& name, FUNCTION function)
		{
			m_Functions[name] = function;
		}
		void inject(FUNCTION function)
		{
			m_Functions[""] = function;
		}
		void injects(const QString& name, FUNCTION function)
		{
			m_MultiFunctions.insert(name, function);
		}
		void injects(FUNCTION function)
		{
			m_MultiFunctions.insert("", function);
		}

		bool hasFunction(const QString& name)
		{
			if (m_Functions.contains(name))
				return true;
			return false;
		}
		bool hasFunctions(const QString& name)
		{
			if (m_MultiFunctions.contains(name))
				return true;
			return false;
		}

	protected:
		FUNCTION function(const QString& name)
		{
			if (m_Functions.contains(name))
				return m_Functions[name];
			return Q_NULLPTR;
		}
		FUNCTION function()
		{
			if (m_Functions.contains(QString("")))
				return m_Functions[QString("")];
			return Q_NULLPTR;
		}

		QList<FUNCTION> functions(const QString& name)
		{
			if (m_MultiFunctions.contains(name))
				return m_MultiFunctions.values(name);
			return QList<FUNCTION>();
		}
		QList<FUNCTION> functions()
		{
			if (m_MultiFunctions.contains(QString("")))
				return m_MultiFunctions.values(QString(""));
			return QList<FUNCTION>();
		}
	private:
		QMap<QString, FUNCTION> m_Functions;
		QMultiMap<QString, FUNCTION> m_MultiFunctions;
	};

	//-----------------------------------------------------------------------------
	// Author:  Tobias Post
	// Company: CUBE4DEV GmbH
	// Date:    16.02.2023
	// Context: DefaultNamespace
	// Class:   XFunctionInject
	// Method:  XStringInject
	//----------------------------------------------------------------------------- 
	class CORESTORAGE_EXPORT XStringInject :public  XFunctionInject<XFunctionString>
	{
	public:
		QString call(const QString& content)
		{
			if (function())
				return function()(content);
			return QString();
		}
		QString call(const QString& name, const QString& content)
		{
			if (function(name))
				return function(name)(content);
			return content;
		}
	};

	//-----------------------------------------------------------------------------
	// Author:  Tobias Post
	// Company: CUBE4DEV GmbH
	// Date:    16.02.2023
	// Context: DefaultNamespace
	// Class:   XFunctionInject
	// Method:  XInject
	//----------------------------------------------------------------------------- 

	class CORESTORAGE_EXPORT XInject
	{
	public:
		static void inject(const QString& name, XFunctionString function);
		static QString call(const QString& name, const QString& content);
		static bool hasStringFunction(const QString& name);

		static void Trace(const QString& name);
		static void ClearTrace(const QString& name=QString());
		static bool isTrace(const QString& name)
		{
			return m_Trace.contains(name);
		}
	private:
		static XStringInject m_StringInjects;
		static QSet<QString> m_Trace;
	};


};
using namespace CoreStorage;
