/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Dialogs module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
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
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QQUICKFILEDIALOG_P_H
#define QQUICKFILEDIALOG_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qquickabstractfiledialog_p.h"
#include <QJSValue>
#include <QStandardPaths>

QT_BEGIN_NAMESPACE

class QQuickFileDialog : public QQuickAbstractFileDialog
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem* contentItem READ contentItem WRITE setContentItem DESIGNABLE false)
    Q_PROPERTY(QJSValue shortcuts READ shortcuts NOTIFY shortcutsChanged) // map of QStandardDirectory names to QUrls
    Q_PROPERTY(QJSValue __shortcuts READ __shortcuts NOTIFY shortcutsChanged) // map of details for QML dialog implementations
    Q_CLASSINFO("DefaultProperty", "contentItem")    // AbstractFileDialog in QML can have only one child

public:
    explicit QQuickFileDialog(QObject *parent = 0);
    ~QQuickFileDialog();
    virtual QList<QUrl> fileUrls() const;

    QJSValue shortcuts();
    QJSValue __shortcuts();

Q_SIGNALS:
    void shortcutsChanged();

public Q_SLOTS:
    void clearSelection();
    bool addSelection(const QUrl &path);

protected:
    virtual QPlatformFileDialogHelper *helper() { return 0; }
    Q_INVOKABLE QString urlToPath(const QUrl &url) { return url.toLocalFile(); }
    Q_INVOKABLE QUrl pathToUrl(const QString &path) { return QUrl::fromLocalFile(path); }
    Q_INVOKABLE QUrl pathFolder(const QString &path);

    void addShortcut(const QString &name, const QString &visibleName, const QString &path);
    void addShortcutFromStandardLocation(const QString &name, QStandardPaths::StandardLocation loc, bool local = true);
    void populateShortcuts();
    void updateModes() Q_DECL_OVERRIDE;

private:
    QList<QUrl> m_selections;

    Q_DISABLE_COPY(QQuickFileDialog)
    QJSValue m_shortcuts;
    QJSValue m_shortcutDetails;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickFileDialog *)

#endif // QQUICKFILEDIALOG_P_H
