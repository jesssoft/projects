//
// 2016.04.15 jesssoft
//

#include "msg.h"

Msg::Msg(void)
{
	Init();
}

Msg::Msg(unsigned short id)
{
	Init();
	SetId(id);
}

Msg::~Msg(void)
{
	Cleanup();
}

void
Msg::Init(void)
{
	error_			= false;
	full_			= false;
	full_check_		= false;
	cur_pos_		= sizeof(MSG_HEADER);
	last_pos_		= sizeof(MSG_HEADER);
	SetId(0);
	SetArgCount(0);
}

void
Msg::ResetCurPos(void)
{
	cur_pos_ = sizeof(MSG_HEADER);
}

void
Msg::Cleanup(void)
{	
	Init();
}

bool
Msg::UpdateData(unsigned int pos, uint64_t data)
{
	if (pos >= GetCurPos())
		return false;

	unsigned char type = buf_[pos];
	void *ptr = &buf_[pos+1];

	switch (type) {
	case eARGTYPE_FLOAT:	
		*reinterpret_cast<float *>(ptr) = (float)data; 
		break;	
	case eARGTYPE_DOUBLE:	
		*reinterpret_cast<double *>(ptr) = (double)data; 
		break;	
	case eARGTYPE_BYTE_1:	
		*reinterpret_cast<unsigned char *>(ptr) = (unsigned char)data; 
		break;	
	case eARGTYPE_BYTE_2:	
		*reinterpret_cast<unsigned short *>(ptr) = 
		    (unsigned short)data; 
		break;	
	case eARGTYPE_BYTE_4:	
		*reinterpret_cast<unsigned int *>(ptr) = (unsigned int)data; 
		break;
	case eARGTYPE_BYTE_8:	
		*reinterpret_cast<uint64_t *>(ptr) = (uint64_t)data; 
		break;
	default:
		return false;
	}

	return true;
}

Msg &
Msg::operator=(Msg &arg)
{
	Cleanup();

	memcpy(buf_, arg.GetBufPtr(), arg.GetSize());
	cur_pos_	= arg.cur_pos_;
	last_pos_	= arg.last_pos_;

	return *this;
}

unsigned int 
Msg::GetRestSize(void)
{
	assert(GetMaxSize() >= GetSize());
	return GetMaxSize() - GetSize();
}

unsigned int 
Msg::CalcNeedSize(unsigned short len, unsigned char type)
{
	unsigned int add_len = 0;

	switch (type) {
	case eARGTYPE_FLOAT:	
		add_len = 0; 
		break;
	case eARGTYPE_DOUBLE:	
		add_len = 0; 
		break;
	case eARGTYPE_STRING:	
	case eARGTYPE_BINARY:	
	case eARGTYPE_MSG:		
	case eARGTYPE_ARRAY_1:
	case eARGTYPE_ARRAY_2:
	case eARGTYPE_ARRAY_4:
	case eARGTYPE_ARRAY_8:
	case eARGTYPE_ARRAY_FLOAT:
	case eARGTYPE_ARRAY_DOUBLE:
		add_len = cPARAM_STR_SIZEINFO; 
		break;
	case eARGTYPE_BYTE_1:	
		add_len = 0; 
		break;
	case eARGTYPE_BYTE_2:	
		add_len = 0; 
		break;
	case eARGTYPE_BYTE_4:	
		add_len = 0; 
		break;
	case eARGTYPE_BYTE_8:	
		add_len = 0; 
		break;
	default: 
		break;
	}

	return len + cPARAM_TYPE_SIZEINFO + add_len;
}

bool 
Msg::IsEnoughSize(unsigned short len, unsigned char type)
{
	if (GetRestSize() >= CalcNeedSize(len, type))
		return true;

	return false;
}

void 
Msg::WriteData(const void *arg, unsigned short len, unsigned char type)
{
	unsigned short *buf_ptr;

	if (IsError())
		return;

	if (IsEnoughSize(len, type) == false) {
		SetError();
		SetFull();
		return;
	}

	buf_[cur_pos_++] = type;
	last_pos_++;

	switch (type) {
	case eARGTYPE_STRING:
	case eARGTYPE_BINARY:
	case eARGTYPE_MSG:
	case eARGTYPE_ARRAY_1:
	case eARGTYPE_ARRAY_2:
	case eARGTYPE_ARRAY_4:
	case eARGTYPE_ARRAY_8:
	case eARGTYPE_ARRAY_FLOAT:
	case eARGTYPE_ARRAY_DOUBLE:
		buf_ptr = reinterpret_cast<unsigned short *>(&buf_[cur_pos_]);
		*buf_ptr = (unsigned short)len;
		cur_pos_ += cPARAM_STR_SIZEINFO;
		last_pos_ += cPARAM_STR_SIZEINFO;
		break;
	}

	memcpy(&buf_[cur_pos_], arg, len);

	cur_pos_	+= len;
	last_pos_	+= len;

	IncArgCount();
}

unsigned short 
Msg::ReadData(void *arg, unsigned short len, unsigned char type)
{
	unsigned short *buf_ptr;

	if (IsError())
		return 0;

	if (cur_pos_ >= last_pos_) {
		SetError();
		return 0;
	}

	unsigned char src_type = buf_[cur_pos_];
	
	if (src_type != type) {
		SetError();
		return 0;
	}

	unsigned short arg_len = 0;
	cur_pos_++;

	switch (src_type) {
	case eARGTYPE_STRING:
	case eARGTYPE_BINARY:
	case eARGTYPE_MSG:
	case eARGTYPE_ARRAY_1:
	case eARGTYPE_ARRAY_2:
	case eARGTYPE_ARRAY_4:
	case eARGTYPE_ARRAY_8:
	case eARGTYPE_ARRAY_FLOAT:
	case eARGTYPE_ARRAY_DOUBLE:
		buf_ptr = reinterpret_cast<unsigned short *>(&buf_[cur_pos_]);
		arg_len	= *buf_ptr;
		cur_pos_ += cPARAM_STR_SIZEINFO;
		*static_cast<unsigned char **>(arg) = &buf_[cur_pos_];
		cur_pos_ += arg_len;
		return arg_len;
	}

	memcpy(arg, &buf_[cur_pos_], len);
	cur_pos_ += len;

	return len;
}

bool 
Msg::SetMsgData(unsigned char *data, unsigned int size)
{
	if (size > cDEFAULT_BUFMAX)
		return false;
	
	last_pos_ = size;
	memcpy(buf_, data, last_pos_);

	return true;
}

unsigned int
Msg::GetCurParamSize(void)
{
	unsigned char *ptr = &buf_[cur_pos_];
	unsigned char param_type = *ptr;
	unsigned int param_size = ERROR_PARAM_SIZE;

	if (cur_pos_ >= last_pos_)
		return ERROR_PARAM_SIZE;

	switch (param_type) {		
	case eARGTYPE_FLOAT:	
		param_size = 4; 
		break;
	case eARGTYPE_DOUBLE:	
		param_size = 8; 
		break;
	case eARGTYPE_VOID_PTR: 
		param_size = sizeof(void *); 
		break;
	case eARGTYPE_BYTE_1:	
		param_size = 1;	
		break;
	case eARGTYPE_BYTE_2:	
		param_size = 2; 
		break;
	case eARGTYPE_BYTE_4:	
		param_size = 4;	
		break;
	case eARGTYPE_BYTE_8:	
		param_size = 8; 	
		break;
	case eARGTYPE_STRING:
	case eARGTYPE_BINARY:
	case eARGTYPE_MSG:
	case eARGTYPE_ARRAY_1:
	case eARGTYPE_ARRAY_2:
	case eARGTYPE_ARRAY_4:
	case eARGTYPE_ARRAY_8:
	case eARGTYPE_ARRAY_FLOAT:
	case eARGTYPE_ARRAY_DOUBLE:
		param_size = 
		    (unsigned int)*(reinterpret_cast<unsigned short *>(ptr+1));
		break;
	default:	
		return ERROR_PARAM_SIZE;
	}

	return param_size;
}

unsigned int
Msg::CalcCheckSum(void)
{
	unsigned int checksum = 0;
	unsigned int msg_size = GetSize();

	// checksum = msg_size + buffer.
	checksum = msg_size; 

	unsigned int dword_count = msg_size / sizeof(unsigned int);
	unsigned int byte_count = msg_size % sizeof(unsigned int);

	// 4 bytes
	unsigned int *dw_ptr = reinterpret_cast<unsigned int *>(buf_);
	for (unsigned int i = 0; i < dword_count; i++)
		checksum += dw_ptr[i];

	// 1 byte
	unsigned char *by_ptr = 
	    reinterpret_cast<unsigned char *>(&dw_ptr[dword_count]);
	for (unsigned int i = 0; i < byte_count; i++)
		checksum += (unsigned int)by_ptr[i];

	return checksum;
}

const char *
Msg::GetTypeString(unsigned char type)
{
	const char *ret = NULL;

	switch (type) {
	case eARGTYPE_FLOAT:	
		ret = "eARGTYPE_FLOAT"; 
		break;
	case eARGTYPE_DOUBLE:	
		ret = "eARGTYPE_DOUBLE"; 
		break;
	case eARGTYPE_STRING:	
		ret = "eARGTYPE_STRING"; 
		break;
	case eARGTYPE_BINARY:	
		ret = "eARGTYPE_BINARY"; 
		break;
	case eARGTYPE_MSG:	
		ret = "eARGTYPE_MSG"; 
		break;
	case eARGTYPE_VOID_PTR:	
		ret = "eARGTYPE_VOID_PTR"; 
		break;
	case eARGTYPE_BYTE_1:	
		ret = "eARGTYPE_BYTE_1"; 
		break;
	case eARGTYPE_BYTE_2:	
		ret = "eARGTYPE_BYTE_2"; 
		break;
	case eARGTYPE_BYTE_4:	
		ret = "eARGTYPE_BYTE_4"; 
		break;
	case eARGTYPE_BYTE_8:	
		ret = "eARGTYPE_BYTE_8"; 
		break;
	case eARGTYPE_ARRAY_1:	
		ret = "eARGTYPE_ARRAY_1"; 
		break;
	case eARGTYPE_ARRAY_2:	
		ret = "eARGTYPE_ARRAY_2"; 
		break;
	case eARGTYPE_ARRAY_4:	
		ret = "eARGTYPE_ARRAY_4"; 
		break;
	case eARGTYPE_ARRAY_8:	
		ret = "eARGTYPE_ARRAY_8"; 
		break;
	case eARGTYPE_ARRAY_FLOAT: 
		ret = "eARGTYPE_ARRAY_FLOAT"; 
		break;
	case eARGTYPE_ARRAY_DOUBLE:
		ret = "eARGTYPE_ARRAY_DOUBLE"; 
		break;
	default:
		ret = "Unknown";
		break;
	}

	assert(ret);
	return ret;
}

