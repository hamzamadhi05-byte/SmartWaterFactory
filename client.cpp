#include "client.h"
#include <QSqlQuery>

Client::Client(int id, QString nom, QString type,
               QString telephone, QString email, QString adresse)
{
    this->id = id;
    this->nom = nom;
    this->type = type;
    this->telephone = telephone;
    this->email = email;
    this->adresse = adresse;
}

bool Client::ajouter()
{
    QSqlQuery q;
    q.prepare("INSERT INTO CLIENT (ID_CLIENT, NOM, TYPE_CLIENT, TELEPHONE, EMAIL, ADRESSE) "
              "VALUES (:id, :nom, :type, :tel, :email, :adr)");
    q.bindValue(":id", id);
    q.bindValue(":nom", nom);
    q.bindValue(":type", type);
    q.bindValue(":tel", telephone);
    q.bindValue(":email", email);
    q.bindValue(":adr", adresse);
    return q.exec();
}

QSqlQueryModel* Client::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_CLIENT, NOM, TYPE_CLIENT, TELEPHONE, EMAIL, ADRESSE, "
                    "NB_COMMANDES, CATEGORIE, STATUT FROM CLIENT");
    return model;
}

bool Client::modifier()
{
    QSqlQuery q;
    q.prepare("UPDATE CLIENT SET NOM=:nom, TYPE_CLIENT=:type, TELEPHONE=:tel, "
              "EMAIL=:email, ADRESSE=:adr WHERE ID_CLIENT=:id");
    q.bindValue(":id", id);
    q.bindValue(":nom", nom);
    q.bindValue(":type", type);
    q.bindValue(":tel", telephone);
    q.bindValue(":email", email);
    q.bindValue(":adr", adresse);
    return q.exec();
}

bool Client::supprimer(int id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM CLIENT WHERE ID_CLIENT=:id");
    q.bindValue(":id", id);
    return q.exec();
}

QSqlQueryModel* Client::rechercher(QString nom)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery q;
    q.prepare("SELECT ID_CLIENT, NOM, TYPE_CLIENT, TELEPHONE, EMAIL, ADRESSE, "
              "NB_COMMANDES, CATEGORIE, STATUT FROM CLIENT WHERE NOM LIKE :nom");
    q.bindValue(":nom", "%" + nom + "%");
    q.exec();
    model->setQuery(std::move(q));
    return model;
}

QSqlQueryModel* Client::trierParNbCommandes()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_CLIENT, NOM, TYPE_CLIENT, TELEPHONE, EMAIL, ADRESSE, "
                    "NB_COMMANDES, CATEGORIE, STATUT FROM CLIENT "
                    "ORDER BY NB_COMMANDES DESC");
    return model;
}

bool Client::mettreAJourCategories()
{
    QSqlQuery q;
    return q.exec("UPDATE CLIENT SET CATEGORIE = CASE "
                  "WHEN NB_COMMANDES >= 10 THEN 'VIP' "
                  "WHEN NB_COMMANDES >= 3 THEN 'Régulier' "
                  "ELSE 'Nouveau' END");
}