#include "Console.h"

#include <Display.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
static const uint16_t INFO_COLOR = CDisplay::COLOR_WHITE;
static const uint16_t DATA_COLOR = CDisplay::COLOR_GREEN;

#ifdef DEBUG
static const uint16_t BACK_COLOR = CDisplay::COLOR_RED;
static const uint16_t FILL_COLOR = CDisplay::COLOR_MAGENTA;
#else
static const uint16_t BACK_COLOR = CDisplay::COLOR_BLACK;
static const uint16_t FILL_COLOR = CDisplay::COLOR_BLACK;
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
CConsole::SItem::SItem() :
	fltData( 0.0f ),

	posY( 0 ),
	pString( nullptr ),
	bWasInit( false )
{}
////////////////////////////////////////////////////////////////////////////////////////////////////
CConsole::CConsole( CDisplay *pDisplay ) :
	m_pDisplay( pDisplay ),
	m_posX( TEXT_POS_X ),
	m_posY( TEXT_POS_X )
{}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::Init()
{
	// Initialization
	m_pDisplay->Init();
	m_pDisplay->SetRotation( 0 ); // 2
	m_pDisplay->ClearScreen( FILL_COLOR );

	// Draw information about initialization process
	m_pDisplay->SetTextColor( CDisplay::COLOR_WHITE );
	m_pDisplay->DrawText( TEXT_POS_X, TEXT_POS_X, "Initialization process" );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::RegisterText( const char *pString )
{
	m_pDisplay->DrawText( m_posX, m_posY, pString );
	m_posY += TEXT_STEP_Y;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::RegisterData( const EConsoleType type, const char *pString )
{
	m_item[type].posY = m_posY;
	m_item[type].pString = pString;
	m_item[type].bWasInit = true;
	m_posY += TEXT_STEP_Y;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::RegisterSpace()
{
	m_posY += TEXT_STEP_Y;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::SetWorkMode()
{
	m_pDisplay->SetTextColor( DATA_COLOR );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::DrawHeader()
{
	// Copyright section
	m_pDisplay->SetTextColor( CDisplay::COLOR_YELLOW );
	m_pDisplay->ClearScreen( FILL_COLOR );
	RegisterText( "Ariadne Navigation System" );
	RegisterSpace();
	m_pDisplay->SetTextColor( INFO_COLOR );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::DrawInfo()
{
	for( int i = 0; i < CONSOLE_TYPE_COUNT; ++i )
		if( m_item[i].bWasInit )
			m_pDisplay->DrawText( m_posX, m_item[i].posY, m_item[i].pString );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::DrawData()
{
	for( int i = 0; i < CONSOLE_TYPE_COUNT; ++i )
		if( m_item[i].bWasInit )
		{
			// TODO: выбрать правильный тип
			DrawData( m_item[i].posY, m_item[i].fltData );
		}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::DrawData( const uint16_t y, const int16_t value )
{
	m_pDisplay->FillRect( DATA_POS_X, y, CLEAR_SIZE_X, CLEAR_SIZE_Y, BACK_COLOR );
	m_pDisplay->DrawInt( DATA_POS_X, y, value );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::DrawData( const uint16_t y, const int32_t value )
{
	m_pDisplay->FillRect( DATA_POS_X, y, CLEAR_SIZE_X, CLEAR_SIZE_Y, BACK_COLOR );
	m_pDisplay->DrawInt( DATA_POS_X, y, value );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::DrawData( const uint16_t y, const int64_t value )
{
	m_pDisplay->FillRect( DATA_POS_X, y, CLEAR_SIZE_X, CLEAR_SIZE_Y, BACK_COLOR );
	m_pDisplay->DrawLong( DATA_POS_X, y, value );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CConsole::DrawData( const uint16_t y, const float value )
{
	m_pDisplay->FillRect( DATA_POS_X, y, CLEAR_SIZE_X, CLEAR_SIZE_Y, BACK_COLOR );
	m_pDisplay->DrawFlt( DATA_POS_X, y, value );
}
////////////////////////////////////////////////////////////////////////////////////////////////////