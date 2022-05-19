
#include <string>

//constants and variables

extern std::string import_export_filepath;



//Maximum recording time
extern const int MAX_RECORDING_MINUTES;



//Maximum recording time plus padding
extern const int RECORDING_BUFFER_SECONDS;



//The various recording actions we can take
extern const int STOPPED;
extern const int PLAYING;
extern const int RECORDING;
extern const int PAUSED;
extern int current_state;


//Amount of recording and playback devices detected
extern int recording_device_count;
extern int playback_device_count;



//Audio device IDs
extern SDL_AudioDeviceID recording_device_id;
extern SDL_AudioDeviceID playback_device_id;



//Recieved audio spec
extern SDL_AudioSpec recieved_recording_spec;
extern SDL_AudioSpec recieved_playback_spec;



//Audio Device ID
extern int audio_device_id;



//Startup booleans

extern bool mic_loaded;

extern bool speakers_loaded;



//Recording data buffer
extern uint8_t* recording_buffer;



//Size of data buffer
extern uint32_t buffer_byte_size;



//Bytes per second
extern int bytes_per_second;



//Position in data buffer
extern uint32_t buffer_byte_position;



//Length of samples used in callback functions.
extern int length;



//end of recording
extern uint32_t end_of_recording;



//Maximum position in data buffer for recording
extern uint32_t buffer_byte_max_position;



