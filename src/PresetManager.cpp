#include "PresetManager.hpp"

PresetManager::PresetManager (const unsigned int headerSizeInBytes, const unsigned int maxNumPresets, IStorageMedia* storageMedia) :
	m_HeaderSizeInBytes( headerSizeInBytes ),
	m_MaxNumPresets( maxNumPresets ),
	m_CurrentPreset( 0 ),
	m_StorageMedia( storageMedia ),
	m_NeedToInitializePresets( false )
{
}

PresetManager::~PresetManager()
{
}
