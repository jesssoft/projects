/*
 * jesssoft
 */
using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Security.Cryptography;
using System.IO;


public class Net
{
	/* Constants */
	private const int IO_BUFMAX  = 262144;	/* 262K */
	private const int IO_HEADER_SIZE = 16;
	private const int IO_HEADER_OFFSET_SIZE = 0;
	private const int IO_HEADER_OFFSET_CHECKSUM_BDY = 4;
	private const int IO_HEADER_OFFSET_CHECKSUM_HDR = 8;
	private const int IO_HEADER_OFFSET_DES_DUMMY_SIZE = 12;
	private const int IO_HEADER_OFFSET_RESERVED = 13;
	private const int IO_HEADER_OFFSET_SERIALNO = 14;
	private const int IO_HEADER_OFFSET_DATA = IO_HEADER_SIZE;
	private const int DES_BLOCK_SIZE = 8;

	/* Variables. */
	private DESCryptoServiceProvider 	m_desEncrypt;
	private ICryptoTransform 		m_transformEnc;
	private ICryptoTransform 		m_transformDec;
	private byte[]	m_readTmpBuf;
	private uint 	m_readTmpLastPos;
	private byte[]	m_readBuf;
	private uint 	m_readLastPos;
	private ushort	m_readSerialNo;
	private byte[]	m_writeBuf;
	private uint 	m_writeLastPos;
	private ushort	m_writeSerialNo;
	private Socket 	m_server;
	private Thread 	m_thread;

	public Net() {
		Init();
	}

	private void Init() {
		m_readTmpBuf = new byte[IO_BUFMAX];
		m_readBuf = new byte[IO_BUFMAX];
		m_writeBuf = new byte[IO_BUFMAX];
		m_server = null;
		m_readTmpLastPos = 0;
		m_readLastPos = 0;
		m_readSerialNo = 0;
		m_writeLastPos = 0;
		m_writeSerialNo = 0;

		/*
		 * for des
		 */
		m_desEncrypt = new DESCryptoServiceProvider();
		m_desEncrypt.Mode = CipherMode.ECB;
		m_desEncrypt.Padding = PaddingMode.None;
		m_desEncrypt.Key = Encoding.ASCII.GetBytes("12345678");
		m_transformEnc = m_desEncrypt.CreateEncryptor();
		m_transformDec = m_desEncrypt.CreateDecryptor();
	}

	public bool Connect(string ip, ushort port) {
		if (IsConnected()) {
			Shutdown();
			Init();
		}

		IPEndPoint ipep = new IPEndPoint(IPAddress.Parse(ip), port);
		m_server = new Socket(AddressFamily.InterNetwork,
		    SocketType.Stream, ProtocolType.Tcp);

		try {
			m_server.Connect(ipep);
		} catch (SocketException /*se*/) {
			ipep = null;
			m_server = null;
			return false;
		}

		m_server.Blocking = false;
		m_server.NoDelay = true;
		if (m_server.Connected)
			Console.WriteLine("Server connected");

		m_thread = new Thread(new ThreadStart(BackgroundJob));
		m_thread.Start();
		return true;
	}

	public void Close() {
		if (m_server != null) {
			m_server.Close();
			m_server = null;
		}
	}

	public bool IsClosed() {
		if (m_server == null)
			return true;
		return m_server.Connected == false;
	}

	public bool IsConnected() {
		return IsClosed() == false;
	}

	public void Shutdown() {
		if (IsClosed())
			return;
		m_server.Shutdown(SocketShutdown.Both);
		m_server.Close();
	}

	public Msg GetMsg() {
		int read = 0;
		uint availSize = 0;
		uint tmpDataSize = 0;
		uint desDataSize = 0;
		uint desLeftSize = 0;
		byte[] data;

		if (IsClosed())
			return null;
		if (m_readTmpLastPos >= IO_BUFMAX) {
			Close();
			return null;
		}
		availSize = IO_BUFMAX - m_readTmpLastPos;

		/*
		 * trying to read
		 */
		try {
			read = m_server.Receive(m_readTmpBuf,
			    (int)m_readTmpLastPos, (int)availSize,
			    SocketFlags.None);
		} catch (SocketException se) {
			if (se.SocketErrorCode != SocketError.WouldBlock)
				Shutdown();
			return MakeMsg();
		}

		if (read <= 0)
			return MakeMsg();

		/*
		 * There is something data to read
		 */
		m_readTmpLastPos += (uint)read;
		tmpDataSize = m_readTmpLastPos;
		if (tmpDataSize < DES_BLOCK_SIZE)
			return MakeMsg();

		desLeftSize = tmpDataSize%DES_BLOCK_SIZE;
		desDataSize = (tmpDataSize/DES_BLOCK_SIZE) * DES_BLOCK_SIZE;
		data = Decrypt(m_readTmpBuf, desDataSize);
		Array.Copy(data, 0, m_readBuf, m_readLastPos, desDataSize);
		if (desLeftSize > 0)
			Array.Copy(m_readTmpBuf, desDataSize, m_readTmpBuf, 0,
			    desLeftSize);
		m_readTmpLastPos -= desDataSize;
		m_readLastPos += desDataSize;
		return MakeMsg();
	}

	private Msg MakeMsg() {
		uint bodySize;
		uint checksumHdr;
		uint checksumBdy;
		uint realBodySize;
		uint desDummySize;
		uint totalSize;
		uint leftSize;
		ushort serialNo;

		/*
		 * valid header size?
		 */
		if (m_readLastPos < IO_HEADER_SIZE)
			return null;

		/* body size */
		bodySize = BitConverter.ToUInt32(m_readBuf,
		    IO_HEADER_OFFSET_SIZE);
		/* des dummy size */
		desDummySize = (uint)m_readBuf[IO_HEADER_OFFSET_DES_DUMMY_SIZE];
		/* header checksum */
		checksumHdr = BitConverter.ToUInt32(m_readBuf,
		    IO_HEADER_OFFSET_CHECKSUM_HDR);
		/* body checksum */
		checksumBdy = BitConverter.ToUInt32(m_readBuf,
		    IO_HEADER_OFFSET_CHECKSUM_BDY);
		/* serial no */
		serialNo = (ushort)BitConverter.ToUInt16(m_readBuf,
		    IO_HEADER_OFFSET_SERIALNO);
		/* total size */
		realBodySize = bodySize - desDummySize;
		totalSize = IO_HEADER_SIZE + bodySize;

		/*
		 * valid header?
		 */
		if (checksumHdr != CalcHdrChecksum(m_readBuf, 0)) {
			Console.WriteLine("Error: header checksum error!");
			Close();
			return null;
		}

		/*
		 * valid serial no?
		 */
		if (serialNo != m_readSerialNo) {
			Console.WriteLine("Error: serial no not matched!");
			Close();
			return null;
		}

		/*
		 * valid one packet size?
		 */
		if (totalSize > m_readLastPos)
			return null;

		Msg msg = new Msg();
		msg.SetMsgData(m_readBuf, (uint)IO_HEADER_OFFSET_DATA,
		    realBodySize);

		/*
		 * body checksum is right?
		 */
		if (checksumBdy != msg.CalcChecksum()) {
			Console.WriteLine("Error: body checksum error!");
			Close();
			return null;
		}

		leftSize = m_readLastPos - totalSize;
		if (leftSize > 0)
			Array.Copy(m_readBuf, totalSize, m_readBuf, 0,
			    leftSize);
		m_readLastPos = leftSize;
		m_readSerialNo++;
		return msg;
	}

	private uint CalcHdrChecksum(byte[] buf, uint offset) {
		uint size;
		uint checksumBdy;
		uint checksumHdr;
		uint desDummySize;
		uint reserved;
		uint serialNo;

		/* body size */
		size = BitConverter.ToUInt32(buf,
		    (int)offset + IO_HEADER_OFFSET_SIZE);
		/* body checksum */
		checksumBdy = BitConverter.ToUInt32(buf,
		    (int)offset + IO_HEADER_OFFSET_CHECKSUM_BDY);
		/* des dummy size */
		desDummySize = (uint)buf[offset + 
		    IO_HEADER_OFFSET_DES_DUMMY_SIZE];
		/* reserved */
		reserved = (uint)buf[offset + IO_HEADER_OFFSET_RESERVED];
		/* serial no */
		serialNo = (uint)BitConverter.ToUInt16(buf,
		    (int)offset + IO_HEADER_OFFSET_SERIALNO);

		checksumHdr = checksumBdy + size + desDummySize + reserved +
		    serialNo;
		/*
		Console.WriteLine("test: checksumBdy=" + checksumBdy);
		Console.WriteLine("test: size=" + size);
		Console.WriteLine("test: desDummySize=" + desDummySize);
		Console.WriteLine("test: reserved=" + reserved);
		Console.WriteLine("test: serialNo=" + serialNo);
		Console.WriteLine("test: hdr checksum=" + checksumHdr);
		*/
		return checksumHdr;
	}

	public void SendMsg(Msg msg) {
		uint bodySize;
		uint totalSize;
		byte[] data;
		uint index;
		uint bodysum;
		uint headersum;
		uint desDummySize;
		uint msgSize;

		if (IsClosed())
			return;

		msgSize = msg.GetSize();
		desDummySize = DES_BLOCK_SIZE - msgSize%DES_BLOCK_SIZE;
		bodySize = msgSize + desDummySize;
		totalSize = IO_HEADER_SIZE + bodySize;
		Monitor.Enter(this);
		if (totalSize > GetRestWriteBufSize()) {
			Shutdown();
			Monitor.Exit(this);
			return;
		}

		/*
		 * io header data
		 */
		/* body size */
		data = BitConverter.GetBytes(bodySize);
		index = m_writeLastPos + IO_HEADER_OFFSET_SIZE;
		m_writeBuf[3 + index] = data[3];
		m_writeBuf[2 + index] = data[2];
		m_writeBuf[1 + index] = data[1];
		m_writeBuf[0 + index] = data[0];

		/* body checksum */
		bodysum = msg.CalcChecksum();
		data = BitConverter.GetBytes(bodysum);
		index = m_writeLastPos + IO_HEADER_OFFSET_CHECKSUM_BDY;
		m_writeBuf[3 + index] = data[3];
		m_writeBuf[2 + index] = data[2];
		m_writeBuf[1 + index] = data[1];
		m_writeBuf[0 + index] = data[0];

		/* des dummy size */
		index = m_writeLastPos + IO_HEADER_OFFSET_DES_DUMMY_SIZE;
		m_writeBuf[index] = (byte)desDummySize;

		/* reserved */
		index = m_writeLastPos + IO_HEADER_OFFSET_RESERVED;
		m_writeBuf[index] = 0; 

		/* serial no */
		data = BitConverter.GetBytes(m_writeSerialNo++);
		index = m_writeLastPos + IO_HEADER_OFFSET_SERIALNO;
		m_writeBuf[1 + index] = data[1];
		m_writeBuf[0 + index] = data[0];

		/* header checksum */
		headersum = CalcHdrChecksum(m_writeBuf, m_writeLastPos);
		data = BitConverter.GetBytes(headersum);
		index = m_writeLastPos + IO_HEADER_OFFSET_CHECKSUM_HDR;
		m_writeBuf[3 + index] = data[3];
		m_writeBuf[2 + index] = data[2];
		m_writeBuf[1 + index] = data[1];
		m_writeBuf[0 + index] = data[0];

		/* encoding */
		Array.Copy(msg.GetBufPtr(), 0,
		    m_writeBuf, m_writeLastPos + IO_HEADER_SIZE, msgSize);
		data = Encrypt(m_writeBuf, m_writeLastPos, totalSize);
		Array.Copy(data, 0, m_writeBuf, m_writeLastPos, totalSize);
		m_writeLastPos += totalSize;

		Send();
		Monitor.Exit(this);
	}

	private void Send() {
		int sent = 0;

		try {
			sent = m_server.Send(m_writeBuf, 0, 
			    (int)m_writeLastPos, SocketFlags.None);
		} catch (SocketException se) {
			if (se.SocketErrorCode != SocketError.WouldBlock)
				Shutdown();
			return;
		}

		if (sent > 0) {
			if (sent != m_writeLastPos) {
				Array.Copy(m_writeBuf, sent, m_writeBuf,
				    0, (int)m_writeLastPos - sent);
				m_writeLastPos = m_writeLastPos - (uint)sent;
			}
			else
				m_writeLastPos = 0;
		}
	}

	private uint GetRestWriteBufSize() {
		return (uint)IO_BUFMAX - m_writeLastPos;
	}

	private void BackgroundJob() {
		while (m_server.Connected) {
			Monitor.Enter(this);
			if (m_writeLastPos > 0)
				Send();
			Monitor.Exit(this);
			Thread.Sleep(10);
		}
	}

	private byte[] Encrypt(byte[] data, uint offset, uint dataSize) {
		MemoryStream memStream;
		CryptoStream cryptoStream;

		memStream = new MemoryStream();
		cryptoStream = new CryptoStream(memStream, m_transformEnc,
		    CryptoStreamMode.Write);
		cryptoStream.Write(data, (int)offset, (int)dataSize);
		cryptoStream.FlushFinalBlock();
		return memStream.ToArray();
	}

	private byte[] Decrypt(byte[] data, uint dataSize) {
		MemoryStream memStream;
		CryptoStream cryptoStream;

		memStream = new MemoryStream();
		cryptoStream = new CryptoStream(memStream, m_transformDec,
		    CryptoStreamMode.Write);
		cryptoStream.Write(data, 0, (int)dataSize);
		cryptoStream.FlushFinalBlock();
		return memStream.ToArray();
	}
}
