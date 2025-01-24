#include <QApplication>
#include <QOpenGLWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QTimer>
#include <QTime>
#include <QtMath>

class ClockWidget : public QOpenGLWidget, protected QOpenGLFunctions {
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
    void initializeGL() override {
        initializeOpenGLFunctions();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

    void resizeGL(int w, int h) override {
        glViewport(0, 0, w, h);
    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        drawClockFace();
        drawClockHands();
    }

private:
    QTimer *timer;
    bool is24HourMode;

    void drawClockFace() {
        glColor3f(0.0f, 0.0f, 0.0f);

        // Draw hour markers
        for (int i = 0; i < 60; ++i) {
            glPushMatrix();
            glRotatef(i * 6.0f, 0.0f, 0.0f, 1.0f);

            if (i % 5 == 0) {
                glBegin(GL_LINES);
                glVertex2f(0.0f, 0.9f);
                glVertex2f(0.0f, 0.8f);
                glEnd();
            } else {
                glBegin(GL_LINES);
                glVertex2f(0.0f, 0.9f);
                glVertex2f(0.0f, 0.85f);
                glEnd();
            }

            glPopMatrix();
        }

        // Draw numbers
        for (int i = 1; i <= (is24HourMode ? 24 : 12); ++i) {
            float angle = qDegreesToRadians(360.0f / (is24HourMode ? 24.0f : 12.0f) * i - 90.0f);
            float x = 0.75f * qCos(angle);
            float y = 0.75f * qSin(angle);
            renderText(x, y, 0.0, QString::number(i));
        }
    }

    void drawClockHands() {
        QTime time = QTime::currentTime();

        // Hour hand
        glColor3f(0.0f, 0.0f, 0.0f);
        glPushMatrix();
        int hours = time.hour();
        if (!is24HourMode) {
            hours = hours % 12;
        }
        glRotatef(30.0f * (hours % (is24HourMode ? 24 : 12)) + time.minute() / 2.0f, 0.0f, 0.0f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(-0.02f, 0.0f);
        glVertex2f(0.02f, 0.0f);
        glVertex2f(0.01f, 0.5f);
        glVertex2f(-0.01f, 0.5f);
        glEnd();
        glPopMatrix();

        // Minute hand
        glColor3f(0.0f, 0.0f, 0.0f);
        glPushMatrix();
        glRotatef(6.0f * time.minute() + time.second() / 10.0f, 0.0f, 0.0f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(-0.015f, 0.0f);
        glVertex2f(0.015f, 0.0f);
        glVertex2f(0.01f, 0.7f);
        glVertex2f(-0.01f, 0.7f);
        glEnd();
        glPopMatrix();

        // Second hand
        glColor3f(1.0f, 0.0f, 0.0f);
        glPushMatrix();
        glRotatef(6.0f * time.second(), 0.0f, 0.0f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(-0.01f, 0.0f);
        glVertex2f(0.01f, 0.0f);
        glVertex2f(0.005f, 0.8f);
        glVertex2f(-0.005f, 0.8f);
        glEnd();
        glPopMatrix();
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
