#include "mainwindow.h"
#include "client_interfaces.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
	int retcode;
	if(!initBackend(argc, argv, retcode))
		return retcode;
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    MainWindow w;
    w.show();
	
	a.exec();
	cleanupBackend();
    return 0;
}
