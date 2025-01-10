#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QSlider>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QTime>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>

class CuckooClockWidget : public QWidget {
    Q_OBJECT

public:
    CuckooClockWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Cuckoo Clock");
        resize(300, 250);

        QVBoxLayout *layout = new QVBoxLayout(this);

        clockLabel = new QLabel(this);
        clockLabel->setAlignment(Qt::AlignCenter);
        clockLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
        layout->addWidget(clockLabel);

        halfHourChimeCheckBox = new QCheckBox("Enable Half-Hour Chime", this);
        layout->addWidget(halfHourChimeCheckBox);

        QPushButton *selectSoundButton = new QPushButton("Select Cuckoo Sound", this);
        layout->addWidget(selectSoundButton);
        connect(selectSoundButton, &QPushButton::clicked, this, &CuckooClockWidget::selectSound);

        QPushButton *selectHalfHourChimeButton = new QPushButton("Select Half-Hour Chime", this);
        layout->addWidget(selectHalfHourChimeButton);
        connect(selectHalfHourChimeButton, &QPushButton::clicked, this, &CuckooClockWidget::selectHalfHourChime);

        QSlider *volumeSlider = new QSlider(Qt::Horizontal, this);
        volumeSlider->setRange(0, 100);
   //    layout->addWidget(volumeSlider);
   //     connect(volumeSlider, &QSlider::valueChanged, this, &CuckooClockWidget::setVolume);


    //    loadConfig(volumeSlider); // Load saved settings on startup

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &CuckooClockWidget::updateTime);
        timer->start(1000); // Update the time every second

        updateTime();

        initializeMediaPlayers();
        loadSounds(); // Load the saved sound file paths on startup

        createTrayIcon();

        playCuckoo(5);
    }

    ~CuckooClockWidget() {
        delete trayIcon;
    }

protected:
    void closeEvent(QCloseEvent *event) override {
        if (trayIcon && trayIcon->isVisible()) {
            this->hide();
            event->ignore();
        }
    }

private slots:
    void updateTime() {
        QTime currentTime = QTime::currentTime();
        clockLabel->setText(currentTime.toString("hh:mm:ss"));

        if (currentTime.minute() == 0 && currentTime.second() == 0) {
            int hour12 = currentTime.hour() % 12;
            if (hour12 == 0) hour12 = 12; // Convert 0 to 12 for 12-hour format
            playCuckoo(hour12);
        } else if (halfHourChimeCheckBox->isChecked() && currentTime.minute() == 30 && currentTime.second() == 0) {
            playHalfHourChime();
        }
    }

    void playCuckoo(int count = 1) {
        if (cuckooPlayer->source().isEmpty()) return;

        int soundDuration = cuckooPlayer->duration(); // Get duration in milliseconds
        if (soundDuration == 0) {
            cuckooPlayer->play();
            return;
        }

        for (int i = 0; i < count; ++i) {
            QTimer::singleShot(i * soundDuration, this, [this]() {
                cuckooPlayer->play();
            });
        }
    }

    void playHalfHourChime() {
        if (halfHourPlayer->source().isEmpty()) return;
        halfHourPlayer->play();
    }

    void selectSound() {
        QString soundFilePath = QFileDialog::getOpenFileName(this, "Select Cuckoo Sound", "/Applications/clock.app/Contents/MacOS/sounds", "Sound Files (*.wav)");
        if (!soundFilePath.isEmpty()) {
            cuckooPlayer->setSource(QUrl::fromLocalFile(soundFilePath));
            saveConfig();
        }
    }

    void selectHalfHourChime() {
        QString soundFilePath = QFileDialog::getOpenFileName(this, "Select Half-Hour Chime Sound", "/Applications/clock.app/Contents/MacOS/sounds", "Sound Files (*.wav)");
        if (!soundFilePath.isEmpty()) {
            halfHourPlayer->setSource(QUrl::fromLocalFile(soundFilePath));
            saveConfig();
        }
    }

    void setVolume(int value) {
        float volume = value / 100.0f;
        cuckooAudioOutput->setVolume(volume);
        halfHourAudioOutput->setVolume(volume);
        saveConfig();
    }

    void saveConfig() {
        QFile file("cuckoo_clock_config.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << cuckooPlayer->source().toString() << "\n";
            out << halfHourPlayer->source().toString() << "\n";
            out << static_cast<int>(cuckooAudioOutput->volume() * 100) << "\n";
            out << (halfHourChimeCheckBox->isChecked() ? "1" : "0") << "\n";
            file.close();
        }
    }

    void loadConfig(QSlider *slider) {
        QFile file("cuckoo_clock_config.txt");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString cuckooSoundPath = in.readLine();
            QString halfHourSoundPath = in.readLine();
            int volume;
            in >> volume;
            in.readLine(); // Consume the newline
            int halfHourChimeEnabled;
            in >> halfHourChimeEnabled;
            file.close();
            cuckooPlayer->setSource(QUrl::fromLocalFile("cuckoo.wav"));
            cuckooPlayer->setPosition(0); // Ensure the file is loaded
            if (!cuckooSoundPath.isEmpty() && QFile::exists(cuckooSoundPath)) {
                cuckooPlayer->setSource(QUrl::fromLocalFile(cuckooSoundPath));
                cuckooPlayer->setPosition(0); // Ensure the file is loaded
            }
            if (!halfHourSoundPath.isEmpty() && QFile::exists(halfHourSoundPath)) {
                halfHourPlayer->setSource(QUrl::fromLocalFile(halfHourSoundPath));
                halfHourPlayer->setPosition(0); // Ensure the file is loaded
            }

            slider->setValue(volume);
            setVolume(volume);

            halfHourChimeCheckBox->setChecked(halfHourChimeEnabled);
        } else {
            cuckooPlayer->setSource(QUrl::fromLocalFile("cuckoo.wav"));
            cuckooPlayer->setPosition(0); // Ensure the file is loaded

            slider->setValue(50); // Default volume
            setVolume(50);
            halfHourChimeCheckBox->setChecked(false);
        }
    }

    void loadSounds() {
        QFile file("cuckoo_clock_config.txt");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString cuckooSoundPath = in.readLine();
            QString halfHourSoundPath = in.readLine();
            file.close();

            if (!cuckooSoundPath.isEmpty() && QFile::exists(cuckooSoundPath)) {
                cuckooPlayer->setSource(QUrl::fromLocalFile(cuckooSoundPath));
                cuckooPlayer->setPosition(0);
            }
            if (!halfHourSoundPath.isEmpty() && QFile::exists(halfHourSoundPath)) {
                halfHourPlayer->setSource(QUrl::fromLocalFile(halfHourSoundPath));
                halfHourPlayer->setPosition(0);
            }
        }
    }

    void showMainWidget() {
        this->show();
        this->raise();
        this->activateWindow();
    }

    void exitApplication() {
        QApplication::quit();
    }

private:
    void initializeMediaPlayers() {
        cuckooAudioOutput = new QAudioOutput(this);
        cuckooPlayer = new QMediaPlayer(this);
        cuckooPlayer->setAudioOutput(cuckooAudioOutput);
    }

    void createTrayIcon() {
        trayIcon = new QSystemTrayIcon(QIcon("icon.png"), this);
        QMenu *menu = new QMenu(this);

        QAction *showAction = new QAction("Show", this);
        connect(showAction, &QAction::triggered, this, &CuckooClockWidget::showMainWidget);
        menu->addAction(showAction);

        QAction *exitAction = new QAction("Exit", this);
        connect(exitAction, &QAction::triggered, this, &CuckooClockWidget::exitApplication);
        menu->addAction(exitAction);

        trayIcon->setContextMenu(menu);
        trayIcon->setToolTip("Cuckoo Clock");
        trayIcon->show();

        connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::Trigger) {
                showMainWidget();
            }
        });
    }

    QLabel *clockLabel;
    QCheckBox *halfHourChimeCheckBox;
    QMediaPlayer *cuckooPlayer;
    QMediaPlayer *halfHourPlayer;
    QAudioOutput *cuckooAudioOutput;
    QAudioOutput *halfHourAudioOutput;
    QSystemTrayIcon *trayIcon;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CuckooClockWidget clockWidget;
    clockWidget.show();

    return app.exec();
}

#include "main.moc";
