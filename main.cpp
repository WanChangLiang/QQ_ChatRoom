#include "widget.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "denglu.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "20240222_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    denglu b;
    b.show();
    return a.exec();
}
