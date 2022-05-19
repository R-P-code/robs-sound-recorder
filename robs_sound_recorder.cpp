//Simple sound recorder using GTK for the GUI and SDL for the audio recording and playback functions.


#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "glob_vars.h"
#include "recorder_functions.h"
#include "gui_functions.h"
#include <iostream>
#include <exception>


  

int main (int argc, char *argv[]) {
  
	try {

		gtk_init(&argc, &argv);

		set_up_window();

		add_repeating_functions_to_main_loop();

		start_main_loop();

		close_SDL();
 
	}

	catch(std::exception& e){

		std::cerr << "\n" << "Error: " << e.what() << std::endl;
		return 1;

	}


	return 0;


}








