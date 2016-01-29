#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Settings.h"
#include "AudioPlayer.h"

#include <QLocale>
#include <QApplication>
#include <QSlider>
#include <QFont>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QDebug>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    version = 1;
    factor = 10000;
    ui->setupUi(this);
    initButtons();
    initSliders();
//    onPlayerCallback();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initButtons()
{
    Osc1 = ui->buttonOsc1Sine;
    Osc2 = ui->buttonOsc2Sine;
    Lfo = ui->buttonLfoSine;
    Filter = ui->buttonFilterLow;
    Power = ui->buttonPower12dB;

    Osc1Id = 0;
    Osc2Id = 0;
    LfoId = 0;
    FilterId = 0;
    PowerId = 0;

    Osc1->setChecked(true);
    Osc2->setChecked(true);
    Lfo->setChecked(true);
    Filter->setChecked(true);
    Power->setChecked(true);

}

void MainWindow::setButtons()
{
    deactiveButton(Osc1);
    deactiveButton(Osc2);
    deactiveButton(Lfo);
    deactiveButton(Filter);
    deactiveButton(Power);

    initButtons();
}

void MainWindow::unsetButtons()
{
    deactiveButton(Osc1);
    deactiveButton(Osc2);
    deactiveButton(Lfo);
    deactiveButton(Filter);
    deactiveButton(Power);
}

void MainWindow::initSliders()
{
//set range
    ui->sliderOsc1Semi->setRange(-36, 36);
    ui->sliderOsc1Cents->setRange(0, 100);

    ui->sliderOsc2Semi->setRange(-36, 36);
    ui->sliderOsc2Cents->setRange(0, 100);
    ui->sliderOsc2Phase->setRange(0, 360);

    setSliderFloatRange(ui->sliderOscmix, 0, 1);

    setSliderFloatRange(ui->sliderLfoAmp, 0, 1);
    setSliderFloatRange(ui->sliderLfoCutoff, 0, 1);
    setSliderFloatRange(ui->sliderLfoOsc, 0, 1);
    ui->sliderLfoSpeed->setRange(0, 50);

    setSliderFloatRange(ui->sliderAmpEnvAttack, 0, 2.5);
    setSliderFloatRange(ui->sliderAmpEnvDecay, 0, 2.5);
    setSliderFloatRange(ui->sliderAmpEnvRelease, 0, 2.5);
    setSliderFloatRange(ui->sliderAmpEnvSustain, 0, 1);

    setSliderFloatRange(ui->sliderFilterCutoff, 0, 1);
    setSliderFloatRange(ui->sliderFilterRes, 0, 1);
//set default values
    ui->sliderOsc1Semi->setValue(0);
    ui->sliderOsc1Cents->setValue(0);

    ui->sliderOsc2Semi->setValue(0);
    ui->sliderOsc2Cents->setValue(0);
    ui->sliderOsc2Phase->setValue(0);

    setSliderFloatValue(ui->sliderOscmix, 0.5);

    ui->sliderLfoAmp->setValue(0);
    ui->sliderLfoCutoff->setValue(0);
    ui->sliderLfoOsc->setValue(0);
    ui->sliderLfoSpeed->setValue(0);

    ui->sliderAmpEnvAttack->setValue(0);
    ui->sliderAmpEnvDecay->setValue(0);
    ui->sliderAmpEnvRelease->setValue(0);
    setSliderFloatValue(ui->sliderAmpEnvSustain, 1);

    setSliderFloatValue(ui->sliderFilterCutoff, 1);
    ui->sliderFilterRes->setValue(0);

//set slider default values to lable
    ui->labelOsc1SemiValue->setText("0");
    ui->labelOsc1CentsValue->setText("0");

    ui->labelOscmixValue->setText("0.5000");

    ui->labelOsc2SemiValue->setText("0");
    ui->labelOsc2CentsValue->setText("0");
    ui->labelOsc2PhaseValue->setText("0");

    ui->labelLfoAmpValue->setText("0");
    ui->labelLfoCutoffValue->setText("0");
    ui->labelLfoOscValue->setText("0");
    ui->labelLfoSpeedValue->setText("0");

    ui->labelAmpEnvAttackValue->setText("0");
    ui->labelAmpEnvDecayValue->setText("0");
    ui->labelAmpEnvReleaseValue->setText("0");
    ui->labelAmpEnvSustainValue->setText("1.0000");

    ui->labelFilterCutoffValue->setText("1.0000");
    ui->labelFilterResValue->setText("0");
}

void MainWindow::deactiveButton(QPushButton* button)
{
    button->setChecked(false);
}

void MainWindow::on_buttonOsc1Sine_clicked()
{
    deactiveButton(Osc1);
    Osc1 = ui->buttonOsc1Sine;
    Osc1Id = 0;
    ui->buttonOsc1Sine->setChecked(true);
}

void MainWindow::on_buttonOsc1Saw_clicked()
{
    deactiveButton(Osc1);
    Osc1 = ui->buttonOsc1Saw;
    Osc1Id = 1;
    ui->buttonOsc1Saw->setChecked(true);
}

void MainWindow::on_buttonOsc1Squ_clicked()
{
    deactiveButton(Osc1);
    Osc1 = ui->buttonOsc1Squ;
    Osc1Id = 2;
    ui->buttonOsc1Squ->setChecked(true);
}


void MainWindow::on_buttonOsc1Tri_clicked()
{
    deactiveButton(Osc1);
    Osc1 = ui->buttonOsc1Tri;
    Osc1Id = 3;
    ui->buttonOsc1Tri->setChecked(true);
}

void MainWindow::on_buttonOsc1Noise_clicked()
{
    deactiveButton(Osc1);
    Osc1 = ui->buttonOsc1Noise;
    Osc1Id = 4;
    ui->buttonOsc1Noise->setChecked(true);
}

void MainWindow::on_buttonOsc2Sine_clicked()
{
    deactiveButton(Osc2);
    Osc2 = ui->buttonOsc2Sine;
    Osc2Id = 0;
    ui->buttonOsc2Sine->setChecked(true);
}

void MainWindow::on_buttonOsc2Saw_clicked()
{
    deactiveButton(Osc2);
    Osc2 = ui->buttonOsc2Saw;
    Osc2Id = 1;
    ui->buttonOsc2Saw->setChecked(true);
}

void MainWindow::on_buttonOsc2Squ_clicked()
{
    deactiveButton(Osc2);
    Osc2 = ui->buttonOsc2Squ;
    Osc2Id = 2;
    ui->buttonOsc2Squ->setChecked(true);
}

void MainWindow::on_buttonOsc2Tri_clicked()
{
    deactiveButton(Osc2);
    Osc2 = ui->buttonOsc2Tri;
    Osc2Id = 3;
    ui->buttonOsc2Tri->setChecked(true);
}

void MainWindow::on_buttonOsc2Noise_clicked()
{
    deactiveButton(Osc2);
    Osc2 = ui->buttonOsc2Noise;
    Osc2Id = 4;
    ui->buttonOsc2Noise->setChecked(true);
}

void MainWindow::on_buttonLfoSine_clicked()
{
    deactiveButton(Lfo);
    Lfo = ui->buttonLfoSine;
    LfoId = 0;
    ui->buttonLfoSine->setChecked(true);
}

void MainWindow::on_buttonLfoSaw_clicked()
{
    deactiveButton(Lfo);
    Lfo = ui->buttonLfoSaw;
    LfoId = 1;
    ui->buttonLfoSaw->setChecked(true);
}


void MainWindow::on_buttonLfoTri_clicked()
{
    deactiveButton(Lfo);
    Lfo = ui->buttonLfoTri;
    LfoId = 2;
    ui->buttonLfoTri->setChecked(true);
}

void MainWindow::on_buttonLfoNoise_clicked()
{
    deactiveButton(Lfo);
    Lfo = ui->buttonLfoNoise;
    LfoId = 3;
    ui->buttonLfoNoise->setChecked(true);
}

void MainWindow::on_buttonFilterLow_clicked()
{
    deactiveButton(Filter);
    Filter = ui->buttonFilterLow;
    FilterId = 0;
    ui->buttonFilterLow->setChecked(true);
}

void MainWindow::on_buttonFilterBand_clicked()
{
    deactiveButton(Filter);
    Filter = ui->buttonFilterBand;
    FilterId = 1;
    ui->buttonFilterBand->setChecked(true);
}

void MainWindow::on_buttonFilterHigh_clicked()
{
    deactiveButton(Filter);
    Filter = ui->buttonFilterHigh;
    FilterId = 2;
    ui->buttonFilterHigh->setChecked(true);
}

void MainWindow::on_buttonPower12dB_clicked()
{
    deactiveButton(Power);
    Power = ui->buttonPower12dB;
    PowerId = 0;
    ui->buttonPower12dB->setChecked(true);
}

void MainWindow::on_buttonPower24dB_clicked()
{
    deactiveButton(Power);
    Power = ui->buttonPower24dB;
    FilterId = 1;
    ui->buttonPower24dB->setChecked(true);
}

bool MainWindow::on_toolButtonOpen_released()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr(".subsynth (*.subsynth)"));
    Settings input;
    input.read(fileName.toStdString().c_str());

    unsetButtons();
    Osc1Id = input.getInt("osc1.mode", 0);
    Osc2Id = input.getInt("osc2.mode", 0);
    LfoId = input.getInt("lfo.mode", 0);
    FilterId = input.getInt("filter.mode", 0);
    PowerId = input.getInt("filter.slope", 0);

    switch(Osc1Id){
    case 0:
        ui->buttonOsc1Sine->setChecked(true); break;
    case 1:
        ui->buttonOsc1Saw->setChecked(true); break;
    case 2:
        ui->buttonOsc1Squ->setChecked(true); break;
    case 3:
        ui->buttonOsc1Tri->setChecked(true); break;
    case 4:
        ui->buttonOsc1Noise->setChecked(true);
    }

    switch(Osc2Id){
    case 0:
        ui->buttonOsc2Sine->setChecked(true); break;
    case 1:
        ui->buttonOsc2Saw->setChecked(true); break;
    case 2:
        ui->buttonOsc2Squ->setChecked(true); break;
    case 3:
        ui->buttonOsc2Tri->setChecked(true); break;
    case 4:
        ui->buttonOsc2Noise->setChecked(true);
    }

    switch(LfoId){
    case 0:
        ui->buttonLfoSine->setChecked(true); break;
    case 1:
        ui->buttonLfoSaw->setChecked(true); break;
    case 2:
        ui->buttonLfoTri->setChecked(true); break;
    case 3:
        ui->buttonLfoNoise->setChecked(true);
    }

    switch(FilterId){
    case 0:
        ui->buttonFilterLow->setChecked(true); break;
    case 1:
        ui->buttonFilterBand->setChecked(true); break;
    case 2:
        ui->buttonFilterHigh->setChecked(true);
    }

    switch(PowerId){
    case 0:
        ui->buttonPower12dB->setChecked(true); break;
    case 1:
        ui->buttonPower24dB->setChecked(true);
    }

    int intValue;
    float floatValue;
//OSC1
    intValue = input.getInt("osc1.semi", 0);
    ui->sliderOsc1Semi->setSliderPosition(intValue);
    ui->labelOsc1SemiValue->setText(QString::number(intValue));

    intValue = input.getInt("osc1.fine", 0);
    ui->sliderOsc1Cents->setSliderPosition(intValue);
    ui->labelOsc1CentsValue->setText(QString::number((intValue)));
//OSC2
    intValue = input.getInt("osc2.semi", 0);
    ui->sliderOsc2Semi->setSliderPosition(intValue);
    ui->labelOsc2SemiValue->setText(QString::number(intValue));

    intValue = input.getInt("osc2.fine", 0);
    ui->sliderOsc2Cents->setSliderPosition(intValue);
    ui->labelOsc2SemiValue->setText(QString::number(intValue));

    intValue = input.getInt("osc2.phase", 0);
    ui->sliderOsc2Phase->setSliderPosition(intValue);
    ui->labelOsc2PhaseValue->setText(QString::number(intValue));
//OSCMIX
    floatValue = input.getFloat("osc.mix", 0);
    ui->sliderOscmix->setSliderPosition(floatValue * factor);
    ui->labelOscmixValue->setText(QString::number(floatValue));
//LFO
    floatValue = input.getFloat("lfo.amp", 0);
    ui->sliderLfoAmp->setSliderPosition(floatValue * factor);
    ui->labelLfoAmpValue->setText(QString::number(floatValue));

    floatValue = input.getFloat("lfo.cutoff", 0);
    ui->sliderLfoCutoff->setSliderPosition(floatValue * factor);
    ui->labelLfoCutoffValue->setText(QString::number(floatValue));

    floatValue = input.getFloat("lfo.osc", 0);
    ui->sliderLfoOsc->setSliderPosition(floatValue * factor);
    ui->labelLfoOscValue->setText(QString::number(floatValue));

    intValue = input.getInt("lfo.speed", 0);
    ui->sliderLfoSpeed->setSliderPosition(intValue);
    ui->labelLfoSpeedValue->setText(QString::number(intValue));
//AmpEnv
    floatValue = input.getFloat("amp.attack", 0);
    ui->sliderAmpEnvAttack->setSliderPosition(floatValue * factor);
    ui->labelAmpEnvAttackValue->setText(QString::number(floatValue));

    floatValue = input.getFloat("amp.decay", 0);
    ui->sliderAmpEnvDecay->setSliderPosition(floatValue * factor);
    ui->labelAmpEnvDecayValue->setText(QString::number(floatValue));

    floatValue = input.getFloat("amp.release", 0);
    ui->sliderAmpEnvRelease->setSliderPosition(floatValue * factor);
    ui->labelAmpEnvReleaseValue->setText(QString::number(floatValue));

    floatValue = input.getFloat("amp.sustain", 0);
    ui->sliderAmpEnvSustain->setSliderPosition(floatValue * factor);
    ui->labelAmpEnvSustainValue->setText(QString::number(floatValue));
//Filter
    floatValue = input.getFloat("filter.cutoff", 0);
    ui->sliderFilterCutoff->setSliderPosition(floatValue * factor);
    ui->labelFilterCutoffValue->setText(QString::number(floatValue));

    floatValue = input.getFloat("filter.res", 0);
    ui->sliderFilterRes->setSliderPosition(floatValue * factor);
    ui->labelFilterResValue->setText(QString::number(floatValue));

    return true;
}


void MainWindow::on_toolButtonNew_released()
{
   setButtons();
   initSliders();
}

bool MainWindow::on_toolButtonSave_released()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr(".subsynth (*.subsynth)"));
    if(fileName.isEmpty() == true)
        return false;

    Settings output;

    output.set("version", version);
    output.set("osc1.mode", Osc1Id);
    output.set("osc1.semi", ui->sliderOsc1Semi->value());
    output.set("osc1.fine", ui->sliderOsc1Cents->value());

    output.set("osc2.mode", Osc2Id);
    output.set("osc2.fine", ui->sliderOsc2Cents->value());
    output.set("osc2.semi", ui->sliderOsc2Semi->value());
    output.set("osc2.phase", ui->sliderOsc2Phase->value());

    output.set("osc.mix", getSliderFloatValue(ui->sliderOscmix));

    output.set("lfo.mode", LfoId);
    output.set("lfo.speed", ui->sliderLfoSpeed->value());
    output.set("lfo.cutoff", getSliderFloatValue(ui->sliderLfoCutoff));
    output.set("lfo.osc", getSliderFloatValue(ui->sliderLfoOsc));
    output.set("lfo.amp", getSliderFloatValue(ui->sliderLfoAmp));

    output.set("amp.attack", getSliderFloatValue(ui->sliderAmpEnvAttack));
    output.set("amp.decay", getSliderFloatValue(ui->sliderAmpEnvDecay));
    output.set("amp.sustain", getSliderFloatValue(ui->sliderAmpEnvSustain));
    output.set("amp.release", getSliderFloatValue(ui->sliderAmpEnvRelease));

    output.set("filter.mode", FilterId);
    output.set("filter.slope", PowerId);
    output.set("filter.cutoff", getSliderFloatValue(ui->sliderFilterCutoff));
    output.set("filter.res", getSliderFloatValue(ui->sliderFilterRes));

    output.write(fileName.toStdString().c_str());
    return true;
}

float MainWindow::getSliderFloatValue(QSlider* slider)
{
    return (slider->value() / factor);
}

bool MainWindow::setSliderFloatValue(QSlider* slider, float value)
{
    slider->setValue(value * factor);
    return true;
}

bool MainWindow::setSliderFloatRange(QSlider* slider, float from, float until)
{
    slider->setRange(from * factor, until * factor);
    return true;
}

void MainWindow::setFloatLabelValue(QSlider* slider, QLabel* label)
{
    label->setText(QString::number(getSliderFloatValue(slider)));
}

void MainWindow::setIntLabelValue(QSlider* slider, QLabel* label)
{
    label->setText(QString::number(slider->value()));
}

void MainWindow::on_sliderOsc1Semi_sliderMoved()
{
    setIntLabelValue(ui->sliderOsc1Semi, ui->labelOsc1SemiValue);
}

void MainWindow::on_sliderOsc1Cents_sliderMoved()
{
    setIntLabelValue(ui->sliderOsc1Cents, ui->labelOsc1CentsValue);
}

void MainWindow::on_sliderOsc2Semi_sliderMoved()
{
    setIntLabelValue(ui->sliderOsc2Semi, ui->labelOsc2SemiValue);
}

void MainWindow::on_sliderOsc2Cents_sliderMoved()
{
    setIntLabelValue(ui->sliderOsc2Cents, ui->labelOsc2CentsValue);
}

void MainWindow::on_sliderOsc2Phase_sliderMoved()
{
    setIntLabelValue(ui->sliderOsc2Phase, ui->labelOsc2PhaseValue);
}

void MainWindow::on_sliderOscmix_sliderMoved()
{
    setFloatLabelValue(ui->sliderOscmix, ui->labelOscmixValue);
}

void MainWindow::on_sliderAmpEnvAttack_sliderMoved()
{
    setFloatLabelValue(ui->sliderAmpEnvAttack, ui->labelAmpEnvAttackValue);
}

void MainWindow::on_sliderAmpEnvDecay_sliderMoved()
{
    setFloatLabelValue(ui->sliderAmpEnvDecay, ui->labelAmpEnvDecayValue);
}

void MainWindow::on_sliderAmpEnvSustain_sliderMoved()
{
    setFloatLabelValue(ui->sliderAmpEnvSustain, ui->labelAmpEnvSustainValue);
}

void MainWindow::on_sliderAmpEnvRelease_sliderMoved()
{
    setFloatLabelValue(ui->sliderAmpEnvRelease, ui->labelAmpEnvReleaseValue);
}

void MainWindow::on_sliderFilterRes_sliderMoved()
{
    setFloatLabelValue(ui->sliderFilterRes, ui->labelFilterResValue);
}

void MainWindow::on_sliderFilterCutoff_sliderMoved()
{
    setFloatLabelValue(ui->sliderFilterCutoff, ui->labelFilterCutoffValue);
}

void MainWindow::on_sliderLfoSpeed_sliderMoved()
{
    setIntLabelValue(ui->sliderLfoSpeed, ui->labelLfoSpeedValue);
}

void MainWindow::on_sliderLfoOsc_sliderMoved()
{
    setFloatLabelValue(ui->sliderLfoOsc, ui->labelLfoOscValue);
}

void MainWindow::on_sliderLfoCutoff_sliderMoved()
{
    setFloatLabelValue(ui->sliderLfoCutoff, ui->labelLfoCutoffValue);
}

void MainWindow::on_sliderLfoAmp_sliderMoved()
{
    setFloatLabelValue(ui->sliderLfoAmp, ui->labelLfoAmpValue);
}

void MainWindow::on_toolButtonPlay_released()
{
    if(ui->toolButtonPlay->isChecked() == true)
    {
        ui->toolButtonPlay->setIcon(QIcon(":/images/pause.png"));
        ui->toolButtonPlay->setChecked(true);
    }
    else
    {
        ui->toolButtonPlay->setIcon(QIcon(":/images/play.png"));
        ui->toolButtonSave->setChecked(false);
    }
}

int MainWindow::onPlayerCallback(void* buf, unsigned long len)
{
    return len;
}
