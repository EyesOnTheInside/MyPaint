#include "colorboard.h"
#include "colorlabel.h"
#include "util.h"
#include <QLabel>
#include <QGridLayout>
#include <QVector>
#include <QGraphicsEffect>
#include <QLinearGradient>
#include <QBrush>
#include <QPointF>
#include <QPainter>

ColorBoard::ColorBoard(QWidget *parent) : QWidget(parent)
{
    layout = new QGridLayout(this);

    text = new QLabel(tr("Open an image first."), this);
    text->setAlignment(Qt::AlignCenter);
    layout->addWidget(text, 0, 0, 1, 3);
}

QVector<ColorLabel *> ColorBoard::getColorLabels() const
{
    return colorLabels;
}

/**
 * @brief ColorBoard::setColorLabels
 *
 * If the color vector is empty (the first time to load image), create all member of the class.
 * Else change these relating variables.
 */
void ColorBoard::setColorLabels()
{
    if(colors.empty()) {
        createColorLabels();
    }
    else {
        changeColorLabels();
    }
}

/**
 * @brief ColorBoard::computeMainColor
 *
 * The core algoritem of compute the main color of the image.
 * MMCQ (Modified Median Cut Quantization)
 */
void ColorBoard::computeMainColor()
{
    colors.push_back(QColor("#feb3b0"));
    colors.push_back(QColor("#3b3c5a"));
    colors.push_back(QColor("#0d0e23"));
    colors.push_back(QColor("#706182"));
    colors.push_back(QColor("#fce4fc"));
    colors.push_back(QColor("#706182"));
    colors.push_back(QColor("#706182"));
}

/**
 * @brief ColorBoard::createColorLabels
 *
 * create these relating variables and add them to a grid layout.
 */
void ColorBoard::createColorLabels()
{
    text->setText(tr("Image main color"));

    computeMainColor();

    QVector<QColor>::const_iterator it;
    for(it = colors.constBegin(); it != colors.constEnd(); it++) {
        int index = it - colors.begin();

        ColorLabel *colorLabel = new ColorLabel(*it, this);

        colorLabels.push_back(colorLabel);
        layout->addWidget(colorLabel, index * 3 + 1, 0, 3, 2);

        connect(colorLabel, SIGNAL(copySuccessSignalFromColorLabelSignal()),
                SLOT(sendCopySuccessSignal()));

        QLabel *colorValueLabel = new QLabel(this);
        QString colorValue = qcolorToString(*it);

        colorValueLabel->setText(colorValue);
        colorValueLabel->setAlignment(Qt::AlignCenter);

        colorValueLabels.push_back(colorValueLabel);

        layout->addWidget(colorValueLabel, index * 3 + 1, 2, 3, 1);
    }

    layout->setColumnMinimumWidth(0, 50);
    setLayout(layout);
}

/**
 * @brief ColorBoard::changeColorLabels
 *
 * If the color label number doesn't change, change the relating variables, it's easy.
 * Thinking about the condition that users change the color label number in the setting,
 * we should do more actions.
 */
void ColorBoard::changeColorLabels()
{
    colors.clear();
    computeMainColor();

    if(colorLabels.size() == colors.size()) {
        QVector<QColor>::const_iterator it;
        for(it = colors.constBegin(); it != colors.constEnd(); it++) {
            int index = it - colors.constBegin();

            QString colorValue = qcolorToString(*it);

            colorValueLabels[index]->setText(colorValue);

            colorLabels[index]->setColor(*it);
        }
    }
}


void ColorBoard::sendCopySuccessSignal()
{
    emit copySuccessFromColorBoradSignal();
}
