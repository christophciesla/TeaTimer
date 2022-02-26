#include "Window.h"

#include <Windows.h>

Window::Window(QWidget* parent)
	: QWidget(parent)
	, time_(new QTimeEdit)
	, start_button_(new QPushButton("Starten"))
	, stop_button_(new QPushButton("Stoppen"))
	, sec_count_(0)
	, taskbar_item_(0)
{
	setWindowTitle("Tea timer");

	time_->setDisplayFormat("mm:ss");
	time_->setMinimumTime(QTime(0, 0, 0, 0));
	time_->setMaximumTime(QTime(0, 59, 59, 999));

	timer_.setSingleShot(false);
	timer_.setInterval(1000);

	start_button_->setEnabled(false);
	stop_button_->setEnabled(false);

	QVBoxLayout* layout = new QVBoxLayout;
	setLayout(layout);

	QHBoxLayout* timeSetLayout = new QHBoxLayout;
	timeSetLayout->addWidget(new QLabel("Zeit:"));
	timeSetLayout->addWidget(time_);
	timeSetLayout->addStretch();
	layout->addLayout(timeSetLayout);

	QHBoxLayout* pbLayout = new QHBoxLayout;
	pbLayout->addWidget(start_button_);
	pbLayout->addWidget(stop_button_);
	pbLayout->addStretch();
	layout->addLayout(pbLayout);

	bool ok = true;
	ok &= static_cast<bool>(connect(start_button_, SIGNAL(clicked()), SLOT(on_start_clicked())));
	ok &= static_cast<bool>(connect(stop_button_, SIGNAL(clicked()), SLOT(on_stop_clicked())));
	ok &= static_cast<bool>(connect(time_, SIGNAL(timeChanged(const QTime&)), SLOT(on_time_timeChanged(const QTime&))));
	ok &= static_cast<bool>(connect(&timer_, SIGNAL(timeout()), SLOT(on_timer_timeout())));

	// Init task bar item
	CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, reinterpret_cast< void** >(&taskbar_item_));
}

Window::~Window()
{
	if (taskbar_item_)
	{
		taskbar_item_->Release();
		taskbar_item_ = 0;
	}
}

void Window::on_start_clicked()
{
	setWindowTitle(time_->time().toString("mm:ss") + " - Tea timer");
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
	setWindowTitle("Tea timer");
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
	setWindowTitle(time.toString("mm:ss") + " - Tea timer");
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
