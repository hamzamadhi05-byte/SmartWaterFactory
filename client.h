#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QSqlQueryModel>
#include <QColor>
#include <QBrush>

// Model li yلawwen el lignes: orange = Inactif, doré = VIP
class ClientModel : public QSqlQueryModel
{
public:
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::BackgroundRole) {
            QString statut = QSqlQueryModel::data(index(idx.row(), 8)).toString();
            QString categorie = QSqlQueryModel::data(index(idx.row(), 7)).toString();
            if (statut == "Inactif")
                return QBrush(QColor(255, 200, 150));
            if (categorie == "VIP")
                return QBrush(QColor(255, 235, 150));
        }
        return QSqlQueryModel::data(idx, role);
    }
};

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
    int detecterInactifs();
};

#endif