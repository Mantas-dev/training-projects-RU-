#include "mediaplayer.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setStyle(QStyleFactory::create("Fusion"));
    MediaPlayer w;
    w.show();

    return a.exec();
}
