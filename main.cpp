#include "mainwindow.h"
#include "connexion.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "hamza_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    Connexion c;
    if (!c.createConnexion()) {
        QMessageBox::critical(nullptr, "Erreur", "Connexion à la base échouée");
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}