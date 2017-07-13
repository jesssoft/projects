//
// 2016.04.15 jesssoft
//
#ifndef __MSG_H__
#define __MSG_H__

#include <string>
#include <cstring>
#include <cassert>
#include <stdint.h>

#define ERROR_PARAM_SIZE		0xFFFFFFFF

//
// word(id) | word(count) | [byte(type) | param] [] [] ...
//

#pragma pack(push, 4)
struct MSG_HEADER {
	unsigned short	id;
	unsigned short	cnt;
};
#pragma pack(pop)

class Msg {
public:
	Msg(void);
	Msg(unsigned short id);
	virtual ~Msg(void);
	
	Msg		&operator=(Msg &arg);
	inline MSG_HEADER *
			GetHeader(void){
				return (MSG_HEADER *)buf_;
			};

	inline unsigned short
			GetId(void) { 
				MSG_HEADER *msghdr = GetHeader();
				return msghdr->id;
			};

	inline void	SetId(unsigned short id) { 
				MSG_HEADER *msghdr = GetHeader();
				msghdr->id = id;
			};

	inline unsigned int
			GetSize(void) { return last_pos_; };
	inline unsigned int
			GetMaxSize(void) { return cDEFAULT_BUFMAX; };
	inline int	GetArgCount(void) { 
				MSG_HEADER *msghdr = GetHeader();
				return msghdr->cnt;
			};

	inline bool	IsFull(void)	{ return full_; };
	inline bool	IsError(void)	{ return error_; };
	inline bool	IsFullCheck(void) { return full_check_; };
	inline void	SetFullCheck(void) { full_check_ = true; };
	void		Reinit(void) { Cleanup(); Init(); };
	void		ResetCurPos(void);
	bool		SetMsgData(unsigned char *data, unsigned int size);
	inline unsigned char *
			GetBufPtr(void) { return buf_; };
	unsigned int	CalcCheckSum(void);
	inline unsigned int
			GetCurPos(void) { return cur_pos_; };
	bool		UpdateData(unsigned int pos, uint64_t data);
	
	// input
	Msg		&operator<<(char arg);
	Msg		&operator<<(unsigned char arg);
	Msg		&operator<<(short arg);
	Msg		&operator<<(unsigned short arg);
	Msg		&operator<<(int arg);
	Msg		&operator<<(unsigned int arg);
	Msg		&operator<<(int64_t arg);
	Msg		&operator<<(uint64_t arg);
	Msg		&operator<<(long long arg);
	Msg		&operator<<(unsigned long long arg);
	Msg		&operator<<(float arg);
	Msg		&operator<<(double arg);
	Msg		&operator<<(const char *arg);
	Msg		&operator<<(char *arg);
	Msg		&operator<<(std::string &arg);
	Msg		&operator<<(Msg &arg);
	Msg		&operator<<(void *arg);
	Msg		&AddBinary(unsigned char *arg, unsigned short len);
	Msg		&AddArray(unsigned char *arg, unsigned short count,
			    unsigned char size);
	Msg		&AddArrayFloat(float *arg, unsigned short count);
	Msg		&AddArrayDouble(float *arg, unsigned short count);

	// templates
	template<unsigned short size> 
			Msg &operator<<(unsigned char (&arg)[size]);
	template<unsigned short size> 
			Msg &operator<<(short (&arg)[size]);
	template<unsigned short size> 
			Msg &operator<<(unsigned short (&arg)[size]);
	template<unsigned short size> 
			Msg &operator<<(int (&arg)[size]);
	template<unsigned short size> 
			Msg &operator<<(unsigned int (&arg)[size]);
	template<unsigned short size> 
			Msg &operator<<(int64_t (&arg)[size]);
	template<unsigned short size> 
			Msg &operator<<(uint64_t (&arg)[size]);
	template<unsigned short size> 
			Msg &operator<<(long long (&arg)[size]);
	template<unsigned short size> 
			Msg &operator<<(unsigned long long (&arg)[size]);
	template<unsigned short size> 
			Msg &operator<<(float (&arg)[size]);
	template<unsigned short size> 
			Msg &operator<<(double (&arg)[size]);

	// output
	Msg		&operator>>(char &arg);
	Msg		&operator>>(unsigned char &arg);
	Msg		&operator>>(short &arg);
	Msg		&operator>>(unsigned short &arg);
	Msg		&operator>>(int &arg);
	Msg		&operator>>(unsigned int &arg);
	Msg		&operator>>(int64_t &arg);
	Msg		&operator>>(uint64_t &arg);
	Msg		&operator>>(long long &arg);
	Msg		&operator>>(unsigned long long &arg);
	Msg		&operator>>(float &arg);
	Msg		&operator>>(double &arg);
	Msg		&operator>>(std::string &arg);
	Msg		&operator>>(Msg &arg);
	Msg		&operator>>(void *&arg);
	Msg		&operator>>(void **arg);
	Msg		&GetString(char *arg, unsigned short len);
	Msg		&GetBinary(unsigned char *arg, unsigned short len);
	Msg		&GetArray(unsigned char *arg, unsigned short len,
			    unsigned char type);

	// templates
	template<unsigned short size> 
			Msg &operator>>(char (&arg)[size]);
	template<unsigned short size> 
			Msg &operator>>(unsigned char (&arg)[size]);
	template<unsigned short size> 
			Msg &operator>>(short (&arg)[size]);
	template<unsigned short size> 
			Msg &operator>>(unsigned short (&arg)[size]);
	template<unsigned short size> 
			Msg &operator>>(int (&arg)[size]);
	template<unsigned short size> 
			Msg &operator>>(unsigned int (&arg)[size]);
	template<unsigned short size> 
			Msg &operator>>(int64_t (&arg)[size]);
	template<unsigned short size> 
			Msg &operator>>(uint64_t (&arg)[size]);
	template<unsigned short size> 
			Msg &operator>>(long long (&arg)[size]);
	template<unsigned short size> 
			Msg &operator>>(unsigned long long (&arg)[size]);
	template<unsigned short size> 
			Msg &operator>>(float (&arg)[size]);
	template<unsigned short size> 
			Msg &operator>>(double (&arg)[size]);

public:
	static const unsigned int cDEFAULT_BUFMAX	= 8192; // 8k.
private:
	static const unsigned int cPARAM_STR_SIZEINFO	= 2;
	static const unsigned int cPARAM_TYPE_SIZEINFO	= 1;

	enum {
		eARGTYPE_FLOAT,
		eARGTYPE_DOUBLE,
		eARGTYPE_STRING,
		eARGTYPE_BINARY,
		eARGTYPE_MSG,
		eARGTYPE_VOID_PTR,
		eARGTYPE_BYTE_1,
		eARGTYPE_BYTE_2,
		eARGTYPE_BYTE_4,
		eARGTYPE_BYTE_8,
		eARGTYPE_ARRAY_1,
		eARGTYPE_ARRAY_2,
		eARGTYPE_ARRAY_4,
		eARGTYPE_ARRAY_8,
		eARGTYPE_ARRAY_FLOAT,
		eARGTYPE_ARRAY_DOUBLE
	};

	unsigned int	cur_pos_;
	unsigned int	last_pos_;
	unsigned char	buf_[cDEFAULT_BUFMAX];
	bool		error_;
	bool		full_;
	bool		full_check_;

private:
	inline void	SetArgCount(unsigned short count){
				MSG_HEADER *msghdr = GetHeader();
				msghdr->cnt = count;
	};

	inline void	IncArgCount(void){ 
				MSG_HEADER *msghdr = GetHeader();
				msghdr->cnt += 1;
	};

	void		WriteData(const void *arg, unsigned short len,
			    unsigned char type);
	unsigned short	ReadData(void *arg, unsigned short len,
			    unsigned char type);
	void		Init(void);
	void		Cleanup(void);
	unsigned int	GetRestSize(void);
	unsigned int	CalcNeedSize(unsigned short len, unsigned char type);
	bool		IsEnoughSize(unsigned short len, unsigned char type);
	inline void	SetError(void){ error_ = true; };
	inline void	SetFull(void){ full_ = true; };
	unsigned int	GetCurParamSize(void);
	const char	*GetTypeString(unsigned char type);
};

//////////////////////////////////////////////////////////////////////////
inline Msg &Msg::operator<<(char arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_BYTE_1);
	return *this;
}

inline Msg &Msg::operator<<(unsigned char arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_BYTE_1);
	return *this;
}

inline Msg &Msg::operator<<(short arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_BYTE_2);
	return *this;
}

inline Msg &Msg::operator<<(unsigned short arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_BYTE_2);
	return *this;
}

inline Msg &Msg::operator<<(int arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_BYTE_4);
	return *this;
}

inline Msg &Msg::operator<<(unsigned int arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_BYTE_4);
	return *this;
}

inline Msg &Msg::operator<<(int64_t arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_BYTE_8);
	return *this;
}

inline Msg &Msg::operator<<(uint64_t arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_BYTE_8);
	return *this;
}

inline Msg &Msg::operator<<(long long arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_BYTE_8);
	return *this;
}

inline Msg &Msg::operator<<(unsigned long long arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_BYTE_8);
	return *this;
}

inline Msg &Msg::operator<<(float arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_FLOAT);
	return *this;
}

inline Msg &Msg::operator<<(double arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_DOUBLE);
	return *this;
}

inline Msg &Msg::operator<<(const char *arg)
{
	assert(arg);

	WriteData(arg, strlen(arg) + 1, eARGTYPE_STRING);
	return *this;
}

inline Msg &Msg::operator<<(char *arg)
{
	assert(arg);

	WriteData(arg, strlen(arg) + 1, eARGTYPE_STRING);
	return *this;
}

inline Msg &Msg::operator<<(std::string &arg)
{
	WriteData(arg.data(), arg.size() + 1, eARGTYPE_STRING);
	return *this;
}

inline Msg &Msg::operator<<(Msg &arg)
{
	WriteData(arg.GetBufPtr(), arg.GetSize(), eARGTYPE_MSG);
	return *this;
}

inline Msg &Msg::operator<<(void *arg)
{
	WriteData(&arg, sizeof(arg), eARGTYPE_VOID_PTR);
	return *this;
}

inline Msg &Msg::AddBinary(unsigned char *arg, unsigned short len)
{
	WriteData(arg, len, eARGTYPE_BINARY);
	return *this;
}

inline Msg &Msg::AddArray(unsigned char *arg, unsigned short count,
    unsigned char size)
{
	unsigned int uint_len = (unsigned int)count * (unsigned int)size;
	if (uint_len >= 65536) {
		assert(0);
		SetError();
		return *this;
	}

	unsigned short len = (unsigned short)uint_len;
	switch (size) {
	case 1:
		WriteData(arg, len, eARGTYPE_ARRAY_1);
		break;
	case 2:
		WriteData(arg, len, eARGTYPE_ARRAY_2);
		break;
	case 4:
		WriteData(arg, len, eARGTYPE_ARRAY_4);
		break;
	case 8:
		WriteData(arg, len, eARGTYPE_ARRAY_8);
		break;
	default:
		assert(0);
		SetError();
		break;
	}

	return *this;
}

inline Msg &Msg::AddArrayFloat(float *arg, unsigned short count)
{
	unsigned int uint_len = (unsigned int)count 
					* (unsigned int)sizeof(float);
	if (uint_len >= 65536) {
		assert(0);
		SetError();
		return *this;
	}

	unsigned short len = (unsigned short)uint_len;
	WriteData(arg, len, eARGTYPE_ARRAY_FLOAT);
	return *this;
}

inline Msg &Msg::AddArrayDouble(float *arg, unsigned short count)
{
	unsigned int uint_len = (unsigned int)count 
					* (unsigned int)sizeof(double);
	if (uint_len >= 65536) {
		assert(0);
		SetError();
		return *this;
	}

	unsigned short len = (unsigned short)uint_len;
	WriteData(arg, len, eARGTYPE_ARRAY_DOUBLE);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator<<(unsigned char (&arg)[size])
{
	WriteData(arg, size, eARGTYPE_ARRAY_1);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator<<(short (&arg)[size])
{
	WriteData(arg, size, eARGTYPE_ARRAY_2);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator<<(unsigned short (&arg)[size])
{
	WriteData(arg, size, eARGTYPE_ARRAY_2);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator<<(int (&arg)[size])
{
	WriteData(arg, size, eARGTYPE_ARRAY_4);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator<<(unsigned int (&arg)[size])
{
	WriteData(arg, size, eARGTYPE_ARRAY_4);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator<<(int64_t (&arg)[size])
{
	WriteData(arg, size, eARGTYPE_ARRAY_8);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator<<(uint64_t (&arg)[size])
{
	WriteData(arg, size, eARGTYPE_ARRAY_8);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator<<(long long (&arg)[size])
{
	WriteData(arg, size, eARGTYPE_ARRAY_8);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator<<(unsigned long long (&arg)[size])
{
	WriteData(arg, size, eARGTYPE_ARRAY_8);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator<<(float (&arg)[size])
{
	WriteData(arg, size, eARGTYPE_ARRAY_FLOAT);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator<<(double (&arg)[size])
{
	WriteData(arg, size, eARGTYPE_ARRAY_DOUBLE);
	return *this;
}
/////////////////////////////////////////////////////////////////////////
inline Msg &Msg::operator>>(char &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_BYTE_1);
	return *this;
}

inline Msg &Msg::operator>>(unsigned char &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_BYTE_1);
	return *this;
}

inline Msg &Msg::operator>>(short &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_BYTE_2);
	return *this;
}

inline Msg &Msg::operator>>(unsigned short &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_BYTE_2);
	return *this;
}

inline Msg &Msg::operator>>(int &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_BYTE_4);
	return *this;
}

inline Msg &Msg::operator>>(unsigned int &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_BYTE_4);
	return *this;
}

inline Msg &Msg::operator>>(int64_t &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_BYTE_8);
	return *this;
}

inline Msg &Msg::operator>>(uint64_t &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_BYTE_8);
	return *this;
}

inline Msg &Msg::operator>>(long long &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_BYTE_8);
	return *this;
}

inline Msg &Msg::operator>>(unsigned long long &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_BYTE_8);
	return *this;
}

inline Msg &Msg::operator>>(float &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_FLOAT);
	return *this;
}

inline Msg &Msg::operator>>(double &arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_DOUBLE);
	return *this;
}

inline Msg &Msg::operator>>(std::string &arg)
{
	if (IsError())
		return *this;

	char *str = NULL;
	unsigned short size = ReadData(&str, sizeof(str), eARGTYPE_STRING);
	if (size > 0)
		arg = str;

	return *this;
}

inline Msg &Msg::operator>>(Msg &arg)
{
	if (IsError())
		return *this;

	unsigned int len = GetCurParamSize(); 
	if (len == ERROR_PARAM_SIZE) {
		SetError();
		return *this;
	}

	if (len > arg.GetMaxSize())
		return *this;

	unsigned short size = ReadData(arg.GetBufPtr(), len, eARGTYPE_MSG);
	if (size > 0)
		arg.last_pos_ = len;

	return *this;
}

inline Msg &Msg::operator>>(void *&arg)
{
	ReadData(&arg, sizeof(arg), eARGTYPE_VOID_PTR);
	return *this;
}

inline Msg &Msg::operator>>(void **arg)
{
	ReadData(arg, sizeof(arg), eARGTYPE_VOID_PTR);
	return *this;
}

inline Msg &Msg::GetString(char *arg, unsigned short len)
{
	if (IsError())
		return *this;

	unsigned int arg_len = GetCurParamSize();
	if (arg_len == ERROR_PARAM_SIZE) {
		SetError();
		return *this;
	}

	if (arg_len > len) {
		SetError();
		return *this;
	}

	char *str = NULL;
	unsigned short size = ReadData(&str, sizeof(str), eARGTYPE_STRING);
	if (size > 0)
		memcpy(arg, str, arg_len);

	return *this;
}

inline Msg &Msg::GetBinary(unsigned char *arg, unsigned short len)
{
	if (IsError())
		return *this;

	unsigned int arg_len = GetCurParamSize();
	if (arg_len == ERROR_PARAM_SIZE) {
		SetError();
		return *this;
	}

	if (arg_len > len) {
		SetError();
		return *this; 
	}

	unsigned char *data = NULL;
	unsigned short size = ReadData(&data, sizeof(data), eARGTYPE_BINARY);

	if (size > 0)
		memcpy(arg, data, arg_len);

	return *this;
}

inline Msg &Msg::GetArray(unsigned char *arg, unsigned short len,
    unsigned char type)
{
	if (IsError())
		return *this;

	unsigned int arg_len = GetCurParamSize();
	if (arg_len == ERROR_PARAM_SIZE) {
		SetError();
		return *this;
	}

	if (arg_len > len) {
		SetError();
		return *this; 
	}

	unsigned char *data = NULL;
	unsigned short size = ReadData(&data, sizeof(data), type);

	if (size > 0)
		memcpy(arg, data, arg_len);

	return *this;
}

//////////////////////////////////////////////////////////////////////////
// templates
//////////////////////////////////////////////////////////////////////////
template <unsigned short size>
inline Msg &Msg::operator>>(char (&arg)[size])
{
	GetString(arg, size);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator>>(unsigned char (&arg)[size])
{
	GetArray(arg, size, eARGTYPE_ARRAY_1);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator>>(short (&arg)[size])
{
	GetArray(arg, size, eARGTYPE_ARRAY_2);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator>>(unsigned short (&arg)[size])
{
	GetArray(arg, size, eARGTYPE_ARRAY_2);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator>>(int (&arg)[size])
{
	GetArray(arg, size, eARGTYPE_ARRAY_4);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator>>(unsigned int (&arg)[size])
{
	GetArray(arg, size, eARGTYPE_ARRAY_4);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator>>(int64_t (&arg)[size])
{
	GetArray(arg, size, eARGTYPE_ARRAY_8);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator>>(uint64_t (&arg)[size])
{
	GetArray(arg, size, eARGTYPE_ARRAY_8);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator>>(long long (&arg)[size])
{
	GetArray(arg, size, eARGTYPE_ARRAY_8);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator>>(unsigned long long (&arg)[size])
{
	GetArray(arg, size, eARGTYPE_ARRAY_8);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator>>(float (&arg)[size])
{
	GetArray(arg, size, eARGTYPE_ARRAY_FLOAT);
	return *this;
}

template <unsigned short size>
inline Msg &Msg::operator>>(double (&arg)[size])
{
	GetArray(arg, size, eARGTYPE_ARRAY_DOUBLE);
	return *this;
}

#endif

