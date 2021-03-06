/////////////////////////////////////////////////////////////////////////
// Sample using Google protocol buffers C++
//
#include "stdafx.h"
#include <WinSock2.h>
#include <iostream>
#include <fstream>

#include "egm.pb.h" // generated by Google protoc.exe

#pragma comment(lib, "Ws2_32.lib")      // socket lib
#pragma comment(lib, "libprotobuf.lib") // protobuf lib

static int portNumber = 6510;
static unsigned int sequenceNumber = 0;

using namespace std;
using namespace abb::egm;


// Protobuf-C++ is supported by Google and no other third party libraries needed for the protobuf part. 
// It can be a bit tricky to build the Google tools in Windows but here is a guide on how to build 
// protobuf for Windows (http://eli.thegreenplace.net/2011/03/04/building-protobuf-examples-on-windows-with-msvc).
//
// When you have built libprotobuf.lib and protoc.exe:
//	 Run Google protoc to generate access classes, protoc --cpp_out=. egm.proto
//	 Create a win32 console application
//	 Add protobuf source as include directory
//	 Add the generated egm.pb.cc to the project (exclude the file from precompile headers)
//	 Copy the file below
//	 Compile and run
//
//
// Copyright (c) 2014, ABB
// All rights reserved.
//
// Redistribution and use in source and binary forms, with
// or without modification, are permitted provided that 
// the following conditions are met:
//
//    * Redistributions of source code must retain the 
//      above copyright notice, this list of conditions 
//      and the following disclaimer.
//    * Redistributions in binary form must reproduce the 
//      above copyright notice, this list of conditions 
//      and the following disclaimer in the documentation 
//      and/or other materials provided with the 
//      distribution.
//    * Neither the name of ABB nor the names of its 
//      contributors may be used to endorse or promote 
//      products derived from this software without 
//      specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////////////////////
// Create a simple sensor message
void CreateRobotMessage(EgmRobot* pRobotMessage,float pose[])
{
    EgmHeader* header = new EgmHeader();
    header->set_mtype(EgmHeader_MessageType_MSGTYPE_DATA);
    header->set_seqno(sequenceNumber++);
    header->set_tm(GetTickCount());

    pRobotMessage->set_allocated_header(header);

    EgmCartesian *pc = new EgmCartesian();
    pc->set_x(pose[0]);
    pc->set_y(pose[1]);
    pc->set_z(pose[2]);

    EgmQuaternion *pq = new EgmQuaternion();
    pq->set_u0(1.0);
    pq->set_u1(0.0);
    pq->set_u2(0.0);
    pq->set_u3(0.0);

    EgmPose *pcartesian = new EgmPose();
    pcartesian->set_allocated_orient(pq);
    pcartesian->set_allocated_pos(pc);

    EgmFeedBack *feedback = new EgmFeedBack();
    feedback->set_allocated_cartesian(pcartesian);

    pRobotMessage->set_allocated_feedback(feedback);
}

//Send a robot message
int SendRobotMessage(SOCKET sockfd, const sockaddr* serverAddr,int len, EgmRobot* pRobotMessage)
{
	string messageBuffer;
	int n;
	 pRobotMessage->SerializeToString(&messageBuffer);

    // send a message to the robot
    n = sendto(sockfd, messageBuffer.c_str(), messageBuffer.length(), 0, serverAddr, len);
    if (n < 0)
    {
        printf("Error send message\n");
    }
	return n;
}

//////////////////////////////////////////////////////////////////////////
// Display inbound sensor pose message
void DisplaySensorMessage(EgmSensor *pSensorMessage)
{
    if (pSensorMessage->has_header() && pSensorMessage->header().has_seqno() && pSensorMessage->header().has_tm() && pSensorMessage->header().has_mtype())
    {
        printf("SeqNo=%d Tm=%u Type=%d\n", pSensorMessage->header().seqno(), pSensorMessage->header().tm(), pSensorMessage->header().mtype());
		if(pSensorMessage->has_planned())
		{
			//Print Planned Joints
			if(pSensorMessage->planned().has_joints())
			{
				for(int i=0;i<pSensorMessage->planned().joints().joints_size();i++)
					printf("Planned joints%d: %f\n",i+1,pSensorMessage->planned().joints().joints(i));
			}
			//Print Planned Pose
			if(pSensorMessage->planned().has_cartesian())
			{
				printf("cartesian pose:(%f, %f, %f)\n",pSensorMessage->planned().cartesian().pos().x(),pSensorMessage->planned().cartesian().pos().y(),pSensorMessage->planned().cartesian().pos().z());
				printf("cartesian quaternion:(%f, %f, %f, %f)\n",pSensorMessage->planned().cartesian().orient().u0(),pSensorMessage->planned().cartesian().orient().u1(),pSensorMessage->planned().cartesian().orient().u2(),pSensorMessage->planned().cartesian().orient().u3());
			}		
		}
		if(pSensorMessage->has_speedref())
		{
			if(pSensorMessage->speedref().has_joints())
			{
				for(int i=0;i<pSensorMessage->speedref().joints().joints_size();i++)
				printf("SpeedRef joints%d: %f\n",i+1,pSensorMessage->speedref().joints().joints(i));
			}
			if(pSensorMessage->speedref().has_cartesians())
			{
				printf("SpeedRef Cartesians ( %f, ",pSensorMessage->speedref().cartesians().value(0));
				for(int i=1;i<5;i++)
				{
					printf("%f, ",pSensorMessage->speedref().cartesians().value(i));
				}
				printf("%f )\n",pSensorMessage->speedref().cartesians().value(5));
			}
		}
	}
    else
    {
        printf("No header\n");
    }
}

//Recieve a sensor message
int RecieveSensorMessage(SOCKET sockfd,sockaddr* serverAddr,int* len)
{
	int n;
	char protoMessage[1400];
    n = recvfrom(sockfd, protoMessage, 1400, 0, serverAddr, len);
    if (n < 0)
    {
        printf("Error receive message\n");
		return n;
    }
    // deserialize inbound message
    EgmSensor *pSensorMessage = new EgmSensor();
    pSensorMessage->ParseFromArray(protoMessage, n);
    DisplaySensorMessage(pSensorMessage);
    delete pSensorMessage;
	return n;
}

int _tmain(int argc, _TCHAR* argv[])
{
    SOCKET sockfd;
    int flag=0;
    struct sockaddr_in serverAddr, clientAddr;
	float pose[3]={1,0,0};

    /* Init winsock */
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        fprintf(stderr, "Could not open Windows connection.\n");
        exit(0);
    }

    // create socket to listen on
    sockfd = ::socket(AF_INET,SOCK_DGRAM,0);

    memset(&serverAddr, sizeof(serverAddr), 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.168.0.1");
    serverAddr.sin_port = htons(portNumber);
	int len =  sizeof(serverAddr);

    // listen on all interfaces
    //bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    //for (int messages = 0; messages < 100; messages++)
	while(1)
    {
        // create and send a Robot message
        EgmRobot *pRobotMessage = new EgmRobot();
        CreateRobotMessage(pRobotMessage,pose);
		/*string messageBuffer;
		int n;
		 pRobotMessage->SerializeToString(&messageBuffer);

		// send a message to the robot
		n = sendto(sockfd, messageBuffer.c_str(), messageBuffer.length(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
		if (n < 0)
		{
			printf("Error send message\n");
		}*/
		flag=SendRobotMessage(sockfd,(struct sockaddr *)&serverAddr,len,pRobotMessage);
		delete pRobotMessage;

		/*int n;
		char protoMessage[1400];
		n = recvfrom(sockfd, protoMessage, 1400, 0, (struct sockaddr *)&serverAddr, &len);
		if (n < 0)
		{
			printf("Error receive message\n");
		}
		// deserialize inbound message
		EgmSensor *pSensorMessage = new EgmSensor();
		pSensorMessage->ParseFromArray(protoMessage, n);
		DisplaySensorMessage(pSensorMessage);
		delete pSensorMessage;*/
      
		 // receive and display message from sensor
        flag=RecieveSensorMessage(sockfd,(struct sockaddr *)&serverAddr,&len);
		pose[0]++;
		Sleep(1000);
    }
	while(1){}
    return 0;
}


