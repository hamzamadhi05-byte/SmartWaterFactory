#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    rafraichir();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::rafraichir()
{
    Client c;
    ui->tableClients->setModel(c.afficher());
}

bool MainWindow::saisieValide()
{
    if (ui->leId->text().trimmed().isEmpty() || ui->leId->text().toInt() <= 0) {
        QMessageBox::warning(this, "Erreur", "L'ID doit être un nombre positif.");
        return false;
    }
    if (ui->leNom->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom est obligatoire.");
        return false;
    }
    QRegularExpression telRx("^[0-9]{8}$");
    if (!telRx.match(ui->leTel->text()).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Le téléphone doit contenir 8 chiffres.");
        return false;
    }
    QRegularExpression mailRx("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!mailRx.match(ui->leEmail->text()).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Email invalide (exemple : nom@mail.com).");
        return false;
    }
    if (ui->leAdresse->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "L'adresse est obligatoire.");
        return false;
    }
    return true;
}

void MainWindow::on_btnAjouter_clicked()
{
    if (!saisieValide())
        return;

    Client c(ui->leId->text().toInt(), ui->leNom->text(), ui->cbType->currentText(),
             ui->leTel->text(), ui->leEmail->text(), ui->leAdresse->text());
    if (c.ajouter()) {
        QMessageBox::information(this, "Succès", "Client ajouté.");
        rafraichir();
    } else {
        QMessageBox::critical(this, "Erreur", "Ajout impossible (ID déjà utilisé ?).");
    }
}

void MainWindow::on_btnModifier_clicked()
{
    if (!saisieValide())
        return;

    Client c(ui->leId->text().toInt(), ui->leNom->text(), ui->cbType->currentText(),
             ui->leTel->text(), ui->leEmail->text(), ui->leAdresse->text());
    if (c.modifier()) {
        QMessageBox::information(this, "Succès", "Client modifié.");
        rafraichir();
    } else {
        QMessageBox::critical(this, "Erreur", "Modification impossible.");
    }
}

void MainWindow::on_btnSupprimer_clicked()
{
    int id = ui->leId->text().toInt();
    if (QMessageBox::question(this, "Confirmation", "Supprimer ce client ?")
        != QMessageBox::Yes)
        return;
    Client c;
    if (c.supprimer(id)) {
        QMessageBox::information(this, "Succès", "Client supprimé.");
        rafraichir();
    } else {
        QMessageBox::critical(this, "Erreur", "Suppression impossible.");
    }
}

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
}

void MainWindow::on_leRecherche_textChanged(const QString &texte)
{
    Client c;
    ui->tableClients->setModel(c.rechercher(texte));
}

void MainWindow::on_btnTri_clicked()
{
    Client c;
    ui->tableClients->setModel(c.trierParNbCommandes());
}

void MainWindow::on_btnActualiser_clicked()
{
    ui->leRecherche->clear();
    rafraichir();
}

void MainWindow::on_btnPdf_clicked()
{
    QString id = ui->leId->text().trimmed();
    if (id.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Sélectionnez d'abord un client dans le tableau.");
        return;
    }

    QSqlQuery q;
    q.prepare("SELECT ID_CLIENT, NOM, TYPE_CLIENT, TELEPHONE, EMAIL, ADRESSE, "
              "NB_COMMANDES, CATEGORIE, STATUT FROM CLIENT WHERE ID_CLIENT=:id");
    q.bindValue(":id", id.toInt());
    if (!q.exec() || !q.next()) {
        QMessageBox::critical(this, "Erreur", "Client introuvable.");
        return;
    }

    QString fichier = QFileDialog::getSaveFileName(this, "Enregistrer la fiche",
                                                   "fiche_client_" + id + ".pdf",
                                                   "PDF (*.pdf)");
    if (fichier.isEmpty())
        return;

    QPdfWriter pdf(fichier);
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setResolution(120);
    QPainter p(&pdf);

    p.setPen(QColor(31, 92, 153));
    p.setFont(QFont("Arial", 22, QFont::Bold));
    p.drawText(100, 150, "Smart Water Factory");
    p.setFont(QFont("Arial", 16, QFont::Bold));
    p.drawText(100, 230, "Fiche client");

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
    p.end();

    QMessageBox::information(this, "Succès", "Fiche PDF enregistrée.");
}

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
    out.setGenerateByteOrderMark(true);
    out << "ID;Nom;Type;Téléphone;Email;Adresse;Nb commandes;Catégorie;Statut\n";

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

void MainWindow::on_btnInactifs_clicked()
{
    Client c;
    int n = c.detecterInactifs();
    if (n < 0) {
        QMessageBox::critical(this, "Erreur", "Détection impossible.");
        return;
    }
    rafraichir();
    if (n == 0)
        QMessageBox::information(this, "Alerte relance", "Aucun client inactif détecté.");
    else
        QMessageBox::warning(this, "Alerte relance",
                             QString::number(n) + " client(s) inactif(s) à relancer "
                                                  "(aucune commande depuis plus de 6 mois).");
}