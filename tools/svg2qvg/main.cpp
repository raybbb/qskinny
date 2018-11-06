/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#if defined(QSK_STANDALONE)
#include <QskGraphic.cpp>
#include <QskRgbValue.cpp>
#include <QskColorFilter.cpp>
#include <QskPainterCommand.cpp>
#include <QskGraphicPaintEngine.cpp>
#include <QskGraphicIO.cpp>
#else
#include <QskGraphicIO.h>
#include <QskGraphic.h>
#endif

#include <QGuiApplication>
#include <QSvgRenderer>
#include <QPainter>
#include <QDebug>

static void usage( const char* appName )
{
    qWarning() << "usage: " << appName << "svgfile qvgfile";
}

int main( int argc, char* argv[] )
{
    if ( argc != 3 )
    {
        usage( argv[0] );
        return -1;
    }

    // we need an application object, when the SVG loads fonts
    QGuiApplication app( argc, argv );

    QSvgRenderer renderer;
    if ( !renderer.load( QString( argv[1] ) ) )
        return -2;

    QskGraphic graphic;

    QPainter painter( &graphic );
    renderer.render( &painter );
    painter.end();

    QskGraphicIO::write( graphic, argv[2] );

    return 0;
}

