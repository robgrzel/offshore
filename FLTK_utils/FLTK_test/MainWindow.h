#pragma once

#define LOGGER (Logger::instance())

class MainWindowUI;
class Fl_Text_Buffer;
class Fl_Button;
class Fl_Widget;

class MainWindow
{
	friend class Logger;
public:
	MainWindow();
	~MainWindow();
	
	void show();

private:
		static void clearButton_callback(Fl_Button* button, void* data);
		static void startButton_callback(Fl_Button* button, void* data);
private:
	MainWindowUI* ui;
	Fl_Text_Buffer* textBuffer;
		//Logger * log;
};



class Logger
{
public:
	static Logger& instance();
		static void logger(Fl_Widget *, void * v);
		void logger(const char* msg);
		void logger(void *p, const char* msg);

private:
	Logger();
	~Logger();
};
