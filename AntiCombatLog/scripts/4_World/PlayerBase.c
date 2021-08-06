int  COMBAT_MODE_TIMER =  300;
float COMBAT_FLAG_DISTANCE = 300 ;
modded class PlayerBase // modded keyword for modding existing class
{
	
	ref AntiCombatLogger		m_combatlogger;
	bool 				 		m_isCombatLogged = false;
	override void Init()
	{
		super.Init();
		if (GetGame().IsServer())
		{
			COMBAT_MODE_TIMER =  300;
			COMBAT_FLAG_DISTANCE = 300 ;
			m_combatlogger = new AntiCombatLogger(this,COMBAT_MODE_TIMER,COMBAT_FLAG_DISTANCE,false);
			
			Print("PlayerBase::Init::Server: COMBAT_MODE_TIMER:" + COMBAT_MODE_TIMER);
			Print("PlayerBase::Init::Server: COMBAT_FLAG_DISTANCE:" + COMBAT_FLAG_DISTANCE);
		}
		else
		{
			
			m_combatlogger  = new  AntiCombatLogger(this,COMBAT_MODE_TIMER,COMBAT_FLAG_DISTANCE,true);
			Print("PlayerBase::Init::Client: COMBAT_MODE_TIMER:" + COMBAT_MODE_TIMER);
			Print("PlayerBase::Init::Client: COMBAT_FLAG_DISTANCE:" + COMBAT_FLAG_DISTANCE);
		}
		
		Print("Init" +this);
		
		
		

		
		
	}
	AntiCombatLogger GetCombatLogger()
	{
		return m_combatlogger;
	}	
	
	
	//only called on server
	override void OnDisconnect() // overriding existing function
	{
		
		
	 	Print("Player Disconnected!");
		
			
		
			
		//Call OnDisconnect()
		super.OnDisconnect(); 
	}
	
	
};