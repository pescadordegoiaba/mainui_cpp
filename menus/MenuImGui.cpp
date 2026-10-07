#include "imgui.h"
#include "imgui_impl_opengl2.h"

#include "BaseMenu.h"
#include "Utils.h"
#include "ItemsHolder.h"
#include "BaseWindow.h"
#include "PicButton.h"
#include "Action.h"
#include "CheckBox.h"
#include "Slider.h"
#include "SpinControl.h"
#include "Field.h"
#include "Table.h"
#include "Switch.h"
#include "Bitmap.h"
#include "BaseModel.h"
#include "keydefs.h"
#include "MenuImGui.h"

static int g_ready = 0;
static int g_frame = 0;
static int g_mx = 0;
static int g_my = 0;
static int g_down = 0;
static int g_sent_down = 0;
static CMenuBaseItem *g_pending = NULL;
static int g_pending_key = 0;

static bool HasTable( CMenuItemsHolder *holder )
{
	for( int i = 0; i < holder->ItemCount(); ++i )
	{
		CMenuBaseItem *item = holder->GetItemByIndex( i );
		if( !item || !item->IsVisible() )
			continue;
		if( dynamic_cast<CMenuTable *>( item ) )
			return true;
		CMenuItemsHolder *child = dynamic_cast<CMenuItemsHolder *>( item );
		if( child && !child->IsWindow() && HasTable( child ) )
			return true;
	}
	return false;
}

static void DrawTable( CMenuTable *table )
{
	CMenuBaseModel *model = table->GetModel();
	int cols, rows, c, r;

	if( !model )
		return;

	cols = model->GetColumns();
	rows = model->GetRows();
	if( cols < 1 )
		return;
	if( cols > MAX_TABLE_COLUMNS )
		cols = MAX_TABLE_COLUMNS;

	if( rows <= 0 )
	{
		ImGui::TextUnformatted( "Lista vazia" );
		return;
	}

	float height = ImGui::GetContentRegionAvail().y - 36.0f;
	if( height < 180.0f )
		height = 180.0f;

	if( !ImGui::BeginTable( "##lista", cols,
		ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingStretchProp,
		ImVec2( 0.0f, height ) ) )
		return;

	for( c = 0; c < cols; ++c )
		ImGui::TableSetupColumn( table->HeaderText( c ) );
	ImGui::TableHeadersRow();

	if( ImGuiTableSortSpecs *sort = ImGui::TableGetSortSpecs() )
	{
		if( sort->SpecsDirty && sort->SpecsCount > 0 )
		{
			bool ascend = sort->Specs[0].SortDirection != ImGuiSortDirection_Descending;
			table->SetSortingColumn( sort->Specs[0].ColumnIndex, ascend );
			table->_Event( QM_CHANGED );
			sort->SpecsDirty = false;
		}
	}

	for( r = 0; r < rows; ++r )
	{
		ImGui::TableNextRow();
		for( c = 0; c < cols; ++c )
		{
			const char *text = model->GetCellText( r, c );
			bool image = model->GetCellType( r, c ) != CELL_TEXT;
			ImGui::TableSetColumnIndex( c );
			if( image )
				text = ( text && text[0] ) ? "*" : "";
			if( !text )
				text = "";
			if( c == 0 )
			{
				char label[192];
				snprintf( label, sizeof( label ), "%s##row%d", text[0] ? text : " ", r );
				if( ImGui::Selectable( label, r == table->GetCurrentIndex(),
					ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick ) )
				{
					table->SetCurrentIndex( r );
					table->_Event( QM_CHANGED );
					if( ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
						model->OnActivateEntry( r );
				}
			}
			else
				ImGui::TextUnformatted( text );
		}
	}
	ImGui::EndTable();
}

static void Activate( CMenuBaseItem *item, int key )
{
	g_pending = item;
	g_pending_key = key;
}

static void DrawItem( CMenuBaseItem *item )
{
	if( !item || !item->IsVisible() )
		return;
	if( item->iFlags & ( QMF_HIDDENBYPARENT ) )
		return;

	CMenuItemsHolder *child = dynamic_cast<CMenuItemsHolder *>( item );
	if( child && !child->IsWindow() )
	{
		for( int i = 0; i < child->ItemCount(); ++i )
			DrawItem( child->GetItemByIndex( i ) );
		return;
	}
	if( dynamic_cast<CMenuBitmap *>( item ) )
		return;

	if( CMenuTable *table = dynamic_cast<CMenuTable *>( item ) )
	{
		ImGui::PushID( item );
		DrawTable( table );
		ImGui::PopID();
		return;
	}

	if( CMenuSwitch *sw = dynamic_cast<CMenuSwitch *>( item ) )
	{
		ImGui::PushID( item );
		for( int s = 0; s < sw->GetSwitchCount(); ++s )
		{
			if( s )
				ImGui::SameLine();
			if( ImGui::RadioButton( sw->GetSwitchName( s ), sw->GetState() == s ) )
				sw->SetState( s );
		}
		ImGui::PopID();
		return;
	}

	bool off = ( item->iFlags & ( QMF_GRAYED | QMF_INACTIVE ) ) != 0;
	const char *name = item->szName ? item->szName : "";

	ImGui::PushID( item );
	if( off )
		ImGui::BeginDisabled();

	if( CMenuCheckBox *box = dynamic_cast<CMenuCheckBox *>( item ) )
	{
		bool checked = box->bChecked;
		if( ImGui::Checkbox( name[0] ? name : "##box", &checked ) )
			Activate( box, K_ENTER );
	}
	else if( CMenuSlider *slider = dynamic_cast<CMenuSlider *>( item ) )
	{
		ImGui::Text( "%s  %.2f", name, slider->GetCurrentValue() );
		ImGui::SameLine();
		if( ImGui::SmallButton( "-" ) )
			Activate( slider, K_LEFTARROW );
		ImGui::SameLine();
		if( ImGui::SmallButton( "+" ) )
			Activate( slider, K_RIGHTARROW );
	}
	else if( CMenuSpinControl *spin = dynamic_cast<CMenuSpinControl *>( item ) )
	{
		ImGui::Text( "%s", name );
		ImGui::SameLine();
		if( ImGui::SmallButton( "<" ) )
			Activate( spin, K_LEFTARROW );
		ImGui::SameLine();
		if( ImGui::SmallButton( ">" ) )
			Activate( spin, K_RIGHTARROW );
	}
	else if( CMenuField *field = dynamic_cast<CMenuField *>( item ) )
	{
		static CMenuField *last = NULL;
		static char buf[UI_MAX_FIELD_LINE];
		if( last != field )
		{
			last = field;
			Q_strncpy( buf, field->GetBuffer() ? field->GetBuffer() : "", sizeof( buf ) );
		}
		ImGui::TextUnformatted( name );
		if( ImGui::InputText( "##field", buf, sizeof( buf ) ) )
		{
			field->SetBuffer( buf );
			field->WriteCvar();
		}
	}
	else if( name[0] && ( dynamic_cast<CMenuPicButton *>( item ) || dynamic_cast<CMenuAction *>( item ) ) )
	{
		if( ImGui::Selectable( name ) )
			Activate( item, K_ENTER );
		if( item->szStatusText && ImGui::IsItemHovered() )
			ImGui::SetTooltip( "%s", item->szStatusText );
	}

	if( off )
		ImGui::EndDisabled();
	ImGui::PopID();
}

static const char *Title( CMenuItemsHolder *holder )
{
	const char *name = holder->szName ? holder->szName : "Menu";
	if( !strncmp( name, "CMenu", 5 ) )
		name += 5;
	if( !name[0] )
		return "Counter-Strike";
	return name;
}

bool MenuImGui_Draw( CMenuItemsHolder *holder )
{
	if( !holder || !holder->IsWindow() )
		return false;
	if( ScreenWidth < 64 || ScreenHeight < 64 )
		return false;

	bool wide = HasTable( holder );

	if( !g_ready )
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		ImGui::GetIO().IniFilename = NULL;
		ImGui::StyleColorsDark();
		ImGui::GetStyle().WindowRounding = 0.0f;
		if( !ImGui_ImplOpenGL2_Init() )
		{
			ImGui::DestroyContext();
			return false;
		}
		g_ready = 1;
	}

	if( !g_frame )
	{
		ImGuiIO &io = ImGui::GetIO();
		io.DisplaySize = ImVec2( ScreenWidth, ScreenHeight );
		io.DeltaTime = 1.0f / 60.0f;
		io.MouseDrawCursor = false;
		io.AddMousePosEvent( (float)g_mx, (float)g_my );
		if( g_down != g_sent_down )
		{
			io.AddMouseButtonEvent( 0, g_down != 0 );
			g_sent_down = g_down;
		}
		ImGui_ImplOpenGL2_NewFrame();
		ImGui::NewFrame();
		g_frame = 1;
	}

	if( wide )
	{
		ImGui::SetNextWindowPos( ImVec2( 16.0f, 16.0f ), ImGuiCond_FirstUseEver );
		ImGui::SetNextWindowSize( ImVec2( ScreenWidth - 32.0f, ScreenHeight - 32.0f ), ImGuiCond_FirstUseEver );
	}
	else
	{
		ImGui::SetNextWindowPos( ImVec2( 72.0f, 150.0f ), ImGuiCond_FirstUseEver );
		ImGui::SetNextWindowSize( ImVec2( 420.0f, 0.0f ), ImGuiCond_Always );
	}
	ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0.0f, 0.0f, 0.0f, 0.62f ) );
	ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
	ImGui::PushStyleColor( ImGuiCol_Header, ImVec4( 1.0f, 0.69f, 0.0f, 0.35f ) );
	ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ImVec4( 1.0f, 0.69f, 0.0f, 0.55f ) );
	ImGui::PushStyleColor( ImGuiCol_HeaderActive, ImVec4( 1.0f, 0.5f, 0.0f, 0.75f ) );
	char id[128];
	snprintf( id, sizeof( id ), "%s##win", Title( holder ) );
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
	if( !wide )
		flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	bool open = ImGui::Begin( id, NULL, flags );
	if( open )
	{
		ImGui::TextColored( ImVec4( 1.0f, 0.69f, 0.0f, 1.0f ), "%s", Title( holder ) );
		for( int i = 0; i < holder->ItemCount(); ++i )
			DrawItem( holder->GetItemByIndex( i ) );
	}
	ImGui::End();
	ImGui::PopStyleColor( 5 );
	return true;
}

void MenuImGui_EndPass( void )
{
	CMenuBaseItem *item;
	int key;

	if( !g_frame )
		return;

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData( ImGui::GetDrawData() );
	g_frame = 0;

	item = g_pending;
	key = g_pending_key;
	g_pending = NULL;
	if( !item )
		return;
	item->KeyDown( key );
	item->KeyUp( key );
}

void MenuImGui_Mouse( int x, int y )
{
	g_mx = x;
	g_my = y;
}

void MenuImGui_Button( int down )
{
	g_down = down ? 1 : 0;
}
