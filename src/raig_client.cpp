#include "../../libraig/raig.h"

int main(int argc, char* argv[])
{
	char buffer[255];
	raig::Packet packet;
	packet.x = 0;
	packet.y = 0;
	packet.completeFlag = 0;

	raig::Packet* readPacket;

	raig::Raig* raig = new raig::Raig();
	raig->connect((char*)"127.0.0.1");

	while(packet.completeFlag == 0)
	{
		// Get input from user
		//fgets(packet.stringData, sizeof(packet.stringData), stdin);
		printf("Input: x y exit_flag word\n");
		fgets(buffer, sizeof(buffer), stdin);

		// Dissect the input and put in packet
		sscanf(buffer, "%d %d %d %s", &packet.x, &packet.y, &packet.completeFlag, packet.stringData);

		// Send packet
		raig->sendData(&packet);

		// Read network packets
		readPacket = raig->readData();
		printf("Packet received from server :\nx %d\ny %d\nexit %d\nstring %s\n", readPacket->x, readPacket->y, readPacket->completeFlag, readPacket->stringData);
	}
	return 0;
}
