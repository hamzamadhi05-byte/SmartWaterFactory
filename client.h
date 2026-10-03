#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QSqlQueryModel>
#include <QColor>
#include <QBrush>

// ClientModel : un tableau (modèle) qui colore certaines lignes.
// Il hérite de QSqlQueryModel, donc il sait lire le résultat d'une requête SQL.
class ClientModel : public QSqlQueryModel
{
public:
    // data() est appelée par le tableau pour chaque cellule à afficher.
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override
    {
        // Le rôle "BackgroundRole" demande la couleur de fond de la cellule
        if (role == Qt::BackgroundRole) {
            // colonne 8 = STATUT, colonne 7 = CATEGORIE (voir l'ordre du SELECT)
            QString statut = QSqlQueryModel::data(index(idx.row(), 8)).toString();
            QString categorie = QSqlQueryModel::data(index(idx.row(), 7)).toString();
            if (statut == "Inactif")
                return QBrush(QColor(255, 200, 150));   // orange clair
            if (categorie == "VIP")
                return QBrush(QColor(255, 235, 150));   // jaune doré
        }
        // Pour tout le reste, on garde le comportement normal
        return QSqlQueryModel::data(idx, role);
    }
};

// La classe Client représente un client de la table CLIENT.
// Chaque attribut correspond à une colonne de la base.
class Client
{
    int id = 0;
    QString nom, type, telephone, email, adresse;
    int nbCmd = 0;          // nombre de commandes
    QString derniereCmd;    // date de dernière commande (AAAA-MM-JJ)

public:
    Client() {}
    Client(int id, QString nom, QString type,
           QString telephone, QString email, QString adresse);

    void setExtra(int nb, QString dateDerniereCmd);

    // --- CRUD ---
    bool ajouter();                    // Create
    QSqlQueryModel* afficher();        // Read
    bool modifier();                   // Update
    bool supprimer(int id);            // Delete

    // --- Fonctionnalités avancées ---
    QSqlQueryModel* rechercher(QString nom);   // recherche par nom
    QSqlQueryModel* trierParNbCommandes();     // tri décroissant
    bool mettreAJourCategories();              // innovant 1
    int detecterInactifs();                    // innovant 2
};

#endif
