int  gCOMBAT_MODE_TIMER ;
float gCOMBAT_FLAG_DISTANCE ; 
class ConfigTransmit 
{
	vector g_NameTagAnchor ;
	float  g_NameTagDistance;
	int  	COMBAT_MODE_TIMER;
	float	COMBAT_FLAG_DISTANCE;
	
	void ConfigTransmit(string namespace,string  funcName)
	{
		GetRPCManager().AddRPC(namespace, funcName, this, SingleplayerExecutionType.Both);
	}
	
	
	
	void OnConfigTransmitRPCACL(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param2<int,float> data;
		if ( !ctx.Read( data ) ) return;
        
        if( type == CallType.Server )
        {
            Print( "Server function called!" );
        }
        else
        {
			
			
			
			
				Print( "[ACL::RPC]::Client function called!" );
				int timer  = data.param1;
				float dist = data.param2;
			
				COMBAT_MODE_TIMER = timer;
				COMBAT_FLAG_DISTANCE =  dist;
				Print( "timer "+  timer );
				Print ("dist: " + dist);
			
            
        }
	}

};




modded class PlayerBase // modded keyword for modding existing class
{
	ref ConfigTransmit 			m_ConfigRPC;
	ref AntiCombatLogger		m_combatlogger;
	bool 				 		m_isCombatLogged = false;
	override void Init()
	{
		m_ConfigRPC = new ref ConfigTransmit("RPCACL","OnConfigTransmitRPCACL");
		
		
		
		super.Init();
		Print("Init" +this);
		
		
		

		
		
	}
	AntiCombatLogger GetCombatLogger()
	{
		return m_combatlogger;
	}	
	
	
	
	override void OnPlayerLoaded()
	{
		super.OnPlayerLoaded();
		if (GetGame().IsServer())
		{
			
			m_combatlogger = new AntiCombatLogger(this,gCOMBAT_MODE_TIMER,gCOMBAT_FLAG_DISTANCE,false);
			
			Print("PlayerBase::Init::Server: COMBAT_MODE_TIMER:" + gCOMBAT_MODE_TIMER);
			Print("PlayerBase::Init::Server: COMBAT_FLAG_DISTANCE:" + gCOMBAT_FLAG_DISTANCE);
		}
		else
		{
			
			m_combatlogger  = new  AntiCombatLogger(this,m_ConfigRPC.COMBAT_MODE_TIMER,m_ConfigRPC.COMBAT_FLAG_DISTANCE,true);
			
			Print("PlayerBase::Init::Client: COMBAT_MODE_TIMER:" + m_ConfigRPC.COMBAT_MODE_TIMER);
			Print("PlayerBase::Init::Client: COMBAT_FLAG_DISTANCE:" + m_ConfigRPC.COMBAT_FLAG_DISTANCE);
		}
	}
	
	
};