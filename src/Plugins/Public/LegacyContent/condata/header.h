/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 23 апреля 2026 06:54:28
 * Version: 1.0.471
 */


#define LOSS_INTERVALL 4000

struct CONNECTION_DATA
{
// connection data	
	list<uint>	lstLoss;
	uint		iLastLoss;
	uint		iAverageLoss;
	list<uint>	lstPing;
	uint		iAveragePing;
	uint		iPingFluctuation;
	uint		iLastPacketsSent; 
    uint		iLastPacketsReceived; 
    uint		iLastPacketsDropped;
	uint		iLags;
	list<uint>	lstObjUpdateIntervalls;
	mstime		tmLastObjUpdate;
	mstime		tmLastObjTimestamp;

// exception
	bool		bException;
	string		sExceptionReason;
};

uint			set_iPingKickFrame;
uint			set_iPingKick;
uint			set_iFluctKick;
uint			set_iLossKickFrame;
uint			set_iLossKick;
uint			set_iLagDetectionFrame;
uint			set_iLagDetectionMinimum;
uint			set_iLagKick;

// Kick high lag and loss players only if the server load 
// exceeds this threshold.
uint			set_iKickThreshold;