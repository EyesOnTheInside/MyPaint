#ifndef WORKAREA_H
#define WORKAREA_H

#include <QWidget>
#include <QGridLayout>

#include "imagecontainer.h"
#include "colorboard.h"

class WorkArea : public QWidget
{
    Q_OBJECT
public:
    explicit WorkArea(QWidget *parent = 0);

    ImageContainer *getImageContainer() const;
    ColorBoard *getColorBoard() const;

private:
    QGridLayout *layout;
    ImageContainer *imageContainer;
    ColorBoard *colorBoard;
};

#endif // WORKAREA_H
