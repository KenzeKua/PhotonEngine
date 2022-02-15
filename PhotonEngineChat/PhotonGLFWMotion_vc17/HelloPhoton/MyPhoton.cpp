#include "MyPhoton.h"

#include <iostream>
#include <string>

// Kenze's ID: 9d4ccadb-bb50-416b-a840-482ee37f78e8
// Victor's ID: ca1729c5-d695-4c91-b708-963bcf94fca6
// Chan Zhe's ID: 88eab4cf-e716-4fd5-a3cf-109ca086f23f

static const ExitGames::Common::JString appId = L"9d4ccadb-bb50-416b-a840-482ee37f78e8"; // Set your app id here
static const ExitGames::Common::JString appVersion = L"1.0";
static const ExitGames::Common::JString PLAYER_NAME = L"Kemze";

std::string keyLock = "69";
int intKeyLock = std::stoi(keyLock);

//LoadBalancing::Listener& listener, const Common::JString& applicationID, 
//const Common::JString& appVersion, nByte connectionProtocol=Photon::ConnectionProtocol::DEFAULT, bool autoLobbyStats=false, nByte regionSelectionMode=RegionSelectionMode::DEFAULT

// functions
MyPhoton::MyPhoton() : mLoadBalancingClient(*this, appId, appVersion, ExitGames::Photon::ConnectionProtocol::TCP, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT)
{
}

void MyPhoton::connect(void)
{
	std::wcout << "Connecting..." << std::endl;
	mLoadBalancingClient.connect(ExitGames::LoadBalancing::AuthenticationValues(), PLAYER_NAME);
}

void MyPhoton::disconnect(void)
{
	std::wcout << "Disconnecting..." << std::endl;
	mLoadBalancingClient.disconnect();
}

void MyPhoton::sendMyFloatData(void)
{
	float floatData[4] { 0 };
	floatData[0] = rand() % 9 + 1.0;
	floatData[1] = rand() % 9 + 1.0;
	floatData[2] = rand() % 9 + 1.0;
	floatData[3] = rand() % 9 + 1.0;

	std::cout << "Your UID: ";
	for (int i = 0; i < 4; i++)
	{
		std::cout << floatData[i];
	}
	std::cout << "\n\n";

	mLoadBalancingClient.opRaiseEvent(true, floatData, 4, 1);
}
void MyPhoton::sendMyCharData(void)
{
	char charData[] = "Hello this is Kenze";

	ExitGames::Common::JString convertData(charData);

	mLoadBalancingClient.opRaiseEvent(true, convertData, 2);
}
void MyPhoton::sendMyStringData(std::string stringData)
{
	std::string encryptMessage = stringData;
	for (int i = 0; i < encryptMessage.length(); i++)
	{
		encryptMessage[i] ^= intKeyLock;
	}

	ExitGames::Common::JString convertData(encryptMessage.c_str());

	mLoadBalancingClient.opRaiseEvent(true, convertData, 3);
}
void MyPhoton::sendMyByteData(const char* cstring, int stringSize)
{
	//std::cout << "Sending size: " << stringSize << std::endl;
	const unsigned char* bytes = (const unsigned char*)cstring;
	mLoadBalancingClient.opRaiseEvent(true, bytes, stringSize, 4);
}

void MyPhoton::run(void)
{
	mLoadBalancingClient.service();
}

// Protocol implementations

void MyPhoton::debugReturn(int debugLevel, const ExitGames::Common::JString& string)
{
}

void MyPhoton::connectionErrorReturn(int errorCode)
{
	std::wcout << "Connection error return: " << errorCode << std::endl;
}

void MyPhoton::clientErrorReturn(int errorCode)
{
	std::wcout << "Client error return: " << errorCode << std::endl;
}

void MyPhoton::warningReturn(int warningCode)
{
	std::wcout << "Warning return: " << warningCode << std::endl;
}

void MyPhoton::serverErrorReturn(int errorCode)
{
	std::wcout << "Server error return: " << errorCode << std::endl;
}

void MyPhoton::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player)
{
	std::wcout << "Penguin " << playerNr << ", " << player.getName() << " has joined the club\n\n";

	ExitGames::Common::JString NameStore = player.getName();
	PlayerNames[playerNr - 1] = NameStore.UTF8Representation();
	std::cout << "Player: " << playerNr << ", Name: " << player.getName().UTF8Representation().cstr() << "\n\n";
	if (playernrs.getSize() >= 2)
	{
		sendMyFloatData();
		sendMyCharData();
	}
}

void MyPhoton::leaveRoomEventAction(int playerNr, bool isInactive)
{
	std::wcout << "Penguin " << playerNr << " has left the club\n\n";
}

void MyPhoton::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{
	if (eventCode == 1)
	{
		float* floatData = ExitGames::Common::ValueObject<float*>(eventContent).getDataCopy();
		if (floatData)
		{
			std::wcout << "Penguin " << playerNr << "'s UID: ";
			for (int i = 0; i < 4; i++)
			{
				std::wcout << floatData[i];
			}
			std::wcout << std::endl;
		}
		else
		{
			std::cout << "Invalid float data\n\n";
		}
	}
	else if (eventCode == 2)
	{
		ExitGames::Common::JString charData = ExitGames::Common::ValueObject<ExitGames::Common::JString>(eventContent).getDataCopy();
		if (charData)
		{
			std::wcout << "Penguin " << playerNr << "'s intro: " << charData << std::endl;
		}
		else
		{
			std::wcout << "Invalid char data\n\n";
		}
	}
	else if (eventCode == 3)
	{
		ExitGames::Common::JString stringData = ExitGames::Common::ValueObject<ExitGames::Common::JString>(eventContent).getDataCopy();
		std::string decryptMessage = stringData.UTF8Representation();
		//std::string decryptMessage = stringData.ANSIRepresentation();
		if (stringData)
		{
			for (int i = 0; i < decryptMessage.length(); i++)
			{
				decryptMessage[i] ^= intKeyLock;
			}

			std::cout << "Penguin " << playerNr << ": " << decryptMessage << std::endl;
		}
		else
		{
			std::wcout << "Invalid string data\n\n";
		}
	}
	else if (eventCode == 4)
	{
		unsigned char* data = ExitGames::Common::ValueObject<unsigned char*>(eventContent).getDataCopy();
		const int* byteNum = ExitGames::Common::ValueObject<unsigned char*>(eventContent).getSizes();

		if (data)
		{
			//std::cout << "size : " << byteNum[0] << std::endl;

			char* string = (char*)data;
			//std::cout << "Before decryption, Player " << playerNr << ": " << string << std::endl;

			for (int i = 0; i < byteNum[0]; i++)
			{
				data[i] = data[i] ^ charCipherKey;
			}
			data[byteNum[0]] = '\0';

			// Can't use wcout
			std::cout << "Penguin " << playerNr << ": " << data << std::endl;
		}
		else
		{
			std::wcout << "Invalid byte data\n\n";
		}
	}
}

void MyPhoton::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& cluster)
{
	if(errorCode)
		std::wcout << "Failed to connect: " << errorString.cstr() << std::endl;
	else
	{
		std::wcout << "Connected to Photon Server" << std::endl;
		
		std::wcout << "Trying to join a random club..." << std::endl;
		mLoadBalancingClient.opJoinRandomRoom();
	}
}

void MyPhoton::disconnectReturn(void)
{
	std::wcout << "Disconnected" << std::endl;
}

void MyPhoton::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	std::wcout << "Create room return" << std::endl;
}

void MyPhoton::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	std::wcout << "Join or create room return" << std::endl;
}

void MyPhoton::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	std::wcout << "Join room return" << std::endl;
}

void MyPhoton::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	std::wcout << "Join random room return" << std::endl;

	if(errorCode)
	{
		std::wcout << "Failed to join a random club: " << errorCode<< ", " << errorString.cstr()<<std::endl;

		if(errorCode == 32760) //no match found error code
		{
			std::wcout << "Creating a club..." << std::endl;
			// Try to create a room and wait;
			ExitGames::Common::JString name;
			name += GETTIMEMS();
			mLoadBalancingClient.opCreateRoom(name, ExitGames::LoadBalancing::RoomOptions().setMaxPlayers(20));
		}

		return;
	}
}

void MyPhoton::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	std::wcout << "Leave room return" << std::endl;
}

void MyPhoton::joinLobbyReturn(void)
{
	std::wcout << "Join lobby return" << std::endl;
}

void MyPhoton::leaveLobbyReturn(void)
{
	std::wcout << "Leave lobby return" << std::endl;
}

void MyPhoton::onLobbyStatsResponse(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStatsResponse>& lobbyStats)
{
	std::wcout << "On lobby stats response" << std::endl;
}

void MyPhoton::onLobbyStatsUpdate(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStatsResponse>& lobbyStats)
{
	std::wcout << "On lobby stats update" << std::endl;
}

void MyPhoton::onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegions, const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegionServers)
{
	//std::wcout<<availableRegions[0]<<std::endl;
	//std::wcout<<availableRegions[1]<<std::endl;
	//mLoadBalancingClient.selectRegion(availableRegions[0]);
	mLoadBalancingClient.selectRegion("asia");
}
