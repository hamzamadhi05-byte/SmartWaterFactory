#include "connexion.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QDebug>

bool Connexion::createConnexion()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/smartwater.db");

    if (!db.open()) {
        qDebug() << "Erreur:" << db.lastError().text();
        return false;
    }

    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS CLIENT ("
           "ID_CLIENT INTEGER PRIMARY KEY, "
           "NOM TEXT NOT NULL, "
           "TYPE_CLIENT TEXT, "
           "TELEPHONE TEXT, "
           "EMAIL TEXT, "
           "ADRESSE TEXT, "
           "DATE_INSCRIPTION TEXT DEFAULT CURRENT_DATE, "
           "NB_COMMANDES INTEGER DEFAULT 0, "
           "DATE_DERNIERE_CMD TEXT, "
           "CATEGORIE TEXT DEFAULT 'Nouveau', "
           "STATUT TEXT DEFAULT 'Actif')");
    return true;
}