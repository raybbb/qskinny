/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskMetaInvokable.h>

class QObject;

class Callback
{
  public:
    Callback();

    Callback( const QObject*, const QskMetaFunction& );
    Callback( const QObject*, const QMetaMethod& );
    Callback( const QObject*, const QMetaProperty& );

    Callback( const QObject*, const char* methodName );

    void invoke( void* args[], Qt::ConnectionType );

    const QObject* context() const;
    const QskMetaInvokable& invokable() const;

  private:
    QObject* m_context;
    QskMetaInvokable m_invokable;
};

inline const QObject* Callback::context() const
{
    return m_context;
}

inline const QskMetaInvokable& Callback::invokable() const
{
    return m_invokable;
}
