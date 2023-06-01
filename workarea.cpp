#include "workarea.h"
#include "imagecontainer.h"
#include "colorboard.h"
#include <QGridLayout>

WorkArea::WorkArea(QWidget *parent) : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    imageContainer = new ImageContainer(this);
    colorBoard = new ColorBoard(this);

    layout->addWidget(imageContainer, 0, 0, 1, 3);
    layout->addWidget(colorBoard, 0, 3, 1, 1);

    setLayout(layout);
}

ImageContainer *WorkArea::getImageContainer() const
{
    return imageContainer;
}

ColorBoard *WorkArea::getColorBoard() const
{
    return colorBoard;
}
