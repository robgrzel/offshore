// FLTKTest.cpp : Defines the entry point for the console application.
//

#include "MainWindow.h"
#include <FL/Fl.H>
#include <iostream>

int main(int argc, char **argv)  
{  	std::cout<<"main\n";

	MainWindow window;

	Fl::visual(FL_DOUBLE|FL_INDEX);
 	window.show();

	return Fl::run();  
} 