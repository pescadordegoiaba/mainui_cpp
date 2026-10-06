#include "Framework.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "Slider.h"
#include "CheckBox.h"

#define ART_BANNER "gfx/shell/head_advoptions"

class CMenuHudSettings : public CMenuFramework
{
public:
	CMenuHudSettings() : CMenuFramework( "CMenuHudSettings" ) { }

private:
	void _Init() override;
	void _VidInit() override;
	void GetConfig();
	void Reset();
	void SaveAndPopMenu() override;

	CMenuCheckBox showHealth, showArmor, showAmmo, showMoney;
	CMenuCheckBox showRadar, showTimer, showChat, showDeath;

	CMenuSlider healthX, healthY;
	CMenuSlider armorX, armorY;
	CMenuSlider ammoX, ammoY;
	CMenuSlider moneyX, moneyY;
	CMenuSlider radarX, radarY;
	CMenuSlider timerX, timerY;
	CMenuSlider chatX, chatY;
	CMenuSlider deathX, deathY;

	CMenuSlider hudR, hudG, hudB;
	CMenuSlider chatR, chatG, chatB;
	CMenuSlider chatTime;
};

static void Reg( const char *name, const char *value )
{
	EngFuncs::CvarRegister( name, value, FCVAR_ARCHIVE );
}

static void SetupOffset( CMenuSlider &s, const char *name, const char *status, int x, int y )
{
	s.SetNameAndStatus( L( name ), L( status ) );
	s.Setup( -400.0f, 400.0f, 5.0f );
	s.onChanged = CMenuEditable::WriteCvarCb;
	s.SetRect( x, y, 180, 26 );
}

static void SetupByte( CMenuSlider &s, const char *name, int x, int y )
{
	s.SetNameAndStatus( L( name ), L( "0 a 255" ) );
	s.Setup( 0.0f, 255.0f, 1.0f );
	s.onChanged = CMenuEditable::WriteCvarCb;
	s.SetRect( x, y, 180, 26 );
}

void CMenuHudSettings::GetConfig()
{
	showHealth.LinkCvar( "hud_show_health" );
	showArmor.LinkCvar( "hud_show_armor" );
	showAmmo.LinkCvar( "hud_show_ammo" );
	showMoney.LinkCvar( "hud_show_money" );
	showRadar.LinkCvar( "hud_show_radar" );
	showTimer.LinkCvar( "hud_show_timer" );
	showChat.LinkCvar( "hud_show_chat" );
	showDeath.LinkCvar( "hud_show_death" );

	healthX.LinkCvar( "hud_dx_health" );
	healthY.LinkCvar( "hud_dy_health" );
	armorX.LinkCvar( "hud_dx_armor" );
	armorY.LinkCvar( "hud_dy_armor" );
	ammoX.LinkCvar( "hud_dx_ammo" );
	ammoY.LinkCvar( "hud_dy_ammo" );
	moneyX.LinkCvar( "hud_dx_money" );
	moneyY.LinkCvar( "hud_dy_money" );
	radarX.LinkCvar( "hud_dx_radar" );
	radarY.LinkCvar( "hud_dy_radar" );
	timerX.LinkCvar( "hud_dx_timer" );
	timerY.LinkCvar( "hud_dy_timer" );
	chatX.LinkCvar( "hud_dx_chat" );
	chatY.LinkCvar( "hud_dy_chat" );
	deathX.LinkCvar( "hud_dx_death" );
	deathY.LinkCvar( "hud_dy_death" );

	hudR.LinkCvar( "hud_r" );
	hudG.LinkCvar( "hud_g" );
	hudB.LinkCvar( "hud_b" );
	chatR.LinkCvar( "hud_chat_r" );
	chatG.LinkCvar( "hud_chat_g" );
	chatB.LinkCvar( "hud_chat_b" );
	chatTime.LinkCvar( "hud_saytext_time" );
}

void CMenuHudSettings::Reset()
{
	const char *ones[] = {
		"hud_show_health", "hud_show_armor", "hud_show_ammo", "hud_show_money",
		"hud_show_radar", "hud_show_timer", "hud_show_chat", "hud_show_death"
	};
	const char *zeros[] = {
		"hud_dx_health", "hud_dy_health", "hud_dx_armor", "hud_dy_armor",
		"hud_dx_ammo", "hud_dy_ammo", "hud_dx_money", "hud_dy_money",
		"hud_dx_radar", "hud_dy_radar", "hud_dx_timer", "hud_dy_timer",
		"hud_dx_chat", "hud_dy_chat", "hud_dx_death", "hud_dy_death"
	};
	for( size_t i = 0; i < sizeof( ones ) / sizeof( ones[0] ); ++i )
		EngFuncs::CvarSetValue( ones[i], 1 );
	for( size_t i = 0; i < sizeof( zeros ) / sizeof( zeros[0] ); ++i )
		EngFuncs::CvarSetValue( zeros[i], 0 );
	EngFuncs::CvarSetValue( "hud_r", 255 );
	EngFuncs::CvarSetValue( "hud_g", 160 );
	EngFuncs::CvarSetValue( "hud_b", 0 );
	EngFuncs::CvarSetValue( "hud_chat_r", 255 );
	EngFuncs::CvarSetValue( "hud_chat_g", 210 );
	EngFuncs::CvarSetValue( "hud_chat_b", 0 );
	EngFuncs::CvarSetValue( "hud_saytext_time", 5 );
	GetConfig();
}

void CMenuHudSettings::SaveAndPopMenu()
{
	CMenuFramework::SaveAndPopMenu();
}

void CMenuHudSettings::_Init()
{
	Reg( "hud_show_health", "1" );
	Reg( "hud_show_armor", "1" );
	Reg( "hud_show_ammo", "1" );
	Reg( "hud_show_money", "1" );
	Reg( "hud_show_radar", "1" );
	Reg( "hud_show_timer", "1" );
	Reg( "hud_show_chat", "1" );
	Reg( "hud_show_death", "1" );
	Reg( "hud_dx_health", "0" ); Reg( "hud_dy_health", "0" );
	Reg( "hud_dx_armor", "0" ); Reg( "hud_dy_armor", "0" );
	Reg( "hud_dx_ammo", "0" ); Reg( "hud_dy_ammo", "0" );
	Reg( "hud_dx_money", "0" ); Reg( "hud_dy_money", "0" );
	Reg( "hud_dx_radar", "0" ); Reg( "hud_dy_radar", "0" );
	Reg( "hud_dx_timer", "0" ); Reg( "hud_dy_timer", "0" );
	Reg( "hud_dx_chat", "0" ); Reg( "hud_dy_chat", "0" );
	Reg( "hud_dx_death", "0" ); Reg( "hud_dy_death", "0" );
	Reg( "hud_r", "255" ); Reg( "hud_g", "160" ); Reg( "hud_b", "0" );
	Reg( "hud_chat_r", "255" ); Reg( "hud_chat_g", "210" ); Reg( "hud_chat_b", "0" );
	Reg( "hud_saytext_time", "5" );

	banner.SetPicture( ART_BANNER );

	showHealth.SetNameAndStatus( L( "Vida" ), L( "Mostra a vida" ) );
	showArmor.SetNameAndStatus( L( "Colete" ), L( "Mostra o colete" ) );
	showAmmo.SetNameAndStatus( L( "Municao" ), L( "Mostra a municao" ) );
	showMoney.SetNameAndStatus( L( "Dinheiro" ), L( "Mostra o dinheiro" ) );
	showRadar.SetNameAndStatus( L( "Radar" ), L( "Mostra o radar" ) );
	showTimer.SetNameAndStatus( L( "Timer" ), L( "Mostra o tempo do round" ) );
	showChat.SetNameAndStatus( L( "Chat" ), L( "Mostra o chat" ) );
	showDeath.SetNameAndStatus( L( "Mortes" ), L( "Mostra o aviso de mortes" ) );

	showHealth.onChanged = CMenuEditable::WriteCvarCb;
	showArmor.onChanged = CMenuEditable::WriteCvarCb;
	showAmmo.onChanged = CMenuEditable::WriteCvarCb;
	showMoney.onChanged = CMenuEditable::WriteCvarCb;
	showRadar.onChanged = CMenuEditable::WriteCvarCb;
	showTimer.onChanged = CMenuEditable::WriteCvarCb;
	showChat.onChanged = CMenuEditable::WriteCvarCb;
	showDeath.onChanged = CMenuEditable::WriteCvarCb;

	showHealth.SetCoord( 72, 150 );
	showArmor.SetCoord( 72, 190 );
	showAmmo.SetCoord( 72, 230 );
	showMoney.SetCoord( 72, 270 );
	showRadar.SetCoord( 300, 150 );
	showTimer.SetCoord( 300, 190 );
	showChat.SetCoord( 300, 230 );
	showDeath.SetCoord( 300, 270 );

	SetupOffset( healthX, "Vida X", "Desloca a vida na horizontal", 72, 330 );
	SetupOffset( healthY, "Vida Y", "Desloca a vida na vertical", 280, 330 );
	SetupOffset( armorX, "Colete X", "Desloca o colete na horizontal", 520, 330 );
	SetupOffset( armorY, "Colete Y", "Desloca o colete na vertical", 760, 330 );

	SetupOffset( ammoX, "Municao X", "Desloca a municao na horizontal", 72, 390 );
	SetupOffset( ammoY, "Municao Y", "Desloca a municao na vertical", 280, 390 );
	SetupOffset( moneyX, "Dinheiro X", "Desloca o dinheiro na horizontal", 520, 390 );
	SetupOffset( moneyY, "Dinheiro Y", "Desloca o dinheiro na vertical", 760, 390 );

	SetupOffset( radarX, "Radar X", "Desloca o radar na horizontal", 72, 450 );
	SetupOffset( radarY, "Radar Y", "Desloca o radar na vertical", 280, 450 );
	SetupOffset( timerX, "Timer X", "Desloca o timer na horizontal", 520, 450 );
	SetupOffset( timerY, "Timer Y", "Desloca o timer na vertical", 760, 450 );

	SetupOffset( chatX, "Chat X", "Desloca o chat na horizontal", 72, 510 );
	SetupOffset( chatY, "Chat Y", "Desloca o chat na vertical", 280, 510 );
	SetupOffset( deathX, "Mortes X", "Desloca os avisos na horizontal", 520, 510 );
	SetupOffset( deathY, "Mortes Y", "Desloca os avisos na vertical", 760, 510 );

	SetupByte( hudR, "HUD R", 72, 580 );
	SetupByte( hudG, "HUD G", 280, 580 );
	SetupByte( hudB, "HUD B", 520, 580 );
	SetupByte( chatR, "Chat R", 72, 640 );
	SetupByte( chatG, "Chat G", 280, 640 );
	SetupByte( chatB, "Chat B", 520, 640 );

	chatTime.SetNameAndStatus( L( "Tempo do chat" ), L( "Segundos que cada linha fica na tela" ) );
	chatTime.Setup( 1.0f, 20.0f, 0.5f );
	chatTime.onChanged = CMenuEditable::WriteCvarCb;
	chatTime.SetRect( 760, 640, 180, 26 );

	AddItem( banner );
	AddButton( L( "Done" ), L( "Volta para Configuracoes" ), PC_DONE, VoidCb( &CMenuHudSettings::SaveAndPopMenu ), QMF_NOTIFY );
	AddButton( L( "Padrao" ), L( "Restaura o HUD e o chat" ), PC_USE_DEFAULTS, VoidCb( &CMenuHudSettings::Reset ), QMF_NOTIFY );

	AddItem( showHealth ); AddItem( showArmor ); AddItem( showAmmo ); AddItem( showMoney );
	AddItem( showRadar ); AddItem( showTimer ); AddItem( showChat ); AddItem( showDeath );
	AddItem( healthX ); AddItem( healthY ); AddItem( armorX ); AddItem( armorY );
	AddItem( ammoX ); AddItem( ammoY ); AddItem( moneyX ); AddItem( moneyY );
	AddItem( radarX ); AddItem( radarY ); AddItem( timerX ); AddItem( timerY );
	AddItem( chatX ); AddItem( chatY ); AddItem( deathX ); AddItem( deathY );
	AddItem( hudR ); AddItem( hudG ); AddItem( hudB );
	AddItem( chatR ); AddItem( chatG ); AddItem( chatB );
	AddItem( chatTime );
}

void CMenuHudSettings::_VidInit()
{
	GetConfig();
}

ADD_MENU( menu_hudsettings, CMenuHudSettings, UI_HudSettings_Menu );
