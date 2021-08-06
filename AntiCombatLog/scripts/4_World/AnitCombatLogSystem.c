
class AntiCombatLogger 
{
	
	bool 						m_hasFired = false;
	vector 						m_firingPos;
	PlayerBase					m_PlayerBase;
	ref Timer 					m_CombatModeTimer;
	ref array<PlayerBase>		m_PlayerList;
	int  						m_CombatLogTime;
	float 						m_distance;
	string 						m_name;
	bool  						m_isClient;
	
	
	void init(){
		//timer needs to start when shot from player was fired
		m_CombatModeTimer = new Timer();
		m_PlayerList = new array<PlayerBase>;
		
		
		
		
	}
	
	
	bool hasFired()
	{
		return m_hasFired;
	}
	void AntiCombatLogger(PlayerBase player, int  CombatLogTime, float Distance,  bool  isClient){
		
		init();
		m_PlayerBase = player;
		m_CombatLogTime =  CombatLogTime;
		m_distance  = Distance;
		m_isClient  =  isClient;
		player.RegisterNetSyncVariableBool("m_isCombatLogged");
	
	}
	void SetPlayerBase(PlayerBase ident)
	{
		m_PlayerBase = ident;
		
	}
	void setFirePos(vector pos)
	{
		m_firingPos = pos;
	}
	void FlagVicinityPlayers()
	{
		if (m_PlayerList.Count() >  0 )
		{
			for (int  i  = 0;i<m_PlayerList.Count();i++)
			{
				PlayerBase b_player = m_PlayerList.Get(i);
				Print("ACL::FlagVicinityPlayers: Flagging player  in vicinity: "+ m_PlayerBase.GetIdentity().GetName() );
				
				//Check if timer  running  on that Player 
				if (b_player.GetCombatLogger().m_CombatModeTimer.IsRunning())
				{
					//if timer  is running stop  and Restart  
					b_player.GetCombatLogger().m_CombatModeTimer.Stop();
					b_player.GetCombatLogger().m_CombatModeTimer.Run(m_CombatLogTime,this,"TimerRunOut");
					continue;
				}
				b_player.GetCombatLogger().m_hasFired  = true;
				b_player.GetCombatLogger().m_CombatModeTimer.Run(m_CombatLogTime,this,"TimerRunOut");
			
			}
			m_PlayerList.Clear();
			
		}	
	
	}
	void hasFiredStartLogging()
	{
		//check if timer is already running
		m_hasFired = true;
		if(m_CombatModeTimer.IsRunning())
		{
			//if  timer is   running dont execute again  just restart
			Print("ACL::hasFiredStartLogging: Timer running, restarting for  "+ m_PlayerBase.GetIdentity().GetName() );
			m_CombatModeTimer.Stop();
			m_CombatModeTimer.Run(m_CombatLogTime,this,"TimerRunOut");
			return;
		}
		
		m_CombatModeTimer.Run(m_CombatLogTime,this,"TimerRunOut");
		Print("ACL::hasFiredStartLogging:  Timer started!  for  "+ m_PlayerBase.GetIdentity().GetName());
		
		
		array<Man> players = new array<Man>;
		
		//Note: List always empty on client
		GetGame().GetPlayers(players);
		for  (int  i  = 0 ;i<  players.Count();i++)
		{
			Man pMan = players.Get(i);
			PlayerBase b_player = PlayerBase.Cast(pMan);
			//get distance 
			float distance = vector.Distance(b_player.GetPosition(),m_firingPos);
			Print("ACL::hasFiredStartLogging: Distance to firing Pos : " + distance);
			if (distance < m_distance)
			{
				
				if  (b_player)
				{
					if (b_player.GetIdentity() != m_PlayerBase.GetIdentity())
					{
						//players to start combat timer on
						Print("ACL::hasFiredStartLogging: Added player: " +  b_player.GetIdentity().GetName());
						m_PlayerList.Insert(b_player);

					}
					else 
					{
						Print("ACL::hasFiredStartLogging:  Thats the calling player!");

					}
					
				}
			}
			FlagVicinityPlayers();
				
					
		
	
	
		}
	}
		
	
	
	void TimerRunOut()
	{
		Print("Timer Stopped");
		m_hasFired = false;
		
	}
	
	
};
