#include "project2_stop_wait.h"

/* ******************************************************************
 ALTERNATING BIT NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for Project 2, unidirectional and bidirectional
   data transfer protocols.  Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/



/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	// Currently in the waiting state so we can send a new packet
	if (A.wait_above_enabled)
	{

		// Create the outgoing packet
		struct pkt new_packet;
		new_packet.seqnum = not(A.last_pkt_sent.seqnum);
		memcpy(new_packet.payload, message.data, 20);
		new_packet.checksum = calculate_checksum(new_packet);

		sprintf(debug_str,"A_output() Seq: %d] --> Sending new packet to B\n",new_packet.seqnum);
		debug_print(debug_str);

		// Set the state to waiting for ACK and start timer
		A.wait_above_enabled = 0;
		A.last_pkt_sent = new_packet;
		starttimer(0, RTT_INCREMENT);

		// Send the packet
		tolayer3(0, new_packet);

	}
	else
	{
		debug_print("A_output()] --> ACK not yet recieved from B. Ignoring new message\n");
	}
}

/* NOTE: Not in use due to unidirectional transfer */
void B_output(struct msg message){ }

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	// If we are waiting for a call from above
	// this is a duplicate ACK so ignore
	if (A.wait_above_enabled)
		return;

	// If the ACK matches and the checksum is valid,
	// set the state to waiting and stop the timer
	if (packet.checksum == calculate_checksum(packet) &&
		A.last_pkt_sent.seqnum == packet.acknum)
	{

		sprintf(debug_str,"A_input() Ack: %d] --> Recieved valid ACK from B\n",packet.acknum);
		debug_print(debug_str);

		A.wait_above_enabled = 1;
		stoptimer(0);

		successful_acks++;

	}
	else
	{
		
		sprintf(debug_str,"A_input() Ack: %d] --> ACK from B is corrupted/invalid. Ignoring \n",packet.acknum);
		debug_print(debug_str);
	}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	// If we are waiting the timer
	// should not be enabled, but
	// use this as a safe guard
	if(A.wait_above_enabled)
		return;

	sprintf(debug_str,"A_timerintterupt()] --> Resending packet %d from A\n",A.last_pkt_sent.seqnum);
	debug_print(debug_str);

	// Resend last packet and start timer
	starttimer(0,RTT_INCREMENT);
	tolayer3(0,A.last_pkt_sent);



}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	A.wait_above_enabled = 1;
	A.last_pkt_sent.seqnum = 1; // Allows first packet sent to be seq = 0;
}


/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
	// If the seq matches the opposite of the prev ACK
	// and the checksum is valid forward the packet to layer 5,
	// and return the ACK
	if (packet.checksum == calculate_checksum(packet)
		&& packet.seqnum == not(B.last_pkt_acked.acknum))
	{

			// Deliver data
			tolayer5(1,packet.payload);

			sprintf(debug_str,"B_input() Seq: %d] --> B received valid packet from A. Sending Ack: %d\n",packet.seqnum,packet.seqnum);
			debug_print(debug_str);

			// Create return ACK packet
			struct pkt ack_pkt;
			ack_pkt.acknum = packet.seqnum;
			ack_pkt.checksum = calculate_checksum(ack_pkt);

			// Update the last ACKed packet and send it
			B.last_pkt_acked = ack_pkt;
			tolayer3(1,ack_pkt);




	}
	// Resend the previous ACK packet as a NACK
	else
	{
		sprintf(debug_str,"B_input() Seq: %d] --> B received corrupted/unordered packet from A. Resending Ack: %d\n",packet.seqnum,B.last_pkt_acked.acknum);
		debug_print(debug_str);

		tolayer3(1,B.last_pkt_acked);

	}

}

/* NOTE: Not in use. Called when B's timer goes off */
void B_timerinterrupt()
{
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	B.last_pkt_acked.acknum = 1; // Allows us to start the process at seq = 0
}

/* Calculate the checksum from the packet contents */
int calculate_checksum(struct pkt packet)
{
	int check_sum = packet.acknum + packet.seqnum;

	for (size_t i = 0; i < 20; i++)
		check_sum += (int)packet.payload[i];

	return check_sum;
}

/* Helper function to emulate the ! operator */
int not(int num)
{
	if (num == 0)
		return 1;
	else
		return 0;
}

/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
  - emulates the tranmission and delivery (possibly with bit-level corruption
    and packet loss) of packets across the layer 3/4 interface
  - handles the starting/stopping of a timer, and generates timer
    interrupts (resulting in calling students timer handler).
  - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOULD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you definitely should not have to modify
******************************************************************/

struct event
{
	float evtime;           /* event time */
	int evtype;             /* event type code */
	int eventity;           /* entity where event occurs */
	struct pkt *pktptr;     /* ptr to packet (if any) assoc w/ this event */
	struct event *prev;
	struct event *next;
};
struct event *evlist = NULL;   /* the event list */

/* possible events: */
#define  TIMER_INTERRUPT 0
#define  FROM_LAYER5     1
#define  FROM_LAYER3     2

#define  OFF    0
#define  ON     1
#define  A      0
#define  B      1

int TRACE = 1;             /* for my debugging */
int nsim = 0;              /* number of messages from 5 to 4 so far */
int nsimmax;         /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob;            /* probability that a packet is dropped  */
float corruptprob;         /* probability that one bit is packet is flipped */
float lambda;              /* arrival rate of messages from layer 5 */
int ntolayer3;           /* number sent into layer 3 */
int nlost;               /* number lost in media */
int ncorrupt;            /* number corrupted by media*/

void init();
void generate_next_arrival();
void tolayer5(int AorB, char datasent[20]);
void tolayer3(int AorB, struct pkt packet);
void starttimer(int AorB, float increment);
void stoptimer(int AorB);
void insertevent(struct event *p);
void init();
float jimsrand();
void printevlist();

int main()
{
	struct event *eventptr;
	struct msg msg2give;
	struct pkt pkt2give;

	int i, j;
	char c;

	init();
	A_init();
	B_init();

	while (1)
	{
		eventptr = evlist;            /* get next event to simulate */
		if (eventptr == NULL)
			goto terminate;
		evlist = evlist->next;        /* remove this event from event list */
		if (evlist != NULL)
			evlist->prev = NULL;
		if (TRACE >= 2)
		{
			printf("\nEVENT time: %f,", eventptr->evtime);
			printf("  type: %d", eventptr->evtype);
			if (eventptr->evtype == 0)
				printf(", timerinterrupt  ");
			else if (eventptr->evtype == 1)
				printf(", fromlayer5 ");
			else
				printf(", fromlayer3 ");
			printf(" entity: %d\n", eventptr->eventity);
		}
		time = eventptr->evtime;        /* update time to next event time */
		if (nsim == nsimmax)
			break;                        /* all done with simulation */
		if (eventptr->evtype == FROM_LAYER5)
		{
			generate_next_arrival();   /* set up future arrival */
			/* fill in msg to give with string of same letter */
			j = nsim % 26;
			for (i = 0; i < 20; i++)
				msg2give.data[i] = 97 + j;
			if (TRACE > 2)
			{
				printf("          MAINLOOP: data given to student: ");
				for (i = 0; i < 20; i++)
					printf("%c", msg2give.data[i]);
				printf("\n");
			}
			nsim++;
			if (eventptr->eventity == A)
				A_output(msg2give);
			else
				B_output(msg2give);
		}
		else if (eventptr->evtype == FROM_LAYER3)
		{
			pkt2give.seqnum = eventptr->pktptr->seqnum;
			pkt2give.acknum = eventptr->pktptr->acknum;
			pkt2give.checksum = eventptr->pktptr->checksum;
			for (i = 0; i < 20; i++)
				pkt2give.payload[i] = eventptr->pktptr->payload[i];
			if (eventptr->eventity == A)      /* deliver packet by calling */
				A_input(pkt2give);            /* appropriate entity */
			else
				B_input(pkt2give);
			free(eventptr->pktptr);          /* free the memory for packet */
		}
		else if (eventptr->evtype == TIMER_INTERRUPT)
		{
			if (eventptr->eventity == A)
				A_timerinterrupt();
			else
				B_timerinterrupt();
		}
		else
		{
			printf("INTERNAL PANIC: unknown event type \n");
		}
		free(eventptr);
	}

	terminate:
	printf(" Simulator terminated at time %f\n after sending %d msgs from layer5\n", time, nsim);
	printf(" %d successful ACKs were recieved from the reciever\n",successful_acks);
	return 0;
}

void init()                         /* initialize the simulator */
{
	int i;
	float sum, avg;
	float jimsrand();

	printf("-----  Network Simulator Version 1.1 -------- \n\n");
	printf("Enter the number of messages to simulate: ");
	scanf("%d", &nsimmax);
	printf("Enter  packet loss probability [enter 0.0 for no loss]:");
	scanf("%f", &lossprob);
	printf("Enter packet corruption probability [0.0 for no corruption]:");
	scanf("%f", &corruptprob);
	printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
	scanf("%f", &lambda);
	printf("Enter TRACE:");
	scanf("%d", &TRACE);

	srand(9999);              /* init random number generator */
	sum = 0.0;                /* test random number generator for students */
	for (i = 0; i < 1000; i++)
		sum = sum + jimsrand();    /* jimsrand() should be uniform in [0,1] */
	avg = sum / 1000.0;
	if (avg < 0.25 || avg > 0.75)
	{
		printf("It is likely that random number generation on your machine\n");
		printf("is different from what this emulator expects.  Please take\n");
		printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
		exit(1);
	}

	ntolayer3 = 0;
	nlost = 0;
	ncorrupt = 0;

	time = 0.0;                    /* initialize time to 0.0 */
	generate_next_arrival();     /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand()
{
	double mmm = RAND_MAX;   /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
	float x;                   /* individual students may need to change mmm */
	x = rand() / mmm;            /* x should be uniform in [0,1] */
	return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

void generate_next_arrival()
{
	double x;
	struct event *evptr;
	float ttime;
	int tempint;

	if (TRACE > 2)
		printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

	x = lambda * jimsrand() * 2;  /* x is uniform on [0,2*lambda] */
	/* having mean of lambda        */
	evptr = (struct event *) malloc(sizeof(struct event));
	evptr->evtime = time + x;
	evptr->evtype = FROM_LAYER5;
	if (BIDIRECTIONAL && (jimsrand() > 0.5))
		evptr->eventity = B;
	else
		evptr->eventity = A;
	insertevent(evptr);
}

void insertevent(struct event *p)
{
	struct event *q, *qold;

	if (TRACE > 2)
	{
		printf("            INSERTEVENT: time is %lf\n", time);
		printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
	}
	q = evlist;     /* q points to header of list in which p struct inserted */
	if (q == NULL)
	{   /* list is empty */
		evlist = p;
		p->next = NULL;
		p->prev = NULL;
	}
	else
	{
		for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
			qold = q;
		if (q == NULL)
		{   /* end of list */
			qold->next = p;
			p->prev = qold;
			p->next = NULL;
		}
		else if (q == evlist)
		{ /* front of list */
			p->next = evlist;
			p->prev = NULL;
			p->next->prev = p;
			evlist = p;
		}
		else
		{     /* middle of list */
			p->next = q;
			p->prev = q->prev;
			q->prev->next = p;
			q->prev = p;
		}
	}
}

void printevlist()
{
	struct event *q;
	int i;
	printf("--------------\nEvent List Follows:\n");
	for (q = evlist; q != NULL; q = q->next)
	{
		printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype, q->eventity);
	}
	printf("--------------\n");
}



/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stoptimer(int AorB)
{
	struct event *q, *qold;

	if (TRACE > 2)
		printf("          STOP TIMER: stopping timer at %f\n", time);
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
	for (q = evlist; q != NULL; q = q->next)
		if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
		{
			/* remove this event */
			if (q->next == NULL && q->prev == NULL)
				evlist = NULL;         /* remove first and only event on list */
			else if (q->next == NULL) /* end of list - there is one in front */
				q->prev->next = NULL;
			else if (q == evlist)
			{ /* front of list - there must be event after */
				q->next->prev = NULL;
				evlist = q->next;
			}
			else
			{     /* middle of list */
				q->next->prev = q->prev;
				q->prev->next = q->next;
			}
			free(q);
			return;
		}
	printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

void starttimer(int AorB, float increment)
{

	struct event *q;
	struct event *evptr;

	if (TRACE > 2)
		printf("          START TIMER: starting timer at %f\n", time);
	/* be nice: check to see if timer is already started, if so, then  warn */
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
	for (q = evlist; q != NULL; q = q->next)
		if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
		{
			printf("Warning: attempt to start a timer that is already started\n");
			return;
		}

/* create future event for when timer goes off */
	evptr = (struct event *) malloc(sizeof(struct event));
	evptr->evtime = time + increment;
	evptr->evtype = TIMER_INTERRUPT;
	evptr->eventity = AorB;
	insertevent(evptr);
}

/************************** TOLAYER3 ***************/
void tolayer3(int AorB, struct pkt packet)
{
	struct pkt *mypktptr;
	struct event *evptr, *q;
	float lastime, x;
	int i;

	ntolayer3++;

	/* simulate losses: */
	if (jimsrand() < lossprob)
	{
		nlost++;
		if (TRACE > 0)
			printf("          TOLAYER3: packet being lost\n");
		return;
	}

/* make a copy of the packet student just gave me since he/she may decide */
/* to do something with the packet after we return back to him/her */
	mypktptr = (struct pkt *) malloc(sizeof(struct pkt));
	mypktptr->seqnum = packet.seqnum;
	mypktptr->acknum = packet.acknum;
	mypktptr->checksum = packet.checksum;
	for (i = 0; i < 20; i++)
		mypktptr->payload[i] = packet.payload[i];
	if (TRACE > 2)
	{
		printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
		       mypktptr->acknum, mypktptr->checksum);
		for (i = 0; i < 20; i++)
			printf("%c", mypktptr->payload[i]);
		printf("\n");
	}

/* create future event for arrival of packet at the other side */
	evptr = (struct event *) malloc(sizeof(struct event));
	evptr->evtype = FROM_LAYER3;   /* packet will pop out from layer3 */
	evptr->eventity = (AorB + 1) % 2; /* event occurs at other entity */
	evptr->pktptr = mypktptr;       /* save ptr to my copy of packet */
/* finally, compute the arrival time of packet at the other end.
   medium can not reorder, so make sure packet arrives between 1 and 10
   time units after the latest arrival time of packets
   currently in the medium on their way to the destination */
	lastime = time;
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
	for (q = evlist; q != NULL; q = q->next)
		if ((q->evtype == FROM_LAYER3 && q->eventity == evptr->eventity))
			lastime = q->evtime;
	evptr->evtime = lastime + 1 + 9 * jimsrand();



	/* simulate corruption: */
	if (jimsrand() < corruptprob)
	{
		ncorrupt++;
		if ((x = jimsrand()) < .75)
			mypktptr->payload[0] = 'Z';   /* corrupt payload */
		else if (x < .875)
			mypktptr->seqnum = 999999;
		else
			mypktptr->acknum = 999999;
		if (TRACE > 0)
			printf("          TOLAYER3: packet being corrupted\n");
	}

	if (TRACE > 2)
		printf("          TOLAYER3: scheduling arrival on other side\n");
	insertevent(evptr);
}

void tolayer5(int AorB, char datasent[20])
{
	int i;
	if (TRACE > 2)
	{
		printf("          TOLAYER5: data received: ");
		for (i = 0; i < 20; i++)
			printf("%c", datasent[i]);
		printf("\n");
	}
}


void debug_print(const char* message) {
	#if DEBUG
		printf("[%f  ",time);
		printf("%s",message);
	#endif
}
		
