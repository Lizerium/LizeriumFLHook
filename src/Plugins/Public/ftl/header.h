/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 10 апреля 2026 12:33:16
 * Version: 1.0.25
 */

#ifndef __MAIN_H__
#define __MAIN_H__ 1

#include "binarytree.h"

#define ADDR_SRV_GETIOBJRW 0x20670 // 06D00670
#define SRV_ADDR(a) ((char*)hModServer + a)
typedef IObjRW * (__cdecl *_GetIObjRW)(uint iShip);
extern _GetIObjRW GetIObjRW;

#define ADDR_RMCLIENT_LAUNCH 0x5B40
#define ADDR_RMCLIENT_CLIENT 0x43D74

string scUserFile;

struct UINT_WRAP
{
	UINT_WRAP(uint u) { val = u; }
	bool operator==(UINT_WRAP uw) { return uw.val == val; }
	bool operator>=(UINT_WRAP uw) { return uw.val >= val; }
	bool operator<=(UINT_WRAP uw) { return uw.val <= val; }
	bool operator>(UINT_WRAP uw) { return uw.val > val; }
	bool operator<(UINT_WRAP uw) { return uw.val < val; }
	uint val;
};

struct LAUNCH_PACKET
{
	uint iShip;
	uint iDunno[2];
	float fRotate[4];
	float fPos[3];
};

struct FTL_DATA
{
	bool aFTL;
	mstime iFTL;
	bool isFirstLaunch;
	bool bHasFTLFuel;
	int countCurrentFuel;
	int countMaxFuel;
	bool bHasSystem;
	bool bHasSystemSFTL;
	bool bHasAdmin;
	bool Msg;
	bool MsgFTL;
	wstring wscDeniedMsg;
};

Quaternion HkMatrixToQuaternion(Matrix m);

int GetMaxCountBatteryToJump();
HK_ERROR HkInitFTLFuel(uint iClientID);
HK_ERROR HkInitBadSystem(uint iClientID, uint iDockObj);
HK_ERROR HkInitBadSystemSFTL(uint iClientID);
HK_ERROR HkInstantDock(wstring wscCharname, uint iDockObj);
HK_ERROR HkBeamInSys(wstring wscCharname, Vector vOffsetVector, Matrix mOrientation);
void FTLMsgPlayers();
void ClearFTLData(uint iClientID);

void __stdcall HkLightFuse(IObjRW *ship, uint iFuseID, float fDelay, float fLifetime, float fSkip);
void __stdcall HkUnLightFuse(IObjRW *ship, uint iFuseID, float fDelay);

BinaryTree<UINT_WRAP> *set_btBattleShipArchIDs = new BinaryTree<UINT_WRAP>();
BinaryTree<UINT_WRAP> *set_btFreighterShipArchIDs = new BinaryTree<UINT_WRAP>();
BinaryTree<UINT_WRAP> *set_btFighterShipArchIDs = new BinaryTree<UINT_WRAP>();

//FTL
int set_FTLTimer;
list<INISECTIONVALUE> lstFTLFuel;
list<INISECTIONVALUE> lstFTLTimer;
list<INISECTIONVALUE> lstBadDocs;
list<INISECTIONVALUE> lstBadSftl;
string sFtlFX;
_GetIObjRW GetIObjRW;
FTL_DATA ftl[250];

struct MARK_INFO
{
	bool bMarkEverything;
	bool bIgnoreGroupMark;
	float fAutoMarkRadius;
	vector<uint> vMarkedObjs;
	vector<uint> vDelayedSystemMarkedObjs;
	vector<uint> vAutoMarkedObjs;
	vector<uint> vDelayedAutoMarkedObjs;
};

PLUGIN_RETURNCODE returncode;

#endif