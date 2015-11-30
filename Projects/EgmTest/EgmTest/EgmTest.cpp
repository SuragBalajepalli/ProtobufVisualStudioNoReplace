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
// Create a sensor pose message
void CreateSensorPoseMessage(EgmSensor* pSensorMessage,double pose[])
{
    EgmHeader* header = new EgmHeader();
    header->set_mtype(EgmHeader_MessageType_MSGTYPE_CORRECTION);
    header->set_seqno(sequenceNumber++);
    header->set_tm(GetTickCount());

    pSensorMessage->set_allocated_header(header);

    EgmCartesian *pc = new EgmCartesian();
    pc->set_x(pose[0]);
    pc->set_y(pose[1]);
    pc->set_z(pose[2]);

    EgmQuaternion *pq = new EgmQuaternion();
    pq->set_u0(pose[3]);
    pq->set_u1(pose[4]);
    pq->set_u2(pose[5]);
    pq->set_u3(pose[6]);

    EgmPose *pcartesian = new EgmPose();
    pcartesian->set_allocated_orient(pq);
    pcartesian->set_allocated_pos(pc);

    EgmPlanned *planned = new EgmPlanned();
    planned->set_allocated_cartesian(pcartesian);

    pSensorMessage->set_allocated_planned(planned);
}

//Create a sensor joints message
void CreateSensorJointsMessage(EgmSensor* pSensorMessage,double joints[])
{
    int size = 6;
	//cout<<size<<endl;
    size=size>6? 6:size;
	//cout<<size<<endl;
    EgmHeader* header = new EgmHeader();
    header->set_mtype(EgmHeader_MessageType_MSGTYPE_CORRECTION);
    header->set_seqno(sequenceNumber++);
    header->set_tm(GetTickCount());

    pSensorMessage->set_allocated_header(header);

    EgmJoints* pj = new EgmJoints();
    for(int i =0; i<size; i++)
    {
         pj->add_joints(joints[i]);
		 //cout<<joints[i]<<endl;
    }
    EgmPlanned *planned = new EgmPlanned();
    planned->set_allocated_joints(pj);

    pSensorMessage->set_allocated_planned(planned);
}

//Create a sensor speedref pos message
void CreateSensorSpeedPoseMessage(EgmSensor* pSensorMessage,double speedpos[])
{
    int size = 6;
	//cout<<size<<endl;
    size=size>6? 6:size;
	//cout<<size<<endl;
    EgmHeader* header = new EgmHeader();
    header->set_mtype(EgmHeader_MessageType_MSGTYPE_CORRECTION);
    header->set_seqno(sequenceNumber++);
    header->set_tm(GetTickCount());

    pSensorMessage->set_allocated_header(header);

    EgmCartesianSpeed* cs = new EgmCartesianSpeed();
    for(int i =0; i<size; i++)
    {
		cs->add_value(speedpos[i]);
    }
    EgmSpeedRef *speedref = new EgmSpeedRef();
	speedref->set_allocated_cartesians(cs);

	pSensorMessage->set_allocated_speedref(speedref);
}

//Create a sensor speedref pos message
void CreateSensorSpeedPose2Message(EgmSensor* pSensorMessage,double speedpos[],double pose[])
{
    int size = 6;
	//cout<<size<<endl;
    size=size>6? 6:size;
	//cout<<size<<endl;
    EgmHeader* header = new EgmHeader();
    header->set_mtype(EgmHeader_MessageType_MSGTYPE_CORRECTION);
    header->set_seqno(sequenceNumber++);
    header->set_tm(GetTickCount());

    pSensorMessage->set_allocated_header(header);

    EgmCartesianSpeed* cs = new EgmCartesianSpeed();
	//cout<<"speed:";
    for(int i =0; i<size; i++)
    {
		cs->add_value(speedpos[i]);
		//cout<<cs->value(i)<<" ";
    }
	//cout<<endl;
    EgmSpeedRef *speedref = new EgmSpeedRef();
	speedref->set_allocated_cartesians(cs);

	pSensorMessage->set_allocated_speedref(speedref);

    EgmCartesian *pc = new EgmCartesian();
    pc->set_x(pose[0]);
    pc->set_y(pose[1]);
    pc->set_z(pose[2]);
	//cout<<"xyz:"<<pc->x()<<" "<<pc->y()<<" "<<pc->z()<<endl;

    EgmQuaternion *pq = new EgmQuaternion();
    pq->set_u0(pose[3]);
    pq->set_u1(pose[4]);
    pq->set_u2(pose[5]);
    pq->set_u3(pose[6]);

    EgmPose *pcartesian = new EgmPose();
    pcartesian->set_allocated_orient(pq);
    pcartesian->set_allocated_pos(pc);

    EgmPlanned *planned = new EgmPlanned();
    planned->set_allocated_cartesian(pcartesian);

    pSensorMessage->set_allocated_planned(planned);
}

//Create a sensor speedref joints message
void CreateSensorSpeedJointsMessage(EgmSensor* pSensorMessage,double speedjoints[])
{
    int size = 6;
	//cout<<size<<endl;
    size=size>6? 6:size;
	//cout<<size<<endl;
    EgmHeader* header = new EgmHeader();
    header->set_mtype(EgmHeader_MessageType_MSGTYPE_CORRECTION);
    header->set_seqno(sequenceNumber++);
    header->set_tm(GetTickCount());

    pSensorMessage->set_allocated_header(header);

    EgmJoints* pj = new EgmJoints();
    for(int i =0; i<size; i++)
    {
         pj->add_joints(speedjoints[i]);
    };
    EgmSpeedRef *speedref = new EgmSpeedRef();
	speedref->set_allocated_joints(pj);

	pSensorMessage->set_allocated_speedref(speedref);
}

//Send a sensor message
int SendSensorMessage(SOCKET sockfd, const sockaddr* clientAddr,int len, EgmSensor* pSensorMessage,ofstream &outfile)
{
	string messageBuffer;
	int n;
	 pSensorMessage->SerializeToString(&messageBuffer);

    // send a message to the robot
    n = sendto(sockfd, messageBuffer.c_str(), messageBuffer.length(), 0, clientAddr, len);
    if (n < 0)
    {
        printf("Error send message\n");
		outfile<<"Error send message\n";
    }
	return n;
}

//////////////////////////////////////////////////////////////////////////
// Display inbound robot message
void DisplayRobotMessage(EgmRobot *pRobotMessage)
{
    if (pRobotMessage->has_header() && pRobotMessage->header().has_seqno() && pRobotMessage->header().has_tm() && pRobotMessage->header().has_mtype())
    {
        printf("SeqNo=%d Tm=%u Type=%d\n", pRobotMessage->header().seqno(), pRobotMessage->header().tm(), pRobotMessage->header().mtype());
		//Print Feedback
		if(pRobotMessage->has_feedback())
		{
			//Print Feedback Joints
			if(pRobotMessage->feedback().has_joints())
			{
				for(int i=0;i<pRobotMessage->feedback().joints().joints_size();i++)
					printf("feedback joints%d:%f\n",i+1,pRobotMessage->feedback().joints().joints(i));
			}
			//Print Feedback Pose
			if(pRobotMessage->feedback().has_cartesian())
			{
				printf("feedback cartesian pose:(%f, %f, %f)\n",pRobotMessage->feedback().cartesian().pos().x(),pRobotMessage->feedback().cartesian().pos().y(),pRobotMessage->feedback().cartesian().pos().z());
			}		
		}
		//Print Planned
		if(pRobotMessage->has_planned())
		{
			//Print Planned Joints
			if(pRobotMessage->planned().has_joints())
			{
				for(int i=0;i<pRobotMessage->planned().joints().joints_size();i++)
					printf("planned joints%d:%f\n",i+1,pRobotMessage->planned().joints().joints(i));
			}
			//Print Planned Pose
			if(pRobotMessage->planned().has_cartesian())
			{
				printf("planned cartesian pose:(%f, %f, %f)\n",pRobotMessage->planned().cartesian().pos().x(),pRobotMessage->planned().cartesian().pos().y(),pRobotMessage->planned().cartesian().pos().z());
                printf("planned cartesian quaternion:(%f, %f, %f, %f)\n",pRobotMessage->planned().cartesian().orient().u0(),pRobotMessage->planned().cartesian().orient().u1(),pRobotMessage->planned().cartesian().orient().u2(),pRobotMessage->planned().cartesian().orient().u3());
			}		
		}
		//Print Motor State
		if(pRobotMessage->has_motorstate())
		{
			printf("Motor State:%d\n",pRobotMessage->motorstate().state());
		}
		//Print MCI State
		if(pRobotMessage->has_mcistate())
		{
			printf("MCI State:%d\n",pRobotMessage->mcistate().state());
		}
		//Print MCI Convergence Met
		if(pRobotMessage->has_mciconvergencemet())
		{
			printf("MCI_Convergencement State:%d\n",pRobotMessage->mciconvergencemet());
		}
    }
    else
    {
        printf("No header\n");
    }
	 printf("\n");
}

void PrintVelocity(EgmRobot *pRobotMessage, EgmRobot *last_message,ofstream& outfile,double* speed_pose)
{
	double vel[3]={0,0,0};
	if(last_message->has_feedback()&&last_message->feedback().has_cartesian())
	{
		unsigned int temp1=pRobotMessage->header().tm();
		unsigned int temp2=last_message->header().tm();
		double temp_time=(pRobotMessage->header().tm()-last_message->header().tm())/1000.0;
		cout<<temp_time<<endl;
		//outfile<<temp_time<<" ";
		outfile<<temp1<<" "<<temp_time<<" ";
		if(temp_time!=0)
		{
			vel[0]=pRobotMessage->feedback().cartesian().pos().x()-last_message->feedback().cartesian().pos().x();
			vel[1]=pRobotMessage->feedback().cartesian().pos().y()-last_message->feedback().cartesian().pos().y();
			vel[2]=pRobotMessage->feedback().cartesian().pos().z()-last_message->feedback().cartesian().pos().z();
			vel[0]=vel[0]/temp_time;
			vel[1]=vel[1]/temp_time;
			vel[2]=vel[2]/temp_time;
			cout<<"velocity:"<<vel[0]<<" "<<vel[1]<<" "<<vel[2]<<endl;
			outfile<<vel[0]<<" "<<vel[1]<<" "<<vel[2]<<" "<<speed_pose[0]<<" "<<speed_pose[1]<<" "<<speed_pose[2]<<endl;
		}
	}
}

//Recieve and display a robot message
int RecieveRobotMessage(SOCKET sockfd, sockaddr* clientAddr,int* len,EgmRobot* last_message,ofstream& outfile,double* speed_pose)
{
	int n;
	char protoMessage[1400];
    n = recvfrom(sockfd, protoMessage, 1400, 0, clientAddr, len);
    if (n < 0)
    {
        printf("Error receive message\n");
		outfile<<"Error receive message\n";
		return n;
	}
    // deserialize inbound message
    EgmRobot *pRobotMessage = new EgmRobot();
    pRobotMessage->ParseFromArray(protoMessage, n);
	PrintVelocity(pRobotMessage,last_message,outfile,speed_pose);
	*last_message=*pRobotMessage;
    delete pRobotMessage;
	return n;
}

int _tmain(int argc, _TCHAR* argv[])
{
    SOCKET sockfd;
    struct sockaddr_in serverAddr, clientAddr;
	double pose[7]={0,0,10,1.0,0,0,0};
	double joints[6]={1,2,3,4,5,6};
	double speed_pose[6]={0,0,0,0,0,0};
	ofstream outfile("velocity.txt");
	EgmRobot last_message;
	//cout<<sizeof(joints)<<endl;
	int flag=0;
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
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(portNumber);
	int len = sizeof(clientAddr);

    // listen on all interfaces
    bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    //for (int messages = 0; messages < 100; messages++)
	while(1)
    {
        //receive and display message from robot
		flag=RecieveRobotMessage(sockfd,(struct sockaddr *)&clientAddr,&len, &last_message,outfile,speed_pose);
		if (flag<0)
        continue;
		/*int len,n;
		char protoMessage[1400];
		len = sizeof(clientAddr);
		n = recvfrom(sockfd, protoMessage, 1400, 0, (struct sockaddr *)&clientAddr, &len);
		if (n < 0)
		{
			printf("Error receive message\n");
			continue;
		}
		EgmRobot *pRobotMessage = new EgmRobot();
		pRobotMessage->ParseFromArray(protoMessage, n);
		DisplayRobotMessage(pRobotMessage);
		delete pRobotMessage;*/
      
        // create and send a sensor message
		//if(messages==0)
		{
			EgmSensor *pSensorMessage = new EgmSensor();
			//CreateSensorPoseMessage(pSensorMessage,pose);
			//CreateSensorJointsMessage(pSensorMessage,joints);
			//CreateSensorSpeedJointsMessage(pSensorMessage,joints);
			//CreateSensorSpeedPoseMessage(pSensorMessage,speed_pose);
			CreateSensorSpeedPose2Message(pSensorMessage,speed_pose,pose);
			flag=SendSensorMessage(sockfd,(struct sockaddr *)&clientAddr,len,pSensorMessage,outfile);
			delete pSensorMessage;
		}
		speed_pose[2]+=1;
		/*for (int i=0;i<3;i++)
		{
			pose[i]=pose[i]+1;
		}*/
		//Sleep(10);
	}
	while(1){}
    return 0;
}



