#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMovie>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QScreen* screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->availableGeometry().size();

    int screenHeight = screenSize.height();

    // Пример: берём % от высоты экрана
    int fontSize = screenHeight * 0.01333;
    int fontSizeS = screenHeight * 0.01223;//
    ui->setupUi(this);
    QString style = QString("color: white; font-size: %1pt; font-family: 'Segoe UI';").arg(fontSize);
    QString styleSmall = QString("color: white; font-size: %1pt; font-family: 'Segoe UI';").arg(fontSizeS);
    ui->labelText->setStyleSheet(style);
    ui->labelBottom->setStyleSheet(styleSmall);
    QMovie* movie = new QMovie("spinner.gif");  // путь к gif
    ui->labelGif->setMovie(movie);              // соединяем QMovie с QLabel
    this->showFullScreen();
    ui->centerLayout->setContentsMargins(0, 30, 0, 0);
    ui->labelText->setText("Работа с обновлениями: 0%\nНе выключайте компьютер. Это может занять некоторое время.");
    ui->labelBottom->setText("Компьютер перезапустится несколько раз.\n");
    setCursor(Qt::BlankCursor);
    movie->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

