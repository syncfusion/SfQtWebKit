/*
 * Copyright (C) 2015 The Qt Company Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "QtPrintContext.h"

#include "GraphicsContext.h"
#include "IntRect.h"
#include "PrintContext.h"
#include "QWebFrameAdapter.h"
#include "RenderLayer.h"

#include "Frame.h"
#include "FrameLoaderClientQt.h"
#include "FrameTree.h"
#include "FrameView.h"
#include "FloatQuad.h"
#include "FlowThreadController.h"


using namespace WebCore;
QT_BEGIN_NAMESPACE
extern Q_GUI_EXPORT int qt_defaultDpi();
QT_END_NAMESPACE

WebCore::Element* renderElement;
QtPrintContext::QtPrintContext(QPainter* painter, const QRect& pageRect, QWebFrameAdapter* frameAdapter, WebCore::Element* element)
    : m_graphicsContext(new GraphicsContext(painter))
    , m_printContext(new PrintContext(frameAdapter->frame))
{
	m_printContext->setElementToDraw(element);
	renderElement = element;
    totalPageLayoutSize = m_printContext->begin(pageRect.width(), pageRect.height());
    printStatus = true;
    float pageHeight = 0;
    m_printContext->computePageRects(IntRect(pageRect), /* headerHeight */ 0, /* footerHeight */ 0, /* userScaleFactor */ 1.0, pageHeight);
}
QtPrintContext::QtPrintContext(QPainter* painter, const QRect& pageRect, QWebFrameAdapter* frameAdapter, bool status)
    : m_graphicsContext(new GraphicsContext(painter))
    , m_printContext(new PrintContext(frameAdapter->frame))
{
    totalPageLayoutSize = m_printContext->begin(pageRect.width(), pageRect.height());
    printStatus = status;
    float pageHeight = 0;
    m_printContext->computePageRects(IntRect(pageRect), /* headerHeight */ 0, /* footerHeight */ 0, /* userScaleFactor */ 1.0, pageHeight);
}
QtPrintContext::QtPrintContext(QPainter* painter, QWebFrameAdapter* frameAdapter)
{

    printStatus = false;
}

QtPrintContext::~QtPrintContext()
{
    if(printStatus)
    {
        m_printContext->end();
        delete m_printContext;
        delete m_graphicsContext;
    }


}
QPair<int, QRectF> QtPrintContext::QGetRectangle(QWebFrameAdapter* frameAdapter,Vector<IntRect>& pagerect, const QWebElement & e)
{

            if (elementToRenderObject.empty())
                for(WebCore::RenderObject * o = frameAdapter->frame->document()->renderer();o;o=o->nextInPreOrder())
                    if(o->node())
                        elementToRenderObject[o->node()] =o;

            if(!elementToRenderObject.contains(e.m_element))
                return QPair<int, QRectF>(-1,QRectF());
            const WebCore::RenderObject * ro = elementToRenderObject[e.m_element];
           // Frame* fr = frameAdapter->frame->
            //PrintContext* printContext = new PrintContext(frameAdapter->frame);
//            const qreal zoomFactorX = printer->logicalDpiX() / qt_defaultDpi();
//            const qreal zoomFactorY = printer->logicalDpiY() / qt_defaultDpi();
//            IntRect pageRect(0, 0,
//                                 int(printer->width() / zoomFactorX),
//                                 int(printer->height() / zoomFactorY));

            //m_printContext->begin(pageRect.width(), pageRect.height());
            //float pageHeight = 0;
            //m_printContext->computePageRects(pageRect, /* headerHeight */ 0, /* footerHeight */ 0, /* userScaleFactor */ 1.0, pageHeight);

            //painter.scale(zoomFactorX, zoomFactorY);
            //int printWidth = pageRect.width();
            const Vector<IntRect> & pageRects = pagerect;
            if (pageRects.size() == 0)
                    return QPair<int,QRectF>(-1, QRectF());
           // WebCore::RenderView *root = toRenderView(frameAdapter->frame->document()->renderer());
            //float scale = (float)printWidth / (float)root->width();
            QRectF r(const_cast<WebCore::RenderObject *>(ro)->absoluteBoundingBoxRect());
			IntRect renderElementBounds;
			if (renderElement != NULL)
				renderElementBounds = renderElement->pixelSnappedBoundingBox();
            int low=0;
            int high=pageRects.size();
            int c = r.y() + r.height() / 2;
			int elementMaxWidth = r.x() + r.width() / 2;
			if (renderElement == NULL || r.x() >= renderElementBounds.x() && elementMaxWidth <= renderElementBounds.x() + renderElementBounds.width() && r.y() >= renderElementBounds.y() && c <= renderElementBounds.y() + renderElementBounds.height())
			{
				if (renderElement != NULL)
				{
					r = QRectF(r.x() - renderElementBounds.x(), r.y() - renderElementBounds.y(), r.width(), r.height());
					c = r.y() + r.height();
				}
            while(low <= high)
            {
                int m = (low + high)/2;
                if(c < pageRects[m].y())
                    high = m-1;
                else if(c > pageRects[m].maxY())
                            low = m +1;
                else {
                       QRectF tr = r.translated(0, -pageRects[m].y());
                       return QPair<int, QRectF>(m+1, QRect(tr.x() , tr.y(), tr.width(), tr.height()));
                     }
            }
			}
    return QPair<int,QRectF>(-1, QRectF());
}
Vector<IntRect> QtPrintContext::pageRects() const
{
    return m_printContext->pageRects();
}
int QtPrintContext::pageCount() const
{
    return m_printContext->pageCount();
}

void QtPrintContext::spoolPage(int pageNumber, float width)
{
	//Set element to draw
	m_printContext->setElementToDraw(renderElement);
    m_printContext->spoolPage(*m_graphicsContext, pageNumber, width);
}


