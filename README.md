# DayZ AntiCombatLogger

Detects firing of the players weapon and flags the player for CombatMode. Nearby players in a configurable radius will also get flagged. Time to be flagged is also configurable.
Logging out in any way ( e.g. ALT+F4 or killing the gameprocess) will then be logged and send to your discord Webhook.


# Setting up

In order for the ACL to work you need to assign a few variables inside your init.c on your server.
```java
void main()
{
	[...]
	ENDPOINT_URL   = "yourdiscordhook";
	GetWebhook().UpdateURL();
}



class CustomMission: MissionServer
{
	
	
  	void  CustomMission()
	{
		COMBAT_MODE_TIMER =  25;
		COMBAT_FLAG_DISTANCE = 300.0;
		
		gCOMBAT_MODE_TIMER =  25;
		gCOMBAT_FLAG_DISTANCE = 300.0;
	}

 	override void OnClientReadyEvent(PlayerIdentity identity, PlayerBase player)
	{
		super.OnClientReadyEvent(identity,player);
		GetRPCManager().SendRPC("RPCACL","OnConfigTransmitRPCACL",new Param2<int,float>(COMBAT_MODE_TIMER,COMBAT_FLAG_DISTANCE),false,identity);

	}
```


