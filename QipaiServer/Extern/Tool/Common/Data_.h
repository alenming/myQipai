#ifndef __MY_DATA_H__
#define __MY_DATA_H__

#include "CommonHead.h"

class Data_
{
public:
	static const Data_ Null;

	Data_();
	Data_(const Data_& other);
	Data_(Data_&& other);
	~Data_();

	Data_& operator= (const Data_& other);
	Data_& operator= (Data_&& other);

	unsigned char* getBytes() const;
	void setBytes(unsigned char* bytes);

	void setSize(size_t size);
	size_t getSize() const;

	void copy(unsigned char* bytes, const size_t size);

	void fastSet(unsigned char* bytes, const size_t size);

	void clear();

	bool isNull() const;

	unsigned char* takeBuffer(size_t* size);

private:
	void move(Data_& other);

private:
	unsigned char* _bytes;
	size_t _size;
};


#endif // __DATA_H__
