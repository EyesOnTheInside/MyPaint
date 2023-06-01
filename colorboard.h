#ifndef COLORBOARD_H
#define COLORBOARD_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QVector>

#include "colorlabel.h"

class ColorBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ColorBoard(QWidget *parent = 0);

    QVector<ColorLabel *> getColorLabels() const;
    void setColorLabels();
private:
    QGridLayout *layout;
    QVector<QColor> colors;
    QVector<QLabel*> colorValueLabels;
    QVector<ColorLabel*> colorLabels;
    QLabel *text;

    void computeMainColor();
    void createColorLabels();
    void changeColorLabels();

signals:
    void copySuccessFromColorBoradSignal();
public slots:
    void sendCopySuccessSignal();

};

#endif // COLORBOARD_H
