#include <iostream> // cout||endl
#include <sstream> // string2int 
#include <sys/socket.h> // socket bind
#include <arpa/inet.h> // send||recv
#include <unistd.h> // sleep||usleep

class Writer
{ // Взято из cpp-brawl и чуть переделано
  public:
	int offset = 7, maxOffset;
	char *buffer;
	void allocate_memory(int chars);
	void writeInt(int a1);
	void writeString(std::string a1);
	void writeUInt(int a1);
	void writeByte(int a1);
	void writeBoolean(bool a1);
	void writeVInt(int a1);
	void writeDataReference(int a1, int a2);
	bool canWrite(int a1);
	void send_packet(int server, int a2, int a3);
};

void Writer::allocate_memory(int chars)
{
	buffer = new char[chars + 14];
	offset = 7;
	maxOffset = chars;
};

void Writer::send_packet(int server, int id, int version)
{
	buffer[0] = (id >> 8 & 0xFF);
	buffer[1] = (id & 0xFF);
	int len = offset - 7;
	buffer[2] = (len >> 16 & 0xFF);
	buffer[3] = (len >> 8 & 0xFF);
	buffer[4] = (len & 0xFF);
	buffer[5] = (version >> 8 & 0xFF);
	buffer[6] = (version & 0xFF);
	send(server, buffer, offset, 0);
	std::cout << id << std::endl;
}

void Writer::writeInt(int a1)
{
	if (canWrite(4))
	{
		buffer[offset] = (a1 >> 24 & 0xFF);
		offset += 1;
		buffer[offset] = (a1 >> 16 & 0xFF);
		offset += 1;
		buffer[offset] = (a1 >> 8 & 0xFF);
		offset += 1;
		buffer[offset] = (a1 & 0xFF);
		offset += 1;
	}
	else
	{
		std::cout << "can't write Int: offset out of index!" << std::endl;
	}
}

void Writer::writeString(std::string a1 = "none")
{
	if (a1 == "none")
	{
		writeInt(-1);
	}
	else
	{
		int len = a1.length();
		writeInt(len);
		if (canWrite(len))
		{
			for (int index = 0; index < len; index++)
			{
				buffer[offset] = a1[index];
				offset += 1;
			}
		}
		else
		{
			std::cout << "Can't write string: offset out of index!" << std::endl;
		}
	}
}

void Writer::writeUInt(int a1)
{
	if (canWrite(1))
	{
		buffer[offset] = (a1 & 0xFF);
		offset += 1;
	}
	else
	{
		std::cout << "Can't write UInt: offset out of index!" << std::endl;
	}
}

void Writer::writeByte(int a1)
{
	writeUInt(a1);
}

void Writer::writeBoolean(bool a1)
{
	if (a1)
	{
		writeUInt(1);
	}
	else
	{
		writeUInt(0);
	}
}

void Writer::writeVInt(int a1)
{
	int v1, v2, v3;
	v1 = (((a1 >> 25) & 0x40) | (a1 & 0x3F));
	v2 = ((a1 ^ (a1 >> 31)) >> 6);
	a1 >>= 6;
	if (v2 == 0)
	{
		writeByte(v1);
	}
	else
	{
		writeByte(v1 | 0x80);
		v2 >>= 7;
		v3 = 0;
		if (v2)
		{
			v3 = 0x80;
		}
		writeByte((a1 & 0x7F) | v3);
		a1 >>= 7;
		while (v2 != 0)
		{
			v2 >>= 7;
			v3 = 0;
			if (v2)
			{
				v3 = 0x80;
			}
			writeByte((a1 & 0x7F) | v3);
			a1 >>= 7;
		}
	}
}

void Writer::writeDataReference(int a1, int a2)
{
	if (a1 == 0)
	{
		writeVInt(0);
	}
	else
	{
		writeVInt(a1);
		writeVInt(a2);
	}
}

bool Writer::canWrite(int a1)
{
	if ((offset + a1) - maxOffset > 0)
	{
		return false;
	}
	return true;
}

int main(int argc, char const *argv[])
{
    if (!(argc==3))
    {
        std::cout << "./<program_name> <ip> <port>" << std::endl;
        return -1;
    }
    
    int port;
    int sock = 0;
    const char *host = argv[1];
    struct sockaddr_in server;
    std::istringstream port_buffer(argv[2]);
    port_buffer >> port;
        
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
        
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)))
    {
        std::cout << "Соединение прервано" << std::endl;
        return -1;
    }
    
    Writer w;
    
    if(1) 
    // 0 = off
    // 1 = on
    { // login
        w.allocate_memory(250);
        w.writeInt(0);
        w.writeInt(0);
        w.writeString("");
        w.writeInt(26);
        w.writeInt(0);
        w.writeInt(165);
        w.writeString("1660a966daa31a498bee59a78670c89ef4364b05");
        w.writeInt(0);
        w.writeString("???");
        w.send_packet(sock, 10101, 0);
    }
    
    usleep(1*1000000);
    
    if (1)
    { // set name
        w.allocate_memory(2048);
        w.writeString("Sinix");// Name
        w.send_packet(sock, 10212, 0);
    }
    
    /*
    К сожалению некоторые сервера не успевают принимать данные которые приходят слишком быстро, поэтому приходится делать задержку. Вы можете убрать задержку если сервер не на питоне.
    */
    
    usleep(1*1000000);
    
    if (0)
    { // create club
        w.allocate_memory(2048);
        w.writeString("?????");// Name
        w.writeString("?????");// Description
        w.writeVInt(1);// Badge Identifier
        w.writeVInt(1);// BadgeID
        w.writeVInt(1);// Region Identifier
        w.writeVInt(1);// RegionID 
        w.writeVInt(1);// Type
        w.writeVInt(228);// Trophy needed
        w.writeVInt(1);// Family friendly
        w.send_packet(sock, 14301, 0);
    }
    
    for (int i; i<10000; i++)
    { // sd win
        w.allocate_memory(2048);
        w.writeVInt(0);//battle result
        w.writeVInt(1);//???
        w.writeVInt(1);//rank?
        w.writeVInt(15);//Locations CsvID
        w.writeVInt(17);// Selected Map
        w.writeVInt(10);// Battle End Players
        
        w.writeVInt(1);// Brawler CsvID
        w.writeVInt(1);// Selected Brawler
        w.writeVInt(1);// Skin CsvID
        w.writeVInt(1);// Selected Skin
        w.writeVInt(1);// ???
        w.writeVInt(1);// ???
        w.writeString("?????");// Name
        
        w.writeVInt(1);// ???
        w.writeVInt(1);// bot brawler
        w.writeVInt(1);// bot skin
        w.writeVInt(1);// blue or red team
        w.writeVInt(0);// ???
        w.writeString("?????");// bot name
        
        w.writeVInt(1);// ???
        w.writeVInt(1);// bot brawler
        w.writeVInt(1);// bot skin
        w.writeVInt(1);// blue or red team
        w.writeVInt(0);// ???
        w.writeString("?????");// bot name
                
        w.writeVInt(1);// ???
        w.writeVInt(1);// bot brawler
        w.writeVInt(1);// bot skin
        w.writeVInt(1);// blue or red team
        w.writeVInt(0);// ???
        w.writeString("?????");// bot name
                
        w.writeVInt(1);// ???
        w.writeVInt(1);// bot brawler
        w.writeVInt(1);// bot skin
        w.writeVInt(1);// blue or red team
        w.writeVInt(0);// ???
        w.writeString("?????");// bot name
                
        w.writeVInt(1);// ???
        w.writeVInt(1);// bot brawler
        w.writeVInt(1);// bot skin
        w.writeVInt(1);// blue or red team
        w.writeVInt(0);// ???
        w.writeString("?????");// bot name
                
        w.writeVInt(1);// ???
        w.writeVInt(1);// bot brawler
        w.writeVInt(1);// bot skin
        w.writeVInt(1);// blue or red team
        w.writeVInt(0);// ???
        w.writeString("?????");// bot name
                
        w.writeVInt(1);// ???
        w.writeVInt(1);// bot brawler
        w.writeVInt(1);// bot skin
        w.writeVInt(1);// blue or red team
        w.writeVInt(0);// ???
        w.writeString("?????");// bot name
                
        w.writeVInt(1);// ???
        w.writeVInt(1);// bot brawler
        w.writeVInt(1);// bot skin
        w.writeVInt(1);// blue or red team
        w.writeVInt(0);// ???
        w.writeString("?????");// bot name
        
        w.send_packet(sock, 14110, 0);
        usleep(0.2*1000000);
    }
    
    return 0;
}
