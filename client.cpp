#include "client.h"
#include <QSqlQuery>
#include <QVariant>

// Liste des colonnes qu'on affiche dans le tableau.
// ATTENTION : l'ordre compte, car ClientModel lit la colonne 7 (CATEGORIE)
// et la colonne 8 (STATUT) pour colorer les lignes.
static const QString COLONNES =
    "ID_CLIENT, NOM, TYPE_CLIENT, TELEPHONE, EMAIL, ADRESSE, "
    "NB_COMMANDES, CATEGORIE, STATUT, DATE_DERNIERE_CMD";

// Constructeur : range les valeurs reçues dans les attributs de l'objet.
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

// Ajoute les deux champs supplémentaires (nb commandes + dernière commande)
void Client::setExtra(int nb, QString dateDerniereCmd)
{
    nbCmd = nb;
    derniereCmd = dateDerniereCmd;
}

// CREATE : insère un client dans la base
bool Client::ajouter()
{
    QSqlQuery q;
    // prepare() + bindValue() : on met des :paramètres au lieu de coller le texte.
    // Cela protège contre l'injection SQL.
    q.prepare("INSERT INTO CLIENT (ID_CLIENT, NOM, TYPE_CLIENT, TELEPHONE, EMAIL, ADRESSE, "
              "NB_COMMANDES, DATE_DERNIERE_CMD) "
              "VALUES (:id, :nom, :type, :tel, :email, :adr, :nb, :dc)");
    q.bindValue(":id", id);
    q.bindValue(":nom", nom);
    q.bindValue(":type", type);
    q.bindValue(":tel", telephone);
    q.bindValue(":email", email);
    q.bindValue(":adr", adresse);
    q.bindValue(":nb", nbCmd);
    // Si la date est vide, on enregistre NULL (valeur vide) dans la base
    q.bindValue(":dc", derniereCmd.isEmpty() ? QVariant() : QVariant(derniereCmd));
    return q.exec();   // true si l'insertion a réussi
}

// READ : récupère tous les clients pour le tableau
QSqlQueryModel* Client::afficher()
{
    ClientModel* model = new ClientModel();
    model->setQuery("SELECT " + COLONNES + " FROM CLIENT");
    return model;
}

// UPDATE : modifie le client dont l'ID correspond
bool Client::modifier()
{
    QSqlQuery q;
    q.prepare("UPDATE CLIENT SET NOM=:nom, TYPE_CLIENT=:type, TELEPHONE=:tel, "
              "EMAIL=:email, ADRESSE=:adr, NB_COMMANDES=:nb, DATE_DERNIERE_CMD=:dc "
              "WHERE ID_CLIENT=:id");
    q.bindValue(":id", id);
    q.bindValue(":nom", nom);
    q.bindValue(":type", type);
    q.bindValue(":tel", telephone);
    q.bindValue(":email", email);
    q.bindValue(":adr", adresse);
    q.bindValue(":nb", nbCmd);
    q.bindValue(":dc", derniereCmd.isEmpty() ? QVariant() : QVariant(derniereCmd));
    return q.exec();
}

// DELETE : supprime le client dont l'ID est donné
bool Client::supprimer(int id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM CLIENT WHERE ID_CLIENT=:id");
    q.bindValue(":id", id);
    return q.exec();
}

// RECHERCHE : clients dont le nom contient le texte tapé
QSqlQueryModel* Client::rechercher(QString nom)
{
    ClientModel* model = new ClientModel();
    QSqlQuery q;
    // LIKE avec % devant et derrière = "contient". Ex : "ha" trouve "Khalil Hamza"
    q.prepare("SELECT " + COLONNES + " FROM CLIENT WHERE NOM LIKE :nom");
    q.bindValue(":nom", "%" + nom + "%");
    q.exec();
    model->setQuery(std::move(q));
    return model;
}

// TRI : du client qui a le plus de commandes au moins de commandes
QSqlQueryModel* Client::trierParNbCommandes()
{
    ClientModel* model = new ClientModel();
    // DESC = décroissant (le plus grand en premier)
    model->setQuery("SELECT " + COLONNES + " FROM CLIENT ORDER BY NB_COMMANDES DESC");
    return model;
}

// INNOVANT 1 : calcule la catégorie de chaque client selon son nb de commandes
bool Client::mettreAJourCategories()
{
    QSqlQuery q;
    // CASE WHEN = un "si / sinon si / sinon" en SQL
    return q.exec("UPDATE CLIENT SET CATEGORIE = CASE "
                  "WHEN NB_COMMANDES >= 10 THEN 'VIP' "
                  "WHEN NB_COMMANDES >= 3 THEN 'Régulier' "
                  "ELSE 'Nouveau' END");
}

// INNOVANT 2 : passe en "Inactif" les clients sans commande depuis 6 mois.
// Retourne le nombre de clients modifiés, ou -1 en cas d'erreur.
int Client::detecterInactifs()
{
    QSqlQuery q;
    // COALESCE prend la date de dernière commande, ou à défaut la date d'inscription.
    // date('now','-6 months') = la date d'il y a 6 mois (syntaxe SQLite).
    bool ok = q.exec("UPDATE CLIENT SET STATUT = 'Inactif' "
                     "WHERE STATUT = 'Actif' "
                     "AND COALESCE(DATE_DERNIERE_CMD, DATE_INSCRIPTION) "
                     "< date('now', '-6 months')");
    if (!ok)
        return -1;
    return q.numRowsAffected();   // nombre de lignes modifiées
}
