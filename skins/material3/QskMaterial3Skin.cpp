/******************************************************************************
 * QSkinny - Copyright (C) 2022 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskMaterial3Skin.h"

#include <QskSkinHintTableEditor.h>

#include <QskBox.h>
#include <QskCheckBox.h>
#include <QskComboBox.h>
#include <QskColorFilter.h>
#include <QskDialogButtonBox.h>
#include <QskFocusIndicator.h>
#include <QskFunctions.h>
#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicProvider.h>
#include <QskInputPanelBox.h>
#include <QskListView.h>
#include <QskMenu.h>
#include <QskPageIndicator.h>
#include <QskPushButton.h>
#include <QskProgressBar.h>
#include <QskRadioBox.h>
#include <QskScrollView.h>
#include <QskSegmentedBar.h>
#include <QskSeparator.h>
#include <QskShadowMetrics.h>
#include <QskSlider.h>
#include <QskSpinBox.h>
#include <QskStandardSymbol.h>
#include <QskSubWindow.h>
#include <QskSwitchButton.h>
#include <QskSwitchButtonSkinlet.h>
#include <QskTabBar.h>
#include <QskTabButton.h>
#include <QskTabView.h>
#include <QskTextInput.h>
#include <QskTextLabel.h>
#include <QskVirtualKeyboard.h>

#include <QskAnimationHint.h>
#include <QskAspect.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskMargins.h>
#include <QskRgbValue.h>

#include <QskNamespace.h>
#include <QskPlatform.h>

#include <QGuiApplication>
#include <QScreen>

static const int qskDuration = 150;

namespace
{
    // see: https://en.wikipedia.org/wiki/Device-independent_pixel
    inline qreal dpToPixels( long double value )
    {
        static qreal factor = -1.0;

        if ( factor < 0.0 )
        {
            factor = 1.0;

            if ( const auto screen = QGuiApplication::primaryScreen() )
            {
                const qreal pdpi = screen->physicalDotsPerInch();
                qreal dpi;

                // calculate buckets according to https://developer.android.com/training/multiscreen/screendensities
                if( pdpi <= 140.0 )
                {
                    dpi = 120.0; // ldpi
                }
                else if( pdpi <= 200.0 )
                {
                    dpi = 160.0; // mdpi
                }
                else if( pdpi <= 280.0 )
                {
                    dpi = 240.0; // hdpi
                }
                else if( pdpi <= 400.0 )
                {
                    dpi = 320.0; // xhdpi
                }
                else if( pdpi <= 560.0 )
                {
                    dpi = 480.0; // xxhdpi
                }
                else
                {
                    dpi = 640.0; // xxxhdpi
                }

                factor = dpi / 160.0;
            }
        }

        return value * factor;
    }

    inline double operator ""_dp( long double value )
    {
        return dpToPixels( value );
    }

    inline double operator ""_dp( unsigned long long int value )
    {
        return dpToPixels( value );
    }

    class Editor : private QskSkinHintTableEditor
    {
      public:
        Editor( QskSkinHintTable* table, const QskMaterial3Theme& palette )
            : QskSkinHintTableEditor( table )
            , m_pal( palette )
        {
        }

        void setup();

      private:
        void setupBox();
        void setupCheckBox();
        void setupComboBox();
        void setupDialogButtonBox();
        void setupFocusIndicator();
        void setupInputPanel();
        void setupVirtualKeyboard();
        void setupListView();
        void setupMenu();
        void setupPageIndicator();
        void setupPopup();
        void setupProgressBar();
        void setupRadioBox();
        void setupPushButton();
        void setupScrollView();
        void setupSegmentedBar();
        void setupSeparator();
        void setupSubWindow();
        void setupSlider();
        void setupSpinBox();
        void setupSwitchButton();
        void setupTabButton();
        void setupTabBar();
        void setupTabView();
        void setupTextInput();
        void setupTextLabel();

        const QskMaterial3Theme& m_pal;
    };

    QFont createFont( const QString& name, int lineHeight,
        int size, qreal tracking, QFont::Weight weight )
    {
        QFont font( name, size );
        font.setPixelSize( lineHeight );

        if( !qskFuzzyCompare( tracking, 0.0 ) )
        {
            font.setLetterSpacing( QFont::AbsoluteSpacing, tracking );
        }

        font.setWeight( weight );

        return font;
    }

    inline QRgb flattenedColor(
        QRgb foregroundColor, QRgb backgroundColor, qreal ratio )
    {
        return QskRgb::interpolated( backgroundColor, foregroundColor, ratio );
    }

    inline QRgb stateLayerColor( QRgb rgb, qreal opacity )
    {
        return QskRgb::toTransparentF( rgb, opacity );
    }
}

void Editor::setup()
{
    setupBox();
    setupCheckBox();
    setupComboBox();
    setupDialogButtonBox();
    setupFocusIndicator();
    setupInputPanel();
    setupVirtualKeyboard();
    setupListView();
    setupMenu();
    setupPageIndicator();
    setupPopup();
    setupProgressBar();
    setupPushButton();
    setupRadioBox();
    setupScrollView();
    setupSegmentedBar();
    setupSeparator();
    setupSlider();
    setupSpinBox();
    setupSubWindow();
    setupSwitchButton();
    setupTabButton();
    setupTabBar();
    setupTabView();
    setupTextLabel();
    setupTextInput();
}

void Editor::setupCheckBox()
{
    // skin hints are ordered according to
    // https://m3.material.io/components/checkbox/specs

    using Q = QskCheckBox;

    setSpacing( Q::Panel, 40_dp );

    setStrutSize( Q::Box, 18_dp, 18_dp );
    setBoxShape( Q::Box, 2_dp );

    setBoxBorderColors( Q::Box, m_pal.onSurface );
#if 1
    // hack: if border metrics == box shape, alpha value will be discarded
    setBoxBorderMetrics( Q::Box, 1.99_dp );
#endif

    setGradient( Q::Box, m_pal.background ); // not mentioned in the specs, but needed for animation
    setGradient( Q::Box | Q::Checked, m_pal.primary );
    setBoxBorderMetrics( Q::Box | Q::Checked, 0 );

    setPadding( Q::Box, 3_dp ); // "icon size"

    setGraphicRole( Q::Indicator, QskMaterial3Skin::GraphicRoleOnPrimary );

    setBoxBorderColors( Q::Box | Q::Error, m_pal.error );

    setGradient( Q::Box | Q::Checked | Q::Error, m_pal.error );

    setGraphicRole( Q::Indicator | Q::Error, QskMaterial3Skin::GraphicRoleOnError );

    setStrutSize( Q::Ripple, 40_dp, 40_dp );
    setBoxShape( Q::Ripple, 100, Qt::RelativeSize );
    setGradient( Q::Ripple, Qt::transparent );

    setColor( Q::Text, m_pal.onBackground ); // not mentioned in the specs

    // States

    // 2. Disabled

    setBoxBorderColors( Q::Box | Q::Disabled, m_pal.onSurface38 );
    setBoxShape( Q::Box | Q::Disabled, 2_dp );

    setGradient( Q::Box | Q::Disabled | Q::Checked, m_pal.onSurface38 );
    setGradient( Q::Box | Q::Disabled | Q::Checked | Q::Error, m_pal.onSurface38 );

    setGraphicRole( Q::Indicator | Q::Disabled | Q::Checked, QskMaterial3Skin::GraphicRoleSurface );

    // 3. Hovered

    setGradient( Q::Ripple | Q::Hovered | Q::Checked, m_pal.primary8 );
    setGradient( Q::Ripple | Q::Hovered, m_pal.onSurface8 );
    setGradient( Q::Ripple | Q::Error | Q::Hovered, m_pal.error8 );
    setGradient( Q::Ripple | Q::Error | Q::Hovered | Q::Checked, m_pal.error8 );

    // 4. Focused

    setGradient( Q::Ripple | Q::Focused | Q::Checked, m_pal.primary12 );
    setGradient( Q::Ripple | Q::Focused, m_pal.onSurface12 );
    setGradient( Q::Ripple | Q::Error | Q::Focused, m_pal.error12 );
    setGradient( Q::Ripple | Q::Error | Q::Focused | Q::Checked, m_pal.error12 );

    // 5. Pressed

    setGradient( Q::Ripple | Q::Pressed, m_pal.primary12 );
    setGradient( Q::Ripple | Q::Pressed | Q::Checked, m_pal.primary12 );
    setGradient( Q::Ripple | Q::Hovered | Q::Pressed, m_pal.primary12 );
    setGradient( Q::Ripple | Q::Error | Q::Pressed, m_pal.error12 );
    setGradient( Q::Ripple | Q::Error | Q::Pressed | Q::Checked, m_pal.error12 );
}

void Editor::setupComboBox()
{
    using Q = QskComboBox;

    setStrutSize( Q::Panel,  -1.0, 56_dp );
    setPadding( Q::Panel, { 12_dp, 8_dp, 12_dp, 8_dp } );
    setGradient( Q::Panel, m_pal.surfaceVariant );
    setBoxShape( Q::Panel, m_pal.shapeExtraSmallTop );
    setBoxBorderMetrics( Q::Panel, { 0, 0, 0, 1_dp } );
    setBoxBorderColors( Q::Panel, m_pal.onSurfaceVariant );
    setSpacing( Q::Panel, 8_dp );

    const auto hoverColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.hoverOpacity );
    setGradient( Q::Panel | Q::Hovered, hoverColor );

    const auto focusColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.focusOpacity );
    setGradient( Q::Panel | Q::Focused, focusColor );

    const auto pressedColor = flattenedColor( m_pal.onSurfaceVariant,
        m_pal.surfaceVariant, m_pal.pressedOpacity );
    setGradient( Q::Panel | Q::Pressed, pressedColor );

    setStrutSize( Q::Graphic, 24_dp, 24_dp );
    setGraphicRole( Q::Graphic, QskMaterial3Skin::GraphicRoleOnSurface );

    setColor( Q::Text, m_pal.onSurface );
    setFontRole( Q::Text, QskMaterial3Skin::M3BodyMedium );

    setStrutSize( Q::OpenMenuGraphic, 12_dp, 12_dp );
    setGraphicRole( Q::OpenMenuGraphic, QskMaterial3Skin::GraphicRoleOnSurface );
    setAlignment( Q::OpenMenuGraphic, Qt::AlignRight | Qt::AlignVCenter );


    const auto disabledPanelColor = QskRgb::toTransparentF( m_pal.onSurface, 0.04 );
    setGradient( Q::Panel | Q::Disabled, disabledPanelColor );
    setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.onSurface38 );

    setGraphicRole( Q::Graphic, QskMaterial3Skin::GraphicRoleOnSurface38 );

    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    setGraphicRole( Q::OpenMenuGraphic, QskMaterial3Skin::GraphicRoleOnSurface38 );
}

void Editor::setupBox()
{
    using Q = QskBox;
    using A = QskAspect;

    setGradient( Q::Panel, m_pal.background );
    setGradient( Q::Panel | A::Header, m_pal.surface2 );
    setGradient( Q::Panel | A::Footer, m_pal.surface2 );
}

void Editor::setupPopup()
{
    using A = QskAspect;
    using Q = QskPopup;

    setFlagHint( Q::Overlay | A::Style, true );
    setGradient( Q::Overlay, stateLayerColor( m_pal.outline, 0.8 ) );
}

void Editor::setupMenu()
{
    using A = QskAspect;
    using Q = QskMenu;

    setBoxShape( Q::Panel, 4_dp );
    setBoxBorderMetrics( Q::Panel, 0 );
    setPadding( Q::Panel, 0 );

    setGradient( Q::Overlay, Qt::transparent );

    // The color here is primary with an opacity of 8% - we blend that
    // with the background, because we don't want the menu to have transparency:
    const auto panel = flattenedColor( m_pal.primary, m_pal.background, 0.08 );
    setGradient( Q::Panel, panel );

    setShadowMetrics( Q::Panel, m_pal.elevationLight2 );
    setShadowColor( Q::Panel, m_pal.shadow );

    setMetric( Q::Separator | A::Size, 1_dp );
    setBoxShape( Q::Separator, 0 );
    setBoxBorderMetrics( Q::Separator, 0 );
    setGradient( Q::Separator, m_pal.primary12 );

    setPadding( Q::Segment, 6_dp );
    setSpacing( Q::Segment, 5_dp );
    setGradient( Q::Segment, Qt::transparent );

    setGradient( Q::Cursor, m_pal.primary12 );

    setPadding( Q::Graphic, 7_dp );
    setStrutSize( Q::Graphic, 24_dp, 24_dp );
    setGraphicRole( Q::Graphic, QskMaterial3Skin::GraphicRoleOnSurface );

    setColor( Q::Text, m_pal.onSurface );
    setFontRole( Q::Text, QskMaterial3Skin::M3BodyMedium );

    setPosition( Q::Panel, 0 );
    setPosition( Q::Panel | QskPopup::Closed, 1_dp );

    setAnimation( Q::Panel | A::Metric, 150 );
    setAnimation( Q::Cursor | A::Position | A::Metric, 75, QEasingCurve::OutCubic );
}

void Editor::setupTextLabel()
{
    using Q = QskTextLabel;

    setAlignment( Q::Text, Qt::AlignCenter );
    setColor( Q::Text, m_pal.onSurface );

    setPadding( Q::Panel, 5_dp );
}


void Editor::setupTextInput()
{
    using Q = QskTextInput;

    setAlignment( Q::Text, Qt::AlignLeft | Qt::AlignTop );

    setColor( Q::Text, m_pal.onBackground );

    setPadding( Q::Panel, 5_dp );
    setBoxShape( Q::Panel, 4_dp, 4_dp, 0, 0 );
    setBoxBorderMetrics( Q::Panel, 0, 0, 0, 1_dp );
    setBoxBorderColors( Q::Panel, m_pal.onSurface );

    setBoxBorderMetrics( Q::Panel | Q::Focused, 0, 0, 0, 2_dp );
    setBoxBorderColors( Q::Panel | Q::Focused, m_pal.primary );

    setBoxBorderMetrics( Q::Panel | Q::Editing, 0, 0, 0, 2_dp );
    setBoxBorderColors( Q::Panel | Q::Editing, m_pal.primary );

    setBoxBorderMetrics( Q::Panel | Q::Hovered, 0, 0, 0, 1_dp );
    setBoxBorderColors( Q::Panel | Q::Hovered, m_pal.onSurface );

    setGradient( Q::Panel, m_pal.surfaceVariant );

    const auto c1 = QskRgb::toTransparentF( m_pal.onSurface, 0.04 );
    setGradient( Q::Panel | Q::Disabled, c1 );
    setBoxBorderMetrics( Q::Panel | Q::Disabled, 0, 0, 0, 1_dp );

    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );
    setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.onSurface38 );
}

void Editor::setupProgressBar()
{
    using A = QskAspect;
    using Q = QskProgressBar;

    auto size = 5_dp;

    for ( auto subControl : { Q::Groove, Q::Bar } )
    {
        setMetric( subControl | A::Size, size );
        setPadding( subControl, 0 );

        setBoxShape( subControl, 0 );
        setBoxBorderMetrics( subControl, 0 );
    }

    setMetric( Q::Groove | A::Size, size );
    setGradient( Q::Groove, m_pal.primaryContainer );

    setGradient( Q::Groove | Q::Disabled, m_pal.onSurface12 );

    setGradient( Q::Bar, m_pal.primary );
    setGradient( Q::Bar | Q::Disabled, m_pal.onSurface38 );
}

void Editor::setupRadioBox()
{
    using Q = QskRadioBox;
    using A = QskAspect;

    setSpacing(Q::Panel, 10_dp);
    
    setStrutSize( Q::Button, { 20_dp, 20_dp });
    setStrutSize( Q::Symbol, { 10_dp, 10_dp });
    setStrutSize( Q::Ripple, { 40_dp, 40_dp });
    
    setBoxShape(Q::Button, 20_dp);
    setBoxShape(Q::Ripple, 40_dp);
    setBoxBorderMetrics( Q::Button, 2_dp );

    setBoxBorderColors( Q::Button, m_pal.onBackground );
    setBoxBorderColors( Q::Button | Q::Selected, m_pal.primary );

    setColor( Q::Text, m_pal.onBackground );
    setColor( Q::Symbol, m_pal.primary );
    setColor( Q::Ripple, stateLayerColor( m_pal.onSurface, m_pal.focusOpacity ) );
    setColor( Q::Ripple | Q::Selected, stateLayerColor( m_pal.primary, m_pal.focusOpacity ) );

    setBoxBorderColors( Q::Button | Q::Disabled, m_pal.onSurface38 );
    setBoxBorderColors( Q::Button | Q::Disabled | Q::Selected, m_pal.onSurface38 );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    setColor( Q::Symbol | Q::Disabled, m_pal.onSurface38 );
	
    setMargin( Q::Text, QskMargins( 10_dp, 0, 10_dp, 0 ));

    setAlignment( Q::Text, Qt::AlignBottom );

    setAnimation( Q::Ripple | A::Metric | A::Position, qskDuration );
}

void Editor::setupFocusIndicator()
{
    using Q = QskFocusIndicator;
    setGradient( Q::Panel, QskGradient() );
}

void Editor::setupSegmentedBar()
{
    // copied from Squiek: we need something similar to a tab bar here. TODO ...
    using A = QskAspect;
    using Q = QskSegmentedBar;

    const QSize panelStrutSize( -1, 48_dp );
    const QSize segmentStrutSize( 48_dp, 40_dp );

    {
        // Container

        setGradient( Q::Panel, Qt::transparent );
        setPadding( Q::Panel, 0 );
        setSpacing( Q::Panel, 8_dp );

        setBoxShape( Q::Panel, 100, Qt::RelativeSize );

        setBoxBorderMetrics( Q::Panel, 1_dp );
        setBoxBorderColors( Q::Panel, m_pal.outline );
        setBoxBorderColors( Q::Panel | Q::Disabled, m_pal.onSurface12 );

        setStrutSize( Q::Panel | A::Horizontal, panelStrutSize );
        setStrutSize( Q::Panel | A::Vertical, panelStrutSize.transposed() );
    }

    {
        // Segment

        setStrutSize( Q::Segment | A::Horizontal, segmentStrutSize );
        setStrutSize( Q::Segment | A::Vertical, segmentStrutSize.transposed() );
        setGradient( Q::Segment, Qt::transparent );
        setPadding( Q::Segment | A::Horizontal, { 12_dp, 0, 12_dp, 0 } );
        setPadding( Q::Segment | A::Vertical, { 0, 12_dp, 0, 12_dp } );
    }

    {
        // Separator

        setStrutSize( Q::Separator | A::Horizontal, 1_dp, segmentStrutSize.height() );
        setStrutSize( Q::Separator | A::Vertical, segmentStrutSize.height(), 1_dp );
        setPadding( Q::Separator, 0 );
        setGradient( Q::Separator, m_pal.outline );
        setColor( Q::Separator | Q::Disabled, m_pal.onSurface12 );
    }

    {
        // Cursor
        setBoxShape( Q::Cursor, 0 );

        setBoxShape( Q::Cursor | Q::Minimum | A::Horizontal,
            { 100, 0, 100, 0, Qt::RelativeSize },
            { QskStateCombination::CombinationNoState, Q::Disabled } );

        setBoxShape( Q::Cursor | Q::Maximum | A::Horizontal,
            { 0, 100, 0, 100, Qt::RelativeSize },
            { QskStateCombination::CombinationNoState, Q::Disabled } );

        setBoxShape( Q::Cursor | Q::Minimum | A::Vertical,
            { 100, 100, 0, 0, Qt::RelativeSize },
            { QskStateCombination::CombinationNoState, Q::Disabled } );

        setBoxShape( Q::Cursor | Q::Maximum | A::Vertical,
            { 0, 0, 100, 100, Qt::RelativeSize },
            { QskStateCombination::CombinationNoState, Q::Disabled } );

        setGradient( Q::Cursor, m_pal.secondaryContainer );
        setGradient( Q::Cursor | Q::Disabled, m_pal.onSurface12 );
    }

    {
        // Text

        setFontRole( Q::Text, QskMaterial3Skin::M3LabelLarge );
        setTextOptions( Q::Text, Qt::ElideMiddle, QskTextOptions::NoWrap );

        setColor( Q::Text, m_pal.onSurface );
        setColor( Q::Text | Q::Selected, m_pal.onSecondaryContainer );

        setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    }

    {
        // Graphic

        setPadding( Q::Graphic, 0_dp );
        setStrutSize( Q::Graphic, 18_dp, 18_dp );

        setGraphicRole( Q::Graphic, QskMaterial3Skin::GraphicRoleOnSurface );
        setGraphicRole( Q::Graphic | Q::Selected,
            QskMaterial3Skin::GraphicRoleOnSecondaryContainer );
        setGraphicRole( Q::Graphic | Q::Disabled, QskMaterial3Skin::GraphicRoleOnSurface38 );
    }
}

void Editor::setupSeparator()
{
    using A = QskAspect;
    using Q = QskSeparator;

    for ( auto variation : { A::Horizontal, A::Vertical } )
    {
        const auto aspect = Q::Panel | variation;

        setMetric( aspect | A::Size, 4_dp );
        setBoxShape( Q::Panel, 0 );
        setBoxBorderMetrics( Q::Panel, 0 );
        setGradient( aspect, m_pal.background );
    }
}

void Editor::setupPageIndicator()
{
    using Q = QskPageIndicator;

    const auto extent = 9_dp;
    setStrutSize( Q::Bullet, extent, extent );

    // circles, without border
    setBoxShape( Q::Bullet, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Bullet, 0 );

    setGradient( Q::Bullet, m_pal.primaryContainer );
    setGradient( Q::Bullet | Q::Selected, m_pal.primary );

    setGradient( Q::Bullet | Q::Disabled, m_pal.onSurface12 );
    setGradient( Q::Bullet | Q::Selected | Q::Disabled, m_pal.onSurface38 );

    setSpacing( Q::Panel, 3_dp );
    setPadding( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() ); // invisible
}

void Editor::setupPushButton()
{
    using A = QskAspect;
    using Q = QskPushButton;

    setFlagHint( Q::Panel | QskAspect::Direction, Qsk::LeftToRight );
    setStrutSize( Q::Panel, -1, 40_dp );
    setSpacing( Q::Panel, 8_dp );
    setPadding( Q::Panel, { 24_dp, 0, 24_dp, 0 } );
    setBoxShape( Q::Panel, 100, Qt::RelativeSize );

    setStrutSize( Q::Graphic, 18_dp, 18_dp );
    setPadding( Q::Graphic, { 0, 0, 8_dp, 0 } );
    setGraphicRole( Q::Graphic, QskMaterial3Skin::GraphicRoleOnPrimary );

    setFontRole( Q::Text, QskMaterial3Skin::M3LabelLarge );
    setPadding( Q::Text, 0 );

    // normal buttons (i.e. Filled):

    setGradient( Q::Panel, m_pal.primary );
    setGradient( Q::Panel | Q::Disabled, m_pal.onSurface12 );

    const auto hoverColor = flattenedColor( m_pal.onPrimary, m_pal.primary, 0.08 );

    setGradient( Q::Panel | Q::Hovered, hoverColor );
    setShadowMetrics( Q::Panel | Q::Hovered, m_pal.elevationLight1 );
    setShadowColor( Q::Panel | Q::Hovered, m_pal.shadow );

    const auto focusColor = flattenedColor( m_pal.onPrimary, m_pal.primary, 0.12 );
    setGradient( Q::Panel | Q::Focused, focusColor );

    setGradient( Q::Panel | Q::Pressed, focusColor );

    setGradient( Q::Ripple, stateLayerColor( m_pal.onPrimary, m_pal.hoverOpacity ) );

    setColor( Q::Text, m_pal.onPrimary );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    setTextOptions( Q::Text, Qt::ElideMiddle, QskTextOptions::NoWrap );

    setAnimation( Q::Panel | A::Color, qskDuration );
    setAnimation( Q::Panel | A::Metric, qskDuration );
    setAnimation( Q::Ripple | A::Color, qskDuration );
    setAnimation( Q::Text | A::Color, qskDuration );
}

void Editor::setupDialogButtonBox()
{
    using Q = QskDialogButtonBox;

    setGradient( Q::Panel, m_pal.secondaryContainer );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
}

void Editor::setupSlider()
{
    using A = QskAspect;
    using Q = QskSlider;

    const qreal extent = 30_dp;

    // Panel

    setMetric( Q::Panel | A::Size, extent );
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, QskGradient() );

    setPadding( Q::Panel | A::Horizontal, QskMargins( 0.5 * extent, 0 ) );
    setPadding( Q::Panel | A::Vertical, QskMargins( 0, 0.5 * extent ) );

    // Groove, Fill

    for ( auto subControl : { Q::Groove, Q::Fill } )
    {
        setPadding( subControl, 0 );

        setBoxShape( subControl, 0 );
        setBoxBorderMetrics( subControl, 0 );
    }

    setMetric( Q::Groove | A::Size, 4_dp );
    setMetric( Q::Fill | A::Size, 6_dp );

    setGradient( Q::Groove, m_pal.primaryContainer );
    setGradient( Q::Groove | Q::Disabled, m_pal.onSurface12 );

    setGradient( Q::Fill, m_pal.primary );
    setGradient( Q::Fill | Q::Disabled, m_pal.onSurface38 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setBoxBorderMetrics( Q::Handle, 0 );

    setStrutSize( Q::Handle, 20_dp, 20_dp );

    setGradient( Q::Handle, m_pal.primary );
    setGradient( Q::Handle | Q::Pressed, m_pal.primary );

    const auto disabledColor = flattenedColor( m_pal.onSurface, m_pal.background, 0.38 );
    setGradient( Q::Handle | Q::Disabled, disabledColor );

    setStrutSize( Q::Ripple, 40_dp, 40_dp );
    setBoxShape( Q::Ripple, 100, Qt::RelativeSize );
    setGradient( Q::Ripple, Qt::transparent );
    setGradient( Q::Ripple | Q::Hovered, m_pal.primary12 );
    setGradient( Q::Ripple | Q::Pressed, m_pal.primary12 );

    // move the handle smoothly, when using keys
    setAnimation( Q::Handle | A::Metric | A::Position, 2 * qskDuration );
    setAnimation( Q::Handle | A::Metric | A::Position | Q::Pressed, 0 );
}

void Editor::setupSpinBox()
{
    using Q = QskSpinBox;

    setSpacing( Q::Panel, 4_dp );

    setStrutSize( Q::TextPanel, 80_dp, 40_dp );
    setStrutSize( Q::IncrementPanel, 40_dp,40_dp );
    setStrutSize( Q::DecrementPanel, 40_dp, 40_dp );

    setAlignment( Q::Panel, Qt::AlignHCenter );
    setAlignment( Q::Text, Qt::AlignCenter );

    for( const auto& state : { Q::DecrementPanel, Q::IncrementPanel, Q::TextPanel } )
    {
        setBoxShape( state, 4_dp );
        setBoxBorderMetrics( state, 1_dp );
    }

    for( const auto& state : { Q::DecrementPanel, Q::IncrementPanel } )
    {
        setGradient( state, m_pal.primary );
        setGradient( state | Q::Disabled, m_pal.onSurface12 );

        const auto focusColor = flattenedColor( m_pal.onPrimary, m_pal.primary, 0.12 );
        setGradient( state | Q::Focused, focusColor );
        setGradient( state | Q::Pressed, focusColor );

        const auto hoverColor = flattenedColor( m_pal.onPrimary, m_pal.primary, 0.08 );
        setGradient( state | Q::Hovered, hoverColor );
        setShadowMetrics( state | Q::Hovered, m_pal.elevationLight1 );
        setShadowColor( state | Q::Hovered, m_pal.shadow );
    }

    for( const auto& state : { Q::DecrementIndicator, Q::IncrementIndicator } )
    {
        setColor( state, m_pal.onPrimary );
        setColor( state | Q::Disabled, m_pal.onSurface38 );
        setAlignment(state, Qt::AlignCenter );
        setFontRole( state, QskMaterial3Skin::M3LabelLarge );
    }

    setColor( Q::Text, m_pal.onBackground );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    setPadding( Q::TextPanel, 5_dp );
    setBoxShape( Q::TextPanel, 4_dp, 4_dp, 0, 0 );
    setBoxBorderMetrics( Q::TextPanel, 0, 0, 0, 1_dp );
    setBoxBorderColors( Q::TextPanel, m_pal.onSurface );

    setBoxBorderMetrics( Q::TextPanel | Q::Focused, 0, 0, 0, 2_dp );
    setBoxBorderColors( Q::TextPanel | Q::Focused, m_pal.primary );

//  setBoxBorderMetrics( Q::TextPanel | Q::Editing, 0, 0, 0, 2_dp );
//  setBoxBorderColors( Q::TextPanel | Q::Editing, m_pal.primary );

    setBoxBorderMetrics( Q::TextPanel | Q::Hovered, 0, 0, 0, 1_dp );
    setBoxBorderColors( Q::TextPanel | Q::Hovered, m_pal.onSurface );

    setGradient( Q::TextPanel, m_pal.surfaceVariant );

    const auto c1 = QskRgb::toTransparentF( m_pal.onSurface, 0.04 );
    setGradient( Q::TextPanel | Q::Disabled, c1 );
    setBoxBorderMetrics( Q::TextPanel | Q::Disabled, 0, 0, 0, 1_dp );

    setColor( Q::TextPanel | Q::Disabled, m_pal.onSurface38 );
    setBoxBorderColors( Q::TextPanel | Q::Disabled, m_pal.onSurface38 );
}

void Editor::setupSwitchButton()
{
    using A = QskAspect;
    using Q = QskSwitchButton;

    setBoxShape( Q::Groove, 100, Qt::RelativeSize );
    const QSizeF strutSize( 52_dp, 32_dp );
    setStrutSize( Q::Groove | A::Horizontal, strutSize );
    setStrutSize( Q::Groove | A::Vertical, strutSize.transposed() );
    setGradient( Q::Groove, m_pal.surfaceVariant );

    setGradient( Q::Groove | Q::Disabled, m_pal.surfaceVariant12 );
    setGradient( Q::Groove | Q::Checked, m_pal.primary );

    setGradient( Q::Groove | Q::Checked | Q::Disabled, m_pal.onSurface12 );
    setBoxBorderMetrics( Q::Groove, 2_dp );
    setBoxBorderColors( Q::Groove, m_pal.outline );

    setBoxBorderMetrics( Q::Groove | Q::Checked, 0 );

    setBoxShape( Q::Handle, 100, Qt::RelativeSize );
    setStrutSize( Q::Handle, 16_dp, 16_dp );
    setStrutSize( Q::Handle | Q::Checked, 24_dp, 24_dp,
        { QskStateCombination::CombinationNoState, Q::Disabled } );

    setGradient( Q::Handle, m_pal.outline );
    setGradient( Q::Handle | Q::Checked, m_pal.primaryContainer );

    setGradient( Q::Handle | Q::Disabled, m_pal.onSurface38 );
    setGradient( Q::Handle | Q::Disabled | Q::Checked, m_pal.surface );

    // just to keep the strut size the same at all times:
    setStrutSize( Q::Ripple, 40_dp, 40_dp );
    setGradient( Q::Ripple, Qt::transparent );

    setStrutSize( Q::Ripple | Q::Hovered, 40_dp, 40_dp );
    setBoxShape( Q::Ripple, 100, Qt::RelativeSize );
    setGradient( Q::Ripple | Q::Hovered, stateLayerColor( m_pal.onSurface, m_pal.focusOpacity ) );
    setGradient( Q::Ripple | Q::Hovered | Q::Checked,
        stateLayerColor( m_pal.primary, m_pal.focusOpacity ) );

    setBoxBorderColors( Q::Handle, m_pal.outline );
    setBoxBorderColors( Q::Handle | Q::Checked, m_pal.primary );

    for ( auto state : { A::NoState, Q::Disabled } )
    {
        auto aspect = Q::Handle | state;

        setPosition( aspect, 0.15 );
        setPosition( aspect | Q::Checked, 0.85 );
    }

    setAnimation( Q::Handle | A::Color, qskDuration );
    setAnimation( Q::Handle | A::Metric, qskDuration );
    setAnimation( Q::Groove | A::Color, qskDuration );
}

void Editor::setupTabButton()
{
    using A = QskAspect;
    using Q = QskTabButton;

    setStrutSize( Q::Panel, 48_dp, 48_dp );
    setGradient( Q::Panel, m_pal.surface );

    setColor( Q::Text, m_pal.onSurfaceVariant );
    setColor( Q::Text | A::Footer, m_pal.onSecondaryContainer );
    setColor( Q::Text | Q::Disabled, m_pal.onSurface38 );

    setColor( Q::Text | Q::Checked, m_pal.primary );
    setColor( Q::Text | Q::Hovered, m_pal.primary );

    for ( const auto variation : { A::Left, A::Right, A::Top, A::Bottom } )
    {
        const auto aspect = Q::Panel | variation;

        Qt::Edge edge;

        switch( variation )
        {
            case A::Left:
                edge = Qt::RightEdge;
                break;

            case A::Right:
                edge = Qt::LeftEdge;
                break;

            case A::Top:
                edge = Qt::BottomEdge;
                break;

            case A::Bottom:
                edge = Qt::TopEdge;
                break;

            default:
                edge = Qt::Edge( 0 ); // making gcc4 happy
        }

        QskBoxBorderMetrics border;
        border.setWidthAt( edge, 3_dp );
        setBoxBorderMetrics( aspect, border );

        QskBoxBorderColors borderColors( m_pal.surface );
        setBoxBorderColors( aspect, borderColors );

        borderColors.setGradientAt( edge, m_pal.primary );
        setBoxBorderColors( aspect | Q::Checked, borderColors );
    }

    setGradient( Q::Panel | Q::Hovered,
        QskRgb::toTransparentF( m_pal.surface, m_pal.hoverOpacity ) );

    setGradient( Q::Panel | Q::Focused,
        QskRgb::toTransparentF( m_pal.surface, m_pal.focusOpacity ) );

    setGradient( Q::Panel | Q::Pressed,
        QskRgb::toTransparentF( m_pal.surface, m_pal.pressedOpacity ) );

    setGradient( Q::Panel | A::Footer, m_pal.surface2 );
    setGradient( Q::Panel | A::Footer | Q::Checked, m_pal.secondaryContainer );
    setGradient( Q::Panel | A::Footer | Q::Hovered,
        stateLayerColor( m_pal.onSurfaceVariant, m_pal.hoverOpacity ) );
    setGradient( Q::Panel | A::Footer | Q::Hovered | Q::Checked,
        stateLayerColor( m_pal.onSurface, m_pal.hoverOpacity ) );
    setGradient( Q::Panel | A::Footer | Q::Focused,
        stateLayerColor( m_pal.onSurfaceVariant, m_pal.focusOpacity ) );
    setGradient( Q::Panel | A::Footer | Q::Focused | Q::Checked,
        stateLayerColor( m_pal.onSurface, m_pal.focusOpacity ) );
    setGradient( Q::Panel | A::Footer | Q::Pressed,
        stateLayerColor( m_pal.onSurfaceVariant, m_pal.pressedOpacity ) );
    setGradient( Q::Panel | A::Footer | Q::Pressed | Q::Checked,
        stateLayerColor( m_pal.onSurface, m_pal.pressedOpacity ) );

    setAnimation( Q::Panel | A::Color, qskDuration );

    setFontRole( Q::Text, QskMaterial3Skin::M3LabelLarge );
    setAlignment( Q::Text, Qt::AlignCenter );
}

void Editor::setupTabBar()
{
    using A = QskAspect;
    using Q = QskTabBar;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );

    setGradient( Q::Panel, m_pal.secondaryContainer );
    setPadding( Q::Panel, 0 );

    setFlagHint( Q::Panel | A::Style, Qt::BottomEdge );

    // when flicking
    setAnimation( Q::Panel | A::Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void Editor::setupTabView()
{
    using Q = QskTabView;

    setGradient( Q::Page, m_pal.background );
    setAnimation( Q::Page, qskDuration );
}

void Editor::setupInputPanel()
{
    using Q = QskInputPanelBox;

    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, m_pal.secondaryContainer );
    setBoxBorderColors( Q::Panel, m_pal.background );
}

void Editor::setupVirtualKeyboard()
{
    using A = QskAspect;
    using Q = QskVirtualKeyboard;

    // key panel
    setMargin( Q::ButtonPanel, 2_dp );

    setBoxShape( Q::ButtonPanel, 20.0, Qt::RelativeSize );
    setBoxBorderMetrics( Q::ButtonPanel, 2_dp );
    setBoxBorderColors( Q::ButtonPanel, m_pal.background );

    for ( auto state : { A::NoState, Q::Focused } )
        setBoxBorderColors( Q::ButtonPanel | QskPushButton::Pressed | state,
            m_pal.secondary );

    setAnimation( Q::ButtonPanel | A::Color, qskDuration );
    setAnimation( Q::ButtonPanel | A::Metric, qskDuration );

    // panel
    setBoxShape( Q::Panel, 0 );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, m_pal.secondaryContainer );
    setBoxBorderColors( Q::Panel, m_pal.background );
}

void Editor::setupScrollView()
{
    using A = QskAspect;
    using Q = QskScrollView;

    setGradient( Q::Panel, m_pal.background );

    setGradient( Q::Viewport, m_pal.secondaryContainer );

    for ( auto subControl : { Q::HorizontalScrollBar, Q::VerticalScrollBar } )
    {
        setMetric( subControl | A::Size, 10_dp );
        setPadding( subControl, 0 );
    }

    const auto handleExtent = 40_dp;
    setStrutSize( Q::HorizontalScrollHandle, handleExtent, 0.0 );
    setStrutSize( Q::VerticalScrollHandle, 0.0, handleExtent );

    for ( auto subControl : { Q::HorizontalScrollHandle, Q::VerticalScrollHandle } )
    {
        setBoxShape( subControl, 3_dp );
        setBoxBorderMetrics( subControl, 0 );
        setGradient( subControl, m_pal.primary );
        setAnimation( subControl | A::Color, qskDuration );
    }

    // when changing the position by QskScrollView::scrollTo
    setAnimation( Q::Viewport | A::Metric, QskAnimationHint( 200, QEasingCurve::InCubic ) );
}

void Editor::setupListView()
{
    using Q = QskListView;

    setPadding( Q::Cell, { 16_dp, 12_dp, 16_dp, 12_dp } );
    setBoxBorderMetrics( Q::Cell, { 0, 0, 0, 1_dp } );
    setBoxBorderColors( Q::Cell, m_pal.outline );
    setColor( Q::Cell, m_pal.surface );
    setColor( Q::Cell | Q::Selected, m_pal.primary12 );

    setColor( Q::Text, m_pal.onSurfaceVariant );
}

void Editor::setupSubWindow()
{
    using A = QskAspect;
    using Q = QskSubWindow;

    // Panel

    setPadding( Q::Panel, { 24_dp, 0, 24_dp, 24_dp } );
    setStrutSize( Q::Panel, { 280_dp, -1 } );
    setBoxShape( Q::Panel, 28_dp );
    setBoxBorderMetrics( Q::Panel, 0 );
    setGradient( Q::Panel, m_pal.secondaryContainer );
    setShadowMetrics( Q::Panel, m_pal.elevationLight3 );
    setShadowColor( Q::Panel, m_pal.shadow );

    // TitleBarPanel
    setBoxShape( Q::TitleBarPanel, { 28_dp, 28_dp, 0, 0 } );
    setPadding( Q::TitleBarPanel, { 24_dp, 24_dp, 24_dp, 16_dp } );
    setFlagHint( Q::TitleBarPanel | QskAspect::Style,
        Q::TitleBar | Q::Title | Q::Symbol );

    setGradient( Q::TitleBarPanel, m_pal.secondaryContainer );

    // TitleBarText
    setFontRole( Q::TitleBarText, QskMaterial3Skin::M3HeadlineSmall );
    setColor( Q::TitleBarText, m_pal.onSurface );
    setAlignment( Q::TitleBarText, Qt::AlignCenter );

    setTextOptions( Q::TitleBarText, Qt::ElideRight, QskTextOptions::NoWrap );

    for ( auto subControl : { Q::Panel, Q::TitleBarPanel, Q::TitleBarText } )
        setAnimation( subControl | A::Color, qskDuration );

}

QskMaterial3Theme::QskMaterial3Theme( Lightness lightness )
    : QskMaterial3Theme( lightness,
                        { // default Material colors:
                        0xff6750A4,
                        0xff625B71,
                        0xff7D5260,
                        0xffB3261E,
                        0xff605D62,
                        0xff605D66,
                        } )
{
}

QskMaterial3Theme::QskMaterial3Theme( Lightness lightness,
        std::array< QskHctColor, NumPaletteTypes > palettes )
    : m_palettes( palettes )
{
    if ( lightness == Light )
    {
        primary = m_palettes[ Primary ].toned( 40 ).rgb();
        onPrimary = m_palettes[ Primary ].toned( 100 ).rgb();
        primaryContainer = m_palettes[ Primary ].toned( 90 ).rgb();
        onPrimaryContainer = m_palettes[ Primary ].toned( 10 ).rgb();

        secondary = m_palettes[ Secondary ].toned( 40 ).rgb();
        onSecondary = m_palettes[ Secondary ].toned( 100 ).rgb();
        secondaryContainer = m_palettes[ Secondary ].toned( 90 ).rgb();
        onSecondaryContainer = m_palettes[ Secondary ].toned( 10 ).rgb();

        tertiary = m_palettes[ Tertiary ].toned( 40 ).rgb();
        onTertiary = m_palettes[ Tertiary ].toned( 100 ).rgb();
        tertiaryContainer = m_palettes[ Tertiary ].toned( 90 ).rgb();
        onTertiaryContainer = m_palettes[ Tertiary ].toned( 10 ).rgb();

        error = m_palettes[ Error ].toned( 40 ).rgb();
        onError = m_palettes[ Error ].toned( 100 ).rgb();
        errorContainer = m_palettes[ Error ].toned( 90 ).rgb();
        onErrorContainer = m_palettes[ Error ].toned( 10 ).rgb();

        background = m_palettes[ Neutral ].toned( 99 ).rgb();
        onBackground = m_palettes[ Neutral ].toned( 10 ).rgb();
        surface = m_palettes[ Neutral ].toned( 99 ).rgb();
        onSurface = m_palettes[ Neutral ].toned( 10 ).rgb();

        surfaceVariant = m_palettes[ NeutralVariant ].toned( 90 ).rgb();
        onSurfaceVariant = m_palettes[ NeutralVariant ].toned( 30 ).rgb();
        outline = m_palettes[ NeutralVariant ].toned( 50 ).rgb();

        shadow = m_palettes[ Neutral ].toned( 0 ).rgb();
    }
    else if ( lightness == Dark )
    {
        primary = m_palettes[ Primary ].toned( 80 ).rgb();
        onPrimary = m_palettes[ Primary ].toned( 20 ).rgb();
        primaryContainer = m_palettes[ Primary ].toned( 30 ).rgb();
        onPrimaryContainer = m_palettes[ Primary ].toned( 90 ).rgb();

        secondary = m_palettes[ Secondary ].toned( 80 ).rgb();
        onSecondary = m_palettes[ Secondary ].toned( 20 ).rgb();
        secondaryContainer = m_palettes[ Secondary ].toned( 30 ).rgb();
        onSecondaryContainer = m_palettes[ Secondary ].toned( 90 ).rgb();

        tertiary = m_palettes[ Tertiary ].toned( 80 ).rgb();
        onTertiary = m_palettes[ Tertiary ].toned( 20 ).rgb();
        tertiaryContainer = m_palettes[ Tertiary ].toned( 30 ).rgb();
        onTertiaryContainer = m_palettes[ Tertiary ].toned( 90 ).rgb();

        error = m_palettes[ Error ].toned( 80 ).rgb();
        onError = m_palettes[ Error ].toned( 20 ).rgb();
        errorContainer = m_palettes[ Error ].toned( 30 ).rgb();
        onErrorContainer = m_palettes[ Error ].toned( 90 ).rgb();

        background = m_palettes[ Neutral ].toned( 10 ).rgb();
        onBackground = m_palettes[ Neutral ].toned( 90 ).rgb();
        surface = m_palettes[ Neutral ].toned( 10 ).rgb();
        onSurface = m_palettes[ Neutral ].toned( 80 ).rgb();

        surfaceVariant = m_palettes[ NeutralVariant ].toned( 30 ).rgb();
        onSurfaceVariant = m_palettes[ NeutralVariant ].toned( 80 ).rgb();
        outline = m_palettes[ NeutralVariant ].toned( 60 ).rgb();

        shadow = m_palettes[ Neutral ].toned( 0 ).rgb();
    }

    primary8 = QskRgb::toTransparentF( primary, 0.08 );
    primary12 = QskRgb::toTransparentF( primary, 0.12 );

    error8 = QskRgb::toTransparentF( error, 0.08 );
    error12 = QskRgb::toTransparentF( error, 0.12 );

    surface1 = flattenedColor( primary, background, 0.05 );
    surface2 = flattenedColor( primary, background, 0.08 );
    surface3 = flattenedColor( primary, background, 0.11 );
    surface4 = flattenedColor( primary, background, 0.12 );
    surface5 = flattenedColor( primary, background, 0.14 );

    onSurface8 = QskRgb::toTransparentF( onSurface, 0.08 );
    onSurface12 = QskRgb::toTransparentF( onSurface, 0.12 );
    onSurface38 = QskRgb::toTransparentF( onSurface, 0.38 );

    surfaceVariant12 = QskRgb::toTransparentF( surfaceVariant, 0.12 );

    elevationLight1 = QskShadowMetrics( -3, 5, { 0, 2 } );
    elevationLight2 = QskShadowMetrics( -2, 8, { 0, 2 } );
    elevationLight3 = QskShadowMetrics( -1, 11, { 0, 2 } );

    shapeExtraSmallTop = QskBoxShapeMetrics( 4_dp, 4_dp, 0, 0 );
}

QskMaterial3GraphicProvder::QskMaterial3GraphicProvder( QObject* parent )
    : Inherited( parent )
{
}

const QskGraphic* QskMaterial3GraphicProvder::loadGraphic( const QString& id ) const
{
    const QString name = QString( ":/icons/qvg/%1.qvg" ).arg( id );
    const QskGraphic graphic = QskGraphicIO::read( name );

    return graphic.isNull() ? nullptr : new QskGraphic( graphic );
}

QskMaterial3Skin::QskMaterial3Skin( const QskMaterial3Theme& palette, QObject* parent )
    : Inherited( parent )
{
    addGraphicProvider( {}, new QskMaterial3GraphicProvder() );

    setupFonts();
    setupGraphicFilters( palette );

    Editor editor( &hintTable(), palette );
    editor.setup();
}

QskMaterial3Skin::~QskMaterial3Skin()
{
}

QskGraphic QskMaterial3Skin::symbol( int symbolType ) const
{
    const auto* provider = graphicProvider( {} );

    switch ( symbolType )
    {
        case QskStandardSymbol::CheckMark:
            return *( provider->requestGraphic( "check_small" ) );

        case QskStandardSymbol::CrossMark:
            return {};

        case QskStandardSymbol::SegmentedBarCheckMark:
            return *( provider->requestGraphic( "segmented-button-check" ) );

        case QskStandardSymbol::ComboBoxSymbolPopupClosed:
            return *( provider->requestGraphic( "combo-box-arrow-closed" ) );

        case QskStandardSymbol::ComboBoxSymbolPopupOpen:
            return *( provider->requestGraphic( "combo-box-arrow-open" ) );

        default:
            return Inherited::symbol( symbolType );
    }
}

void QskMaterial3Skin::setupFonts()
{
    Inherited::setupFonts( QStringLiteral( "Roboto" ) );

    setFont( M3BodyMedium,
        createFont( QStringLiteral( "Roboto Regular"), 20_dp, 14_dp, 0.25, QFont::Normal ) );
    setFont( M3BodyLarge,
        createFont( QStringLiteral( "Roboto Medium" ), 24_dp, 16_dp, 0.5, QFont::Normal ) );
    setFont( M3HeadlineSmall,
        createFont( QStringLiteral( "Roboto Regular" ), 32_dp, 28_dp, 0.0, QFont::Normal ) );
    setFont( M3LabelLarge, createFont( "Roboto Medium", 20_dp, 14_dp, 0.1, QFont::Medium ) );
}

void QskMaterial3Skin::setupGraphicFilters( const QskMaterial3Theme& palette )
{
    QskColorFilter onPrimaryFilter;
    onPrimaryFilter.addColorSubstitution( Qt::white, palette.onPrimary );
    setGraphicFilter( GraphicRoleOnPrimary, onPrimaryFilter );

    QskColorFilter onSecondaryContainerFilter;
    onSecondaryContainerFilter.addColorSubstitution( Qt::white, palette.onSecondaryContainer );
    setGraphicFilter( GraphicRoleOnSecondaryContainer, onSecondaryContainerFilter );

    QskColorFilter onErrorFilter;
    onErrorFilter.addColorSubstitution( Qt::white, palette.onError );
    setGraphicFilter( GraphicRoleOnError, onErrorFilter );

    QskColorFilter onSurfaceFilter;
    onSurfaceFilter.addColorSubstitution( Qt::white, palette.onSurface );
    setGraphicFilter( GraphicRoleOnSurface, onSurfaceFilter );

    QskColorFilter onSurfaceFilter38;
    onSurfaceFilter38.addColorSubstitution( Qt::white, palette.onSurface38 );
    setGraphicFilter( GraphicRoleOnSurface38, onSurfaceFilter38 );

    QskColorFilter onSurfaceVariantFilter;
    onSurfaceVariantFilter.addColorSubstitution( Qt::white, palette.onSurfaceVariant );
    setGraphicFilter( GraphicRoleOnSurfaceVariant, onSurfaceVariantFilter );

    QskColorFilter surfaceFilter;
    surfaceFilter.addColorSubstitution( Qt::white, palette.surface );
    setGraphicFilter( GraphicRoleSurface, surfaceFilter );
}

#include "moc_QskMaterial3Skin.cpp"
