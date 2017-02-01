#ifndef _HTTP_H
#define _HTTP_H

//HTTP command status
#define EMPTY 				0
#define WAITING 			1
#define SENDING				2
#define WAITING_FOR_REPLY		3


typedef enum {GET, POST, HEAD} HTTP_Method;    	// 'keys' for HTTP functions

void submitHTTPRequest( HTTP_Method method, char* p_http_link, char* p_post_body);

#endif