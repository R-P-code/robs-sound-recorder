

//The sound recorder itself using SDL audio

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "glob_vars.h"
#include "recorder_functions.h"
#include <fstream>
#include <string>
#include <iostream>




//Function definitions







bool init_SDL_audio() {


	if (SDL_Init( SDL_INIT_AUDIO ) != 0) {

		std::cout << "Failed to initialize SDL" << std::endl;
		return false;	
	
	}

	else {
		
		std::cout << "SDL_initialized" << std::endl;
		return true;

	}

}















void close_SDL()
{

	//Stop recording and playback
	if (speakers_loaded) SDL_PauseAudioDevice( playback_device_id, SDL_TRUE ); 

	if (mic_loaded) SDL_PauseAudioDevice( recording_device_id, SDL_TRUE );
	
	//Go back to beginning of buffer
	set_track_position(0); 

	//Free playback audio
	if( recording_buffer != NULL )
	{
		delete[] recording_buffer;
		recording_buffer = NULL;
	}
	
	//Quit SDL subsystems
	SDL_Quit();

	std::cout << "SDL closed" << std::endl;

}















// The audio program running in the background


bool process_audio() {

	set_end_of_recording();

	load_mic();
		
	load_speakers();

	update_if_recording();

	update_if_playing();

	return 1;
	
}















void set_track_position(int new_pos) {

	SDL_LockAudioDevice( playback_device_id );
	SDL_LockAudioDevice( recording_device_id );

	buffer_byte_position = new_pos;

	SDL_UnlockAudioDevice( playback_device_id );
	SDL_UnlockAudioDevice( recording_device_id );

}















Uint32 get_track_position() {

	Uint32 temp;

	SDL_LockAudioDevice( playback_device_id );
	SDL_LockAudioDevice( recording_device_id );

	temp = buffer_byte_position;

	SDL_UnlockAudioDevice( playback_device_id );
	SDL_UnlockAudioDevice( recording_device_id );

	return temp;

}















double calculate_pos_seconds() {

	if (bytes_per_second != 0) {

		int temp_1 = get_track_position() / bytes_per_second;
		
		if (temp_1 <= 59) return temp_1;
		
		int temp_2 = temp_1 / 60;

		temp_1 = temp_1 - temp_2 *60;

		return temp_1;

	}

	return 0;

}















Uint32 get_end_position() {return end_of_recording;}















void set_end_position(Uint32 new_pos) {end_of_recording = new_pos;}















double calculate_end_seconds() {

	if (bytes_per_second != 0) {
		
		int temp_1 = get_end_position() / bytes_per_second;

		if (temp_1 <= 59) return temp_1;
		
		int temp_2 = temp_1 / 60;

		temp_1 = temp_1 - temp_2 *60;

		return temp_1;

	}

	return 0;
}















double calculate_pos_minutes() { 

	if (bytes_per_second != 0) {

		return (get_track_position() / bytes_per_second) / 60;

	}

	else return 0;

}















double calculate_end_minutes() {

	if (bytes_per_second != 0) {

		return (get_end_position() / bytes_per_second) / 60;

	}

	else return 0;

}
















void audioRecordingCallback( void* userdata, Uint8* stream, int len )
{
	//Copy audio from stream
	memcpy( &recording_buffer[ buffer_byte_position ], stream, len );

	//Move along buffer
	buffer_byte_position += len;
}
















void audioPlaybackCallback( void* userdata, Uint8* stream, int len )
{
	//Copy audio to stream
	memcpy( stream, &recording_buffer[ buffer_byte_position ], len );

	//Move along buffer
	buffer_byte_position += len;

}
















void set_end_of_recording() {

	if ( get_end_position() < get_track_position() ) set_end_position( get_track_position() );

}
















void load_mic() {

	if ( !mic_loaded )
		{
			recording_device_count = SDL_GetNumAudioDevices( SDL_TRUE );

			if( recording_device_count > 0 ) {

				//Default audio spec
				SDL_AudioSpec desiredRecordingSpec;
				SDL_zero(desiredRecordingSpec);
				desiredRecordingSpec.freq = 44100; //DSP frequency (samples per second)
				desiredRecordingSpec.format = AUDIO_S32SYS; //32-bit integer samples in native byte order
				desiredRecordingSpec.channels = 2; //1 == mono, 2 == stereo, 4 == quad and 6 == 5.1
				desiredRecordingSpec.samples = 4096; //audio buffer size in samples
				desiredRecordingSpec.callback = audioRecordingCallback;

				//Open recording device
				recording_device_id = SDL_OpenAudioDevice( SDL_GetAudioDeviceName( audio_device_id, SDL_TRUE ), SDL_TRUE, &desiredRecordingSpec, &recieved_recording_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE );
				std::cout << SDL_GetAudioDeviceName( 0, SDL_TRUE ) << "<<recorder"<< std::endl;
			
				int bytesPerSample = recieved_recording_spec.channels * ( SDL_AUDIO_BITSIZE( recieved_recording_spec.format ) / 8 );
				bytes_per_second = recieved_recording_spec.freq * bytesPerSample;
				buffer_byte_size = RECORDING_BUFFER_SECONDS * bytes_per_second;
				buffer_byte_max_position = MAX_RECORDING_MINUTES * 60 * bytes_per_second;

				//Allocate and initialize byte buffer
				recording_buffer = new Uint8[ buffer_byte_size ];
				memset( recording_buffer, 0, buffer_byte_size );

				mic_loaded = true;

			}

		}				
					
}
















void load_speakers() {

	if (!speakers_loaded)
		{
			playback_device_count = SDL_GetNumAudioDevices( SDL_FALSE );

			if (playback_device_count > 0) {

				//Default audio spec
				SDL_AudioSpec desiredPlaybackSpec;
				SDL_zero(desiredPlaybackSpec);
				desiredPlaybackSpec.freq = 44100; //DSP frequency (samples per second)
				desiredPlaybackSpec.format = AUDIO_S32SYS; //32-bit integer samples in native byte order
				desiredPlaybackSpec.channels = 2; //1 == mono, 2 == stereo, 4 == quad and 6 == 5.1
				desiredPlaybackSpec.samples = 4096; //audio buffer size in samples
				desiredPlaybackSpec.callback = audioPlaybackCallback;

				//Open playback device
				playback_device_id = SDL_OpenAudioDevice( NULL, SDL_FALSE, &desiredPlaybackSpec, &recieved_playback_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE );	
				length = recieved_playback_spec.size;
				std::cout << SDL_GetAudioDeviceName( 0 , SDL_FALSE ) << "<<speaker"<< std::endl;
				speakers_loaded = true;

			}

		}

}
















void update_if_recording() {

	if(current_state == RECORDING) {

		//Finished recording
		if( get_track_position() > buffer_byte_max_position )
		{
			//Stop recording audio
			SDL_PauseAudioDevice( recording_device_id, SDL_TRUE );
			set_track_position(0);
			current_state = STOPPED;
		}

	}

}
















void update_if_playing() {

	if(current_state == PLAYING) {
		//Finished playback
		if( get_track_position() >= get_end_position() )
		{
			//Stop playing audio
			SDL_PauseAudioDevice( playback_device_id, SDL_TRUE );
			//Set byte position to 0
			set_track_position(0);
			//Go on to next state
			current_state = STOPPED;
		}
	}

}
















void stop() {
	
	//Stop recording and playback
	if (speakers_loaded) SDL_PauseAudioDevice( playback_device_id, SDL_TRUE );
	if (mic_loaded) SDL_PauseAudioDevice( recording_device_id, SDL_TRUE );

	//Go back to beginning of buffer
	set_track_position(0); 

	current_state = STOPPED;

}
















void pause_audio() {

	if (mic_loaded) SDL_PauseAudioDevice( recording_device_id, SDL_TRUE );

	if (speakers_loaded) SDL_PauseAudioDevice( playback_device_id, SDL_TRUE );

	current_state = PAUSED;

}















void record() {

	if (mic_loaded) {

		//Stop playing audio
		SDL_PauseAudioDevice( playback_device_id, SDL_TRUE );

		//Start recording
		SDL_PauseAudioDevice( recording_device_id, SDL_FALSE );
		current_state = RECORDING;

	}

}
















void play() {

	//stop recording
	SDL_PauseAudioDevice( recording_device_id, SDL_TRUE );

	if (speakers_loaded && get_end_position() > 0) {
		
		//Start playing audio
		SDL_PauseAudioDevice( playback_device_id, SDL_FALSE );

		//Go on to next state
		current_state = PLAYING;

	}

}





















int read_wav_file()
{

	stop(); //Stop playback or record

	if (!check_if_wav()) return 0;

	reset_track_positions();

	if (input_file_into_buffer()) return 1;

	return 0;

}



int write_wav_file() {

	stop(); //Stop playback or record

	if (!check_if_wav()) return 0;

	if (output_recording_into_file()) return 1;

	return 0;

}
















void reset_track_positions() {

  	set_track_position(0);
	set_end_position(0);

}




















void set_buffer_variables(uint32_t bytes_per_sec) {

		bytes_per_second = bytes_per_sec;
		buffer_byte_size = RECORDING_BUFFER_SECONDS * bytes_per_second;
		buffer_byte_max_position = MAX_RECORDING_MINUTES * 60 * bytes_per_second;

}




















bool check_if_wav() {

	int size = import_export_filepath.size();

	std::string test = import_export_filepath.substr(size - 3);

	if (test != "wav") {

		std::cout << "Not a wav" << std::endl; 		

		import_export_filepath = "";

		return false;
 
	}

	return true;
}























//import wav header
struct imp_wav_hdr
{
   

	 /* RIFF Chunk Descriptor */

	uint8_t			RIFF[4];		// RIFF Header Magic header

	uint32_t		ChunkSize;	  // RIFF Chunk Size

	uint8_t			WAVE[4];		// WAVE Header



	/* "fmt" sub-chunk */

	uint8_t			fmt[4];		 // FMT header

	uint32_t		Subchunk1Size;  // Size of the fmt chunk

	uint16_t		AudioFormat;	// Audio format 1=PCM,6=mulaw,7=alaw,	 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

	uint16_t		NumOfChan;	  // Number of channels 1=Mono 2=Sterio

	uint32_t		SamplesPerSec;  // Sampling Frequency in Hz

	uint32_t		bytesPerSec;	// bytes per second

	uint16_t		blockAlign;	 // 2=16-bit mono, 4=16-bit stereo

	uint16_t		bitsPerSample;  // Number of bits per sample


	/* "data" sub-chunk */

	uint8_t			Subchunk2ID[4]; // "data"  string

	uint32_t		Subchunk2Size;  // Sampled data length


};





const int headerSize = sizeof(imp_wav_hdr);


























bool input_file_into_buffer() {

	imp_wav_hdr wavHeader;

	std::ifstream ifs(import_export_filepath.c_str(), std::ios::binary);

	if (ifs) {
	
		ifs.read(reinterpret_cast<char*>(&wavHeader), headerSize);//Read the header

		set_buffer_variables(wavHeader.bytesPerSec);

		delete recording_buffer;

		recording_buffer = new Uint8[buffer_byte_size];

		ifs.read(reinterpret_cast<char*>(recording_buffer), wavHeader.Subchunk2Size);
		
		set_end_position(ifs.gcount());

		ifs.close();
	
		return true;

	}

	ifs.close();

	std::cout << "error reading data" << std::endl;

	return false;
}















//output wav header
struct  exp_wav_hdr
{
	

	/* RIFF Chunk Descriptor */

	uint8_t		 RIFF[4] = {'R','I','F','F'};		// RIFF Header Magic header

	uint32_t		ChunkSize;	  // RIFF Chunk Size

	uint8_t		 WAVE[4] = {'W','A','V','E'};		// WAVE Header



	/* "fmt" sub-chunk */

	uint8_t		 fmt[4] = {'f','m','t', ' '};		 // FMT header

	uint32_t		Subchunk1Size = 16;  // Size of the fmt chunk

	uint16_t		AudioFormat = 1;	// Audio format 1=PCM,6=mulaw,7=alaw,	 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

	uint16_t		NumOfChan = 2;	  // Number of channels 1=Mono 2=Sterio

	uint32_t		SamplesPerSec = 44100;  // Sampling Frequency in Hz

	uint32_t		bytesPerSec;	// bytes per second

	uint16_t		blockAlign = 8;	 // 2=16-bit mono, 4=16-bit stereo

	uint16_t		bitsPerSample = 32;  // Number of bits per sample



	/* "data" sub-chunk */

	uint8_t		 Subchunk2ID[4] = {'d','a','t','a'}; // "data"  string

	uint32_t		Subchunk2Size;  // Sampled data length


};
















void set_data_size(exp_wav_hdr& wav_hdr) {


	wav_hdr.ChunkSize = 48 + get_end_position();
	wav_hdr.Subchunk2Size = get_end_position();
	wav_hdr.bytesPerSec = bytes_per_second;


}






















bool output_recording_into_file(){

	exp_wav_hdr wavHeader;

	set_data_size(wavHeader);

	std::ofstream ofs(import_export_filepath.c_str(), std::ios::binary);

	if(ofs) {

		ofs.write(reinterpret_cast<char*>(&wavHeader), headerSize); //Write the header
		
		ofs.write(reinterpret_cast<char*>(recording_buffer), wavHeader.Subchunk2Size);

		ofs.close();  

		return 1;

	}

	ofs.close();

	std::cout << "error writing to" << import_export_filepath << std::endl;

	return 0;


}


