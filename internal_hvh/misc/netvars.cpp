#include "../include_cheat.h"

CNetVars::CNetVars()
{
	for ( auto clazz = g_pClient->GetAllClasses(); clazz; clazz = clazz->m_pNext )
		if ( clazz->m_pRecvTable )
			dump_recursive( clazz->m_pNetworkName, clazz->m_pRecvTable, 0 );
}
//===============================================
void CNetVars::dump_recursive( const char* base_class, RecvTable* table, uint16_t offset )
{
	for ( auto i = 0; i < table->m_nProps; ++i )
	{
		auto prop_ptr = &table->m_pProps[ i ];

		//Skip trash array items
		if ( !prop_ptr || isdigit( prop_ptr->m_pVarName[ 0 ] ) )
			continue;

		//We dont care about the base class, we already know that
		_( baseclass, "baseclass" );
		if ( strcmp( prop_ptr->m_pVarName, baseclass ) == 0 )
			continue;

		if ( prop_ptr->m_RecvType == DPT_DataTable &&
			 prop_ptr->m_pDataTable != nullptr &&
			 prop_ptr->m_pDataTable->m_pNetTableName[ 0 ] == 'D' ) // Skip shitty tables
		{
			dump_recursive( base_class, prop_ptr->m_pDataTable, offset + prop_ptr->m_Offset );
		}

		char hash_name[ 256 ];

		_( arr, "->" );
		strcpy_s( hash_name, base_class );
		strcat_s( hash_name, arr );
		strcat_s( hash_name, prop_ptr->m_pVarName );

		// Need to cast it to prevent FnvHash using the recursive hasher
		// which would hash all 256 bytes
		auto hash = FnvHash( static_cast<const char*>( hash_name ) );

		m_props[ hash ] = { prop_ptr,  uint16_t( offset + prop_ptr->m_Offset ) };
	}
}