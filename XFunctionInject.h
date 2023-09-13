#pragma once

#include "CoreStorageGlobal.h"
#include <QMap>
#include <QMultiMap>
#include <qset.h>

// The file XFunctionInject.h defines three classes: XFunctionInject, XStringInject,
// and XInject with the purpose of providing functionality for injecting and calling
// stored functions in a generic way. The main idea is to provide a template that can
// be used for defining functionality for injecting and calling stored functions.
// XFunctionInject provides a way to inject and call stored functions of arbitrary
// types, XStringInject provides a way to inject and call stored functions specifically
// of type XFunctionString, which is defined as std::function<QString(const QString&)>,
// while XInject is a collection of utilities that provide functionalities like
// injecting new functions, invoking functions by name, and tracing function calls.
// Additionally, XInject has a isTrace method that verifies if a specific function
// call was traced and a ClearTrace method that removes a specific function call
// from the trace set. Overall, this file is a part of the CoreStorage global
// namespace and provides an important template for injecting and calling
// shared libraries in Qt using generic and string function overrides.

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
	// Description: The overall purpose of the XFunctionInject class is to provide
	//	a template for defining functionality for injecting and calling stored
	//	functions. Specifically, it contains methods such as inject, call,
	//	hasFunction, Trace, and ClearTrace. These methods respectively inject
	//	a new function into an XStringInject object, invoke a function in an
	//	XStringInject object, return whether the provided function name is an
	//	injected function, add a function name to a QSet object for tracking,
	//	and clear a QSet object and remove a specific function name from it.
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
	// Description: The purpose of the XStringInject class is to provide a
	//	template for defining functionality for injecting and calling stored
	//	functions. Specifically, it contains methods such as inject, call,
	//	hasFunction, Trace, and ClearTrace. These methods respectively
	//	inject a new function into an XStringInject object, invoke a
	//	function in an XStringInject object, return whether the provided
	//	function name is an injected function, add a function name to a
	//	QSet object for tracking, and clear a QSet object and remove a
	//	specific function name from it.
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
	// Class:   XInject
	// Description: The purpose of these methods is to provide functionality for
	//	injecting and calling stored functions. The inject method injects a new
	//	function into the XStringInject object using a given name and function.
	//	The call method invokes the function in XStringInject object by the
	//	given name using the provided content. hasStringFunction is a utility
	//	method that returns a boolean result indicating whether the provided
	//	function name is an injected function. Trace and ClearTrace are methods
	//	for tracking certain function calls in a class-wide QSet object. The
	//	Trace method adds a provided function name to the QSet object and
	//	ClearTrace method clears the QSet object and removes the provided
	//	function name from the object if requested.
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
