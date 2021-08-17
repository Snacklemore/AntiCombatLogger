
modded class MissionServer
{
	
	//Player Logouts happen in here 
	//OnClientDisconnectEvent gets called as soon the "Leave" button was pressed or the gameprocess is killed 
	//OnClientDisconnectEvent  returns after adding that player to a logoutQueue and logs him out in UpdateLogOutPlayers
	//if it is a instant logout  PlayerDisconnected gets  called  immedieatly (only when player dead or no connection to backend(??))
	//ALT+F4  or  killing the process also queues the player in the logoutqueue, calling PlayerDisconnected in  UpdateLogoutPlayers
	//(Could just delay the logout by x minutes when combatLogging, you probably cant reconnect then)
	//LogoutCancelEvent occurs only in OnEvent.
	
	
	//note: the Identity is most likely destroyed already when PlayerDisconncted is called, the UID is always available	
	//note: suicide does not trigger the combattimer
	
	//known issue: logging when player dead, timer running and disconnecting(false positive)
	//			  
	
	
	ref ConfigTransmit m_ConfigRPC_ACL;
	int COMBAT_MODE_TIMER ;
	float COMBAT_FLAG_DISTANCE ;
	void MissionServer()
	{
		Init();
		
	}
	void Init()
	{
		GetWebhook();
		m_ConfigRPC_ACL  =  new ConfigTransmit("RPCACL","OnConfigTransmitRPCACL");
		Print("InitServerMission");
		
	}
	
	override void OnInit()
	{
		super.OnInit();
		
	}
	override void OnClientDisconnectedEvent(PlayerIdentity identity, PlayerBase player, int logoutTime, bool authFailed)
	{
		super.OnClientDisconnectedEvent(identity,player,logoutTime,authFailed);
		if (GetHive() && !authFailed)
		{			
			if (player.IsAlive())
			{	
				if (!m_LogoutPlayers.Contains(player) && !m_NewLogoutPlayers.Contains(player))
				{
					
					
					
					
					
					if (player.GetCombatLogger().hasFired())
					{
						Print("[ACL]::OnClientDisconnectedEvent: Setting CombatLog  to true ");
						//could save the name here, and access in PlayerDisconnected to get the name(which is deleted in  PlayerDisconnected  most likely)
						
						player.m_isCombatLogged  =  true;
						player.GetCombatLogger().m_name = identity.GetName();
					}
					
				}
				return;
			}		
		}
		
		
	}
	
	
	
	
	
	
	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		//Breaks VPPAdminTools!!!
		// Note: At this point, identity can be already deleted
		super.PlayerDisconnected(player,identity,uid);
		
		
		
		
		if (player.IsAlive())
		{
			if (player.GetCombatLogger().hasFired())
					{
						Print("[ACL]:PlayerDisconnected: Setting CombatLog  to true ");
						player.m_isCombatLogged  =  true;
					}
					
			if  (player.m_isCombatLogged)
					{
						//restAPI call to discord webhook
					
						Print("[ACL]:PlayerDisconnected: Calling to Webhook!!");
						GetWebhook().Send("CombatLog for UID: "  + uid+ "  Name: "+  player.GetCombatLogger().m_name,"currentDate");
						
						if (identity)
							Print("[ACL]:PlayerDisconnected: COMBATLOG FOR  Playername: "+ identity.GetName());
						else
							{
							Print("[ACL]:PlayerDisconnected: Identity already deleted!");
							Print("CombatLoggerBackup::Name:  "+ player.GetCombatLogger().m_name);
							}
						
						
						
						Print("[ACL]:PlayerDisconnected: COMBATLOG FOR PlayerID: "+ uid);
					}
		}
		
	}
	
	
	override void OnEvent(EventType eventTypeId, Param params) 
	{
		super.OnEvent(eventTypeId,params);
		PlayerIdentity identity;
		PlayerBase player;
		switch(eventTypeId)
		{
		
			case LogoutCancelEventTypeID:
				LogoutCancelEventParams logoutCancelParams;
			
				Class.CastTo(logoutCancelParams, params);				
				Class.CastTo(player, logoutCancelParams.param1);
				//player canceled logout
				Print("[ACL]::OnEvent: Canceled Logout, m_isCombatLogged  = false");
				player.m_isCombatLogged = false;
			
				identity = player.GetIdentity();
				if (identity)
				{
				// disable reconnecting to old char
				// GetGame().RemoveFromReconnectCache(identity.GetId());
				Print("[Logout]: Player " + identity.GetId() + " cancelled"); 
				}
				else
				{
				Print("[Logout]: Player cancelled"); 
				}
				m_LogoutPlayers.Remove(player);
				m_NewLogoutPlayers.Remove(player);
				break;
		}
	}
};