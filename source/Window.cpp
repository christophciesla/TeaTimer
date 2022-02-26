#include "Window.h"

#include <Windows.h>

namespace
{
	const QString kWindowTitle{ "Windows tea timer" };
}

Window::Window(QWidget* parent)
	: QWidget(parent)
	, time_{ new QTimeEdit{} }
	, start_button_{ new QPushButton{tr("Start") } }
	, stop_button_{ new QPushButton{tr("Stop")} }
	, timer_{}
	, running_{ false }
	, sec_total_{ 0U }
	, sec_count_{ 0U }
	, taskbar_item_{ nullptr }
{
	UpdateTitle();

	time_->setDisplayFormat("mm:ss");
	time_->setMinimumTime(QTime(0, 0, 0, 0));
	time_->setMaximumTime(QTime(0, 59, 59, 999));
	time_->setMinimumWidth(260);
	time_->setAlignment(Qt::AlignCenter);

	timer_.setSingleShot(false);
	timer_.setInterval(1000);

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

	std::ignore = connect(start_button_, SIGNAL(clicked()), SLOT(on_start_clicked()));
	std::ignore = connect(stop_button_, SIGNAL(clicked()), SLOT(on_stop_clicked()));
	std::ignore = connect(time_, SIGNAL(timeChanged(const QTime&)), SLOT(on_time_timeChanged(const QTime&)));
	std::ignore = connect(&timer_, SIGNAL(timeout()), SLOT(on_timer_timeout()));

	// Init task bar item
	std::ignore = CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, reinterpret_cast< void** >(&taskbar_item_));
}

Window::~Window()
{
	if (taskbar_item_)
	{
		taskbar_item_->Release();
		taskbar_item_ = nullptr;
	}
}

void Window::UpdateTitle()
{
	if (running_)
	{
		setWindowTitle(time_->time().toString("mm:ss") + " - " + kWindowTitle);
	}
	else
	{
		setWindowTitle(kWindowTitle);
	}
}

void Window::UpdateStartButton(const QTime& time)
{
	start_button_->setEnabled(!running_ && (time.msecsSinceStartOfDay() > 0));
}

void Window::UpdateWidgets()
{
	UpdateStartButton(time_->time());
	stop_button_->setEnabled(running_);
	time_->setReadOnly(running_);
}

void Window::UpdateTaskbar()
{
	if (taskbar_item_)
	{
		if (running_)
		{
			taskbar_item_->SetProgressValue(reinterpret_cast<HWND>(winId()), sec_total_ - sec_count_, sec_total_ - 1);
		}
		else
		{
			taskbar_item_->SetProgressState(reinterpret_cast<HWND>(winId()), TBPF_NOPROGRESS);
		}
	}
}

void Window::on_start_clicked()
{
	running_ = true;
	sec_count_ = time_->time().minute() * 60 + time_->time().second();
	sec_total_ = sec_count_;
	timer_.start();
	UpdateTitle();
	UpdateTaskbar();
	UpdateWidgets();
}

void Window::on_stop_clicked()
{
	running_ = false;
	timer_.stop();
	sec_count_ = 0;
	sec_total_ = 0;
	UpdateTitle();
	UpdateTaskbar();
	UpdateWidgets();
}

void Window::on_timer_timeout()
{
	--sec_count_;
	time_->setTime(QTime{ 0, sec_count_ / 60, sec_count_ % 60 });
	UpdateTitle();
	UpdateTaskbar();
	if (sec_count_ == 0)
	{
		on_stop_clicked();
		::LockWorkStation();
	}
}

void Window::on_time_timeChanged(const QTime& time)
{
	UpdateStartButton(time);
}

void Window::keyPressEvent(QKeyEvent* ev)
{
	if ((ev->key() == Qt::Key_Return) || (ev->key() == Qt::Key_Enter))
	{
		if (!running_)
		{
			on_start_clicked();
		}
	}
	if (ev->key() == Qt::Key_Escape)
	{
		if (running_)
		{
			on_stop_clicked();
		}
	}
}
