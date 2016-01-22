/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLATFORMINTEGRATION_WAYLAND_H
#define QPLATFORMINTEGRATION_WAYLAND_H

#include <qpa/qplatformintegration.h>

#include <QtWaylandClient/private/qwaylandclientexport_p.h>
QT_BEGIN_NAMESPACE

namespace QtWaylandClient {

class QWaylandBuffer;
class QWaylandDisplay;
class QWaylandClientBufferIntegration;
class QWaylandServerBufferIntegration;
class QWaylandShellIntegration;
class QWaylandInputDeviceIntegration;
class QWaylandInputDevice;

class Q_WAYLAND_CLIENT_EXPORT QWaylandIntegration : public QPlatformIntegration
{
public:
    QWaylandIntegration();
    ~QWaylandIntegration();

    bool hasCapability(QPlatformIntegration::Capability cap) const;
    QPlatformWindow *createPlatformWindow(QWindow *window) const;
#ifndef QT_NO_OPENGL
    QPlatformOpenGLContext *createPlatformOpenGLContext(QOpenGLContext *context) const;
#endif
    QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const;

    QAbstractEventDispatcher *createEventDispatcher() const;
    void initialize();

    QPlatformFontDatabase *fontDatabase() const;

    QPlatformNativeInterface *nativeInterface() const;

    QPlatformClipboard *clipboard() const;

    QPlatformDrag *drag() const;

    QPlatformInputContext *inputContext() const;

    QVariant styleHint(StyleHint hint) const;

    QPlatformAccessibility *accessibility() const;

    QPlatformServices *services() const;

    QWaylandDisplay *display() const;

    QStringList themeNames() const;

    QPlatformTheme *createPlatformTheme(const QString &name) const;

    QWaylandInputDevice *createInputDevice(QWaylandDisplay *display, int version, uint32_t id);

    virtual QWaylandClientBufferIntegration *clientBufferIntegration() const;
    virtual QWaylandServerBufferIntegration *serverBufferIntegration() const;
    virtual QWaylandShellIntegration *shellIntegration() const;

protected:
    QWaylandClientBufferIntegration *mClientBufferIntegration;
    QWaylandServerBufferIntegration *mServerBufferIntegration;
    QWaylandShellIntegration *mShellIntegration;
    QWaylandInputDeviceIntegration *mInputDeviceIntegration;

private:
    void initializeClientBufferIntegration();
    void initializeServerBufferIntegration();
    void initializeShellIntegration();
    void initializeInputDeviceIntegration();

    QPlatformFontDatabase *mFontDb;
    QPlatformClipboard *mClipboard;
    QPlatformDrag *mDrag;
    QWaylandDisplay *mDisplay;
    QPlatformNativeInterface *mNativeInterface;
    QScopedPointer<QPlatformInputContext> mInputContext;
    QPlatformAccessibility *mAccessibility;
    bool mClientBufferIntegrationInitialized;
    bool mServerBufferIntegrationInitialized;
    bool mShellIntegrationInitialized;

    friend class QWaylandDisplay;
};

}

QT_END_NAMESPACE

#endif
