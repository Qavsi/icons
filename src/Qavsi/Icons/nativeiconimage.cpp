// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
#include "nativeiconimage.h"
#include <QPainter>
#include <QQuickWindow>
#include <QQmlContext>
#include <QQmlEngine>
#include <cmath>
namespace Qavsi::Icons {
NativeIconImage::NativeIconImage(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    setRenderTarget(QQuickPaintedItem::Image);
    setAntialiasing(true);
}
void NativeIconImage::setName(const QString &value)
{
    if (m_name == value) return;
    m_name = value;
    reload();
    emit nameChanged();
}
void NativeIconImage::setSource(const QUrl &value)
{
    if (m_source == value) return;
    m_source = value;
    reload();
    emit sourceChanged();
}
void NativeIconImage::setSourceOwner(QObject *value)
{
    if (m_sourceOwner == value) return;
    m_sourceOwner = value;
    reload();
    emit sourceOwnerChanged();
}
void NativeIconImage::setColor(const QColor &value)
{
    if (m_color == value) return;
    m_color = value;
    polish();
    emit colorChanged();
}
void NativeIconImage::reload()
{
    QUrl resolved = m_source;
    if (!resolved.isEmpty() && resolved.isRelative()) {
        // Use the AppIcon caller context, not this helper's library QML URL or cwd.
        if (QQmlContext *context = qmlContext(m_sourceOwner))
            resolved = context->resolvedUrl(resolved);
    }
    const QString file = resolved.isLocalFile() ? resolved.toLocalFile()
        : resolved.scheme() == QLatin1String("qrc") ? QLatin1Char(':') + resolved.path()
        : QString{};
    const QIcon fallback(file);
    m_icon = m_name.isEmpty() ? fallback : QIcon::fromTheme(m_name, fallback);
    const Status next = m_name.isEmpty() && m_source.isEmpty() ? Null
        : m_icon.isNull() || m_icon.actualSize(QSize(24, 24)).isEmpty() ? Error : Ready;
    if (m_status != next) {
        m_status = next;
        emit statusChanged();
    }
    polish();
}
void NativeIconImage::geometryChange(const QRectF &next, const QRectF &previous)
{
    QQuickPaintedItem::geometryChange(next, previous);
    if (next.size() != previous.size()) polish();
}
void NativeIconImage::itemChange(ItemChange change, const ItemChangeData &data)
{
    QQuickPaintedItem::itemChange(change, data);
    if (change == ItemSceneChange || change == ItemDevicePixelRatioHasChanged) polish();
}
void NativeIconImage::updatePolish()
{
    m_image = {};
    const qreal dpr = window() ? window()->devicePixelRatio() : 1;
    if (m_status == Ready && width() > 0 && height() > 0 && m_color.isValid()
        && m_color.alpha() > 0 && width() * dpr <= 4096 && height() * dpr <= 4096) {
        // QIcon/QPixmap stay on the GUI thread; paint() only reads the synchronized QImage.
        m_image = m_icon.pixmap(QSize(std::ceil(width()), std::ceil(height())), dpr).toImage()
            .convertToFormat(QImage::Format_ARGB32_Premultiplied);
        if (!m_image.isNull()) {
            QPainter tint(&m_image);
            tint.setCompositionMode(QPainter::CompositionMode_SourceIn);
            tint.fillRect(m_image.rect(), m_color);
        }
    }
    update();
}
void NativeIconImage::paint(QPainter *painter)
{
    if (m_image.isNull()) return;
    const QSizeF extent = (QSizeF(m_image.size()) / m_image.devicePixelRatio()).scaled(QSizeF(width(), height()), Qt::KeepAspectRatio);
    painter->drawImage(QRectF(QPointF((width() - extent.width()) / 2, (height() - extent.height()) / 2), extent), m_image);
}
}
