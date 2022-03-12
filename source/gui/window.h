#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <QtWidgets>
#include <cstdint>

#include "gui/taskbar.h"

namespace gui
{
class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget* parent = nullptr);

private:
    QTimeEdit* time_;

    QPushButton* start_button_;
    QPushButton* stop_button_;

    QTimer timer_;

    bool running_;
    std::int32_t sec_total_;
    std::int32_t sec_count_;

    gui::Taskbar* taskbar_;

    void UpdateTitle();
    void UpdateStartButton(const QTime& time);
    void UpdateWidgets();
    void UpdateTaskbar();

    void StartTimer();
    void StopTimer();
    void TimerTimeout();

    void keyPressEvent(QKeyEvent* ev) override;

    void LockSession();
};
}

#endif // GUI_WINDOW_H
