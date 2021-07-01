#include "Data_.h"


const Data_ Data_::Null;

Data_::Data_() :
_bytes(nullptr),
_size(0)
{
	//printf("In the empty constructor of Data.");
}

Data_::Data_(Data_&& other) :
_bytes(nullptr),
_size(0)
{
	//printf("In the move constructor of Data.");
	move(other);
}

Data_::Data_(const Data_& other) :
_bytes(nullptr),
_size(0)
{
	//printf("In the copy constructor of Data.");
	copy(other._bytes, other._size);
}

Data_::~Data_()
{
	//printf("deallocing Data: %p", this);
	clear();
}

Data_& Data_::operator= (const Data_& other)
{
	//printf("In the copy assignment of Data.");
	copy(other._bytes, other._size);
	return *this;
}

Data_& Data_::operator= (Data_&& other)
{
	//printf("In the move assignment of Data.");
	move(other);
	return *this;
}

void Data_::move(Data_& other)
{
	_bytes = other._bytes;
	_size = other._size;

	other._bytes = nullptr;
	other._size = 0;
}

bool Data_::isNull() const
{
	return (_bytes == nullptr || _size == 0);
}

unsigned char* Data_::getBytes() const
{
	return _bytes;
}

size_t Data_::getSize() const
{
	return _size;
}

void Data_::setBytes(unsigned char* bytes)
{
	_bytes = bytes;
}

void Data_::setSize(size_t size)
{
	_size = size;
}

void Data_::copy(unsigned char* bytes, const size_t size)
{
	clear();

	if (size > 0)
	{
		_size = size;
		_bytes = (unsigned char*)malloc(sizeof(unsigned char)* _size);
		memcpy(_bytes, bytes, _size);
	}
}

void Data_::fastSet(unsigned char* bytes, const size_t size)
{
	_bytes = bytes;
	_size = size;
}

void Data_::clear()
{
	free(_bytes);
	_bytes = nullptr;
	_size = 0;
}

unsigned char* Data_::takeBuffer(size_t* size)
{
	auto buffer = getBytes();
	if (size)
		*size = getSize();
	fastSet(nullptr, 0);
	return buffer;
}
