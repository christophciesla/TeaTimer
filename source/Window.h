#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <QtWidgets>
#include "Shobjidl.h"

class Window : public QWidget
{
	Q_OBJECT

public:
	Window(QWidget* parent = 0);
	~Window();

private:
	QTimeEdit* time_;

	QPushButton* start_button_;
	QPushButton* stop_button_;

	QTimer timer_;

	unsigned int sec_total_;
	unsigned int sec_count_;

	ITaskbarList3* taskbar_item_;

	void keyPressEvent(QKeyEvent* ev);

private Q_SLOTS:
	void on_timer_timeout();
	void on_start_clicked();
	void on_stop_clicked();
	void on_time_timeChanged(const QTime& time);
};

#endif
