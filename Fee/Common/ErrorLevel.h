#ifndef __LOG_ERROR_LEVEL_DEF_SMC__
#define __LOG_ERROR_LEVEL_DEF_SMC__

enum WARN_LEVEL
{
    LOG_WARN_GRAVITY_ERROR = 0, //严重错误
		LOG_WARN_IMPORTANCE = 1,	//重要错误
		LOG_WARN_ERROR = 2,			//一般错误
		LOG_WARN_PRMT = 3,			//提示消息
		LOG_WARN_OTHER = 4,         //其它消息
};
#endif