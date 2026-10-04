#include "mainwindow.h"
#include "ui_mainwindow.h"   // généré par Qt à partir de mainwindow.ui
#include "client.h"
#include "statsdialog.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QPdfWriter>
#include <QPageSize>
#include <QPainter>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QSqlQuery>
#include <QDate>

// Constructeur : s'exécute à l'ouverture de la fenêtre
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);   // construit l'interface dessinée dans le Designer
    rafraichir();        // remplit le tableau dès le départ
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Recharge tous les clients dans le tableau
void MainWindow::rafraichir()
{
    Client c;
    ui->tableClients->setModel(c.afficher());
}

// Contrôle de saisie : retourne true seulement si tous les champs sont corrects.
// Dès qu'un champ est faux, on affiche un message et on arrête (return false).
bool MainWindow::saisieValide()
{
    // ID : obligatoire et strictement positif
    if (ui->leId->text().trimmed().isEmpty() || ui->leId->text().toInt() <= 0) {
        QMessageBox::warning(this, "Erreur", "L'ID doit être un nombre positif.");
        return false;
    }
    // Nom : non vide (trimmed() enlève les espaces au début et à la fin)
    if (ui->leNom->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom est obligatoire.");
        return false;
    }
    // Téléphone : exactement 8 chiffres ([0-9]{8})
    QRegularExpression telRx("^[0-9]{8}$");
    if (!telRx.match(ui->leTel->text()).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Le téléphone doit contenir 8 chiffres.");
        return false;
    }
    // Email : forme texte@domaine.extension
    QRegularExpression mailRx("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!mailRx.match(ui->leEmail->text()).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Email invalide (exemple : nom@mail.com).");
        return false;
    }
    // Adresse : non vide
    if (ui->leAdresse->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "L'adresse est obligatoire.");
        return false;
    }

    // Nombre de commandes : facultatif, mais s'il est rempli, entier >= 0
    QString nb = ui->leNbCmd->text().trimmed();
    if (!nb.isEmpty()) {
        bool ok = false;
        int n = nb.toInt(&ok);   // ok devient false si ce n'est pas un nombre
        if (!ok || n < 0) {
            QMessageBox::warning(this, "Erreur", "Le nombre de commandes doit être un entier positif.");
            return false;
        }
    }

    // Date : facultative, mais s'il y en a une, elle doit être valide (AAAA-MM-JJ)
    QString date = ui->leDerniereCmd->text().trimmed();
    if (!date.isEmpty() && !QDate::fromString(date, "yyyy-MM-dd").isValid()) {
        QMessageBox::warning(this, "Erreur",
                             "La date doit avoir le format AAAA-MM-JJ (exemple : 2026-03-15).");
        return false;
    }
    return true;
}

// Slot du bouton "Ajouter".
// Qt relie automatiquement le bouton btnAjouter à cette fonction grâce au nom
// on_<nomDuBouton>_clicked().
void MainWindow::on_btnAjouter_clicked()
{
    if (!saisieValide())
        return;   // on s'arrête si la saisie est incorrecte

    // On lit les champs du formulaire et on crée un objet Client
    Client c(ui->leId->text().toInt(), ui->leNom->text(), ui->cbType->currentText(),
             ui->leTel->text(), ui->leEmail->text(), ui->leAdresse->text());
    c.setExtra(ui->leNbCmd->text().toInt(), ui->leDerniereCmd->text().trimmed());

    if (c.ajouter()) {
        QMessageBox::information(this, "Succès", "Client ajouté.");
        rafraichir();   // on recharge le tableau pour voir le nouveau client
    } else {
        // L'insertion échoue notamment si l'ID existe déjà (clé primaire)
        QMessageBox::critical(this, "Erreur", "Ajout impossible (ID déjà utilisé ?).");
    }
}

// Slot du bouton "Modifier" : même principe, mais on met à jour le client existant
void MainWindow::on_btnModifier_clicked()
{
    if (!saisieValide())
        return;

    Client c(ui->leId->text().toInt(), ui->leNom->text(), ui->cbType->currentText(),
             ui->leTel->text(), ui->leEmail->text(), ui->leAdresse->text());
    c.setExtra(ui->leNbCmd->text().toInt(), ui->leDerniereCmd->text().trimmed());

    if (c.modifier()) {
        QMessageBox::information(this, "Succès", "Client modifié.");
        rafraichir();
    } else {
        QMessageBox::critical(this, "Erreur", "Modification impossible.");
    }
}

// Slot du bouton "Supprimer" : on demande confirmation avant de supprimer
void MainWindow::on_btnSupprimer_clicked()
{
    int id = ui->leId->text().toInt();
    if (QMessageBox::question(this, "Confirmation", "Supprimer ce client ?")
        != QMessageBox::Yes)
        return;   // l'utilisateur a répondu Non
    Client c;
    if (c.supprimer(id)) {
        QMessageBox::information(this, "Succès", "Client supprimé.");
        rafraichir();
    } else {
        QMessageBox::critical(this, "Erreur", "Suppression impossible.");
    }
}

// Quand on clique sur une ligne du tableau, on recopie ses valeurs dans le formulaire
// (pratique pour modifier ou supprimer). index.row() = numéro de la ligne cliquée.
void MainWindow::on_tableClients_clicked(const QModelIndex &index)
{
    int row = index.row();
    auto m = ui->tableClients->model();
    ui->leId->setText(m->index(row, 0).data().toString());
    ui->leNom->setText(m->index(row, 1).data().toString());
    ui->cbType->setCurrentText(m->index(row, 2).data().toString());
    ui->leTel->setText(m->index(row, 3).data().toString());
    ui->leEmail->setText(m->index(row, 4).data().toString());
    ui->leAdresse->setText(m->index(row, 5).data().toString());
    ui->leNbCmd->setText(m->index(row, 6).data().toString());
    ui->leDerniereCmd->setText(m->index(row, 9).data().toString());
}

// Recherche en direct : s'exécute à chaque lettre tapée dans le champ de recherche
void MainWindow::on_leRecherche_textChanged(const QString &texte)
{
    Client c;
    ui->tableClients->setModel(c.rechercher(texte));
}

// Bouton "Trier" : affiche les clients du plus grand nb de commandes au plus petit
void MainWindow::on_btnTri_clicked()
{
    Client c;
    ui->tableClients->setModel(c.trierParNbCommandes());
}

// Bouton "Actualiser" : vide la recherche et réaffiche tout
void MainWindow::on_btnActualiser_clicked()
{
    ui->leRecherche->clear();
    rafraichir();
}

// Export PDF : crée une fiche pour le client sélectionné
void MainWindow::on_btnPdf_clicked()
{
    QString id = ui->leId->text().trimmed();
    if (id.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Sélectionnez d'abord un client dans le tableau.");
        return;
    }

    // On relit le client dans la base pour avoir ses valeurs à jour
    QSqlQuery q;
    q.prepare("SELECT ID_CLIENT, NOM, TYPE_CLIENT, TELEPHONE, EMAIL, ADRESSE, "
              "NB_COMMANDES, CATEGORIE, STATUT FROM CLIENT WHERE ID_CLIENT=:id");
    q.bindValue(":id", id.toInt());
    if (!q.exec() || !q.next()) {
        QMessageBox::critical(this, "Erreur", "Client introuvable.");
        return;
    }

    // Fenêtre "Enregistrer sous" pour choisir où mettre le PDF
    QString fichier = QFileDialog::getSaveFileName(this, "Enregistrer la fiche",
                                                   "fiche_client_" + id + ".pdf",
                                                   "PDF (*.pdf)");
    if (fichier.isEmpty())
        return;   // l'utilisateur a annulé

    // QPdfWriter = la "feuille", QPainter = le "stylo" qui écrit dessus
    QPdfWriter pdf(fichier);
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setResolution(120);
    QPainter p(&pdf);

    // En-tête en bleu (couleur de la charte graphique)
    p.setPen(QColor(31, 92, 153));
    p.setFont(QFont("Arial", 22, QFont::Bold));
    p.drawText(100, 150, "Smart Water Factory");
    p.setFont(QFont("Arial", 16, QFont::Bold));
    p.drawText(100, 230, "Fiche client");

    // Corps : une ligne par information, 80 pixels plus bas à chaque fois
    p.setPen(Qt::black);
    p.setFont(QFont("Arial", 12));
    QStringList labels = {"ID Client", "Nom", "Type", "Téléphone", "Email",
                          "Adresse", "Nb commandes", "Catégorie", "Statut"};
    int y = 330;
    for (int i = 0; i < labels.size(); ++i) {
        p.drawText(100, y, labels[i] + " : " + q.value(i).toString());
        y += 80;
    }
    p.drawText(100, y + 60, "Généré le " + QDate::currentDate().toString("dd/MM/yyyy"));
    p.end();   // termine et enregistre le PDF

    QMessageBox::information(this, "Succès", "Fiche PDF enregistrée.");
}

// Export Excel : écrit un fichier CSV (séparé par des ;) que Excel sait ouvrir
void MainWindow::on_btnExcel_clicked()
{
    QString fichier = QFileDialog::getSaveFileName(this, "Exporter la liste",
                                                   "clients.csv", "CSV (*.csv)");
    if (fichier.isEmpty())
        return;

    QFile f(fichier);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier.");
        return;
    }
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    out.setGenerateByteOrderMark(true);   // pour que Excel affiche bien les accents
    out << "ID;Nom;Type;Téléphone;Email;Adresse;Nb commandes;Catégorie;Statut\n";

    // Une ligne CSV par client
    QSqlQuery q("SELECT ID_CLIENT, NOM, TYPE_CLIENT, TELEPHONE, EMAIL, ADRESSE, "
                "NB_COMMANDES, CATEGORIE, STATUT FROM CLIENT");
    while (q.next()) {
        QStringList ligne;
        for (int i = 0; i < 9; ++i)
            ligne << q.value(i).toString();
        out << ligne.join(";") << "\n";
    }
    f.close();
    QMessageBox::information(this, "Succès", "Liste exportée (ouvrable avec Excel).");
}

// Innovant 1 : recalcule les catégories (Nouveau / Régulier / VIP)
void MainWindow::on_btnCategorie_clicked()
{
    Client c;
    if (c.mettreAJourCategories()) {
        QMessageBox::information(this, "Succès", "Catégories mises à jour.");
        rafraichir();
    } else {
        QMessageBox::critical(this, "Erreur", "Mise à jour impossible.");
    }
}

// Innovant 2 : détecte les clients sans commande depuis plus de 6 mois
void MainWindow::on_btnInactifs_clicked()
{
    Client c;
    int n = c.detecterInactifs();   // n = nombre de clients devenus inactifs
    if (n < 0) {
        QMessageBox::critical(this, "Erreur", "Détection impossible.");
        return;
    }
    rafraichir();   // les lignes inactives apparaissent en orange
    if (n == 0)
        QMessageBox::information(this, "Alerte relance", "Aucun client inactif détecté.");
    else
        QMessageBox::warning(this, "Alerte relance",
                             QString::number(n) + " client(s) inactif(s) à relancer "
                             "(aucune commande depuis plus de 6 mois).");
}

// Statistiques : ouvre la fenêtre avec le camembert
void MainWindow::on_btnStats_clicked()
{
    StatsDialog dlg(this);
    dlg.exec();   // exec() ouvre la fenêtre et attend qu'on la ferme
}
