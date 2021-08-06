modded class DayZPlayerImplement{

	
	override void HandleWeapons(float pDt, Entity pInHands, HumanInputController pInputs, out bool pExitIronSights)
	{
		HumanCommandWeapons		hcw = GetCommandModifier_Weapons();
    	GetDayZPlayerInventory().HandleWeaponEvents(pDt, pExitIronSights);

		Weapon_Base weapon;
		Class.CastTo(weapon, pInHands);
		
		CheckLiftWeapon();
		ProcessLiftWeapon();

		// @TODO: refactor to action (rm from input, mk action)
		/*if (pInputs.IsReloadOrMechanismSingleUse() && GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT )
		{
			if (weapon && weapon.CanProcessWeaponEvents())
			{
				if (GetWeaponManager().CanEjectBullet(weapon))
				{
					GetWeaponManager().EjectBullet();
					pExitIronSights = true;
				}
			}
		}*/
		
		GetMovementState(m_MovementState);
		
		//Print("IsInIronsights " + IsInIronsights());
		//Print("IsInOptics " + IsInOptics());
		// hold breath
		if (pInputs.IsHoldBreath() && m_MovementState.IsRaised() && (IsInIronsights() || IsInOptics()))
		{
			m_IsTryingHoldBreath = true;
		}
		else
		{
			m_IsTryingHoldBreath = false;
		}
		
		ItemOptics optic = weapon.GetAttachedOptics();

		if (pInputs.IsFireModeChange())
		{
			GetWeaponManager().SetNextMuzzleMode();
		}
		if (pInputs.IsZeroingUp())
		{
			if (optic && (optic.IsInOptics() || optic.IsUsingWeaponIronsightsOverride()) )
				optic.StepZeroingUp();
			else
			{
				//weapon.StepZeroingUp(weapon.GetCurrentMuzzle());
				weapon.StepZeroingUpAllMuzzles();
			}
		}
		if (pInputs.IsZeroingDown())
		{
			if (optic && (optic.IsInOptics() || optic.IsUsingWeaponIronsightsOverride()) )
				optic.StepZeroingDown();
			else
			{
				//weapon.StepZeroingDown(weapon.GetCurrentMuzzle());
				weapon.StepZeroingDownAllMuzzles();
			}
		}
		
		if (!m_LiftWeapon_player && (m_CameraIronsight || !weapon.CanEnterIronsights() || m_CameraOptics/*m_ForceHandleOptics*/)) 	// HACK straight to optics, if ironsights not allowed
		{
			if (optic)
				HandleOptic(optic, false, pInputs, pExitIronSights);
		}

		/*if (!m_LiftWeapon_player && weapon && weapon.CanProcessWeaponEvents())
		{
			if (pInputs.IsReloadOrMechanismContinuousUseStart())
			{
				if (GetWeaponManager().CanUnjam(weapon))
				{
					//weapon.ProcessWeaponEvent(new WeaponEventUnjam(this));
					GetWeaponManager().Unjam();
					//pExitIronSights = true;
				}
			}
		}*/

		if (!m_MovementState.IsRaised())
		{
			m_IsFireWeaponRaised = false;
			if (weapon && weapon.IsInOptics())
				weapon.ExitOptics();
			
			StopADSTimer();

			return; // if not raised => return
		}
		else
		{
			m_IsFireWeaponRaised = true;
			if ( !m_WeaponRaiseCompleted && (!m_ADSAutomationTimer || (m_ADSAutomationTimer && !m_ADSAutomationTimer.IsRunning())) )
			{
				if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER || !GetGame().IsMultiplayer())
					RunADSTimer();
			}
			if (m_ProcessWeaponRaiseCompleted)
			{
				CompleteWeaponRaise();
				m_ProcessWeaponRaiseCompleted = false;
			}
		}
		
		#ifdef PLATFORM_CONSOLE
			if ( !pInputs.IsMeleeFastAttackModifier() )
			{
		#endif
		#ifdef SERVER_FOR_CONSOLE
			if ( !pInputs.IsMeleeFastAttackModifier() )
			{
		#endif
		
		//! fire
		//if (!m_LiftWeapon_player && weapon && !weapon.IsDamageDestroyed() && weapon.CanProcessWeaponEvents() )
		if (GetWeaponManager().CanFire(weapon))
		{
			bool autofire = weapon.GetCurrentModeAutoFire(weapon.GetCurrentMuzzle()) && weapon.IsChamberEjectable(weapon.GetCurrentMuzzle());
			int burst = weapon.GetCurrentModeBurstSize(weapon.GetCurrentMuzzle());
			int burst_count = weapon.GetBurstCount();
			if (!autofire && (burst < 2 || burst_count < 1) )
			{
				if (pInputs.IsUseButtonDown())
				{
					////hook in here,AntiCombatSystem
					GetWeaponManager().Fire(weapon);
					
						
								PlayerBase player = PlayerBase.Cast(this);
								player.GetCombatLogger().setFirePos(player.GetPosition());
								player.GetCombatLogger().hasFiredStartLogging();
						
					
					
					
							
				}
			}
			else if (autofire || burst > 1)
			{
#ifdef DEVELOPER
				int burst_option = GetWeaponManager().GetBurstOption();
				if ( burst_option == 0)
				{
#endif
					if (pInputs.IsUseButton())
					{
						if ( autofire || burst_count < burst )
						{
							GetWeaponManager().Fire(weapon);
						}
					}
					else
					{
						weapon.ResetBurstCount();	
					}
#ifdef DEVELOPER
				}
				else if (burst_option == 1)
				{
					if ( burst > 1 && burst_count == burst )
					{
						weapon.ResetBurstCount();	
					}
					else if (burst > 1 && burst_count < burst)
					{
						GetWeaponManager().Fire(weapon);	
					}
					else
					{
						//Autofire
						if (pInputs.IsUseButton())
						{
							GetWeaponManager().Fire(weapon);	
						}						
					}	
				}
#endif
			}
		}

		#ifdef PLATFORM_CONSOLE
		if ( GetGame().GetInput().LocalRelease( "UAFire", false ) || m_ShouldReload )
		{
			if ( !weapon.IsWaitingForActionFinish() && !IsFighting() )
			{
				int muzzle_index = weapon.GetCurrentMuzzle();
			
				if ( weapon.IsChamberFiredOut( muzzle_index ) )
				{
					if ( weapon.CanProcessWeaponEvents() )
					{
						if ( GetWeaponManager().CanEjectBullet(weapon) )
						{
							GetWeaponManager().EjectBullet();
							pExitIronSights = true;
							m_ShouldReload = false;
						}
					}
				}
			}
			else
			{
				m_ShouldReload = true;
			}
		}
		#endif
		#ifdef PLATFORM_CONSOLE
			}
		#endif
		#ifdef SERVER_FOR_CONSOLE
			}
		#endif
	}

};