class CfgPatches
{
 class AntiCombatLog
 {
  requiredAddons[]=
  {
    // ""
  };
 }
}
class CfgMods
{
 class AntiCombatLog
 {
  type = "mod";
  dependencies[]=
		{
			"Game",
			"World",
			"Mission",
			"gui"
		};
  
  class defs
  {
   class worldScriptModule
			   {
				value = "";
				files[] = {"AntiCombatLog/scripts/4_World"};

			   }
   class missionScriptModule
			{
				value  = "";
				files[]=
				{
					"AntiCombatLog/scripts/5_Mission"
				};
			};
	class gameScriptModule
			{
				value  = "";
				files[]=
				{
					"AntiCombatLog/scripts/3_Game"
				};
			};
   
  }
  
 }
 
}