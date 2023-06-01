#include "colorlabel.h"
#include "util.h"
#include <QColor>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QClipboard>
#include <QCursor>
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>

ColorLabel::ColorLabel(QColor color, QWidget *parent) : QWidget(parent)
{
    colorLabel = new QLabel(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(colorLabel);
    setLayout(layout);

    setColor(color);
}

void ColorLabel::mousePressEvent(QMouseEvent *event)
{
    emit copySuccessSignalFromColorLabelSignal();

    QClipboard *clipBoard = QApplication::clipboard();
    QString colorValue = qcolorToString(color);

    clipBoard->setText(colorValue);
}

void ColorLabel::enterEvent(QEvent *event)
{
    setCursor(Qt::PointingHandCursor);
}

void ColorLabel::setColor(QColor color)
{
    QPalette palette;
    palette.setColor(QPalette::Background, color);
    colorLabel->setAutoFillBackground(true);
    colorLabel->setPalette(palette);

    this->color = color;
}

