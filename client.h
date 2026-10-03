#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QSqlQueryModel>

class Client
{
    int id;
    QString nom, type, telephone, email, adresse;

public:
    Client() {}
    Client(int id, QString nom, QString type,
           QString telephone, QString email, QString adresse);

    bool ajouter();
    QSqlQueryModel* afficher();
    bool modifier();
    bool supprimer(int id);
    QSqlQueryModel* rechercher(QString nom);
    QSqlQueryModel* trierParNbCommandes();
    bool mettreAJourCategories();
};

#endif