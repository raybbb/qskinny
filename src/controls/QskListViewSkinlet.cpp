/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskListViewSkinlet.h"
#include "QskListView.h"

#include "QskColorFilter.h"
#include "QskGraphic.h"
#include "QskBoxHints.h"
#include "QskSGNode.h"
#include "QskSkinStateChanger.h"

#include <qmath.h>
#include <qsgnode.h>
#include <qtransform.h>

class QskListViewNode final : public QSGTransformNode
{
  public:
    inline QskListViewNode( int columnCount )
        : m_columnCount( columnCount )
    {
        m_backgroundNode.setFlag( QSGNode::OwnedByParent, false );
        appendChildNode( &m_backgroundNode );

        m_foregroundNode.setFlag( QSGNode::OwnedByParent, false );
        appendChildNode( &m_foregroundNode );
    }

    QSGNode* backgroundNode()
    {
        return &m_backgroundNode;
    }

    QSGNode* foregroundNode()
    {
        return &m_foregroundNode;
    }

    inline void resetRows( int rowMin, int rowMax )
    {
        m_rowMin = rowMin;
        m_rowMax = rowMax;
    }

    inline int rowMin() const
    {
        return m_rowMin;
    }

    inline int rowMax() const
    {
        return m_rowMax;
    }

    inline bool intersects( int rowMin, int rowMax ) const
    {
        return ( rowMin <= m_rowMax ) && ( rowMax >= m_rowMin );
    }

    inline int nodeCount() const
    {
        return ( m_rowMin >= 0 ) ? ( m_rowMax - m_rowMin + 1 ) * m_columnCount : 0;
    }

    inline int columnCount() const
    {
        return m_columnCount;
    }

    inline void invalidate()
    {
        m_rowMin = m_rowMax = -1;
    }

  private:
    int m_columnCount;
    int m_rowMin = -1;
    int m_rowMax = -1;

    QSGNode m_backgroundNode;
    QSGNode m_foregroundNode;
};

QskListViewSkinlet::QskListViewSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskListViewSkinlet::~QskListViewSkinlet() = default;

QSGNode* QskListViewSkinlet::updateContentsNode(
    const QskScrollView* scrollView, QSGNode* node ) const
{
    const auto listView = static_cast< const QskListView* >( scrollView );

    auto listViewNode = static_cast< QskListViewNode* >( node );
    if ( listViewNode == nullptr )
        listViewNode = new QskListViewNode( listView->columnCount() );

    QTransform transform;
    transform.translate( -listView->scrollPos().x(), -listView->scrollPos().y() );
    listViewNode->setMatrix( transform );

    updateBackgroundNodes( listView, listViewNode );
    updateForegroundNodes( listView, listViewNode );

    return listViewNode;
}

void QskListViewSkinlet::updateBackgroundNodes(
    const QskListView* listView, QskListViewNode* listViewNode ) const
{
    using Q = QskListView;
    using A = QskAspect;

    auto backgroundNode = listViewNode->backgroundNode();

    const auto cellHeight = listView->rowHeight();
    const auto viewRect = listView->viewContentsRect();

    const auto scrolledPos = listView->scrollPos();
    const int rowMin = qFloor( scrolledPos.y() / cellHeight );

    int rowMax = qCeil( ( scrolledPos.y() + viewRect.height() ) / cellHeight );
    if ( rowMax >= listView->rowCount() )
        rowMax = listView->rowCount() - 1;

    const auto x0 = viewRect.left() + scrolledPos.x();
    const auto y0 = viewRect.top();

    auto rowNode = backgroundNode->firstChild();

    const auto boxHints1 = listView->boxHints( Q::Cell | A::Lower );
    const auto boxHints2 = listView->boxHints( Q::Cell | A::Upper );

    for ( int row = rowMin; row <= rowMax; row++ )
    {
        /*
            We do not use sampleRect to avoid doing the calculation
            of viewRect for each row.
         */
        const QRectF rect( x0, y0 + row * cellHeight, viewRect.width(), cellHeight );

        auto newNode = updateBoxNode( listView, rowNode, rect,
            ( row % 2 ) ? boxHints2 : boxHints1 );

        if ( newNode )
        {
            if ( newNode->parent() != backgroundNode )
                backgroundNode->appendChildNode( newNode );
            else
                rowNode = newNode->nextSibling();
        }
    }

    const int rowSelected = listView->selectedRow();
    if ( rowSelected >= rowMin && rowSelected <= rowMax )
    {
        QskSkinStateChanger stateChanger( listView );
        stateChanger.setStates( listView->skinStates() | QskListView::Selected );

        const QRectF rect( x0, y0 + rowSelected * cellHeight,
            viewRect.width(), cellHeight );
        
        rowNode = updateBoxNode( listView, rowNode, rect, Q::Cell );
        if ( rowNode && rowNode->parent() != backgroundNode )
            backgroundNode->appendChildNode( rowNode );
    }

    QskSGNode::removeAllChildNodesAfter( backgroundNode, rowNode );
}

void QskListViewSkinlet::updateForegroundNodes(
    const QskListView* listView, QskListViewNode* listViewNode ) const
{
    auto parentNode = listViewNode->foregroundNode();

    if ( listView->rowCount() <= 0 || listView->columnCount() <= 0 )
    {
        parentNode->removeAllChildNodes();
        listViewNode->invalidate();
        return;
    }

    const auto margins = listView->paddingHint( QskListView::Cell );

    const auto cr = listView->viewContentsRect();
    const auto scrolledPos = listView->scrollPos();

    const int rowMin = qFloor( scrolledPos.y() / listView->rowHeight() );

    int rowMax = rowMin + qCeil( cr.height() / listView->rowHeight() );
    if ( rowMax >= listView->rowCount() )
        rowMax = listView->rowCount() - 1;

#if 1
    // should be optimized for visible columns only
    const int colMin = 0;
    const int colMax = listView->columnCount() - 1;
#endif

    bool forwards = true;

    if ( listViewNode->intersects( rowMin, rowMax ) )
    {
        /*
            We try to avoid reallcations when scrolling, by reusing
            the nodes of the cells leaving the viewport for those becoming visible.
         */

        forwards = ( rowMin >= listViewNode->rowMin() );

        if ( forwards )
        {
            // usually scrolling down
            for ( int row = listViewNode->rowMin(); row < rowMin; row++ )
            {
                for ( int col = 0; col < listView->columnCount(); col++ )
                {
                    QSGNode* childNode = parentNode->firstChild();
                    parentNode->removeChildNode( childNode );
                    parentNode->appendChildNode( childNode );
                }
            }
        }
        else
        {
            // usually scrolling up
            for ( int row = rowMax; row < listViewNode->rowMax(); row++ )
            {
                for ( int col = 0; col < listView->columnCount(); col++ )
                {
                    QSGNode* childNode = parentNode->lastChild();
                    parentNode->removeChildNode( childNode );
                    parentNode->prependChildNode( childNode );
                }
            }
        }
    }

    updateVisibleForegroundNodes( listView, listViewNode,
        rowMin, rowMax, colMin, colMax, margins, forwards );

    // finally putting the nodes into their position
    auto node = parentNode->firstChild();

    const auto rowHeight = listView->rowHeight();
    auto y = cr.top() + rowMin * rowHeight;

    for ( int row = rowMin; row <= rowMax; row++ )
    {
        qreal x = cr.left();

        for ( int col = colMin; col <= colMax; col++ )
        {
            Q_ASSERT( node->type() == QSGNode::TransformNodeType );
            auto transformNode = static_cast< QSGTransformNode* >( node );

            QTransform transform;
            transform.translate( x + margins.left(), y + margins.top() );

            transformNode->setMatrix( transform );

            node = node->nextSibling();
            x += listView->columnWidth( col );
        }

        y += rowHeight;
    }

    listViewNode->resetRows( rowMin, rowMax );
}

void QskListViewSkinlet::updateVisibleForegroundNodes(
    const QskListView* listView, QskListViewNode* listViewNode,
    int rowMin, int rowMax, int colMin, int colMax, const QMarginsF& margins,
    bool forward ) const
{
    auto parentNode = listViewNode->foregroundNode();

    const int rowCount = rowMax - rowMin + 1;
    const int colCount = colMax - colMin + 1;
    const int obsoleteNodesCount = listViewNode->nodeCount() - rowCount * colCount;

    if ( forward )
    {
        for ( int i = 0; i < obsoleteNodesCount; i++ )
            delete parentNode->lastChild();

        auto node = parentNode->firstChild();

        for ( int row = rowMin; row <= rowMax; row++ )
        {
            const auto h = listView->rowHeight() - ( margins.top() + margins.bottom() );

            for ( int col = 0; col < listView->columnCount(); col++ )
            {
                const auto w = listView->columnWidth( col ) - ( margins.left() + margins.right() );

                node = updateForegroundNode( listView,
                    parentNode, static_cast< QSGTransformNode* >( node ),
                    row, col, QSizeF( w, h ), forward );

                node = node->nextSibling();
            }
        }
    }
    else
    {
        for ( int i = 0; i < obsoleteNodesCount; i++ )
            delete parentNode->firstChild();

        auto* node = parentNode->lastChild();

        for ( int row = rowMax; row >= rowMin; row-- )
        {
            const qreal h = listView->rowHeight() - ( margins.top() + margins.bottom() );

            for ( int col = listView->columnCount() - 1; col >= 0; col-- )
            {
                const auto w = listView->columnWidth( col ) - ( margins.left() + margins.right() );

                node = updateForegroundNode( listView,
                    parentNode, static_cast< QSGTransformNode* >( node ),
                    row, col, QSizeF( w, h ), forward );

                node = node->previousSibling();
            }
        }
    }
}

QSGTransformNode* QskListViewSkinlet::updateForegroundNode(
    const QskListView* listView, QSGNode* parentNode, QSGTransformNode* cellNode,
    int row, int col, const QSizeF& size, bool forward ) const
{
    const QRectF cellRect( 0.0, 0.0, size.width(), size.height() );

    /*
        Text nodes already have a transform root node - to avoid inserting extra
        transform nodes, the code below becomes a bit more complicated.
     */
    QSGTransformNode* newCellNode = nullptr;

    if ( cellNode && ( cellNode->type() == QSGNode::TransformNodeType ) )
    {
        QSGNode* oldNode = cellNode;

        auto newNode = updateCellNode( listView, oldNode, cellRect, row, col );
        if ( newNode )
        {
            if ( newNode->type() == QSGNode::TransformNodeType )
            {
                newCellNode = static_cast< QSGTransformNode* >( newNode );
            }
            else
            {
                newCellNode = new QSGTransformNode();
                newCellNode->appendChildNode( newNode );
            }
        }
    }
    else
    {
        QSGNode* oldNode = cellNode ? cellNode->firstChild() : nullptr;
        auto newNode = updateCellNode( listView, oldNode, cellRect, row, col );

        if ( newNode )
        {
            if ( newNode->type() == QSGNode::TransformNodeType )
            {
                newCellNode = static_cast< QSGTransformNode* >( newNode );
            }
            else
            {
                if ( cellNode == nullptr )
                {
                    newCellNode = new QSGTransformNode();
                    newCellNode->appendChildNode( newNode );
                }
                else
                {
                    if ( newNode != oldNode )
                    {
                        delete cellNode->firstChild();
                        cellNode->appendChildNode( newNode );

                        newCellNode = cellNode;
                    }
                }
            }
        }
    }

    if ( newCellNode == nullptr )
        newCellNode = new QSGTransformNode();

    if ( cellNode != newCellNode )
    {
        if ( cellNode )
        {
            parentNode->insertChildNodeAfter( newCellNode, cellNode );
            delete cellNode;
        }
        else
        {
            if ( forward )
                parentNode->appendChildNode( newCellNode );
            else
                parentNode->prependChildNode( newCellNode );
        }
    }

    return newCellNode;
}

QSGNode* QskListViewSkinlet::updateCellNode( const QskListView* listView,
    QSGNode* contentNode, const QRectF& rect, int row, int col ) const
{
    using namespace QskSGNode;

    auto rowStates = listView->skinStates();
    if ( row == listView->selectedRow() )
        rowStates |= QskListView::Selected;

    QskSkinStateChanger stateChanger( listView );
    stateChanger.setStates( rowStates );

    QSGNode* newNode = nullptr;

#if 1
    /*
        Alignments, text options etc are user definable attributes and need
        to be adjustable - at least individually for each column - from the
        public API of QskListView TODO ...
     */
#endif
    const auto alignment = listView->alignmentHint(
        QskListView::Cell, Qt::AlignVCenter | Qt::AlignLeft );

    const auto value = listView->valueAt( row, col );

    if ( value.canConvert< QskGraphic >() )
    {
        if ( nodeRole( contentNode ) == GraphicRole )
            newNode = contentNode;

        const auto colorFilter = listView->graphicFilterAt( row, col );

        newNode = updateGraphicNode( listView, newNode,
            value.value< QskGraphic >(), colorFilter, rect, alignment );

        if ( newNode )
            setNodeRole( newNode, GraphicRole );
    }
    else if ( value.canConvert< QString >() )
    {
        if ( nodeRole( contentNode ) == TextRole )
            newNode = contentNode;

        newNode = updateTextNode( listView, newNode, rect, alignment,
            value.toString(), QskListView::Text );

        if ( newNode )
            setNodeRole( newNode, TextRole );
    }
    else
    {
        qWarning() << "QskListViewSkinlet: got unsupported QVariant type" << value.typeName();
    }

    return newNode;
}

QSizeF QskListViewSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    const auto listView = static_cast< const QskListView* >( skinnable );

    qreal w = -1.0; // shouldn't we return something ???

    if ( which != Qt::MaximumSize )
    {
        if ( listView->preferredWidthFromColumns() )
        {
            w = listView->scrollableSize().width();
            w += listView->metric( QskScrollView::VerticalScrollBar | QskAspect::Size );
        }
    }

    return QSizeF( w, -1.0 );
}

QRectF QskListViewSkinlet::sampleRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskListView;
    const auto listView = static_cast< const QskListView* >( skinnable );

    if ( subControl == Q::Cell )
    {
        const auto cellHeight = listView->rowHeight();
        const auto viewRect = listView->viewContentsRect();
        const auto scrolledPos = listView->scrollPos();

        const double x0 = viewRect.left() + scrolledPos.x();
        const double y0 = viewRect.top();

        return QRectF( x0, y0 + index * cellHeight, viewRect.width(), cellHeight );
    }

    return Inherited::sampleRect( skinnable, contentsRect, subControl, index );
}

#include "moc_QskListViewSkinlet.cpp"
