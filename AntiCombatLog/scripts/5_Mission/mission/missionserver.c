
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
	override void OnClientDisconnectedEvent(PlayerIdentity identity, PlayerBase player, int logoutTime, bool authFailed)
	{
		bool disconnectNow = true;
		
		// TODO: get out of vehicle
		// using database and no saving if authorization failed
		if (GetHive() && !authFailed)
		{			
			if (player.IsAlive())
			{	
				if (!m_LogoutPlayers.Contains(player) && !m_NewLogoutPlayers.Contains(player))
				{
					
					
					Print("[Logout]: New player " + identity.GetId() + " with logout time " + logoutTime.ToString());
					
					
					if (player.GetCombatLogger().hasFired())
					{
						Print("[ACL]::OnClientDisconnectedEvent: Setting CombatLog  to true ");
						//could save the name here, and access in PlayerDisconnected to get the name(which is deleted in  PlayerDisconnected  most likely)
						
						player.m_isCombatLogged  =  true;
						player.GetCombatLogger().m_name = identity.GetName();
					}
					
				
			
					// inform client about logout time
					GetGame().SendLogoutTime(player, logoutTime);
			
					// wait for some time before logout and save
					LogoutInfo params = new LogoutInfo(GetGame().GetTime() + logoutTime * 1000, identity.GetId());
					
					m_NewLogoutPlayers.Insert(player, params);
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AddNewPlayerLogout, 0, false, player, params);
					
					// allow reconnecting to old char only if not in cars, od ladders etc. as they cannot be properly synchronized for reconnect
					//if (!player.GetCommand_Vehicle() && !player.GetCommand_Ladder())
					//{
					//	GetGame().AddToReconnectCache(identity);
					//}
					// wait until logout timer runs out
					disconnectNow = false;
				}
				return;
			}		
		}
		
		
		//appearently  this only executes when player dead, or smthing went wrong badly
		if (disconnectNow)
		{
			Print("[Logout]: New player " + identity.GetId() + " with instant logout");
			
			
			
			// inform client about instant logout
			GetGame().SendLogoutTime(player, 0);
			
			PlayerDisconnected(player, identity, identity.GetId());
		}
	}
	
	
	
	
	
	
	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		// Note: At this point, identity can be already deleted
		if (!player)
		{
			Print("[Logout]: Skipping player " + uid + ", already removed");
			//set variable  in MissionServer  to save  the combatLog state 
			//in case the PlayerBase is destroyed too soon
			return;
		}
		
		
		
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
		
			
		
		// disable reconnecting to old char
		//GetGame().RemoveFromReconnectCache(uid);
		
		// now player can't cancel logout anymore, so call everything needed upon disconnect
		InvokeOnDisconnect(player);

		Print("[Logout]: Player " + uid + " finished");

		if (GetHive())
		{
			// save player
			player.Save();
			
			// unlock player in DB	
			GetHive().CharacterExit(player);		
		}
		
		// handle player's existing char in the world
		player.ReleaseNetworkControls();
		HandleBody(player);
		
		// remove player from server
		GetGame().DisconnectPlayer(identity, uid);
		// Send list of players at all clients
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SyncEvents.SendPlayerList, 1000);
	}
	
	
	override void OnEvent(EventType eventTypeId, Param params) 
	{
		PlayerIdentity identity;
		PlayerBase player;
		int counter = 0;
		
		switch(eventTypeId)
		{
		case ClientPrepareEventTypeID:
			ClientPrepareEventParams clientPrepareParams;
			Class.CastTo(clientPrepareParams, params);
			
			OnClientPrepareEvent(clientPrepareParams.param1, clientPrepareParams.param2, clientPrepareParams.param3, clientPrepareParams.param4, clientPrepareParams.param5);
			break;

		case ClientNewEventTypeID:
			ClientNewEventParams newParams;
			Class.CastTo(newParams, params);
			player = OnClientNewEvent(newParams.param1, newParams.param2, newParams.param3);
			if (!player)
			{
				Debug.Log("ClientNewEvent: Player is empty");
				return;
			}
			identity = newParams.param1;
			InvokeOnConnect(player,identity );
			SyncEvents.SendPlayerList();
			ControlPersonalLight(player);
			SyncGlobalLighting(player);
			break;
			
		case ClientReadyEventTypeID:
			ClientReadyEventParams readyParams;
			Class.CastTo(readyParams, params);
			identity = readyParams.param1;
			Class.CastTo(player, readyParams.param2);
			if (!player)
			{
				Debug.Log("ClientReadyEvent: Player is empty");
				return;
			}
			
			OnClientReadyEvent(identity, player);
			InvokeOnConnect(player, identity);
			// Send list of players at all clients
			SyncEvents.SendPlayerList();
			ControlPersonalLight(player);
			SyncGlobalLighting(player);
			break;
					
		case ClientRespawnEventTypeID:
			ClientRespawnEventParams respawnParams;
			Class.CastTo(respawnParams, params);
			identity = respawnParams.param1;
			Class.CastTo(player, respawnParams.param2);
			if (!player)
			{
				Debug.Log("ClientRespawnEvent: Player is empty");
				return;
			}
			
			OnClientRespawnEvent(identity, player);
			break;
			
		case ClientReconnectEventTypeID:
			ClientReconnectEventParams reconnectParams;
			Class.CastTo(reconnectParams, params);
			
			identity = reconnectParams.param1;
			Class.CastTo(player, reconnectParams.param2);
			if (!player)
			{
				Debug.Log("ClientReconnectEvent: Player is empty");
				return;
			}
			
			OnClientReconnectEvent(identity, player);
			break;
		
		case ClientDisconnectedEventTypeID:
			ClientDisconnectedEventParams discoParams;
			Class.CastTo(discoParams, params);		
			
			identity = discoParams.param1;
			Class.CastTo(player, discoParams.param2);			
			int logoutTime = discoParams.param3;
			bool authFailed = discoParams.param4;

			if (!player)
			{
				Debug.Log("ClientDisconnectenEvent: Player is empty");
				return;
			}
			
			OnClientDisconnectedEvent(identity, player, logoutTime, authFailed);
			break;
			
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