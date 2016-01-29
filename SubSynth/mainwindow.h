#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AudioPlayer.h"

#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QAbstractSlider>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, OnAudioPlayerListener
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
//clicked button
    void on_buttonOsc1Sine_clicked();
    void on_buttonOsc1Saw_clicked();
    void on_buttonOsc1Squ_clicked();
    void on_buttonOsc1Tri_clicked();
    void on_buttonOsc1Noise_clicked();

    void on_buttonOsc2Sine_clicked();
    void on_buttonOsc2Saw_clicked();
    void on_buttonOsc2Squ_clicked();
    void on_buttonOsc2Tri_clicked();
    void on_buttonOsc2Noise_clicked();

    void on_buttonLfoSine_clicked();
    void on_buttonLfoSaw_clicked();
    void on_buttonLfoTri_clicked();
    void on_buttonLfoNoise_clicked();

    void on_buttonFilterLow_clicked();
    void on_buttonFilterBand_clicked();
    void on_buttonFilterHigh_clicked();

    void on_buttonPower12dB_clicked();
    void on_buttonPower24dB_clicked();

//toolbar
    bool on_toolButtonOpen_released();
    void on_toolButtonNew_released();
    bool on_toolButtonSave_released();

//moved slider
    void on_sliderOsc1Semi_sliderMoved();
    void on_sliderOsc1Cents_sliderMoved();

    void on_sliderOsc2Semi_sliderMoved();
    void on_sliderOsc2Cents_sliderMoved();
    void on_sliderOsc2Phase_sliderMoved();

    void on_sliderOscmix_sliderMoved();

    void on_sliderAmpEnvAttack_sliderMoved();
    void on_sliderAmpEnvDecay_sliderMoved();
    void on_sliderAmpEnvSustain_sliderMoved();
    void on_sliderAmpEnvRelease_sliderMoved();

    void on_sliderFilterRes_sliderMoved();
    void on_sliderFilterCutoff_sliderMoved();

    void on_sliderLfoSpeed_sliderMoved();
    void on_sliderLfoOsc_sliderMoved();
    void on_sliderLfoCutoff_sliderMoved();
    void on_sliderLfoAmp_sliderMoved();

    void on_toolButtonPlay_released();

private:
//implementation
    void deactiveButton(QPushButton* button);
    void initButtons();
    void setButtons();
    void initSliders();
    float getSliderFloatValue(QSlider *slider);
    bool setSliderFloatValue(QSlider* slider, float value);
    bool setSliderFloatRange(QSlider* slider, float from, float until);
    void setFloatLabelValue(QSlider* slider, QLabel* label);
    void setIntLabelValue(QSlider* slider, QLabel* label);
    void unsetButtons();

//members
    QPushButton* Osc1;
    QPushButton* Osc2;
    QPushButton* Lfo;
    QPushButton* Filter;
    QPushButton* Power;
    int Osc1Id, Osc2Id, LfoId, FilterId, PowerId;
    Ui::MainWindow *ui;

    int onPlayerCallback(void *buf, unsigned long len);

    int version;
    float factor;
};

#endif // MAINWINDOW_H
