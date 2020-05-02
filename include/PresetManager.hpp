#ifndef PRESETMANAGER_HPP
#define PRESETMANAGER_HPP

/*************************************************************************
 * A PresetManager stores presets in some storage media (file, eeprom,
 * ect). It is intended to sit outside of a VoiceManager and ask the
 * voice manager for preset data for storing. When retrieving preset data
 * from a storage media the PresetManager will pass it back to the
 * VoiceManager to set the states and then the UI will reset.
 *
 * It would be nice if the PresetManager could upgrade preset versions
 * by itself, but since each synth (or whatever else) will have its own
 * completely different fields, it is currently left to the synth
 * specific implementation to implement the IPresetUpgrader interface.
*************************************************************************/

#include "IStorageMedia.hpp"

class PresetManager;

class IPresetUpgrader
{
	public:
		virtual ~IPresetUpgrader() {}

		void setPresetManager (PresetManager* presetManager) { m_PresetManager = presetManager; }
		virtual void upgradePresets() = 0;

	protected:
		PresetManager* m_PresetManager;
};

class PresetManager
{
	public:
		PresetManager (const unsigned int headerSizeInBytes, const unsigned int maxNumPresets, IStorageMedia* storageMedia);
		~PresetManager();

		bool needToInitializePresets() { return m_NeedToInitializePresets; }

		void upgradePresets (IPresetUpgrader* presetUpgrader)
		{
			if ( m_StorageMedia->needsInitialization() )
			{
				m_StorageMedia->initialize();
				m_NeedToInitializePresets = true;
			}

			presetUpgrader->setPresetManager( this );
			presetUpgrader->upgradePresets();

			if (m_NeedToInitializePresets)
			{
				m_StorageMedia->afterInitialize();
				m_NeedToInitializePresets = false;
			}
		}

		template <typename T>
		void writeHeader (T& header)
		{
			m_StorageMedia->writeToMedia( Variant( &header ), m_HeaderSizeInBytes, 0 );
		}

		template <typename T>
		T retrieveHeader()
		{
			Variant header = m_StorageMedia->readFromMedia( sizeof(T), 0 );

			return header.get<T>();
		}

		template <typename T>
		void writePreset (T& preset, unsigned int presetNum)
		{
			m_StorageMedia->writeToMedia( Variant( &preset ), sizeof(T), m_HeaderSizeInBytes + (presetNum * sizeof(T)) );
		}

		template <typename T>
		T retrievePreset (const unsigned int presetNum)
		{
			Variant preset = m_StorageMedia->readFromMedia( sizeof(T), m_HeaderSizeInBytes + (presetNum * sizeof(T)) );

			return preset.get<T>();
		}

		template <typename T>
		T nextPreset()
		{
			if (m_CurrentPreset < m_MaxNumPresets - 1)
			{
				m_CurrentPreset++;
			}

			return retrievePreset<T>( m_CurrentPreset );
		}

		template <typename T>
		T prevPreset()
		{
			if (m_CurrentPreset > 0)
			{
				m_CurrentPreset--;
			}

			return retrievePreset<T>( m_CurrentPreset );
		}

		unsigned int getMaxNumPresets() { return m_MaxNumPresets; }
		unsigned int getCurrentPresetNum() { return m_CurrentPreset; }

	private:
		unsigned int m_HeaderSizeInBytes;
		unsigned int m_MaxNumPresets;
		unsigned int m_CurrentPreset;

		IStorageMedia* m_StorageMedia;
		bool           m_NeedToInitializePresets;
};

#endif // PRESET_MANAGER_HPP
