/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultiMESSAGING module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMESSAGINGGLOBAL_H
#define QMESSAGINGGLOBAL_H

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
#  if defined(QT_NODLL)
#    undef QT_MAKEDLL
#    undef QT_DLL
#  elif defined(QT_MAKEDLL)
#    if defined(QT_DLL)
#      undef QT_DLL
#    endif
#    if defined(QT_BUILD_MESSAGING_LIB)
#      define Q_MESSAGING_EXPORT Q_DECL_EXPORT
#    else
#      define Q_MESSAGING_EXPORT Q_DECL_IMPORT
#    endif
#  elif defined(QT_DLL) /* use a Qt DLL library */
#    define Q_MESSAGING_EXPORT Q_DECL_IMPORT
#  endif
#else
#endif

#if !defined(Q_MESSAGING_EXPORT)
#  if defined(QT_SHARED)
#    define Q_MESSAGING_EXPORT Q_DECL_EXPORT
#  else
#    define Q_MESSAGING_EXPORT
#  endif
#endif

// A variant of Q_GLOBAL_STATIC for use in class scope
#define Q_SCOPED_STATIC_DECLARE(TYPE, NAME)                      \
    static TYPE *NAME();

#if defined(QT_NO_THREAD)
#  define Q_SCOPED_STATIC_DEFINE(TYPE, SCOPE, NAME)              \
    TYPE *SCOPE::NAME()                                          \
    {                                                            \
        static TYPE this_##NAME;                                 \
        static QGlobalStatic<TYPE > global_##NAME(&this_##NAME); \
        return global_##NAME.pointer;                            \
    }
#  else
#  define Q_SCOPED_STATIC_DEFINE(TYPE, SCOPE, NAME)                     \
    Q_GLOBAL_STATIC_INIT(TYPE, NAME);                                   \
    TYPE *SCOPE::NAME()                                                 \
    {                                                                   \
        if (!this_##NAME.pointer && !this_##NAME.destroyed) {           \
            TYPE *x = new TYPE;                                         \
            if (!this_##NAME.pointer.testAndSetOrdered(0, x))           \
                delete x;                                               \
            else                                                        \
                static QGlobalStaticDeleter<TYPE > cleanup(this_##NAME); \
        }                                                               \
        return this_##NAME.pointer;                                     \
    }
#endif

#endif // QMESSAGINGGLOBAL_H
