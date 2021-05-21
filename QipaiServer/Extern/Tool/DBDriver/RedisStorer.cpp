#include "RedisStorer.h"
#include <string>
#include <vector>
#include <map>
#pragma comment(lib, "ws2_32.lib")

#define CHECKCONTEXT(c)		Retry: {	\
										\
	if (nullptr == GetRedisContext())		\
		return FAILED;					\
}

#define CHECKREPLY(r) {					\
	if (nullptr == r) {					\
		printf("redisReply is NULL! f=%s, l=%d\n", __FUNCTION__, __LINE__);	\
	    goto Retry;						\
    }                                   \
}


RedisStorer::RedisStorer()
{
	m_pContext = nullptr;
}

RedisStorer::~RedisStorer()
{
	if (nullptr != m_pContext)
	{
		redisFree(m_pContext);
		m_pContext = nullptr;
	}
}


int RedisStorer::Connect(std::string ip, int port, std::string passward)
{
	//struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	//m_pContext = redisConnectWithTimeout(ip.c_str(), port, timeout);
	//m_pContext = redisConnectNonBlock(ip.c_str(), port);
	m_pContext = redisConnect(ip.c_str(), port);
	if (m_pContext != nullptr && m_pContext->err == 0)
	{
		m_Ip = ip;
		m_Port = port;
		m_pssward = passward;

		if (m_pssward != "")
		{
			CHECKCONTEXT(m_pContext);
			m_pReply = (redisReply*)redisCommand(m_pContext, "auth %s", m_pssward.c_str());
			CHECKREPLY(m_pReply);

			if (m_pReply->type != REDIS_REPLY_ERROR)
			{
				//处理成功
				freeReplyObject(m_pReply);
				return SUCCESS;
			}
		}
		else
		{
			return SUCCESS;
		}
	}

	redisFree(m_pContext);
	m_pContext = nullptr;
	return FAILED;
}

int RedisStorer::reconnect()
{
	return Connect(m_Ip, m_Port,m_pssward);
}

redisContext *RedisStorer::GetRedisContext()
{
	bool bNeedConnect = false;
	if (m_pContext == nullptr)
	{
		bNeedConnect = true;
		printf("m_pContext = NULL ! and rediContext will reconnect ...\n");
	}
    else if (m_pContext->err != 0)
    {
        bNeedConnect = true;
		printf("rediContext will reconnect, err=%d, erroInfo=%s\n", m_pContext->err, m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = nullptr;
    }

	if (bNeedConnect)
	{
		if (SUCCESS != reconnect())
		{
			printf("redisContext reconnect failed!\n");
			return nullptr;
		}
		printf("redisContext reconnect success!\n");
	}

	return m_pContext;
}

int RedisStorer::DelKey(const std::string &key)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "del %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::DelKey(std::vector<std::string> &keys)
{
	std::string command = "del";
	for (std::vector<std::string>::iterator it = keys.begin(); it != keys.end(); ++it)
	{
		command = command + " " + *it;
	}
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "del %s", command.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetKey(const std::string &key, std::vector<std::string> vec)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "keys %s*", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			vec.push_back(m_pReply->element[j]->str);
		}
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ExistKey(const std::string &key)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "exists %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER && m_pReply->integer > 0)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ExpireAt(const std::string &key, unsigned int timestamp)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "expireat %s %d", key.c_str(), timestamp);
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::Expire(const std::string &key, int seconds)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "expire %s %d", key.c_str(), seconds);
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::TTL(const std::string &key, int &seconds)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "ttl %s", key.c_str());

	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		seconds = static_cast<int>(m_pReply->integer);
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::Persist(const std::string &key)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "persist %s", key.c_str());

	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetAdd(const std::string &key, const std::string &value)
{
	return SetAdd(key, value.c_str(), value.length());
}

int RedisStorer::SetAdd(const std::string &key, const char* value, unsigned int len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "sadd %s %b", key.c_str(), value, len);
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetAdd(const std::string &key, int& value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "sadd %s %d", key.c_str(), value);
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetDel(const std::string &key, const std::string &value)
{
	return SetDel(key, value.c_str(), value.length());
}

int RedisStorer::SetDel(const std::string &key, const char* value, unsigned int len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "srem %s %b", key.c_str(), value, len);
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetDel(const std::string &key, int &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "srem %s %d", key.c_str(), value);
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::IsSet(const std::string &key, std::string value)
{
	return IsSet(key, value.c_str(), value.length());
}

int RedisStorer::IsSet(const std::string &key, const char* value, unsigned int len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "sismember %s %b", key.c_str(), value, len);
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetMove(const std::string &src, const std::string &dest, const std::string &value)
{
	return SetMove(src, dest, value.c_str(), value.length());
}

int RedisStorer::SetMove(const std::string &src, const std::string &dest, const char* value, unsigned int len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "smove %s %s %b", src.c_str(), dest.c_str(), value, len);
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetSetAll(const std::string &key, std::set<std::string> &setret)
{
    CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "smembers %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			setret.insert(std::string(m_pReply->element[j]->str));
		}
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetSetAll(const std::string &key, std::set<int> &setret)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "smembers %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			int value = atoi(m_pReply->element[j]->str);
			setret.insert(value);
		}

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetSetAllString(const std::string &key, std::set<int> &setret)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "smembers %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			setret.insert(atoi(m_pReply->element[j]->str));
		}

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetString(const std::string &key, char** value, unsigned int &len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "get %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		//没有数据或缓冲区不足
		if (m_pReply->len <= 0 || m_pReply->len > static_cast<int>(len))
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		//拷贝数据到输出缓冲区
		memcpy(*value, m_pReply->str, m_pReply->len);
		len = m_pReply->len;

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetString(const std::string &key, std::string &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "get %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		//拷贝字符串
		value = std::string(m_pReply->str);
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetString(const std::string &key, int &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "get %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		value = atoi(m_pReply->str);
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetString(const std::string &key, const char* value, unsigned int len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "set %s %b", key.c_str(), value, len);
	//它总返回字符串OK
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetString(const std::string &key, const std::string &value)
{
	return SetString(key, value.c_str(), value.length());
}

int RedisStorer::SetString(const std::string &key, int value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "set %s %d", key.c_str(), value);
	//它总返回字符串OK
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetStringRange(const std::string &key, int offset, int len, char** value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "getrange %s %d %d", key.c_str(), offset, offset + len);

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		//没有数据或缓冲区不足
		if (m_pReply->len <= 0 || m_pReply->len > len)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		//拷贝数据到输出缓冲区
		memcpy(*value, m_pReply->str, m_pReply->len);
		len = m_pReply->len;

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetStringRange(const std::string &key, int offset, int valuelen, char* value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "setrange %s %d %b", key.c_str(), offset, value, valuelen);
	//只有成功
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::IncrString(const std::string &key, int &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "incrby %s %d", key.c_str(), value);
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		value = static_cast<int>(m_pReply->integer);
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}


int RedisStorer::Zadd(const std::string &key, int id, int source)
{
	char szid[10] = { 0 };
	sprintf_s(szid, sizeof(szid), "%09d", id);
	source = MAX_RANGESCORE - source;
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "zadd %s %d %s", key.c_str(), source, szid);
	//只有成功,第一次添加返回1，第N次添加返回0
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ZCount(const std::string &key, int &count, int minsource, int maxsource)
{
	CHECKCONTEXT(m_pContext);
	if (minsource != 0 || maxsource != 0)
	{
		m_pReply = (redisReply*)redisCommand(m_pContext, "zcount %s %d %d", key.c_str(), minsource, maxsource);
	}
	else
	{
		//所有
		m_pReply = (redisReply*)redisCommand(m_pContext, "zcount %s -inf +inf", key.c_str());
	}
	
	//只有成功
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//返回分数     
		count = static_cast<int>(m_pReply->integer);

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ZRank(const std::string &key, int id, int &rank)
{
	char szid[10] = { 0 };
	sprintf_s(szid, sizeof(szid), "%09d", id);
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "zrank %s %s", key.c_str(), szid);
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//返回排名     
		rank = static_cast<int>(m_pReply->integer)+1;
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

//获取某个成员的积分
int RedisStorer::ZScore(const std::string &key, int id, int &Score)
{
	char szid[10] = { 0 };
	sprintf_s(szid, sizeof(szid), "%09d", id);
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "ZSCORE %s %s", key.c_str(),szid);
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		//返回积分 
		Score = MAX_RANGESCORE - static_cast<int>(atoi(m_pReply->str));
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ZRange(const std::string &key, std::vector<RankData> &ranks, int start, int stop)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "zrange %s %d %d withscores", key.c_str(), start, stop);
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			RankData rank;
			rank.id = atoi(m_pReply->element[j]->str);
			rank.source = MAX_RANGESCORE - atoi(m_pReply->element[++j]->str);
			ranks.push_back(rank);
		}

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

//删除排名数据
int RedisStorer::ZDel(const std::string &key, int start, int stop)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "ZREMRANGEBYRANK %s %d %d", key.c_str(), start, stop);
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

//批量设置一个Hash
int RedisStorer::SetHash(const std::string &key, const char *format, ...)
{
	CHECKCONTEXT(m_pContext);
	va_list ap;
	va_start(ap, format);
	m_pReply = (redisReply*)redisvCommand(m_pContext, format, ap);
	va_end(ap);
	//必定成功
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetHash(const std::string &key, std::map<int, int> &mapset)
{
	CHECKCONTEXT(m_pContext);
	char szTempToStr[32];

	std::string Command = "hmset ";
	Command += key;

	for (std::map<int, int>::iterator iter = mapset.begin();
		iter != mapset.end();
		++iter)
	{
		memset(szTempToStr, 0, sizeof(szTempToStr));
		sprintf_s(szTempToStr, sizeof(szTempToStr), " %d", iter->first);
		Command += szTempToStr;
		memset(szTempToStr, 0, sizeof(szTempToStr));
		sprintf_s(szTempToStr, sizeof(szTempToStr), " %d", iter->second);
		Command += szTempToStr;
	}
	//它总返回字符串OK
	m_pReply = (redisReply*)redisCommand(m_pContext, Command.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STATUS)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

//批量设置一个Hash
int RedisStorer::SetHash(const std::string &key, std::map<std::string, std::string> &mapset)
{
	CHECKCONTEXT(m_pContext);
	//拼凑命令
	std::string Command = "hmset ";
	Command += key;

	for (std::map<std::string, std::string>::iterator iter = mapset.begin();
		iter != mapset.end(); ++iter)
	{
		Command += " ";
		Command += iter->first;
		Command += " ";
		Command += iter->second;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, Command.c_str());
	//它总返回字符串OK
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetHash(const std::string &key, std::map<int, std::string> &mapset)
{
	CHECKCONTEXT(m_pContext);
	char szTempToStr[32];

	std::string Command = "hmset ";
	Command += key;

	for (std::map<int, std::string>::iterator iter = mapset.begin();
		iter != mapset.end();
		++iter)
	{
		memset(szTempToStr, 0, sizeof(szTempToStr));
		sprintf_s(szTempToStr, sizeof(szTempToStr), " %d", iter->first);
		Command += szTempToStr;
		Command += iter->second;
	}
	//它总返回字符串OK
	m_pReply = (redisReply*)redisCommand(m_pContext, Command.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STATUS)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetHash(const std::string &key, std::map<std::string, int> &mapset)
{
	CHECKCONTEXT(m_pContext);
	//拼凑命令
	std::string Command = "hmset ";
	Command += key;

	char szTempToStr[32];
	for (std::map<std::string, int>::iterator iter = mapset.begin();
		iter != mapset.end(); ++iter)
	{
		Command += " ";
		Command += iter->first;
		memset(szTempToStr, 0, sizeof(szTempToStr));
		sprintf_s(szTempToStr, sizeof(szTempToStr), " %d", iter->second);
		Command += szTempToStr;
	}
	//它总返回字符串OK
	m_pReply = (redisReply*)redisCommand(m_pContext, Command.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STATUS)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHash(const std::string &key, std::map<std::string, std::string> &mapret)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hgetall %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements/2; ++j)
		{
			std::string mykey = std::string(m_pReply->element[j * 2]->str);
			mapret[mykey] = std::string(m_pReply->element[j * 2 + 1]->str);
		}

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHash(const std::string &key, std::map<int, int> &mapret)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hgetall %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			int mykey = atoi(m_pReply->element[j]->str);
			int myval = atoi(m_pReply->element[++j]->str);
			mapret[mykey] = myval;
		}

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHash(const std::string &key, std::map<std::string, int> &mapret)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hgetall %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			std::string mykey = m_pReply->element[j]->str;
			int myval = atoi(m_pReply->element[++j]->str);
			mapret[mykey] = myval;
		}
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHash(const std::string &key, std::map<int, std::string> &mapret)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hgetall %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			int mykey = atoi(m_pReply->element[j]->str);
			mapret[mykey] = m_pReply->element[++j]->str;;
		}
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHash(const std::string &key, std::map<int, RedisBinaryData> &mapret)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hgetall %s", key.c_str());

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			int mykey = atoi(m_pReply->element[j]->str);

			int dataIdx = ++j;
			RedisBinaryData sbinary;
			mapret[mykey] = sbinary;
			mapret[mykey].Write(m_pReply->element[dataIdx]->str, m_pReply->element[dataIdx]->len);
		}
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

//获取所有的hash二进制内容
int RedisStorer::GetHash(const std::string &key, std::map<std::string, RedisBinaryData> &mapret)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hgetall %s", key.c_str());

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		//成对出现的id和分数，这里不为0的话，下面的代码会崩溃
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			std::string mykey = m_pReply->element[j]->str;
			int dataIdx = ++j;
			RedisBinaryData sbinary;
			mapret[mykey] = sbinary;
			mapret[mykey].Write(m_pReply->element[dataIdx]->str, m_pReply->element[dataIdx]->len);
		}
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

//获取某个Field的函数
int RedisStorer::GetHashByField(const std::string &key, std::map<std::string, std::string> &mapret)
{
	CHECKCONTEXT(m_pContext);
	//拼凑命令
	std::string Command = "hget ";
	Command += key;

	for (std::map<std::string, std::string>::iterator iter = mapret.begin();
		iter != mapret.end();
		++iter)
	{
		Command += " ";
		Command += iter->first;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, Command.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
        std::map<std::string, std::string>::iterator iterData = mapret.begin();
        for (size_t j = 0; j < m_pReply->elements && iterData != mapret.end();
            ++iterData, ++j)
        {
            if (m_pReply->element[j]->type == REDIS_REPLY_STRING
                && nullptr != m_pReply->element[j]->str)
            {
				iterData->second = std::string(m_pReply->element[j]->str);
            }
        }
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHashByField(const std::string &key, std::map<int, int> &mapret)
{
    CHECKCONTEXT(m_pContext);
    char tmp[32];
	//拼凑命令
	std::string Command = "hmget ";
    Command += key;

	for (std::map<int, int>::iterator iter = mapret.begin();
		iter != mapret.end();
		++iter)
	{
		sprintf_s(tmp,sizeof(tmp)," %d", iter->first);
		Command += tmp;
	}

    m_pReply = (redisReply*)redisCommand(m_pContext, Command.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
        std::map<int, int>::iterator iterData = mapret.begin();
        for (size_t j = 0; j < m_pReply->elements && iterData != mapret.end();
            ++iterData, ++j)
        {
            if (m_pReply->element[j]->type == REDIS_REPLY_STRING
                && nullptr != m_pReply->element[j]->str)
            {
                iterData->second = atoi(m_pReply->element[j]->str);
            }
        }
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHashByField(const std::string &key, int field, int &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hget %s %d", key.c_str(), field);

	//第一次Set返回1 第N次返回0
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		value = atoi(m_pReply->str);
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

//获取某个Field的函数
int RedisStorer::GetHashByField(const std::string &key, const std::string &field, std::string &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hget %s %s", key.c_str(), field.c_str());

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		value = std::string(m_pReply->str);
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHashByField(const std::string &key, const std::string &field, int &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hget %s %s", key.c_str(), field.c_str());

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		value = atoi(m_pReply->str);
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHashByField(const std::string &key, int field, std::string &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hget %s %d", key.c_str(), field);

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		value = std::string(m_pReply->str);
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHashByField(const std::string &key, std::string field, char *value, int &len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hget %s %s", key.c_str(), field.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		memcpy(value, m_pReply->str, m_pReply->len);
		len = m_pReply->len;
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHashByField(const std::string &key, int field, char *value, int &len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hget %s %d", key.c_str(), field);
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		memcpy(value, m_pReply->str, m_pReply->len);
		len = m_pReply->len;
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::GetHashBy2Field(const std::string &key, int field1, int field2, int &value1, int &value2)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hmget %s %d %d", key.c_str(), field1, field2);
	CHECKREPLY(m_pReply);

	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		if (m_pReply->elements % 2 != 0)
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		value1 = atoi(m_pReply->element[0]->str);
		value2 = atoi(m_pReply->element[1]->str);

		freeReplyObject(m_pReply);

		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetHashByField(const std::string &key, int field, int value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hset %s %d %d", key.c_str(), field, value);
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetHashByField(const std::string &key, const std::string &field, const std::string &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hset %s %s %s", key.c_str(), field.c_str(), value.c_str());
	//第一次Set返回1 第N次返回0
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetHashByField(const std::string &key, const std::string &field, int &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hset %s %s %d", key.c_str(), field.c_str(), value);

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetHashByField(const std::string &key, int field, std::string value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hset %s %d %s", key.c_str(), field, value.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetHashByField(const std::string &key, const std::string &field, char *value, int len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hset %s %s %b", key.c_str(), field.c_str(), value, len);

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::SetHashByField(const std::string &key, int field, char *value, int len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hset %s %d %b", key.c_str(), field, value, len);

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::DelHashByField(const std::string &key, const std::string &field)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hdel %s %s", key.c_str(), field.c_str());

	//返回删除的field的数量
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::DelHashByField(const std::string &key, int field)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hdel %s %d", key.c_str(), field);

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		//处理成功
		freeReplyObject(m_pReply);
        return SUCCESS;
	}

	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::HashFieldExist(const std::string &key, const std::string &field)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hexists %s %d", key.c_str(), field.c_str());

	//存在返回1 不存在返回0
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER && 1 == m_pReply->integer)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}



int RedisStorer::HashFieldExist(const std::string &key, int field)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hexists %s %d", key.c_str(), field);

	//存在返回1 不存在返回0
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER && 1 == m_pReply->integer)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}


int RedisStorer::IncreHashByField(const std::string &key, const std::string &filed, int &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hincrby %s %s %d", key.c_str(), filed.c_str(), value);
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		value = static_cast<int>(m_pReply->integer);
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::IncreHashByField(const std::string &key, int field, int &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hincrby %s %d %d", key.c_str(), field, value);
	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER)
	{
		value = static_cast<int>(m_pReply->integer);
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::HashLen(const std::string &key, long long &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "hlen %s", key.c_str());

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER
		&& m_pReply->integer != 0)
	{
		value = m_pReply->integer;
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ListPush(const std::string &key, char *value, int len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "lpush %s %b", key.c_str(), value, len);

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER
		&& m_pReply->integer != 0)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ListPush(const std::string &key, int value)
{
    CHECKCONTEXT(m_pContext);
    m_pReply = (redisReply*)redisCommand(m_pContext, "lpush %s %d", key.c_str(), value);

    CHECKREPLY(m_pReply);
    if (m_pReply->type == REDIS_REPLY_INTEGER
        && m_pReply->integer != 0)
    {
        //处理成功
        freeReplyObject(m_pReply);
        return SUCCESS;
    }
    //获取的数据错误
    freeReplyObject(m_pReply);
    return FAILED;
}

int RedisStorer::ListPush(const std::string &key, std::string &str)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "lpush %s %s", key.c_str(), str.c_str());
	CHECKREPLY(m_pReply);

	if (m_pReply->type == REDIS_REPLY_INTEGER
		&& m_pReply->integer != 0)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;

}

// int CRedisStorer::ListBlockPop(const std::string &key, char *value, int len, int seconds)
// {
//     return SUCCESS;
// }

int RedisStorer::ListPop(const std::string &key, char *value, int &len)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "lPop %s", key.c_str());

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		//没有数据或缓冲区不足
		if (m_pReply->len <= 0 || m_pReply->len > static_cast<int>(len))
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}

		//拷贝数据到输出缓冲区
		memcpy(value, m_pReply->str, m_pReply->len);
		len = m_pReply->len;

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ListPop(const std::string &key, int &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "lPop %s", key.c_str());
	CHECKREPLY(m_pReply);
	if (m_pReply->type != REDIS_REPLY_ERROR)
	{
		value = atoi(m_pReply->str);
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ListRPop(const std::string &key, int &value)
{
    CHECKCONTEXT(m_pContext);
    m_pReply = (redisReply*)redisCommand(m_pContext, "rPop %s", key.c_str());
    CHECKREPLY(m_pReply);
    if (m_pReply->type != REDIS_REPLY_ERROR)
    {
        value = atoi(m_pReply->str);
        //处理成功
        freeReplyObject(m_pReply);
        return SUCCESS;
    }
    //获取的数据错误
    freeReplyObject(m_pReply);
    return FAILED;
}

int RedisStorer::ListRange(const std::string &key, std::set<std::string> &str)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "lrange %s 0 -1", key.c_str());

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			str.insert(std::string(m_pReply->element[j]->str));
		}

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ListRange(const std::string &key, std::vector<int> &str)
{
    CHECKCONTEXT(m_pContext);
    m_pReply = (redisReply*)redisCommand(m_pContext, "lrange %s 0 -1", key.c_str());

    CHECKREPLY(m_pReply);
    if (m_pReply->type == REDIS_REPLY_ARRAY)
    {
        for (unsigned int j = 0; j < m_pReply->elements; ++j)
        {
            str.push_back(atoi(m_pReply->element[j]->str));
        }

        //处理成功
        freeReplyObject(m_pReply);
        return SUCCESS;
    }

    //获取的数据错误
    freeReplyObject(m_pReply);
    return FAILED;
}

int RedisStorer::ListRange(const std::string &key, std::vector<std::string> &str)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "lrange %s 0 -1", key.c_str());
	CHECKREPLY(m_pReply);

	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			str.push_back(std::string(m_pReply->element[j]->str));
		}

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}

	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ListRange(const std::string &key, std::list<int> &lst)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "lrange %s 0 -1", key.c_str());
	CHECKREPLY(m_pReply);

	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		for (unsigned int j = 0; j < m_pReply->elements; ++j)
		{
			lst.push_back(atoi(m_pReply->element[j]->str));
		}
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ListIndex(const std::string &key, int index, char *value, int &len)
{
	if (nullptr == value)
	{
		return DATAEXCEPTION;
	}

	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "lindex %s %d", key.c_str(), index);

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{
		//没有数据或缓冲区不足
		if (m_pReply->len <= 0 || m_pReply->len > static_cast<int>(len))
		{
			freeReplyObject(m_pReply);
			return DATAEXCEPTION;
		}
		//拷贝数据到输出缓冲区
		memcpy(value, m_pReply->str, m_pReply->len);
		len = m_pReply->len;

		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ListIndex(const std::string &key, int index, std::string &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "lindex %s %d", key.c_str(), index);

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_STRING)
	{

		value = std::string(m_pReply->str);
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ListRemove(const std::string &key, int count, std::string &value)
{
	CHECKCONTEXT(m_pContext);
	m_pReply = (redisReply*)redisCommand(m_pContext, "lrem %s %d %s", key.c_str(), count, value.c_str());

	CHECKREPLY(m_pReply);
	if (m_pReply->type == REDIS_REPLY_INTEGER
		&& m_pReply->integer != 0)
	{
		//处理成功
		freeReplyObject(m_pReply);
		return SUCCESS;
	}
	//获取的数据错误
	freeReplyObject(m_pReply);
	return FAILED;
}

int RedisStorer::ListRemove(const std::string &key, int count, int value)
{
    CHECKCONTEXT(m_pContext);
    m_pReply = (redisReply*)redisCommand(m_pContext, "lrem %s %d %d", key.c_str(), count, value);

    CHECKREPLY(m_pReply);
    if (m_pReply->type == REDIS_REPLY_INTEGER
        && m_pReply->integer != 0)
    {
        //处理成功
        freeReplyObject(m_pReply);
        return SUCCESS;
    }
    //获取的数据错误
    freeReplyObject(m_pReply);
    return FAILED;
}


