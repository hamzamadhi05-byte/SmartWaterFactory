#include "mainwindow.h"
#include "connexion.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Connexion c;
    if (!c.createConnexion()) {
        QMessageBox::critical(nullptr, "Erreur", "Connexion à la base échouée");
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}