// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
#pragma once
#include <QIcon>
#include <QImage>
#include <QQuickPaintedItem>
#include <QPointer>
#include <QUrl>
#include <QtQml/qqmlregistration.h>
namespace Qavsi::Icons {
class NativeIconImage : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged FINAL)
    Q_PROPERTY(QObject *sourceOwner READ sourceOwner WRITE setSourceOwner NOTIFY sourceOwnerChanged FINAL)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)
    Q_PROPERTY(int status READ status NOTIFY statusChanged FINAL)
public:
    enum Status { Null, Ready, Loading, Error };
    Q_ENUM(Status)
    explicit NativeIconImage(QQuickItem *parent = nullptr);
    QString name() const { return m_name; }
    QUrl source() const { return m_source; }
    QObject *sourceOwner() const { return m_sourceOwner; }
    QColor color() const { return m_color; }
    int status() const { return m_status; }
    void setName(const QString &value);
    void setSource(const QUrl &value);
    void setSourceOwner(QObject *value);
    void setColor(const QColor &value);
    void paint(QPainter *painter) override;
signals:
    void nameChanged();
    void sourceChanged();
    void sourceOwnerChanged();
    void colorChanged();
    void statusChanged();
protected:
    void updatePolish() override;
    void geometryChange(const QRectF &next, const QRectF &previous) override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;
private:
    void reload();
    QString m_name;
    QUrl m_source;
    QPointer<QObject> m_sourceOwner;
    QColor m_color = Qt::transparent;
    QIcon m_icon;
    QImage m_image;
    Status m_status = Null;
};
}
