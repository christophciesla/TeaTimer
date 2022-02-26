#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <QtGui>
#include "Shobjidl.h"

class Window : public QWidget
{
	Q_OBJECT

public:
	Window( QWidget* parent = 0 );
	~Window();

private:
	QTimeEdit* _time;

	QPushButton* _start;
	QPushButton* _stop;

	QTimer _timer;

	unsigned int _secTotal;
	unsigned int _secCount;

	ITaskbarList3* _taskbarItem;

	void keyPressEvent( QKeyEvent* ev );

private Q_SLOTS:
	void on_timer_timeout();
	void on_start_clicked();
	void on_stop_clicked();
	void on_time_timeChanged( const QTime& time );
};

#endif
