#include <QApplication>
#include "yuv_gl_widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    YUV_GL_Widget w;
    w.show();

    return a.exec();
}
