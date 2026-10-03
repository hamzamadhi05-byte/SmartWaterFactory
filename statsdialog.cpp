#include "statsdialog.h"
#include <QPainter>
#include <QSqlQuery>

StatsDialog::StatsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Statistiques clients par catégorie");
    resize(560, 360);

    donnees["Nouveau"] = 0;
    donnees["Régulier"] = 0;
    donnees["VIP"] = 0;

    QSqlQuery q("SELECT CATEGORIE, COUNT(*) FROM CLIENT GROUP BY CATEGORIE");
    while (q.next())
        donnees[q.value(0).toString()] = q.value(1).toInt();
}

void StatsDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int total = 0;
    for (int v : donnees)
        total += v;

    p.setFont(QFont("Arial", 13, QFont::Bold));
    p.drawText(20, 30, "Répartition des clients par catégorie");

    if (total == 0) {
        p.setFont(QFont("Arial", 11));
        p.drawText(20, 70, "Aucun client à afficher.");
        return;
    }

    QStringList noms = {"Nouveau", "Régulier", "VIP"};
    QList<QColor> couleurs = {QColor(31, 92, 153), QColor(232, 116, 59), QColor(245, 179, 1)};

    QRect cercle(30, 60, 240, 240);
    int angleDebut = 90 * 16;
    for (int i = 0; i < noms.size(); ++i) {
        int n = donnees[noms[i]];
        int angle = -static_cast<int>(360.0 * 16 * n / total);
        p.setBrush(couleurs[i]);
        p.drawPie(cercle, angleDebut, angle);
        angleDebut += angle;
    }

    p.setFont(QFont("Arial", 11));
    int y = 110;
    for (int i = 0; i < noms.size(); ++i) {
        p.setBrush(couleurs[i]);
        p.drawRect(320, y - 12, 16, 16);
        int n = donnees[noms[i]];
        p.drawText(345, y, QString("%1 : %2 (%3%)")
                               .arg(noms[i]).arg(n).arg(100.0 * n / total, 0, 'f', 1));
        y += 40;
    }
}