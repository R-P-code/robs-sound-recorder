#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include "glob_vars.h"
#include "recorder_functions.h"
#include "gui_functions.h"



//String to hold filepaths
std::string import_export_filepath;





//Maximum recording time
const int MAX_RECORDING_MINUTES = 20;



//Maximum recording time plus padding
const int RECORDING_BUFFER_SECONDS = MAX_RECORDING_MINUTES*60 + 5;


const int STOPPED = 0;
const int PLAYING = 1;
const int RECORDING = 2;
const int PAUSED = 3;
int current_state = STOPPED;


//Amount of recording and playback devices detected
int recording_device_count = 0;
int playback_device_count = 0;



//Audio device IDs
SDL_AudioDeviceID recording_device_id = 0;
SDL_AudioDeviceID playback_device_id = 0;



//Recieved audio spec
SDL_AudioSpec recieved_recording_spec;
SDL_AudioSpec recieved_playback_spec;



//Audio Device ID
int audio_device_id = 0;



//Startup booleans

bool mic_loaded = false;

bool speakers_loaded = false;



//Recording data buffer
Uint8* recording_buffer = NULL;



//Size of data buffer
Uint32 buffer_byte_size = 0;



//Bytes per second
int bytes_per_second;



//Position in data buffer
Uint32 buffer_byte_position = 0;



//Length of samples used in callback functions.
int length;



//end of recording
Uint32 end_of_recording = 0;



//Maximum position in data buffer for recording
Uint32 buffer_byte_max_position = 0;









