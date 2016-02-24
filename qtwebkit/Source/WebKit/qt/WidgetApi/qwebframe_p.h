/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2007 Staikos Computing Services Inc.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef qwebframe_p_h
#define qwebframe_p_h

#include "QWebFrameAdapter.h"

#include "qwebframe.h"
#include "qwebpage_p.h"

#include "GraphicsContext.h"
#include "KURL.h"
#if ENABLE(ORIENTATION_EVENTS) && ENABLE(DEVICE_ORIENTATION)
#include "qorientationsensor.h"
#endif
#include "qwebelement.h"
#include "wtf/RefPtr.h"
#include "Frame.h"
#include "ViewportArguments.h"
#include <qpainter.h>
#include "PrintContext.h"
#include "GraphicsContext.h"
#include "QtPrintContext.h"
#if USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER)
#include "texmap/TextureMapper.h"
#endif

namespace WebCore {
class FrameLoaderClientQt;
class FrameView;
class HTMLFrameOwnerElement;
class Scrollbar;
class TextureMapperLayer;
}
class QWebPage;

class QWebFramePrivate : public QWebFrameAdapter {
public:
    QWebFramePrivate()
        : q(0)
        , page(0)
    { }
    void setPage(QWebPage*);

    inline QWebFrame *parentFrame() { return qobject_cast<QWebFrame*>(q->parent()); }

    static QWebFrame* kit(const QWebFrameAdapter*);

    // Adapter implementation
    virtual QWebFrame* apiHandle() OVERRIDE;
    virtual QObject* handle() OVERRIDE;
    virtual void contentsSizeDidChange(const QSize &) OVERRIDE;
    virtual int scrollBarPolicy(Qt::Orientation) const OVERRIDE;
    virtual void emitUrlChanged() OVERRIDE;
    virtual void didStartProvisionalLoad() OVERRIDE;
    virtual void didClearWindowObject() OVERRIDE;
    virtual bool handleProgressFinished(QPoint*) OVERRIDE;
    virtual void emitInitialLayoutCompleted() OVERRIDE;
    virtual void emitIconChanged() OVERRIDE;
    virtual void emitLoadStarted(bool originatingLoad) OVERRIDE;
    virtual void emitLoadFinished(bool originatingLoad, bool ok) OVERRIDE;
    virtual QWebFrameAdapter* createChildFrame(QWebFrameData*) OVERRIDE;

    QWebFrame *q;
    QWebPage *page;

};
class QWebPrinterPrivate {
public:
    Vector<WebCore::IntRect> pageRect;
QWebPrinterPrivate();
~QWebPrinterPrivate();
};
#endif
