# DayZ AntiCombatLogger

Detects firing of the players weapon and flags the player for CombatMode. Nearby players in a configurable radius will also get flagged. Time to be flagged is also configurable.
Logging out in any way ( e.g. ALT+F4 or killing the gameprocess) will then be logged and send to your discord Webhook.


# Setting up

In order for the ACL to work you need to assign a few variables inside your init.c on your server.

`void main()
{
	[...]
	ENDPOINT_URL   = "https://discord.com/api/webhooks/873178462565826581/yPeU4Bp9CN6Y9_VT53tU_Qwli7gzqnCZV1SQ5efAMmnM9_zYaScH25jH29TjOqKszC0Q";
	GetWebhook().UpdateURL();
}`


`class CustomMission: MissionServer
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

	}	`


StackEdit stores your files in your browser, which means all your files are automatically saved locally and are accessible **offline!**

## Create files and folders

The file explorer is accessible using the button in left corner of the navigation bar. You can create a new file by clicking the **New file** button in the file explorer.
You can also create folders by clicking the **New folder** button.
