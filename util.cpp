#include "util.h"

/**
 * @brief qcolorToString
 * @param color a QColor object.
 * @return the color string (one of RGB, HSL, CMYK...)
 *
 * According to the setting, convert the QColor to string.
 */
QString qcolorToString(QColor color)
{
    QString colorValue;
    colorValue += "#";
    colorValue += decToHexString(color.red());
    colorValue += decToHexString(color.green());
    colorValue += decToHexString(color.blue());

    return colorValue;
}

/**
 * @brief decToHexString
 * @param value the decimal number.
 * @return A two digits hexadecimal number.
 *
 * e.g. 5 -> 05  255 -> ff
 */
QString decToHexString(int value)
{
    QString res;
    if(value <= 0x0f) {
        res += "0";
    }
    res += QString::number(value, 16);

    return res;
}
