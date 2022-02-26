#include "Window.h"

#include <Windows.h>

namespace
{
	const QString kWindowTitle{ "Windows tea timer" };
}

Window::Window(QWidget* parent)
	: QWidget(parent)
	, time_(new QTimeEdit)
	, start_button_(new QPushButton(tr("Start")))
	, stop_button_(new QPushButton(tr("Stop")))
	, sec_count_(0)
	, taskbar_item_(0)
{
	setWindowTitle(kWindowTitle);

	time_->setDisplayFormat("mm:ss");
	time_->setMinimumTime(QTime(0, 0, 0, 0));
	time_->setMaximumTime(QTime(0, 59, 59, 999));
	time_->setMinimumWidth(240);
	time_->setAlignment(Qt::AlignCenter);

	timer_.setSingleShot(false);
	timer_.setInterval(1000);

	start_button_->setEnabled(false);
	stop_button_->setEnabled(false);

	QVBoxLayout* layout = new QVBoxLayout;
	setLayout(layout);

	QHBoxLayout* time_set_layout = new QHBoxLayout;
	time_set_layout->addWidget(new QLabel(tr("Time:")));
	time_set_layout->addWidget(time_);
	time_set_layout->addStretch();
	layout->addLayout(time_set_layout);

	QHBoxLayout* push_button_layout = new QHBoxLayout;
	push_button_layout->addWidget(start_button_);
	push_button_layout->addWidget(stop_button_);
	layout->addLayout(push_button_layout);

	bool ok = true;
	ok &= static_cast<bool>(connect(start_button_, SIGNAL(clicked()), SLOT(on_start_clicked())));
	ok &= static_cast<bool>(connect(stop_button_, SIGNAL(clicked()), SLOT(on_stop_clicked())));
	ok &= static_cast<bool>(connect(time_, SIGNAL(timeChanged(const QTime&)), SLOT(on_time_timeChanged(const QTime&))));
	ok &= static_cast<bool>(connect(&timer_, SIGNAL(timeout()), SLOT(on_timer_timeout())));

	// Init task bar item
	std::ignore = CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, reinterpret_cast< void** >(&taskbar_item_));
}

Window::~Window()
{
	if (taskbar_item_)
	{
		taskbar_item_->Release();
		taskbar_item_ = 0;
	}
}

void Window::UpdateTitle()
{
	setWindowTitle(time_->time().toString("mm:ss") + " - " + kWindowTitle);
}

void Window::on_start_clicked()
{
	UpdateTitle();
	sec_count_ = time_->time().minute() * 60 + time_->time().second();
	sec_total_ = sec_count_;
	if (taskbar_item_)
	{
		taskbar_item_->SetProgressValue(reinterpret_cast<HWND>(winId()), sec_count_, sec_total_);
	}
	timer_.start();
	start_button_->setEnabled(false);
	stop_button_->setEnabled(true);
	time_->setReadOnly(true);
}

void Window::on_stop_clicked()
{
	setWindowTitle(kWindowTitle);
	if (taskbar_item_)
	{
		taskbar_item_->SetProgressState(reinterpret_cast<HWND>(winId()), TBPF_NOPROGRESS);
	}
	time_->setReadOnly(false);
	start_button_->setEnabled(true);
	stop_button_->setEnabled(false);
	timer_.stop();
	sec_count_ = 0;
	sec_total_ = 0;
}

void Window::on_timer_timeout()
{
	--sec_count_;
	if (taskbar_item_)
	{
		taskbar_item_->SetProgressValue(reinterpret_cast<HWND>(winId()), sec_count_, sec_total_);
	}
	QTime time(0, sec_count_ / 60, sec_count_ % 60);
	UpdateTitle();
	time_->blockSignals(true);
	time_->setTime(time);
	time_->blockSignals(false);
	if (sec_count_ == 0)
	{
		on_stop_clicked();
		on_time_timeChanged(time);
		::LockWorkStation();
	}
}

void Window::on_time_timeChanged(const QTime& time)
{
	start_button_->setEnabled(time.minute() > 0 || time.second() > 0);
}

void Window::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key_Return || ev->key() == Qt::Key_Enter)
	{
		if (start_button_->isEnabled())
			on_start_clicked();
	}
	if (ev->key() == Qt::Key_Escape)
	{
		if (stop_button_->isEnabled())
			on_stop_clicked();
	}
}
