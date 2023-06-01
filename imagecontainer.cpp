#include "imagecontainer.h"
#include "util.h"
#include <QLabel>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QPixmap>
#include <QImage>
#include <QWheelEvent>
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QToolTip>
#include <QApplication>
#include <QClipboard>
#include <QtAlgorithms>
#include <QFileInfo>
#include <QDebug>

ImageContainer::ImageContainer(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    scaleFactor = 0.05;

    setMouseTracking(true);

    imageArea = new QScrollArea(this);
    layout->addWidget(imageArea);
    setLayout(layout);

    // When no image load, set the label size (0, 0) to make it invisible
    imageLabel = new QLabel(this);
    imageLabel->resize(0, 0);

    image = nullptr;

    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->installEventFilter(this);
    imageLabel->setMouseTracking(true);

    /*
     * It means that put a image file in the image container, the image will scale how many times.
     *
     * the image file size: w1 * h1
     * the scroll area size: w2 * h2
     * fileIntoContainerScaleRatio = w2 / w1 or h2 / h1
     *
     * e.g.
     * the file size: 1920 * 1080
     * the scroll area size: 600 * 500
     * So if I want to put it in the container, the image will be reduced so that the container
     * can wrap it. The scale ratio is the fileIntoContainerScaleRatio.
     */
    fileIntoContainerScaleRatio = 1.0;

    /*
     * When a image is load to the container, the showScaleRatio is 1.0.
     * If users scroll the mouse wheel, the image will zoom out or zoom in, and the showScaleRatio
     * will become bigger or smaller.
     */
    showScaleRatio = 1.0;

    imageArea->setAlignment(Qt::AlignCenter);
    imageArea->setWidget(imageLabel);
}

/**
 * @brief ImageContainer::wheelEvent
 * @param event the mouse wheel event.
 *
 * When the mouse wheel scroll ↑, the image will zoom out.
 * When the mouse wheel scroll ↓, the image will zoom in.
 * If no image load, the event doesn't trigger anything.
 */
void ImageContainer::wheelEvent(QWheelEvent *event)
{
    if(image == nullptr) {
        return;
    }

    if(event->delta() > 0) {
        if(showScaleRatio + scaleFactor > 2) {
            return;
        }
        showScaleRatio += scaleFactor;
        imageLabel->resize(fileIntoContainerScaleRatio * showScaleRatio * imageLabel->pixmap()->size());
    }
    else {
        if(showScaleRatio - scaleFactor < 0.5) {
            return;
        }
        showScaleRatio -= scaleFactor;
        imageLabel->resize(fileIntoContainerScaleRatio * showScaleRatio * imageLabel->pixmap()->size());
    }

    emit showScaleRatioChangeSignal(showScaleRatio);
}

/**
 * @brief ImageContainer::resizeEvent
 * @param event the resize event.
 *
 * When a image is loaded to the container, it will trigger a resize event.
 * In the event we can get the scroll area size to compute the fileIntoContainerScaleRatio,
 * and scale the image to a suitable size.
 */
void ImageContainer::resizeEvent(QResizeEvent *event)
{
    if(image == nullptr) {
        return;
    }
    imageAreaWidth = imageArea->viewport()->geometry().width();
    imageAreaHeight = imageArea->viewport()->geometry().height();

    computeFileIntoContainerScaleRatio();
    imageLabel->resize(fileIntoContainerScaleRatio * showScaleRatio * imageLabel->pixmap()->size());
}

/**
 * @brief ImageContainer::eventFilter
 * @param watched the event target.
 * @param event the event.
 * @return a bool value.
 *
 * If the mouse cursor moves above the image, it will conpute the position and color of the
 * point the cursor points. And send a signal to the main window, for updating the text in
 * status bar.
 * If users double click in the image, it will copy the color value to the clipboard, and
 * send a signal to the main window, for updating the text in status bar and prompting the
 * users that copy successfully.
 */
bool ImageContainer::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == imageLabel) {
        if(image == nullptr) {
            return true;
        }
        if(event->type() == QEvent::MouseMove) {
            QMouseEvent *e = static_cast<QMouseEvent*>(event);

            QColor color = getPixelColor(e->pos().x(), e->pos().y());
            QString colorValue = qcolorToString(color);
            int x = e->pos().x() / showScaleRatio / fileIntoContainerScaleRatio;
            int y = e->pos().y() / showScaleRatio / fileIntoContainerScaleRatio;

            emit cursorInImageSignal(x, y, colorValue);
            emit cursorInImageSignal(color);
        }
        else if(event->type() == QEvent::Enter) {
            emit cursorInImageSignal();
        }
        else if(event->type() == QEvent::Leave) {
            emit cursorOutImageSignal();
        }
        else if(event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent *e = static_cast<QMouseEvent*>(event);

            QColor color = getPixelColor(e->pos().x(), e->pos().y());
            QString colorValue = qcolorToString(color);
            QClipboard *clipBoard = QApplication::clipboard();
            clipBoard->setText(colorValue);

            emit copySuccessFromImageLabelSignal();
        }
    }
}

QLabel *ImageContainer::getImageLabel() const
{
    return imageLabel;
}

double ImageContainer::getShowScaleRatio() const
{
    return showScaleRatio;
}

double ImageContainer::getFileIntoContainerScaleRatio() const
{
    return fileIntoContainerScaleRatio;
}

/**
 * @brief ImageContainer::getPixelColor
 * @param x the mouse cursor pos x
 * @param y the mouse cursor pos y
 * @return the color the cursor points
 */
QColor ImageContainer::getPixelColor(int x, int y)
{
    x /= fileIntoContainerScaleRatio * showScaleRatio;
    y /= fileIntoContainerScaleRatio * showScaleRatio;

    return image->pixelColor(x, y);
}

/**
 * @brief ImageContainer::computeFileIntoContainerScaleRatio
 *
 * The main window is horizontal (width is larger than height) originally and doesn't matched
 * the image size.
 * So we should find a suitable scale ratio to make the image show larger as much as possible
 * and the scroll bar doesn't appear.
 */
void ImageContainer::computeFileIntoContainerScaleRatio()
{
    int imageWidth = imageLabel->pixmap()->size().width();
    int imageHeight = imageLabel->pixmap()->size().height();
    double factor = 0.0;
    double x = 1.0 * imageAreaWidth / imageWidth;
    double y = 1.0 * imageAreaHeight / imageHeight;

    factor = qMin(x, y);
    fileIntoContainerScaleRatio = 0.95 * factor;
}

/**
 * @brief ImageContainer::loadImage
 * @param fileName the image file name
 *
 * Use a label to show the image. And it will send a signal to main window, trigger a function
 * to change the label text in status bar, show the file name and size.
 */
bool ImageContainer::loadImage(QString fileName)
{
    if(image != nullptr) {
        delete image;
    }
    image = new QImage(fileName);

    if(image->isNull()) {
        emit openImageFailedSignal();
        return false;
    }

    imageLabel->setPixmap(QPixmap::fromImage(*image));

    imageAreaWidth = imageArea->viewport()->geometry().width();
    imageAreaHeight = imageArea->viewport()->geometry().height();

    computeFileIntoContainerScaleRatio();
    imageLabel->resize(fileIntoContainerScaleRatio * showScaleRatio * imageLabel->pixmap()->size());

    QFileInfo fi(fileName);
    QString info;

    if(fi.fileName().length() > 15) {
        info += "..., ";
    }
    else {
        info += fi.fileName() + ", ";
    }

    info += QString::number(imageLabel->pixmap()->size().width()) + "*";
    info += QString::number(imageLabel->pixmap()->size().height());

    emit imageFileChangeSignal(info);
    emit showScaleRatioChangeSignal(showScaleRatio);
    emit cursorOutImageSignal();

    return true;
}
