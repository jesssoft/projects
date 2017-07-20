//
// 2016.04.15 jesssoft
//
#ifndef __IOBUF_H__
#define __IOBUF_H__

// sum(4) | length(4) | data...

#pragma pack(push, 4)
struct IO_HEADER {
	unsigned int	checksum;
	unsigned int	length;
};
#pragma pack(pop)

class IOBuf {
public:
	IOBuf(unsigned int bufSize = 8192);
	virtual ~IOBuf();

	bool		AddDataSize(unsigned int size);
	void		SubDataSize(unsigned int size);
	void		Rearrange(void);
	unsigned int	GetRestBufSize(void);
	unsigned int	GetDataSize(void);
	unsigned int	GetMsgSize(void);
	unsigned char	*GetRestBufPtr(void);
	unsigned char 	*GetDataPtr(void);
	unsigned char	*GetMsgDataPtr(void);
	bool		IsEmpty(void);
	bool		IsHeader(void);
	bool		IsMsg(void);
	inline bool	IsError(void) { return error_; };
	inline void	SetError(void) { error_ = true; };
	IO_HEADER	*GetHeader(void);
	IO_HEADER	*GetTailHeader(void);
	
private:
	const unsigned int cIO_BUF_SIZE;

	unsigned char	*buf_;
	unsigned int	head_;
	unsigned int	tail_;
	bool		error_;
};

#endif
