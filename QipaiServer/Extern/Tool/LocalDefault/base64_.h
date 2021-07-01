#ifndef __SUPPORT_BASE64_H__
#define __SUPPORT_BASE64_H__

/*
	base64¼ÓÃÜÂë·½·¨
*/

int base64Decode(const unsigned char* input, unsigned int inLength, unsigned char **output);
    
int base64Encode(const unsigned char* input, unsigned int inLength, char **output);


   

#endif // __SUPPORT_BASE64_H__
