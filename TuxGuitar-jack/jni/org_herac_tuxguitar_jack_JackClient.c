#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <jack/jack.h>
#include <jack/midiport.h>
#include "org_herac_tuxguitar_jack_JackClient.h"

#define EVENT_BUFFER_SIZE 512

typedef struct {
	int event_port;
	int event_size;
	int *event_data;
} jack_jni_event_t;

typedef struct {
	jack_port_t **ports;
	int port_count;
	int event_count;
	jack_jni_event_t event_queue[ EVENT_BUFFER_SIZE ];
} jack_jni_synth_t;

typedef struct {
	pthread_mutex_t lock;
	jack_client_t *client;
	jack_jni_synth_t *midi;
} handle_t;

int  JackProcessCallbackImpl(jack_nframes_t nframes, void *ptr);

JNIEXPORT jlong JNICALL Java_org_herac_tuxguitar_jack_JackClient_malloc(JNIEnv* env, jobject obj)
{
	jlong ptr = 0;
	
	handle_t *handle = (handle_t *) malloc( sizeof(handle_t) );
	handle->client = NULL;
	handle->midi = NULL;
	
	pthread_mutex_init( &handle->lock , NULL );
	
	memcpy(&ptr, &handle, sizeof( handle ));
	
	return ptr;
}

JNIEXPORT void JNICALL Java_org_herac_tuxguitar_jack_JackClient_free(JNIEnv* env, jobject obj, jlong ptr)
{
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		
		pthread_mutex_destroy( &handle->lock );
		
		free( handle );
	}
}

JNIEXPORT void JNICALL Java_org_herac_tuxguitar_jack_JackClient_open(JNIEnv* env, jobject obj, jlong ptr)
{
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client == NULL)
			{
				handle->client = jack_client_new ("TuxGuitar");
				if( handle->client == NULL ){
					fprintf (stderr, "jack server not running?\n");
				}
				
				if( handle->client != NULL ){
					jack_set_process_callback (handle->client, JackProcessCallbackImpl , handle);
					jack_activate (handle->client);
				}
			}
			
			pthread_mutex_unlock( &handle->lock );
		}
	}
}

JNIEXPORT void JNICALL Java_org_herac_tuxguitar_jack_JackClient_close(JNIEnv* env, jobject obj, jlong ptr)
{
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL)
			{
				jack_client_close (handle->client);
				handle->client = NULL;
			}
			
			pthread_mutex_unlock( &handle->lock );
		}
	}
}

JNIEXPORT void JNICALL Java_org_herac_tuxguitar_jack_JackClient_openPorts(JNIEnv* env, jobject obj, jlong ptr, jint ports)
{
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL && handle->midi == NULL )
			{
				handle->midi = (jack_jni_synth_t *) malloc( sizeof(jack_jni_synth_t) );
				handle->midi->event_count = 0;
				
				handle->midi->port_count = ports;
				handle->midi->ports = (jack_port_t **) malloc( sizeof( jack_port_t * ) * handle->midi->port_count );
				
				int port_index = 0;
				for( port_index = 0 ; port_index < handle->midi->port_count ; port_index ++ ){
					char port_name[50];
					sprintf( port_name , "Output Port %d", port_index );
					handle->midi->ports[port_index] = jack_port_register(handle->client, port_name, JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
				}
			}
			pthread_mutex_unlock( &handle->lock );
		}
	}
}

JNIEXPORT void JNICALL Java_org_herac_tuxguitar_jack_JackClient_closePorts(JNIEnv* env, jobject obj, jlong ptr)
{
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL && handle->midi != NULL)
			{
				int port_index = 0;
				for( port_index = 0 ; port_index < handle->midi->port_count ; port_index ++ ){
					jack_port_unregister(handle->client, handle->midi->ports[port_index] );
				}
				
				free( handle->midi->ports );
				free( handle->midi );
				handle->midi = NULL;
			}
			pthread_mutex_unlock( &handle->lock );
		}
	}
}

JNIEXPORT jlong JNICALL Java_org_herac_tuxguitar_jack_JackClient_getTransportUID(JNIEnv* env, jobject obj, jlong ptr)
{
	jlong result = 0;
	
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL)
			{
				jack_position_t pos;
				jack_transport_query( handle->client , &pos );
				
				result = pos.unique_1;
			}
			pthread_mutex_unlock( &handle->lock );
		}
	}
	return result;
}

JNIEXPORT jlong JNICALL Java_org_herac_tuxguitar_jack_JackClient_getTransportFrame(JNIEnv* env, jobject obj, jlong ptr)
{
	jlong result = 0;
	
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL)
			{
				jack_position_t pos;
				jack_transport_query( handle->client , &pos );
				
				result = pos.frame;
			}
			pthread_mutex_unlock( &handle->lock );
		}
	}
	return result;
}

JNIEXPORT jlong JNICALL Java_org_herac_tuxguitar_jack_JackClient_getTransportFrameRate(JNIEnv* env, jobject obj, jlong ptr)
{
	jlong result = 0;
	
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL)
			{
				jack_position_t pos;
				jack_transport_query( handle->client , &pos );
				
				result = pos.frame_rate;
			}
			pthread_mutex_unlock( &handle->lock );
		}
	}
	return result;
}

JNIEXPORT void JNICALL Java_org_herac_tuxguitar_jack_JackClient_setTransportFrame(JNIEnv* env, jobject obj, jlong ptr, jlong frame)
{
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL)
			{
				jack_transport_locate( handle->client, (jack_nframes_t) frame );
			}
			
			pthread_mutex_unlock( &handle->lock );
		}
	}
}

JNIEXPORT void JNICALL Java_org_herac_tuxguitar_jack_JackClient_setTransportStart(JNIEnv* env, jobject obj, jlong ptr)
{
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL)
			{
				jack_position_t pos;
				jack_transport_state_t state = jack_transport_query( handle->client , &pos );
				if( state == JackTransportStopped ) {
					jack_transport_start( handle->client );
				}
			}
			pthread_mutex_unlock( &handle->lock );
		}
	}
}

JNIEXPORT void JNICALL Java_org_herac_tuxguitar_jack_JackClient_setTransportStop(JNIEnv* env, jobject obj, jlong ptr)
{
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL)
			{
				jack_position_t pos;
				jack_transport_state_t state = jack_transport_query( handle->client , &pos );
				if( state != JackTransportStopped ) {
					jack_transport_stop( handle->client );
				}
			}
			pthread_mutex_unlock( &handle->lock );
		}
	}
}

JNIEXPORT jboolean JNICALL Java_org_herac_tuxguitar_jack_JackClient_isTransportRunning(JNIEnv* env, jobject obj, jlong ptr){
	jboolean result = JNI_FALSE;
	
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL)
			{
				jack_position_t pos;
				jack_transport_state_t state = jack_transport_query( handle->client , &pos );
				if( state != JackTransportStopped ) {
					result = JNI_TRUE;
				}
			}
			pthread_mutex_unlock( &handle->lock );
		}
	}
	return result;
}

JNIEXPORT void JNICALL Java_org_herac_tuxguitar_jack_JackClient_addEventToQueue(JNIEnv* env, jobject obj, jlong ptr, jint port, jbyteArray jdata)
{
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL && handle->midi != NULL && handle->midi->ports != NULL )
			{
				if( handle->midi->event_count < EVENT_BUFFER_SIZE ) {
					int i = 0;
					int count = (*env)->GetArrayLength( env,  jdata  );
					if( count > 0 ){
						jbyte* jdataArray = (*env)->GetByteArrayElements( env , jdata, 0);
						if( jdataArray != NULL ) {
							handle->midi->event_queue[ handle->midi->event_count ].event_port = port;
							handle->midi->event_queue[ handle->midi->event_count ].event_size = count;
							handle->midi->event_queue[ handle->midi->event_count ].event_data = (int *)malloc( sizeof( int ) * count );
							if( handle->midi->event_queue[ handle->midi->event_count ].event_data != NULL ) {
								for( i = 0 ; i < count ; i ++ ){
									handle->midi->event_queue[ handle->midi->event_count ].event_data[ i ] = jdataArray[ i ];
								}
								handle->midi->event_count ++;
								(*env)->ReleaseByteArrayElements( env , jdata, jdataArray, 0);
							}
						}
					}
				}
			}
			pthread_mutex_unlock( &handle->lock );
		}
	}
}

int JackProcessCallbackImpl(jack_nframes_t nframes, void *ptr){
	handle_t *handle = NULL;
	memcpy(&handle, &ptr, sizeof(handle));
	if(handle != NULL){
		
		if( pthread_mutex_lock( &handle->lock ) == 0 ){
			
			if(handle->client != NULL && handle->midi != NULL  && handle->midi->ports != NULL )
			{
				int port_index = 0;
				for( port_index = 0 ; port_index < handle->midi->port_count ; port_index ++ ){
					void *buffer = jack_port_get_buffer(handle->midi->ports[port_index], jack_get_buffer_size(handle->client) );
					if( buffer != NULL ){
						jack_midi_clear_buffer( buffer );
					}
				}
				
				int index = 0;
				int count = handle->midi->event_count;
				for( index = 0 ; index < count ; index ++ ){
					int event_index = 0;
					void *buffer = jack_port_get_buffer(handle->midi->ports[handle->midi->event_queue[index].event_port], jack_get_buffer_size(handle->client) );
					
					if( buffer != NULL ){
						jack_midi_data_t *data = jack_midi_event_reserve ( buffer , 0, handle->midi->event_queue[index].event_size);
						if( data != NULL ){
							for( event_index = 0 ; event_index < handle->midi->event_queue[index].event_size ; event_index ++ ){
								data[ event_index ] = handle->midi->event_queue[index].event_data[ event_index ];
							}
						}
						
						handle->midi->event_queue[index].event_size = 0;
						free ( handle->midi->event_queue[index].event_data );
						handle->midi->event_queue[index].event_data = NULL;
						handle->midi->event_count --;
					}
				}
			}
			
			pthread_mutex_unlock( &handle->lock );
		}
	}
	return 0;
}
