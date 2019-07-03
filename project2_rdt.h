//
// Created by Garrett Hagen on 2019-07-02.
//

#ifndef COE_1652_PROJECT_2_PROJECT2_RDT_H
#define COE_1652_PROJECT_2_PROJECT2_RDT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
struct msg {
	char data[20];
};

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct pkt {
	int seqnum;
	int acknum;
	int checksum;
	char payload[20];
};

/* Stores the sender state and packet */
struct sender
{
	int wait_above_enabled;
	struct pkt last_pkt_sent;
} A;


/* Stores the reciever state and packet */
struct reciever
{
	struct pkt last_pkt_acked;
} B;


/* Function headers */
void tolayer5(int AorB, char datasent[20]);
void tolayer3(int AorB, struct pkt packet);
void starttimer(int AorB, float increment);
void stoptimer(int AorB);
int  not(int AorB);
int  calculate_checksum(struct pkt packet);






#endif //COE_1652_PROJECT_2_PROJECT2_RDT_H
