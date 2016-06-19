#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    connect(ui->comboBoxBlackSpecies, SIGNAL(currentTextChanged(QString)), this, SLOT(blackSpeciesChanged(QString)));
    connect(ui->comboBoxWhiteSpecies, SIGNAL(currentTextChanged(QString)), this, SLOT(whiteSpeciesChanged(QString)));
    readSettings();
}

Settings::~Settings()
{
    delete ui;
}

void Settings::writeSettings()
{
    config.beginGroup("Players");
    config.setValue("black_name",  ui->lineEditBlackName->text());
    config.setValue("black_species",  ui->comboBoxBlackSpecies->currentText());
    config.setValue("black_url",  ui->lineEditBlackUrl->text());
    config.setValue("handicap", ui->spinBoxHandicap->value());
    config.setValue("free_placement", ui->checkBoxFreePlacement->isChecked());

    config.setValue("white_name",  ui->lineEditWhiteName->text());
    config.setValue("white_species",  ui->comboBoxWhiteSpecies->currentText());
    config.setValue("white_url",  ui->lineEditWhiteUrl->text());
    config.setValue("komi", ui->doubleSpinBoxKomi->value());

    config.endGroup();

    config.beginGroup("GoGoGo");
    config.setValue("board_size", ui->comboBoxBoardSize->currentText());
    config.endGroup();

    config.beginGroup("Appearance");
    config.endGroup();

    config.beginGroup("Engine");
    config.setValue("program_path",  ui->lineEditEnginePath->text());
    config.endGroup();
}

void Settings::readSettings()
{
    config.beginGroup("Players");
    ui->lineEditBlackName->setText( config.value("black_name").toString());
    ui->comboBoxBlackSpecies->setCurrentText( config.value("black_species", QString("Human")).toString());
    ui->lineEditBlackUrl->setText( config.value("black_url").toString());
    ui->spinBoxHandicap->setValue( config.value("handicap").toInt());
    ui->checkBoxFreePlacement->setChecked( config.value("free_placement").toBool());

    ui->lineEditWhiteName->setText( config.value("white_name").toString());
    ui->comboBoxWhiteSpecies->setCurrentText( config.value("white_species", QString("Human")).toString());
    ui->lineEditWhiteUrl->setText( config.value("white_url").toString());
    ui->doubleSpinBoxKomi->setValue( config.value("komi", 6.5).toDouble());
    config.endGroup();

    config.beginGroup("GoGoGo");
    ui->comboBoxBoardSize->setCurrentText( config.value("board_size", QString("19x19")).toString());
    config.endGroup();

    config.beginGroup("Appearance");
    config.endGroup();

    config.beginGroup("Engine");
    ui->lineEditEnginePath->setText( config.value("program_path", QString("/usr/games/gnugo")).toString());
    config.endGroup();
}
void Settings::on_buttonBox_accepted()
{
    writeSettings();
}

void Settings::blackSpeciesChanged(QString txt){
    if(txt == "Network"){
        ui->labelBlackUrl->setEnabled(true);
        ui->lineEditBlackUrl->setEnabled(true);
    }else{
        ui->labelBlackUrl->setEnabled(false);
        ui->lineEditBlackUrl->setEnabled(false);
    }
}

void Settings::whiteSpeciesChanged(QString txt){
    if(txt == "Network"){
        ui->labelWhiteUrl->setEnabled(true);
        ui->lineEditWhiteUrl->setEnabled(true);
    }else{
        ui->labelWhiteUrl->setEnabled(false);
        ui->lineEditWhiteUrl->setEnabled(false);
    }
}
