#include "ImageViewer.h"
#include <utils/Tools.h>

ImageViewer::ImageViewer(QWidget* parent)
    : WindowOverlayWidget(parent)
    , layout(new QVBoxLayout(this))
    , scaleFactor(1.0) {
    setAttribute(Qt::WA_TranslucentBackground);
    // setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::Widget);
    imageLabel = new QLabel(this);
}

bool ImageViewer::loadImagefrompath(const QString& path) {
    try {
        if (path.isEmpty()) {
            return false;
        }
        QImage loaded(path);
        if (loaded.isNull()) {
            return false;
        }
        this->image = loaded;

        return true;
    } catch (...) {
        return false;
    }
}
bool ImageViewer::loadImage(const QImage& image) {
    try {
        if (image.isNull()) {
            return false;
        }
        this->image = image;
        if (imageLabel) {
            imageLabel->setPixmap(QPixmap::fromImage(this->image));
            imageLabel->setScaledContents(true);
        }
        return true;

    } catch (...) {
        return false;
    }
}
const QString& ImageViewer::getImagePath() {
    imagePath = QFileDialog::getOpenFileName(nullptr,
                                             "Choose Image File",
                                             "",
                                             "Image Files (*.png *.jpg *.bmp *.jpeg *.gif)");
    return imagePath;
}

bool ImageViewer::copyImageToClipboard() {
    if (this->image.isNull()) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText("Image failed to copy to clipboard,plz load image first");
        return false;
    } else {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setImage(this->image);
        return true;
    }
}
void ImageViewer::openImageFileDialog() {
    imagePath = getImagePath();
    if (!imagePath.isEmpty()) {
        loadImagefrompath(imagePath);
    }
}

void ImageViewer::saveImageFileDialog() {
    imagePath = QFileDialog::getSaveFileName(nullptr,
                                             "Save Image File",
                                             "",
                                             "Image Files (*.png *.jpg *.bmp *.jpeg *.gif)");
    if (!imagePath.isEmpty()) {
        this->image.save(imagePath);
    }
}

void ImageViewer::readFullInfo(const QString& path) {
    QFileInfo info(path);
    QString fileInfo = "File Name: " + info.fileName() + "\n";
    fileInfo += "File Resolution: " + QString::number(QImage(path).width()) + "x"
                + QString::number(QImage(path).height()) + "\n";
    fileInfo += "File Path: " + info.absoluteFilePath() + "\n";
    fileInfo += "File Size: " + Tools::fileSizeString(path) + "\n";
    fileInfo += "File Created: " + info.birthTime().toString() + "\n";
    fileInfo += "File Modified: " + info.lastModified().toString() + "\n";
    fileInfo += "File Accessed: " + info.lastRead().toString() + "\n";
    fileInfo += "File Type: " + QImageReader::imageFormat(path).toUpper() + "\n";

    QMessageBox::information(this, "Full Image Information", fileInfo);
}
void ImageViewer::adaptiveResize() {
    // get the resolution of user's monitor
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    //get the size of the image

    QImage loadedImage = this->image;
    int imageWidth = loadedImage.width();
    int imageHeight = loadedImage.height();

    //calculate the size of the Window
    int windowWidth = imageWidth;
    int windowHeight = imageHeight;
    if (imageWidth > screenWidth || imageHeight > screenHeight) {
        qreal widthRatio = static_cast<qreal>(screenWidth) / windowWidth;
        qreal heightRatio = static_cast<qreal>(screenHeight) / windowHeight;
        qreal scaleFactor = qMin(widthRatio, heightRatio);
        windowWidth = static_cast<int>(windowWidth * scaleFactor);
        windowHeight = static_cast<int>(windowHeight * scaleFactor);
    }
    this->resize(windowWidth, windowHeight);
    updateDisplaydImage();
}
void ImageViewer::updateDisplaydImage() {
    if (imageLabel) {
        imageLabel->setPixmap(QPixmap::fromImage(this->image));
        imageLabel->setScaledContents(true);
    }
}
