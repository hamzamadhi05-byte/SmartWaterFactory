#include "mainwindow.h"
#include "connexion.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("Fusion");
    a.setStyleSheet(
        "QMainWindow, QDialog, QWidget { background-color: #F4F7FA; color: #1B2A3A; font-family: 'Segoe UI'; font-size: 10pt; }"
        "QLabel { color: #1F5C99; font-weight: bold; background: transparent; }"
        "QLineEdit, QComboBox { background-color: white; border: 1px solid #1F5C99; border-radius: 4px; padding: 4px; color: #1B2A3A; }"
        "QLineEdit:focus, QComboBox:focus { border: 2px solid #E8743B; }"
        "QPushButton { background-color: #1F5C99; color: white; border: none; border-radius: 5px; padding: 6px 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #E8743B; }"
        "QPushButton:pressed { background-color: #F5B301; color: #1B2A3A; }"
        "QTableView { background-color: white; alternate-background-color: #EAF1F8; gridline-color: #C9D6E3; selection-background-color: #E8743B; selection-color: white; }"
        "QHeaderView::section { background-color: #1F5C99; color: white; padding: 5px; border: none; font-weight: bold; }"
        );

    Connexion c;
    if (!c.createConnexion()) {
        QMessageBox::critical(nullptr, "Erreur", "Connexion à la base échouée");
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}