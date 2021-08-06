modded class LogoutMenu
{
	private TextWidget m_CombatLogTxt;
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("AntiCombatLog/scripts/gui/layouts/day_z_logout_dialog_mod.layout");
		
		m_LogoutTimeText = TextWidget.Cast( layoutRoot.FindAnyWidget("txtLogoutTime") );
		m_DescriptionText = TextWidget.Cast( layoutRoot.FindAnyWidget("txtDescription") );
		m_bLogoutNow = ButtonWidget.Cast( layoutRoot.FindAnyWidget("bLogoutNow") );
		m_bCancel = ButtonWidget.Cast( layoutRoot.FindAnyWidget("bCancel") );
		m_CombatLogTxt = TextWidget.Cast(layoutRoot.FindAnyWidget("CombatLogtxt"));
		
#ifdef PLATFORM_CONSOLE
		m_bCancel.Show( false );
		m_bLogoutNow.Show( false );
		
		layoutRoot.FindAnyWidget("toolbar_bg").Show( true );
		#ifdef PLATFORM_PS4
			string back = "circle";
			if( GetGame().GetInput().GetEnterButton() == GamepadButton.A )
			{
				back = "circle";
			}
			else
			{
				back = "cross";
			}
			ImageWidget toolbar_b = layoutRoot.FindAnyWidget( "BackIcon" );
			toolbar_b.LoadImageFile( 0, "set:playstation_buttons image:" + back );
		#endif
#else
		m_bCancel.Show( true );
		m_bLogoutNow.Show( true );
		layoutRoot.FindAnyWidget("toolbar_bg").Show( false );
#endif
		
		UpdateInfo();
		
		// player should sit down if possible
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious()) 
		{
			player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SITA);
			player.GetEmoteManager().GetEmoteLauncher().SetForced(EmoteLauncher.FORCE_DIFFERENT);
		}
		
		return layoutRoot;
	}
	
	
	
	
	override void SetTime(int time) 
	{
		
		m_iTime = time;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if(player.GetCombatLogger().hasFired())
		{
				m_CombatLogTxt.SetText(" You Are CombatLogging!!!");
		}	
		else
				m_LogoutTimeText.SetText(m_iTime.ToString() + " #layout_logout_dialog_until_logout");
	}
	
};