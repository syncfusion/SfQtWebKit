 /*,,,,,,,,,,,,,,...................../////////////////.,
 * Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
#ifndef QtPrintContext_h
#define QtPrintContext_h

#include <PlatformExportMacros.h>
#include <QPainter>
#include <QRect>
#include <qwebkitglobal.h>
#include "QWebFrameAdapter.h"
#include "IntRect.h"
namespace WebCore {
class PrintContext;
class GraphicsContext;
class IntRect;
}

class QWebFrameAdapter;

class WEBKIT_EXPORTDATA QtPrintContext {
public:
    QtPrintContext(QPainter*, const QRect& pageRect, QWebFrameAdapter*);
    QtPrintContext(QPainter* painter, const QRect& pageRect, QWebFrameAdapter* frameAdapter, bool status);
    QtPrintContext(QPainter* painter, QWebFrameAdapter* frameAdapter);
    ~QtPrintContext();
//    void disposelink() const;
    Vector<WebCore::IntRect> pageRects() const;
    int pageCount() const;
    void spoolPage(int pageNumber, float width);
    WebCore::IntRect totalPageLayoutSize ;
    bool printStatus;
    //QPair<int, QRectF> Q
    QHash<const WebCore::Node*, const WebCore::RenderObject *> elementToRenderObject;
    QPair<int, QRectF> QGetRectangle(QWebFrameAdapter* frameAdapter,Vector<WebCore::IntRect>& pagerect, const QWebElement & e);
    //IntRect GetTotalPageLayoutSize();
private:
    WebCore::GraphicsContext* m_graphicsContext;
    WebCore::PrintContext* m_printContext;

};

#endif
