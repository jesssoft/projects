/*
 *  jesssoft
 */
using System;
using System.Diagnostics;

public class Msg
{
	/*
	 * consts
	 */
	private const int BUFMAX        = 32768;	/* 32k */
	private const int HEADERSIZE    = 12;
	private const int HEADER_OFFSET_ID = 0;
	private const int HEADER_OFFSET_CNT = 2;
	private const int HEADER_OFFSET_FLAG = 4;
	private const int HEADER_OFFSET_RESERVED = 6;
	private const int HEADER_OFFSET_SIZE = 8;
	private const int HEADER_OFFSET_ARG = HEADERSIZE;
	private const int PARAM_TYPE_SIZEINFO = 1;
	private const int PARAM_STR_SIZEINFO = 4;

	private enum ARGTYPE {
		INT8,
		INT16,
		INT32,
		INT64,
		UINT8,
		UINT16,
		UINT32,
		UINT64,
		FLOAT,
		DOUBLE,
		STRING,
		BINARY,		/* Not supported in c#. */
		PKT,		/* Not supported in c#. */
		TM,		/* Not supported in c#. */
		TIME_T,		/* Not supported in c#. */
		TIMEVAL,	/* Not supported in c#. */
		TIMESPEC,	/* Not supported in c#. */
		PTR		/* Not supported in c#. */
	}

	/*
	 * values
	 */	
	protected uint	m_curPos;
	protected uint	m_lastPos;
	protected byte[] m_buf;

	/*
	 * methods
	 */
	public Msg() {
		Init();
	}

	public Msg(ushort Id) {
		Init();
		SetId(Id);
	}

	public void SetId(ushort Id) {
		byte[] data = BitConverter.GetBytes((ushort)Id);

		m_buf[1 + HEADER_OFFSET_ID] = data[1];
		m_buf[0 + HEADER_OFFSET_ID] = data[0];
	}

	public ushort GetId() {
		ushort id = BitConverter.ToUInt16(m_buf, 0);
		return id;
	}

	private void SetTypeInfoFlag() {
		ushort flag = 0x01;
		byte[] data = BitConverter.GetBytes(flag);

		m_buf[1 + HEADER_OFFSET_FLAG] = data[1];
		m_buf[0 + HEADER_OFFSET_FLAG] = data[0];
	}
	
	private void SetDataSize(uint dataSize) {
		uint size;
		byte[] data;
		
		if (dataSize > HEADERSIZE)
			size = dataSize - HEADERSIZE;
		else
			size = 0;
		data = BitConverter.GetBytes(size);

		m_buf[3 + HEADER_OFFSET_SIZE] = data[3];
		m_buf[2 + HEADER_OFFSET_SIZE] = data[2];
		m_buf[1 + HEADER_OFFSET_SIZE] = data[1];
		m_buf[0 + HEADER_OFFSET_SIZE] = data[0];
	}

	public uint GetSize() {
		return m_lastPos;
	}

	public uint GetMaxSize() {
		return BUFMAX;
	}

	public uint GetArgCount() {
		ushort count = BitConverter.ToUInt16(m_buf, HEADER_OFFSET_CNT);
		return (uint)count;
	}

	public uint GetCurParamSize() {
		switch ((ARGTYPE)m_buf[m_curPos]) {
		case ARGTYPE.INT8:
		case ARGTYPE.UINT8:
			return 1;
		case ARGTYPE.INT16:
		case ARGTYPE.UINT16:
			return 2;
		case ARGTYPE.INT32:
		case ARGTYPE.UINT32:
			return 4;
		case ARGTYPE.INT64:
		case ARGTYPE.UINT64:
			return 8;
		case ARGTYPE.FLOAT:
			return 4;
		case ARGTYPE.DOUBLE:
			return 8;
		case ARGTYPE.STRING:
			return BitConverter.ToUInt32(m_buf, (int)m_curPos + 1);
		}
		return 0;
	}

	public bool SetMsgData(byte[] data, uint offset, uint size) {
		Array.Copy(data, offset, m_buf, 0, size);
		m_lastPos = size;
		return true;
	}

	public byte[] GetBufPtr() { 
		return m_buf; 
	}

	public uint CalcChecksum() {
		uint checksum = 0;
		uint msgSize = GetSize();
		uint dwordCount;
		uint byteCount;

		dwordCount = msgSize / sizeof(uint);
		byteCount = msgSize % sizeof(uint);

		/* 4bytes */
		for (uint i = 0; i < dwordCount; i++) {
			uint src = BitConverter.ToUInt32(m_buf,
			    (int)(i*sizeof(uint)));
			checksum += src;
		}
		
		/* 1bytes */
		for (uint i = 0; i < byteCount; i++) {
			checksum += m_buf[dwordCount*sizeof(uint) + i];
		}

		return checksum;
	}

	/*
	 * input
	 */
	public void AddInt8(sbyte val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(sbyte), 
		    ARGTYPE.INT8); 
	}
	public void AddUInt8(byte val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(byte), 
		    ARGTYPE.UINT8); 
	}
	public void AddInt16(short val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(short), 
		    ARGTYPE.INT16); 
	}
	public void AddUInt16(ushort val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(ushort), 
		    ARGTYPE.UINT16); 
	}
	public void AddInt32(int val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(int), 
		    ARGTYPE.INT32); 
	}
	public void AddUInt32(uint val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(uint), 
		    ARGTYPE.UINT32); 
	}
	public void AddInt(int val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(int), 
		    ARGTYPE.INT32); 
	}
	public void AddUInt(uint val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(uint), 
		    ARGTYPE.UINT32); 
	}
	public void AddInt64(long val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(long), 
		    ARGTYPE.INT64); 
	}
	public void AddUInt64(ulong val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(ulong), 
		    ARGTYPE.UINT64); 
	}
	public void AddFloat(float val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(float), 
		    ARGTYPE.FLOAT); 
	}
	public void AddDouble(double val) { 
		WriteData(BitConverter.GetBytes(val), sizeof(double), 
		    ARGTYPE.DOUBLE); 
	}
	public void AddString(string val) 
	{
		int size = System.Text.Encoding.UTF8.GetByteCount(val);
		byte[] data = new byte[size + 1];
		System.Text.Encoding.UTF8.GetBytes(val, 0, val.Length, data, 0);
		data[size] = 0;
		WriteData(data, (ushort)data.Length, ARGTYPE.STRING); 
	}

	/*
	 * output
	 */	
	public sbyte GetInt8() { 
		byte[] data = ReadData(ARGTYPE.INT8); 
		return (sbyte)data[0]; 
	}
	public byte GetUInt8() { 
		byte[] data = ReadData(ARGTYPE.UINT8); 
		return (byte)data[0]; 
	}
	public short GetInt16() { 
		byte[] data = ReadData(ARGTYPE.INT16); 
		return BitConverter.ToInt16(data, 0); 
	}
	public ushort GetUInt16() { 
		byte[] data = ReadData(ARGTYPE.UINT16); 
		return BitConverter.ToUInt16(data, 0); 
	}
	public int GetInt32() { 
		byte[] data = ReadData(ARGTYPE.INT32); 
		return BitConverter.ToInt32(data, 0); 
	}
	public uint GetUInt32() { 
		byte[] data = ReadData(ARGTYPE.UINT32); 
		return BitConverter.ToUInt32(data, 0); 
	}
	public int GetInt() { 
		byte[] data = ReadData(ARGTYPE.INT32); 
		return BitConverter.ToInt32(data, 0); 
	}
	public uint GetUInt() { 
		byte[] data = ReadData(ARGTYPE.UINT32); 
		return BitConverter.ToUInt32(data, 0); 
	}
	public long GetInt64() { 
		byte[] data = ReadData(ARGTYPE.INT64); 
		return BitConverter.ToInt64(data, 0); 
	}
	public ulong GetUInt64() { 
		byte[] data = ReadData(ARGTYPE.UINT64); 
		return BitConverter.ToUInt64(data, 0); 
	}
	public float GetFloat() { 
		byte[] data = ReadData(ARGTYPE.FLOAT); 
		return BitConverter.ToSingle(data, 0); 
	}
	public double GetDouble() { 
		byte[] data = ReadData(ARGTYPE.DOUBLE); 
		return BitConverter.ToDouble(data, 0); 
	}
	public string GetString() {
		byte[] data = ReadData(ARGTYPE.STRING);
		return System.Text.Encoding.UTF8.GetString(data);
	}

	/*
	 * private methods
	 */
	private void SetArgCount(ushort count) {
		byte[] data = BitConverter.GetBytes(count);

		m_buf[1 + HEADER_OFFSET_CNT] = data[1];
		m_buf[0 + HEADER_OFFSET_CNT] = data[0];
	}

	private void IncArgCount() {
		ushort count;
	       
		count = BitConverter.ToUInt16(m_buf, HEADER_OFFSET_CNT);
		count++;
		SetArgCount(count);
	}

	private void WriteData(byte[] arg, uint len, ARGTYPE type) {
		byte[] data;

		if (!IsEnoughSize(len, type))
			return;

		m_buf[m_curPos++] = (byte)type;
		m_lastPos++;
		switch (type) {
		case ARGTYPE.STRING:
			data = BitConverter.GetBytes(len);
			m_buf[m_curPos + 3] = data[3];
			m_buf[m_curPos + 2] = data[2];
			m_buf[m_curPos + 1] = data[1];
			m_buf[m_curPos + 0] = data[0];
			m_curPos += PARAM_STR_SIZEINFO;
			m_lastPos += PARAM_STR_SIZEINFO;
			break;
		}
		Array.Copy(arg, 0, m_buf, m_curPos, len);
		m_curPos += len;
		m_lastPos += len;
		SetDataSize(m_lastPos);
		IncArgCount();
	}

	private byte[] ReadData(ARGTYPE type) {
		byte srcType;
		uint argLen;
		byte[] data;

		if (m_curPos >= m_lastPos) {
			Console.Write("Error: Out of data!\n");
			return null;
		}
		srcType = m_buf[m_curPos];
		if (srcType != (byte)type) {
			Console.Write("Error: mismatched type!\n");
			return null;
		}
		argLen = 0;
		data = null;
		m_curPos++;
		switch ((ARGTYPE)srcType) {
		case ARGTYPE.INT8:
		case ARGTYPE.UINT8:
			data = new byte[1];
			Array.Copy(m_buf, m_curPos, data, 0, 1);
			m_curPos += 1;
			break;
		case ARGTYPE.INT16:
		case ARGTYPE.UINT16:
			data = new byte[2];
			Array.Copy(m_buf, m_curPos, data, 0, 2);
			m_curPos += 2;
			break;
		case ARGTYPE.INT32:
		case ARGTYPE.UINT32:
			data = new byte[4];
			Array.Copy(m_buf, m_curPos, data, 0, 4);
			m_curPos += 4;
			break;
		case ARGTYPE.INT64:
		case ARGTYPE.UINT64:
			data = new byte[8];
			Array.Copy(m_buf, m_curPos, data, 0, 8);
			m_curPos += 8;
			break;
		case ARGTYPE.FLOAT:
			data = new byte[4];
			Array.Copy(m_buf, m_curPos, data, 0, 4);
			m_curPos += 4;
			break;
		case ARGTYPE.DOUBLE:
			data = new byte[8];
			Array.Copy(m_buf, m_curPos, data, 0, 8);
			m_curPos += 8;
			break;
		case ARGTYPE.STRING:
			argLen = BitConverter.ToUInt32(m_buf, (int)m_curPos);
			m_curPos += PARAM_STR_SIZEINFO;
			data = new byte[argLen - 1];
			Array.Copy(m_buf, m_curPos, data, 0, argLen - 1);
			m_curPos += argLen;
			break;
		default:
			Console.Write("Error: Unknown type!\n");
			break;
		}
		return data;
	}

	private void Init() {
		m_curPos = HEADER_OFFSET_ARG;
		m_lastPos = HEADERSIZE;
		m_buf = new byte[BUFMAX];
		SetTypeInfoFlag();
		SetId(0);
		SetArgCount(0);
	}

	public void Reinit() {
		m_curPos = HEADER_OFFSET_ARG;
		m_lastPos = HEADERSIZE;
		SetId(0);
		SetArgCount(0);
	}

	private uint GetRestSize() {
		return GetMaxSize() - GetSize();
	}

	private uint CalcNeedSize(uint len, ARGTYPE type) {
		uint addLen;

		addLen = 0;
		switch (type) {
		case ARGTYPE.STRING: 
			addLen = PARAM_STR_SIZEINFO;
			break;
		}
		return (uint)len + PARAM_TYPE_SIZEINFO + addLen;
	}

	private bool IsEnoughSize(uint len, ARGTYPE type) {
		if (GetRestSize() >= CalcNeedSize(len, type))
			return true;
		return false;
	}
}
