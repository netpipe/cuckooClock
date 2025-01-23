#include <QApplication>
#include <QOpenGLWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QTimer>
#include <QTime>
#include <QPainter>
#include <QWidget>

class ClockWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit ClockWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent), is24HourMode(false) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&ClockWidget::update));
        timer->start(1000); // Update every second
    }

    void set24HourMode(bool enabled) {
        is24HourMode = enabled;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        int side = qMin(width(), height());
        painter.translate(width() / 2, height() / 2);
        painter.scale(side / 200.0, side / 200.0);

        drawClockFace(painter);
        drawClockHands(painter);
    }

private:
    QTimer *timer;
    bool is24HourMode;

    void drawClockFace(QPainter &painter) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::white);
        painter.drawEllipse(-100, -100, 200, 200);

        painter.setPen(Qt::black);
        for (int i = 0; i < 60; ++i) {
            if (i % 5 == 0) {
                painter.drawLine(0, -88, 0, -100);
            } else {
                painter.drawLine(0, -92, 0, -100);
            }
            painter.rotate(6.0);
        }

        painter.setFont(QFont("Arial", 8));
        for (int i = 1; i <= (is24HourMode ? 24 : 12); ++i) {
            int angle = 360 / (is24HourMode ? 24 : 12) * i;
            double x = 85 * qCos(qDegreesToRadians(angle - 90.0));
            double y = 85 * qSin(qDegreesToRadians(angle - 90.0));
            painter.drawText(QRectF(x - 10, y - 10, 20, 20), Qt::AlignCenter, QString::number(i));
        }
    }

    void drawClockHands(QPainter &painter) {
        QTime time = QTime::currentTime();

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);

        // Hour hand
        painter.save();
        int hours = time.hour();
        if (!is24HourMode) {
            hours = hours % 12;
        }
        painter.rotate(30.0 * ((hours % (is24HourMode ? 24 : 12)) + time.minute() / 60.0));
        painter.drawConvexPolygon(QPolygon({QPoint(-5, 0), QPoint(5, 0), QPoint(0, -50)}));
        painter.restore();

        // Minute hand
        painter.save();
        painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
        painter.drawConvexPolygon(QPolygon({QPoint(-3, 0), QPoint(3, 0), QPoint(0, -70)}));
        painter.restore();

        // Second hand
        painter.setBrush(Qt::red);
        painter.save();
        painter.rotate(6.0 * time.second());
        painter.drawConvexPolygon(QPolygon({QPoint(-2, 0), QPoint(2, 0), QPoint(0, -90)}));
        painter.restore();
    }
};

class MainWindow : public QWidget {
    Q_OBJECT

public:
    MainWindow() {
        auto *layout = new QVBoxLayout(this);
        clockWidget = new ClockWidget(this);
        auto *checkbox = new QCheckBox("24 Hour Mode", this);

        layout->addWidget(clockWidget);
        layout->addWidget(checkbox);

        connect(checkbox, &QCheckBox::toggled, clockWidget, &ClockWidget::set24HourMode);
    }

private:
    ClockWidget *clockWidget;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(400, 400);
    window.show();

    return app.exec();
}

#include "main.moc"
