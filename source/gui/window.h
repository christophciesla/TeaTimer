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

    QAction* start_action_;
    QAction* stop_action_;
    QPushButton* start_button_;
    QPushButton* stop_button_;

    QTimer timer_;

    bool running_;
    std::int32_t sec_total_;
    std::int32_t sec_count_;

    gui::Taskbar* taskbar_;

    void UpdateTitle();
    void UpdateStartButtonAndAction(const QTime& time);
    void UpdateWidgetsAndActions();
    void UpdateTaskbar();

    void StartTimer();
    void StopTimer();
    void TimerTimeout();

    void LockSession();
};
}

#endif // GUI_WINDOW_H
