#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QSoundEffect>

class CuckooClockWidget : public QWidget {
    Q_OBJECT

public:
    CuckooClockWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Cuckoo Clock");
        resize(300, 150);

        QVBoxLayout *layout = new QVBoxLayout(this);

        clockLabel = new QLabel(this);
        clockLabel->setAlignment(Qt::AlignCenter);
        clockLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
        layout->addWidget(clockLabel);

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &CuckooClockWidget::updateTime);
        timer->start(1000); // Update the time every second

        updateTime();

        cuckooSound = new QSoundEffect(this);
        cuckooSound->setSource(QUrl::fromLocalFile("cukoo.wav"));
    }

private slots:
    void updateTime() {
        QTime currentTime = QTime::currentTime();
        clockLabel->setText(currentTime.toString("hh:mm:ss"));

        if (currentTime.minute() == 0 && currentTime.second() == 0) {
            playCuckoo(currentTime.hour());
        }
    }

    void playCuckoo(int hours) {
        for (int i = 0; i < hours; ++i) {
            QTimer::singleShot(i * 1000, cuckooSound, &QSoundEffect::play);
        }
    }

private:
    QLabel *clockLabel;
    QSoundEffect *cuckooSound;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CuckooClockWidget clockWidget;
    clockWidget.show();

    return app.exec();
}

#include "main.moc"
