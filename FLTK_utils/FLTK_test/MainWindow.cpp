#include "MainWindow.h"
#include "MainWindowUI.h"
#include <iostream>

static MainWindow* mainWindow = NULL;

Logger::Logger( ) {
	std::cout << "Logger::Logger( )\n";

}

Logger::~Logger( ) {
	std::cout << "Logger::~Logger( )\n";

}

Logger& Logger::instance( ) {
	std::cout << "Logger& Logger::instance( )\n";
	static Logger log;
	return log;
}

void Logger::logger( Fl_Widget*, void* v ) {
	std::cout << "Logger::logger( Fl_Widget*, void* v )\n";
	if (mainWindow == NULL) {
		return;
	}
	mainWindow->ui->output->insert( "log" );
}

void MainWindow::clearButton_callback( Fl_Button* button, void* data ) {
	std::cout << "MainWindow::clearButton_callback( Fl_Button* button, void* data )\n";
	(( MainWindowUI* ) (button->parent()->user_data()))->output->buffer()->text( "clearButton_callback" );
}

void MainWindow::startButton_callback( Fl_Button* button, void* data ) {
	std::cout << "MainWindow::startButton_callback( Fl_Button* button, void* data )\n";
	const char* c = ( const char* ) data;
	(( MainWindowUI* ) (button->parent()->user_data()))->output->buffer()->text( "startButton_callback" );
}

MainWindow::MainWindow( ) {
	std::cout << "MainWindow::MainWindow( ) \n";

	ui = new MainWindowUI;

	//log = &Logger::instance();

	if (mainWindow == NULL) {
		mainWindow = this;
	}
}

MainWindow::~MainWindow( ) {
	std::cout << "MainWindow::~MainWindow( ) \n";

	delete textBuffer;
	delete ui;
}

void MainWindow::show( ) {
	std::cout << "MainWindow::show( )\n";

	textBuffer = new Fl_Text_Buffer;
	ui->output->buffer( textBuffer );
	ui->output->insert( "Output..." );
	ui->mainWindow->show();

	//ui->glWindow->callback((Fl_Callback*)log->logger);
	ui->clearButton->callback( ( Fl_Callback* ) clearButton_callback );
	ui->startButton->callback( ( Fl_Callback* ) startButton_callback );
}