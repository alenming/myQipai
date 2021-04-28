#include "MD5Def.h"
#include "MD5.h"
#include <assert.h>

#define MD5_GASSERT(cond, msg) do { \
if (!(cond)) { \
printf(msg); \
assert(cond); \
} \
} while (0)

std::string MD5::GetMD5(const std::string& strFilePath)
{
    FILE *file;
    file = fopen(strFilePath.c_str(), "rb");
    if(!file)
        return "";
    MD5 MD5Checksum;
    int nLength = 0;
    const int nBufferSize = 1024;
	UnChar_t Buffer[nBufferSize];
    
    while ((nLength =fread(Buffer, 1, nBufferSize, file)) > 0 )
    {
        MD5Checksum.Update( Buffer, nLength );
    }
    
    fclose(file);
    return MD5Checksum.Final();
}

UnInt32_t MD5::RotateLeft(UnInt32_t x, int n)
{
	MD5_GASSERT(sizeof(x) == 4, "md5 sizeof(x) != 4");
	return (x << n) | (x >> (32-n));
}

void MD5::FF( UnInt32_t& A, UnInt32_t B, UnInt32_t C, UnInt32_t D, UnInt32_t X, UnInt32_t S, UnInt32_t T)
{
	UnInt32_t F = (B & C) | (~B & D);
	A += F + X + T;
	A = RotateLeft(A, S);
	A += B;
}

void MD5::GG( UnInt32_t& A, UnInt32_t B, UnInt32_t C, UnInt32_t D, UnInt32_t X, UnInt32_t S, UnInt32_t T)
{
	UnInt32_t G = (B & D) | (C & ~D);
	A += G + X + T;
	A = RotateLeft(A, S);
	A += B;
}

void MD5::HH( UnInt32_t& A, UnInt32_t B, UnInt32_t C, UnInt32_t D, UnInt32_t X, UnInt32_t S, UnInt32_t T)
{
	UnInt32_t H = (B ^ C ^ D);
	A += H + X + T;
	A = RotateLeft(A, S);
	A += B;
}

void MD5::II( UnInt32_t& A, UnInt32_t B, UnInt32_t C, UnInt32_t D, UnInt32_t X, UnInt32_t S, UnInt32_t T)
{
	UnInt32_t I = (C ^ (B | ~D));
	A += I + X + T;
	A = RotateLeft(A, S);
	A += B;
}

void MD5::ByteTouint32_t(UnInt32_t* Output, UnChar_t* Input, unsigned int nLength)
{
	MD5_GASSERT(nLength % 4 == 0, "md5 nLength != 4");
	unsigned int i=0; 
	unsigned int j=0; 

	for ( ; j < nLength; i++, j += 4)
	{
		Output[i] = (UnInt32_t)Input[j]   |
        (UnInt32_t)Input[j+1] << 8 |
        (UnInt32_t)Input[j+2] << 16 |
        (UnInt32_t)Input[j+3] << 24;
	}
}

void MD5::Transform(UnChar_t Block[64])
{
	UnInt32_t a = m_lMD5[0];
	UnInt32_t b = m_lMD5[1];
	UnInt32_t c = m_lMD5[2];
	UnInt32_t d = m_lMD5[3];

	UnInt32_t X[16];
	ByteTouint32_t( X, Block, 64 );

	FF (a, b, c, d, X[ 0], MD5_S11, MD5_T01);
	FF (d, a, b, c, X[ 1], MD5_S12, MD5_T02);
	FF (c, d, a, b, X[ 2], MD5_S13, MD5_T03);
	FF (b, c, d, a, X[ 3], MD5_S14, MD5_T04);
	FF (a, b, c, d, X[ 4], MD5_S11, MD5_T05);
	FF (d, a, b, c, X[ 5], MD5_S12, MD5_T06);
	FF (c, d, a, b, X[ 6], MD5_S13, MD5_T07);
	FF (b, c, d, a, X[ 7], MD5_S14, MD5_T08);
	FF (a, b, c, d, X[ 8], MD5_S11, MD5_T09);
	FF (d, a, b, c, X[ 9], MD5_S12, MD5_T10);
	FF (c, d, a, b, X[10], MD5_S13, MD5_T11);
	FF (b, c, d, a, X[11], MD5_S14, MD5_T12);
	FF (a, b, c, d, X[12], MD5_S11, MD5_T13);
	FF (d, a, b, c, X[13], MD5_S12, MD5_T14);
	FF (c, d, a, b, X[14], MD5_S13, MD5_T15);
	FF (b, c, d, a, X[15], MD5_S14, MD5_T16);
    
	GG (a, b, c, d, X[ 1], MD5_S21, MD5_T17);
	GG (d, a, b, c, X[ 6], MD5_S22, MD5_T18);
	GG (c, d, a, b, X[11], MD5_S23, MD5_T19);
	GG (b, c, d, a, X[ 0], MD5_S24, MD5_T20);
	GG (a, b, c, d, X[ 5], MD5_S21, MD5_T21);
	GG (d, a, b, c, X[10], MD5_S22, MD5_T22);
	GG (c, d, a, b, X[15], MD5_S23, MD5_T23);
	GG (b, c, d, a, X[ 4], MD5_S24, MD5_T24);
	GG (a, b, c, d, X[ 9], MD5_S21, MD5_T25);
	GG (d, a, b, c, X[14], MD5_S22, MD5_T26);
	GG (c, d, a, b, X[ 3], MD5_S23, MD5_T27);
	GG (b, c, d, a, X[ 8], MD5_S24, MD5_T28);
	GG (a, b, c, d, X[13], MD5_S21, MD5_T29);
	GG (d, a, b, c, X[ 2], MD5_S22, MD5_T30);
	GG (c, d, a, b, X[ 7], MD5_S23, MD5_T31);
	GG (b, c, d, a, X[12], MD5_S24, MD5_T32);
    
	HH (a, b, c, d, X[ 5], MD5_S31, MD5_T33);
	HH (d, a, b, c, X[ 8], MD5_S32, MD5_T34);
	HH (c, d, a, b, X[11], MD5_S33, MD5_T35);
	HH (b, c, d, a, X[14], MD5_S34, MD5_T36);
	HH (a, b, c, d, X[ 1], MD5_S31, MD5_T37);
	HH (d, a, b, c, X[ 4], MD5_S32, MD5_T38);
	HH (c, d, a, b, X[ 7], MD5_S33, MD5_T39);
	HH (b, c, d, a, X[10], MD5_S34, MD5_T40);
	HH (a, b, c, d, X[13], MD5_S31, MD5_T41);
	HH (d, a, b, c, X[ 0], MD5_S32, MD5_T42);
	HH (c, d, a, b, X[ 3], MD5_S33, MD5_T43);
	HH (b, c, d, a, X[ 6], MD5_S34, MD5_T44);
	HH (a, b, c, d, X[ 9], MD5_S31, MD5_T45);
	HH (d, a, b, c, X[12], MD5_S32, MD5_T46);
	HH (c, d, a, b, X[15], MD5_S33, MD5_T47);
	HH (b, c, d, a, X[ 2], MD5_S34, MD5_T48);
    
	II (a, b, c, d, X[ 0], MD5_S41, MD5_T49);
	II (d, a, b, c, X[ 7], MD5_S42, MD5_T50);
	II (c, d, a, b, X[14], MD5_S43, MD5_T51);
	II (b, c, d, a, X[ 5], MD5_S44, MD5_T52);
	II (a, b, c, d, X[12], MD5_S41, MD5_T53);
	II (d, a, b, c, X[ 3], MD5_S42, MD5_T54);
	II (c, d, a, b, X[10], MD5_S43, MD5_T55);
	II (b, c, d, a, X[ 1], MD5_S44, MD5_T56);
	II (a, b, c, d, X[ 8], MD5_S41, MD5_T57);
	II (d, a, b, c, X[15], MD5_S42, MD5_T58);
	II (c, d, a, b, X[ 6], MD5_S43, MD5_T59);
	II (b, c, d, a, X[13], MD5_S44, MD5_T60);
	II (a, b, c, d, X[ 4], MD5_S41, MD5_T61);
	II (d, a, b, c, X[11], MD5_S42, MD5_T62);
	II (c, d, a, b, X[ 2], MD5_S43, MD5_T63);
	II (b, c, d, a, X[ 9], MD5_S44, MD5_T64);
    
	m_lMD5[0] += a;
	m_lMD5[1] += b;
	m_lMD5[2] += c;
	m_lMD5[3] += d;
}

MD5::MD5()
{
	memset( m_lpszBuffer, 0, 64 );
	m_nCount[0] = m_nCount[1] = 0;
	m_lMD5[0] = MD5_INIT_STATE_0;
	m_lMD5[1] = MD5_INIT_STATE_1;
	m_lMD5[2] = MD5_INIT_STATE_2;
	m_lMD5[3] = MD5_INIT_STATE_3;
}

void MD5::uint32_tToByte(UnChar_t* Output, UnInt32_t* Input, UnInt32_t nLength)
{
	MD5_GASSERT(nLength % 4 == 0, "md5 nLength != 4");
	UnInt32_t i = 0;
	UnInt32_t j = 0;
	for ( ; j < nLength; i++, j += 4)
	{
		Output[j] = (UnChar_t)(Input[i] & 0xff);
		Output[j + 1] = (UnChar_t)((Input[i] >> 8) & 0xff);
		Output[j + 2] = (UnChar_t)((Input[i] >> 16) & 0xff);
		Output[j + 3] = (UnChar_t)((Input[i] >> 24) & 0xff);
	}
}

std::string MD5::Final()
{
	UnChar_t Bits[8];
	uint32_tToByte( Bits, m_nCount, 8 );
	UnInt32_t nIndex = (UnInt32_t)((m_nCount[0] >> 3) & 0x3f);
	UnInt32_t nPadLen = (nIndex < 56) ? (56 - nIndex) : (120 - nIndex);
	Update( PADDING, nPadLen );
	Update( Bits, 8 );
	const int nMD5Size = 16;
	UnChar_t lpszMD5[nMD5Size];
	uint32_tToByte( lpszMD5, m_lMD5, nMD5Size );
	std::string strMD5;
	for ( int i=0; i < nMD5Size; i++)
	{
		std::string Str;
		if (lpszMD5[i] == 0)
		{
			Str = std::string("00");
		}
		else if (lpszMD5[i] <= 15)
		{
            char tmp[255]={0};
            sprintf(tmp, "0%x",lpszMD5[i]);
            Str =tmp;
		}
		else
		{
            char tmp[255]={0};
            sprintf(tmp, "%x",lpszMD5[i]);
            Str =tmp;
		}
        
		MD5_GASSERT(Str.length() == 2, "md5 str.length != 2");
		strMD5 += Str;
	}
	MD5_GASSERT(strMD5.length() == 32, "md5 strMD5.length != 32");
	return strMD5;
}

void MD5::Update(UnChar_t* Input, UnInt32_t nInputLen)
{
	UnInt32_t nIndex = (UnInt32_t)((m_nCount[0] >> 3) & 0x3F);
	if ( ( m_nCount[0] += nInputLen << 3 )  <  ( nInputLen << 3) )
	{
		m_nCount[1]++;
	}
	m_nCount[1] += (nInputLen >> 29);
	UnInt32_t i = 0;
	UnInt32_t nPartLen = 64 - nIndex;
	if (nInputLen >= nPartLen)
	{
		memcpy( &m_lpszBuffer[nIndex], Input, nPartLen );
		Transform( m_lpszBuffer );
		for (i = nPartLen; i + 63 < nInputLen; i += 64)
		{
			Transform( &Input[i] );
		}
		nIndex = 0;
	}
	else
	{
		i = 0;
	}
	memcpy( &m_lpszBuffer[nIndex], &Input[i], nInputLen-i);
}

std::string MD5::GetMD5OfString(std::string strString)
{
    MD5 MD5Checksum;
    int nLength = strString.length();
	UnChar_t *Buffer;
	Buffer = (UnChar_t*)(strString.c_str());
    MD5Checksum.Update( Buffer, nLength );
    return MD5Checksum.Final();
}


/************************************************************************/
/*                                                                      */
/************************************************************************/

#define GET_UINT32(n,b,i)                                      \
{                                                                                      \
	(n) = (UnLongInt_t)((UnChar_t *)b)[(i)]                                 \
	| (((UnLongInt_t)((UnChar_t *)b)[(i)+1]) << 8)                  \
	| (((UnLongInt_t)((UnChar_t *)b)[(i)+2]) << 16)                \
	| (((UnLongInt_t)((UnChar_t *)b)[(i)+3]) << 24);               \
}

#define PUT_UINT32(n,b,i)                                              \
{                                                                                              \
	(((UnChar_t*)b)[(i)])   = (UnChar_t)(((n)) & 0xFF);             \
	(((UnChar_t*)b)[(i)+1]) = (UnChar_t)(((n) >> 8) & 0xFF);      \
	(((UnChar_t*)b)[(i)+2]) = (UnChar_t)(((n) >> 16) & 0xFF);     \
	(((UnChar_t*)b)[(i)+3]) = (UnChar_t)(((n) >> 24) & 0xFF);     \
}

//extern pthread_mutex_t mutexMemory;

void CMD5::md5_starts(struct md5_context *ctx)
{
	ctx->total[0] = 0;
	ctx->total[1] = 0;
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xEFCDAB89;
	ctx->state[2] = 0x98BADCFE;
	ctx->state[3] = 0x10325476;
}

void CMD5::md5_process(struct md5_context *ctx, UnChar_t data[64])
{
	UnLongInt_t A, B, C, D, X[16];

	GET_UINT32(X[0], data, 0);
	GET_UINT32(X[1], data, 4);
	GET_UINT32(X[2], data, 8);
	GET_UINT32(X[3], data, 12);
	GET_UINT32(X[4], data, 16);
	GET_UINT32(X[5], data, 20);
	GET_UINT32(X[6], data, 24);
	GET_UINT32(X[7], data, 28);
	GET_UINT32(X[8], data, 32);
	GET_UINT32(X[9], data, 36);
	GET_UINT32(X[10], data, 40);
	GET_UINT32(X[11], data, 44);
	GET_UINT32(X[12], data, 48);
	GET_UINT32(X[13], data, 52);
	GET_UINT32(X[14], data, 56);
	GET_UINT32(X[15], data, 60);

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define P(a,b,c,d,k,s,t)                                    \
			{                                                                           \
	a += F(b, c, d) + X[k] + t; a = S(a, s) + b;      \
			}

	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];

#define F(x,y,z) (z ^ (x & (y ^ z)))

	P(A, B, C, D, 0, 7, 0xD76AA478);
	P(D, A, B, C, 1, 12, 0xE8C7B756);
	P(C, D, A, B, 2, 17, 0x242070DB);
	P(B, C, D, A, 3, 22, 0xC1BDCEEE);
	P(A, B, C, D, 4, 7, 0xF57C0FAF);
	P(D, A, B, C, 5, 12, 0x4787C62A);
	P(C, D, A, B, 6, 17, 0xA8304613);
	P(B, C, D, A, 7, 22, 0xFD469501);
	P(A, B, C, D, 8, 7, 0x698098D8);
	P(D, A, B, C, 9, 12, 0x8B44F7AF);
	P(C, D, A, B, 10, 17, 0xFFFF5BB1);
	P(B, C, D, A, 11, 22, 0x895CD7BE);
	P(A, B, C, D, 12, 7, 0x6B901122);
	P(D, A, B, C, 13, 12, 0xFD987193);
	P(C, D, A, B, 14, 17, 0xA679438E);
	P(B, C, D, A, 15, 22, 0x49B40821);

#undef F

#define F(x,y,z) (y ^ (z & (x ^ y)))

	P(A, B, C, D, 1, 5, 0xF61E2562);
	P(D, A, B, C, 6, 9, 0xC040B340);
	P(C, D, A, B, 11, 14, 0x265E5A51);
	P(B, C, D, A, 0, 20, 0xE9B6C7AA);
	P(A, B, C, D, 5, 5, 0xD62F105D);
	P(D, A, B, C, 10, 9, 0x02441453);
	P(C, D, A, B, 15, 14, 0xD8A1E681);
	P(B, C, D, A, 4, 20, 0xE7D3FBC8);
	P(A, B, C, D, 9, 5, 0x21E1CDE6);
	P(D, A, B, C, 14, 9, 0xC33707D6);
	P(C, D, A, B, 3, 14, 0xF4D50D87);
	P(B, C, D, A, 8, 20, 0x455A14ED);
	P(A, B, C, D, 13, 5, 0xA9E3E905);
	P(D, A, B, C, 2, 9, 0xFCEFA3F8);
	P(C, D, A, B, 7, 14, 0x676F02D9);
	P(B, C, D, A, 12, 20, 0x8D2A4C8A);

#undef F

#define F(x,y,z) (x ^ y ^ z)

	P(A, B, C, D, 5, 4, 0xFFFA3942);
	P(D, A, B, C, 8, 11, 0x8771F681);
	P(C, D, A, B, 11, 16, 0x6D9D6122);
	P(B, C, D, A, 14, 23, 0xFDE5380C);
	P(A, B, C, D, 1, 4, 0xA4BEEA44);
	P(D, A, B, C, 4, 11, 0x4BDECFA9);
	P(C, D, A, B, 7, 16, 0xF6BB4B60);
	P(B, C, D, A, 10, 23, 0xBEBFBC70);
	P(A, B, C, D, 13, 4, 0x289B7EC6);
	P(D, A, B, C, 0, 11, 0xEAA127FA);
	P(C, D, A, B, 3, 16, 0xD4EF3085);
	P(B, C, D, A, 6, 23, 0x04881D05);
	P(A, B, C, D, 9, 4, 0xD9D4D039);
	P(D, A, B, C, 12, 11, 0xE6DB99E5);
	P(C, D, A, B, 15, 16, 0x1FA27CF8);
	P(B, C, D, A, 2, 23, 0xC4AC5665);

#undef F

#define F(x,y,z) (y ^ (x | ~z))

	P(A, B, C, D, 0, 6, 0xF4292244);
	P(D, A, B, C, 7, 10, 0x432AFF97);
	P(C, D, A, B, 14, 15, 0xAB9423A7);
	P(B, C, D, A, 5, 21, 0xFC93A039);
	P(A, B, C, D, 12, 6, 0x655B59C3);
	P(D, A, B, C, 3, 10, 0x8F0CCC92);
	P(C, D, A, B, 10, 15, 0xFFEFF47D);
	P(B, C, D, A, 1, 21, 0x85845DD1);
	P(A, B, C, D, 8, 6, 0x6FA87E4F);
	P(D, A, B, C, 15, 10, 0xFE2CE6E0);
	P(C, D, A, B, 6, 15, 0xA3014314);
	P(B, C, D, A, 13, 21, 0x4E0811A1);
	P(A, B, C, D, 4, 6, 0xF7537E82);
	P(D, A, B, C, 11, 10, 0xBD3AF235);
	P(C, D, A, B, 2, 15, 0x2AD7D2BB);
	P(B, C, D, A, 9, 21, 0xEB86D391);

#undef F

	ctx->state[0] += A;
	ctx->state[1] += B;
	ctx->state[2] += C;
	ctx->state[3] += D;
}

void CMD5::md5_update(struct md5_context *ctx, UnChar_t *input, UnLongInt_t length)
{
	UnLongInt_t left, fill;

	if (!length) return;

	left = (ctx->total[0] >> 3) & 0x3F;
	fill = 64 - left;

	ctx->total[0] += length << 3;
	ctx->total[1] += length >> 29;

	ctx->total[0] &= 0xFFFFFFFF;
	ctx->total[1] += ctx->total[0] < length << 3;

	if (left && length >= fill)
	{
		memcpy((void *)(ctx->buffer + left), (void *)input, fill);
		md5_process(ctx, ctx->buffer);
		length -= fill;
		input += fill;
		left = 0;
	}

	while (length >= 64)
	{
		md5_process(ctx, input);
		length -= 64;
		input += 64;
	}

	if (length)
	{
		memcpy((void *)(ctx->buffer + left), (void *)input, length);
	}
}

static UnChar_t md5_padding[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void CMD5::md5_finish(struct md5_context *ctx, UnChar_t digest[16])
{
	UnLongInt_t last, padn;
	UnChar_t msglen[8];

	PUT_UINT32(ctx->total[0], msglen, 0);
	PUT_UINT32(ctx->total[1], msglen, 4);

	last = (ctx->total[0] >> 3) & 0x3F;
	padn = (last < 56) ? (56 - last) : (120 - last);

	md5_update(ctx, md5_padding, padn);
	md5_update(ctx, msglen, 8);

	PUT_UINT32(ctx->state[0], digest, 0);
	PUT_UINT32(ctx->state[1], digest, 4);
	PUT_UINT32(ctx->state[2], digest, 8);
	PUT_UINT32(ctx->state[3], digest, 12);
}

void CMD5::GenerateMD5(UnChar_t* buffer, int bufferlen)
{
	struct md5_context context;
	md5_starts(&context);
	md5_update(&context, buffer, bufferlen);
	md5_finish(&context, (UnChar_t*)m_data);
}

CMD5::CMD5()
{
	for (int i = 0; i < 4; i++)
		m_data[i] = 0;
}

CMD5::CMD5(UnLong_t* md5src)
{
	memcpy(m_data, md5src, 16);
}

char * mystrdup(const char *s)
{
	size_t len = strlen(s) + 1;
	void *newChar = malloc(len);
	if (NULL == newChar)
		return NULL;
	return (char *)memcpy(newChar, s, len);
}

int _httoi(const char *value)
{
	struct CHexMap
	{
		char chr;
		int value;
	};
	const int HexMapL = 16;
	CHexMap HexMap[HexMapL] =
	{
		{ '0', 0 }, { '1', 1 },
		{ '2', 2 }, { '3', 3 },
		{ '4', 4 }, { '5', 5 },
		{ '6', 6 }, { '7', 7 },
		{ '8', 8 }, { '9', 9 },
		{ 'a', 10 }, { 'b', 11 },
		{ 'c', 12 }, { 'd', 13 },
		{ 'e', 14 }, { 'f', 15 }
	};
	//pthread_mutex_lock(&mutexMemory);
	char *mstr = mystrdup(value);
	//pthread_mutex_unlock(&mutexMemory);
	char *s = mstr;
	int result = 0;
	if (*s == '0' && *(s + 1) == 'X') s += 2;
	bool firsttime = true;
	while (*s != '\0')
	{
		bool found = false;
		for (int i = 0; i < HexMapL; i++)
		{
			if (*s == HexMap[i].chr)
			{
				if (!firsttime) result <<= 4;
				result |= HexMap[i].value;
				found = true;
				break;
			}
		}
		if (!found) break;
		s++;
		firsttime = false;
	}
	//pthread_mutex_lock(&mutexMemory);
	free(mstr);
	//pthread_mutex_unlock(&mutexMemory);
	return result;
}


CMD5::CMD5(const char* md5src)
{
	if (strcmp(md5src, "") == 0)
	{
		for (int i = 0; i < 4; i++)
			m_data[i] = 0;
		return;
	}
	for (int j = 0; j < 16; j++)
	{
		char buf[10];
		strncpy(buf, md5src, 2);
		md5src += 2;
		((UnChar_t*)m_data)[j] = _httoi(buf);
	}
}

CMD5 CMD5::operator +(CMD5 adder)
{
	UnLong_t m_newdata[4];
	for (int i = 0; i < 4; i++)
		m_newdata[i] = m_data[i] ^ (adder.m_data[i]);
	return CMD5(m_newdata);
}

bool CMD5::operator ==(CMD5 cmper)
{
	return (memcmp(cmper.m_data, m_data, 16) == 0);
}

//void MD5::operator =(MD5 equer)
//{
//    memcpy(m_data,equer.m_data ,16);
//}

std::string CMD5::ToString()
{
	char output[33];
	for (int j = 0; j < 16; j++)
	{
		sprintf(output + j * 2, "%02x", ((UnChar_t*)m_data)[j]);
	}
	return std::string(output);
}


