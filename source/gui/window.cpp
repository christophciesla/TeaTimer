#include "gui/window.h"
#include "locker/lock_session.h"

namespace gui
{
Window::Window(QWidget* parent)
    : QWidget(parent)
    , time_{ new QTimeEdit{} }
    , start_action_{ new QAction{ this } }
    , stop_action_{ new QAction{ this } }
    , start_button_{ new QPushButton {tr("Start") } }
    , stop_button_{ new QPushButton{ tr("Stop")} }
    , timer_{}
    , running_{ false }
    , sec_total_{ 0 }
    , sec_count_{ 0 }
    , taskbar_{ new Taskbar{this} }
{
    UpdateTitle();

    time_->setDisplayFormat("mm:ss");
    time_->setMinimumTime(QTime(0, 0, 0, 0));
    time_->setMaximumTime(QTime(0, 59, 59, 999));
    time_->setMinimumWidth(260);
    time_->setAlignment(Qt::AlignCenter);

    timer_.setSingleShot(false);
    timer_.setInterval(1000);

    start_action_->setShortcuts({ Qt::Key_Return, Qt::Key_Enter });
    start_action_->setEnabled(false);
    stop_action_->setShortcut(Qt::Key_Escape);
    stop_action_->setEnabled(false);
    addAction(start_action_);
    addAction(stop_action_);
    start_button_->setEnabled(false);
    stop_button_->setEnabled(false);

    QVBoxLayout* layout{ new QVBoxLayout{} };
    setLayout(layout);

    QHBoxLayout* time_set_layout{ new QHBoxLayout{} };
    time_set_layout->addWidget(new QLabel{ tr("Time:") });
    time_set_layout->addWidget(time_);
    time_set_layout->addStretch();
    layout->addLayout(time_set_layout);

    QHBoxLayout* push_button_layout{ new QHBoxLayout{} };
    push_button_layout->addWidget(start_button_);
    push_button_layout->addWidget(stop_button_);
    layout->addLayout(push_button_layout);

    std::ignore = connect(start_action_, &QAction::triggered, [this](bool) { StartTimer(); });
    std::ignore = connect(start_button_, &QPushButton::clicked, [this]() { start_action_->trigger(); });
    std::ignore = connect(stop_action_, &QAction::triggered, [this](bool) { StopTimer(); });
    std::ignore = connect(stop_button_, &QPushButton::clicked, [this]() { stop_action_->trigger(); });
    std::ignore = connect(time_, &QTimeEdit::timeChanged, [this](const QTime& time) {UpdateStartButtonAndAction(time); });
    std::ignore = connect(&timer_, &QTimer::timeout, [this]() { TimerTimeout(); });
}

void Window::UpdateTitle()
{
    if (running_)
    {
        setWindowTitle(time_->time().toString("mm:ss") + " - " + QApplication::instance()->applicationName());
    }
    else
    {
        setWindowTitle(QString{});
    }
}

void Window::UpdateStartButtonAndAction(const QTime& time)
{
    start_action_->setEnabled(!running_);
    start_button_->setEnabled(!running_ && (time.msecsSinceStartOfDay() > 0));
}

void Window::UpdateWidgetsAndActions()
{
    UpdateStartButtonAndAction(time_->time());
    stop_action_->setEnabled(running_);
    stop_button_->setEnabled(running_);
    time_->setReadOnly(running_);
}

void Window::UpdateTaskbar()
{
    taskbar_->Update(running_, sec_total_ - sec_count_ + 1, sec_total_);
}

void Window::StartTimer()
{
    running_ = true;
    sec_count_ = time_->time().minute() * 60 + time_->time().second();
    sec_total_ = sec_count_;
    timer_.start();
    UpdateTitle();
    UpdateTaskbar();
    UpdateWidgetsAndActions();
}

void Window::StopTimer()
{
    running_ = false;
    timer_.stop();
    sec_count_ = 0;
    sec_total_ = 0;
    UpdateTitle();
    UpdateTaskbar();
    UpdateWidgetsAndActions();
}

void Window::TimerTimeout()
{
    --sec_count_;
    time_->setTime(QTime{ 0, sec_count_ / 60, sec_count_ % 60 });
    UpdateTitle();
    UpdateTaskbar();
    if (sec_count_ == 0)
    {
        StopTimer();
        LockSession();
    }
}

void Window::LockSession()
{
    const locker::ErrorCode error_code{locker::LockSession()};
    switch(error_code)
    {
    case locker::ErrorCode::DesktopEnvironmentNotSupported:
        std::ignore = QMessageBox::critical(this, QString{}, tr("Desktop environment is not supported."));
        break;
    case locker::ErrorCode::SystemInaccessible:
        std::ignore = QMessageBox::critical(this, QString{}, tr("Cannot access system to lock session."));
        break;
    case locker::ErrorCode::Success:
    // Intentional fall through
    default:
        break;
    }
}
}
