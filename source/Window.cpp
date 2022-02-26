#include "Window.h"

#include <Windows.h>

Window::Window( QWidget* parent )
	: QWidget( parent )
	, _time( new QTimeEdit )
	, _start( new QPushButton( "Starten" ) )
	, _stop( new QPushButton( "Stoppen" ) )
	, _secCount( 0 )
	, _taskbarItem( 0 )
{
	setWindowTitle( "Tea timer" );

	_time->setDisplayFormat( "mm:ss" );
	_time->setMinimumTime( QTime( 0, 0, 0, 0 ) );
	_time->setMaximumTime( QTime( 0, 59, 59, 999 ) );

	_timer.setSingleShot( false );
	_timer.setInterval( 1000 );

	_start->setEnabled( false );
	_stop->setEnabled( false );

	QVBoxLayout* layout = new QVBoxLayout;
	setLayout( layout );

	QHBoxLayout* timeSetLayout = new QHBoxLayout;
	timeSetLayout->addWidget( new QLabel( "Zeit:" ) );
	timeSetLayout->addWidget( _time );
	timeSetLayout->addStretch();
	layout->addLayout( timeSetLayout );

	QHBoxLayout* pbLayout = new QHBoxLayout;
	pbLayout->addWidget( _start );
	pbLayout->addWidget( _stop );
	pbLayout->addStretch();
	layout->addLayout( pbLayout );

	bool ok = true;
	ok &= connect( _start, SIGNAL( clicked() ), SLOT( on_start_clicked() ) );
	ok &= connect( _stop, SIGNAL( clicked() ), SLOT( on_stop_clicked() ) );
	ok &= connect( _time, SIGNAL( timeChanged( const QTime& ) ), SLOT( on_time_timeChanged( const QTime& ) ) );
	ok &= connect( &_timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );

	// Init task bar item
	CoCreateInstance( CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, reinterpret_cast< void** >( &_taskbarItem ) );
}

Window::~Window()
{
	if ( _taskbarItem )
	{
		_taskbarItem->Release();
		_taskbarItem = 0;
	}
}

void Window::on_start_clicked()
{
	setWindowTitle( _time->time().toString( "mm:ss" ) + " - Tea timer" );
	_secCount = _time->time().minute() * 60 + _time->time().second();
	_secTotal = _secCount;
	if ( _taskbarItem )
	{
		_taskbarItem->SetProgressValue( winId(), _secCount, _secTotal );
	}
	_timer.start();
	_start->setEnabled( false );
	_stop->setEnabled( true );
	_time->setReadOnly( true );
}

void Window::on_stop_clicked()
{
	setWindowTitle( "Tea timer" );
	if ( _taskbarItem )
	{
		_taskbarItem->SetProgressState( winId(), TBPF_NOPROGRESS );
	}
	_time->setReadOnly( false );
	_start->setEnabled( true );
	_stop->setEnabled( false );
	_timer.stop();
	_secCount = 0;
	_secTotal = 0;
}

void Window::on_timer_timeout()
{
	--_secCount;
	if ( _taskbarItem )
	{
		_taskbarItem->SetProgressValue( winId(), _secCount, _secTotal );
	}
	QTime time( 0, _secCount / 60, _secCount % 60 );
	setWindowTitle( time.toString( "mm:ss" ) + " - Tea timer" );
	_time->blockSignals( true );
	_time->setTime( time );
	_time->blockSignals( false );
	if ( _secCount == 0 )
	{
		on_stop_clicked();
		on_time_timeChanged( time );
		::LockWorkStation();
	}
}

void Window::on_time_timeChanged( const QTime& time )
{
	_start->setEnabled( time.minute() > 0 || time.second() > 0 );
}

void Window::keyPressEvent( QKeyEvent* ev )
{
	if ( ev->key() == Qt::Key_Return || ev->key() == Qt::Key_Enter )
	{
		if ( _start->isEnabled() )
			on_start_clicked();
	}
	if ( ev->key() == Qt::Key_Escape )
	{
		if ( _stop->isEnabled() )
			on_stop_clicked();
	}
}
