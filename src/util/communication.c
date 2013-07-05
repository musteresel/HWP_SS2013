



static inline void transmitEscapedByte(uint8_t byte)
{
	if ((byte == ESC) | (byte == DELIM))
	{
		Uart0_transmit(ESC);
	}
	Uart0_transmit(byte);
}




static void writePacket(uint8_t channel, uint8_t * packet, uint8_t size)
{
	static uint8_t outPacketCount = 0;
	uint8_t iterator;
	uint8_t checkSum;
	// Init the checksum using the outPacketCount and the channel
	checkSum = (outPacketCount << 5) | (channel & 0x1F);
	// Transmit channel and packet sequence information
	transmitEscapedByte(checkSum);
	// Transmit packet contents, updating checksum
	for (iterator = 0; iterator < size; iterator++)
	{
		uint8_t data = packet[iterator];
		transmitEscapedByte(data);
		checkSum ^= data;
	}
	// Transmit checksum
	transmitEscapedByte(checkSum);
	// Transmit delimiter
	Uart0_transmit(DELIM);
	// Update packet sequence number
	outPacketCount = (outPacketCount + 1) & 0x07;
}




void TransmitterTask(void)
{
	do
	{
		// Get the first message from the tasks message buffer,
		// blocking the task if no message is available
		Message * message = Message_receive();
		// Extract the channel from the message
		uint8_t channel = message->buffer[0];
		// Write the packet via UART
		writePacket(channel, &(message->buffer[1]), message->size - 1);
		// Free the message
		Message_free(message);
	} while (1);
}






void ReceiverTask(void)
{
	uint8_t receiveCount = 0;
	uint8_t checkSum = 0;
	uint8_t packetBuffer[MAX_PACKET_SIZE];
	do
	{
		uint8_t data = Uart0_receive();
		if (data == DELIM)
		{
			if (checkSum == 0 && receiveCount >= 2) // TODO
			{
				// handle packet
				uint8_t counter = (packetBuffer[0] >> 5);
				if (counter != inPacketCount)
				{
					// packet lost
					inPacketCount = counter;
				}
				uint8_t channel = packetBuffer[0] & 0x1F;
				if (channelReceivers[channel])
				{
					Message * message = Message_create(receiveCount - 2);
					memcpy(message->buffer, &(packetBuffer[1]), receiveCount - 2);
					Message_send(channelReceivers[channel], message);
				}
			}
			else
			{
				// Checksum error
			}
			receiveCount = 0;
			checkSum = 0;
		}
		if (data == ESC)
		{
			data = Uart0_receive();
		}
		packetBuffer[receiveCount++] = data;
		if (receiveCount == MAX_PACKET_SIZE)
		{
			// Buffer overflow
			receiveCount = 0;
			checkSum = 0;
		}
	} while (1);
}




