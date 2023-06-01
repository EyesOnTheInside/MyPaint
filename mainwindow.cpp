#include "mainwindow.h"
#include "util.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QToolBar>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QVector>
#include <QFileDialog>
#include <QInputDialog>
#include <QDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QNetworkReply>
#include <QThread>
#include <QPalette>
#include <QDebug>

#define SCREEN_WIDTH QApplication::desktop()->screenGeometry().width()
#define SCREEN_HEIGHT QApplication::desktop()->screenGeometry().height()

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    createMenu(this);
    createStatusBar(this);
    createWorkArea(this);
    createToolBar(this);

    setMouseTracking(true);
    workArea->setMouseTracking(true);

    connectSlots();

    setCustomStyle();
}

MainWindow::~MainWindow(){}

void MainWindow::setCustomStyle()
{
    QString styles;
    styles += "QMainWindow{background-color: #ffffff;}"

              "QMenuBar{background-color: #e8e8e8; border:none; padding: 0px;}"
              "QMenuBar::item{background-color: #e8e8e8; padding: 3px 10px 3px 10px;}"
              "QMenuBar::item:selected{background-color: #ffffff;}"

              "QMenu{background-color: #ffffff;}"
              "QMenu::item{background-color: #ffffff; padding: 3px 30px 3px 30px;}"
              "QMenu::item:selected{background-color: #e8e8e8; color: #000000;}"
              "QMenu::icon{padding-left: 8px;}"

              "QToolBar{border: 2px solid #e8e8e8; padding: 0px;}"
              "QToolButton{padding: 3px 5px 3px 5px; background-color: #ffffff;border: none;}"
              "QToolButton:hover{background-color: #e8e8e8;}"

              "QScrollArea{background-color: #ffffff; border: 2px solid #e8e8e8;}"

              "QStatusBar{background-color: #e8e8e8;}"
              "QStatusBar::item{border: none;}"

              "QPushButton{background-color: #ffffff; border: 2px solid #e8e8e8; width: 76px; height: 25px;}"
              "QPushButton:hover{background-color: #e8e8e8;}"

              "QDialog{background-color: #ffffff;}"

              "QLineEdit{background-color: #ffffff; border: 2px solid #e8e8e8;}"

              "QProgressBar{background-color: #ffffff; border: 2px solid #e8e8e8; text-align: center;}"
              "QProgressBar:chunk{background-color: #e8e8e8;}"

              "QMessageBox{background-color: #ffffff;}";

    setStyleSheet(styles);
}

/**
 * @brief MainWindow::createMenu
 * @param mainWindow the parent mainWindow.
 * Originally it can be omitted and use "this" in the function, but it will cause name conflict.
 * So use the parameter to avoid it.
 *
 * Create all menu of the application.
 */
void MainWindow::createMenu(QMainWindow *mainWindow)
{
    menuBar = mainWindow->menuBar();

    fileMenu = menuBar->addMenu(tr("File"));
    settingMenu = menuBar->addMenu(tr("Settings"));
    aboutMenu = menuBar->addMenu(tr("About"));

    openImageMenu = fileMenu->addMenu(QIcon(":/icon/icon/image.png"), tr("Open file"));
    openImageByLocalAction = openImageMenu->addAction(QIcon(":/icon/icon/folder.png"), tr("Open local file"));

    openHistoryImageMenu = fileMenu->addMenu(QIcon(":/icon/icon/time-circle.png"), tr("History"));

    saveColorBoardMenu = fileMenu->addMenu(QIcon(":/icon/icon/save.png"), tr("Save"));
    saveAsTxtAction = saveColorBoardMenu->addAction(QIcon(":/icon/icon/file-text.png"), tr("Save as .txt"));
    saveAsJpgAction = saveColorBoardMenu->addAction(QIcon(":/icon/icon/file-image.png"), tr("save as .jpg"));

    restartAction = fileMenu->addAction(QIcon(":/icon/icon/reload.png"), tr("Restart"));

    exitAction = fileMenu->addAction(QIcon(":/icon/icon/close-square.png"), tr("Exit"));

    preferenceAction = settingMenu->addAction(QIcon(":/icon/icon/setting.png"), tr("Settings"));

    referenceAction = aboutMenu->addAction(QIcon(":/icon/icon/cloud.png"), tr("Reference"));
    authorAction = aboutMenu->addAction(QIcon(":/icon/icon/user.png"), tr("Author"));
}

/**
 * @brief MainWindow::createStatusBar
 * @param mainWindow the parent mainWindow. Explanation same as above.
 *
 * Create the status bar and all label in it.
 */
void MainWindow::createStatusBar(QMainWindow *mainWindow)
{
    statusBar = mainWindow->statusBar();

    fileInfoLabel = new QLabel(tr(""), statusBar);
    curInfoLabel = new QLabel(tr(""), statusBar);
    showScaleRatioLabel = new QLabel(tr(""), statusBar);
    colorValueLabel = new QLabel(statusBar);
    helpTextLabel = new QLabel(tr(""));

    fileInfoLabel->setAlignment(Qt::AlignCenter);
    curInfoLabel->setAlignment(Qt::AlignCenter);
    showScaleRatioLabel->setAlignment(Qt::AlignCenter);
    colorValueLabel->setAlignment(Qt::AlignCenter);
    helpTextLabel->setAlignment(Qt::AlignCenter);

    statusBar->addPermanentWidget(fileInfoLabel, 3);
    statusBar->addWidget(curInfoLabel, 4);
    statusBar->addWidget(colorValueLabel, 1);
    statusBar->addWidget(helpTextLabel, 5);
    statusBar->addWidget(showScaleRatioLabel, 2);
}

/**
 * @brief MainWindow::createToolBar
 * @param mainWindow the parent mainWindow. Explanation same as above.
 *
 * Create tool bar which corresponds the menu.
 */
void MainWindow::createToolBar(QMainWindow *mainWindow)
{
    toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    mainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

    toolBar->addAction(openImageByLocalAction);
    toolBar->addAction(openImageByUrlAction);
    toolBar->addAction(saveAsTxtAction);
    toolBar->addAction(saveAsJpgAction);
    toolBar->addAction(restartAction);
    toolBar->addAction(exitAction);
    toolBar->addSeparator();
    toolBar->addAction(preferenceAction);
    toolBar->addSeparator();
    toolBar->addAction(referenceAction);
    toolBar->addAction(authorAction);
}

/**
 * @brief MainWindow::createWorkArea
 * @param mainWindow the parent mainWindow. Explanation same as above.
 *
 * Create work area and make it be the central widget. Work area includes the image container and
 * color board which are the core widget of the application.
 */
void MainWindow::createWorkArea(QMainWindow *mainWindow)
{
    workArea = new WorkArea(mainWindow);
    mainWindow->setCentralWidget(workArea);
}

/**
 * @brief MainWindow::openFileDialog
 *
 * When click the button "open local file", the application will open a file dialog to let users select images.
 * If users cancel it, the file dialog will return a null string and the function doesn't need to
 * do anything more.
 * If users select a image, the application will show it and create the color board of it.
 */
void MainWindow::openFileDialog()
{
    curFileName = QFileDialog::getOpenFileName(this, tr("Open file"), "E:/waste",
                                               tr("Images((*.png *.jpg *.jpeg *.bmp))"));
    if(curFileName == "") {
        return;
    }

    if(showNewSelectedImage(curFileName)) {
        createNewSelectedImageColorBoard();
    }
}

/**
 * @brief MainWindow::openOpenImageFailedMessageBox
 *
 * It's a slot function.
 * If opening image failed, create a messagebox to show that open failed.
 */
void MainWindow::openOpenImageFailedMessageBox()
{
    QMessageBox openImageFailedMessageBox(this);
    openImageFailedMessageBox.setText(tr("Image failed to open."));
    openImageFailedMessageBox.setInformativeText(tr("It is possible that the image format is wrong, please try again."));
    openImageFailedMessageBox.setIcon(QMessageBox::Critical);

    openImageFailedMessageBox.exec();
}

/**
 * @brief MainWindow::showNewSelectedImage
 * @param curFileName the name of file which will be opened.
 *
 * Show the image in the image container.
 */
bool MainWindow::showNewSelectedImage(QString curFileName)
{
    return workArea->getImageContainer()->loadImage(curFileName);
}

/**
 * @brief MainWindow::createNewSelectedImageColorBoard
 *
 * create the color board of the new load image.
 */
void MainWindow::createNewSelectedImageColorBoard()
{
    workArea->getColorBoard()->setColorLabels();
}

/**
 * @brief MainWindow::connectSlots
 * connect a lot of signals and slots(not all).
 */
void MainWindow::connectSlots()
{
    connect(workArea->getImageContainer(),
            SIGNAL(showScaleRatioChangeSignal(double)),
            SLOT(setShowScaleRatioLabelText(double)));
    connect(workArea->getImageContainer(),
            SIGNAL(cursorInImageSignal(int,int,QString&)),
            SLOT(setCurInfoLabelText(int,int,QString&)));
    connect(workArea->getImageContainer(),
            SIGNAL(cursorInImageSignal(QColor&)),
            SLOT(setColorValueLabel(QColor&)));
    connect(workArea->getImageContainer(),
            SIGNAL(cursorInImageSignal()),
            SLOT(setHelpTextLabelCursorInImage()));
    connect(workArea->getImageContainer(),
            SIGNAL(cursorOutImageSignal()),
            SLOT(setHelpTextLabelCursorOutImage()));
    connect(workArea->getColorBoard(),
            SIGNAL(copySuccessFromColorBoradSignal()),
            SLOT(setHelpTextLabelCopySuccess()));
    connect(workArea->getImageContainer(),
            SIGNAL(copySuccessFromImageLabelSignal()),
            SLOT(setHelpTextLabelCopySuccess()));
    connect(workArea->getImageContainer(),
            SIGNAL(imageFileChangeSignal(QString)),
            SLOT(setFileInfoLabelText(QString)));
    connect(workArea->getImageContainer(),
            SIGNAL(openImageFailedSignal()),
            SLOT(openOpenImageFailedMessageBox()));

    connect(openImageByLocalAction,
            SIGNAL(triggered()),
            SLOT(openFileDialog()));
}

/**
 * @brief MainWindow::setShowScaleRatioLabelText
 * @param showScaleRatio the show scale ratio depends on the mouse wheel.
 *
 * It's a slot function.
 * When the users scroll the mouse wheel, the show scale ratio changes, send a signal, trigger
 * the function to change the ratio text in status bar.
 */
void MainWindow::setShowScaleRatioLabelText(double showScaleRatio)
{
    int percent = static_cast<int>(showScaleRatio * 100);
    QString text;
    text = text + "Scale：" + QString::number(percent) + "%";

    showScaleRatioLabel->setText(text);
}

/**
 * @brief MainWindow::setCurInfoLabelText
 * @param x the mouse cursor x pos.
 * @param y the mouse cursor y pos.
 * @param color the color of the point which mouse cursor points.
 *
 * It's a slot function.
 * When the mouse cursor is hovered in the image, the image container will send a signal, trigger
 * the function to change the information in status bar.
 */
void MainWindow::setCurInfoLabelText(int x, int y, QString &color)
{
    QString info;
    info = info + "(x: " + QString::number(x) + ", y: " + QString::number(y) + "), " + color;

    curInfoLabel->setText(info);
}

/**
 * @brief MainWindow::setColorValueLabel
 * @param color the color of the point which mouse cursor points.
 *
 * It's a slot function.
 * When the mouse cursor is hovered in the image, the image container will send a signal, trigger
 * the function to change the color label background color in status bar.
 */
void MainWindow::setColorValueLabel(QColor &color)
{
    QPalette palette;
    palette.setColor(QPalette::Background, color);
    colorValueLabel->setAutoFillBackground(true);
    colorValueLabel->setPalette(palette);
}

/**
 * @brief MainWindow::setHelpTextLabelCursorInImage
 *
 * It's a slot function.
 * When the mouse cursor is hovered in the image, the image container will send a signal, trigger
 * the function to change the help text in status bar and prompt users that can double click the
 * image to copy the color value to the clipboard.
 */
void MainWindow::setHelpTextLabelCursorInImage()
{
    helpTextLabel->setText(tr("Double-click to copy the color of the mouse point to the clipboard."));
    helpTextLabel->setStyleSheet("color: black");
}

/**
 * @brief MainWindow::setHelpTextLabelCursorOutImage
 *
 * It's a slot function.
 * When the mouse cursor isn't hovered in the image, the image container will send a signal, trigger
 * the function to change the help text in status bar and prompt users that can click the color label
 * in color board to copy the color value to the clipboard.
 */
void MainWindow::setHelpTextLabelCursorOutImage()
{
    helpTextLabel->setText(tr("Click the color swatch on the right to copy the color to the clipboard."));
    helpTextLabel->setStyleSheet("color: black");
}

/**
 * @brief MainWindow::setHelpTextLabelCopySuccess
 *
 * It's a slot function.
 * When the mouse double click in the color label in the color board, the color board will send a signal,
 * trigger the function to change the help text in status bar and prompt users that copy successfully.
 */
void MainWindow::setHelpTextLabelCopySuccess()
{
    helpTextLabel->setText(tr("Copy color successfully!"));
    helpTextLabel->setStyleSheet("color: green");
}

/**
 * @brief MainWindow::setFileInfoLabelText
 * @param info the file name and size.
 *
 * It's a slot function.
 * When the image loads, the image container will send a signal, trigger the function to change the
 * label text in status bar and prompt users the file name and size.
 */
void MainWindow::setFileInfoLabelText(QString info)
{
    fileInfoLabel->setText(info);
}
