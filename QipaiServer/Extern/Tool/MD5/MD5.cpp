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
