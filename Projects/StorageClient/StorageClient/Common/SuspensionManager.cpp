//
// SuspensionManager.cpp
// Implementazione della classe SuspensionManager
//

#include "pch.h"
#include "SuspensionManager.h"

#include <algorithm>

using namespace StorageClient::Common;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;

Map<String^, Object^>^ SuspensionManager::_sessionState = ref new Map<String^, Object^>();

const wchar_t* SuspensionManager::sessionStateFilename = L"_sessionState.dat";

std::vector<WeakReference> SuspensionManager::_registeredFrames;

DependencyProperty^ SuspensionManager::FrameSessionStateKeyProperty =
	DependencyProperty::RegisterAttached("_FrameSessionStateKeyProperty",
	TypeName(String::typeid), TypeName(SuspensionManager::typeid), nullptr);

DependencyProperty^ SuspensionManager::FrameSessionBaseKeyProperty =
	DependencyProperty::RegisterAttached("_FrameSessionBaseKeyProperty",
	TypeName(String::typeid), TypeName(SuspensionManager::typeid), nullptr);

DependencyProperty^ SuspensionManager::FrameSessionStateProperty =
	DependencyProperty::RegisterAttached("_FrameSessionStateProperty",
	TypeName(IMap<String^, Object^>::typeid), TypeName(SuspensionManager::typeid), nullptr);

class ObjectSerializeHelper
{
public:
	// Codici utilizzati per l'identificazione dei tipi serializzati
	enum StreamTypes {
		NullPtrType = 0,

		// Tipi IPropertyValue supportati
		UInt8Type, UInt16Type, UInt32Type, UInt64Type, Int16Type, Int32Type, Int64Type,
		SingleType, DoubleType, BooleanType, Char16Type, GuidType, StringType,

		// Ulteriori tipi supportati
		StringToObjectMapType,

		// Valori indicatore utilizzati per garantire l'integrità del flusso
		MapEndMarker
	};
	static String^ ReadString(DataReader^ reader);
	static IMap<String^, Object^>^ ReadStringToObjectMap(DataReader^ reader);
	static Object^ ReadObject(DataReader^ reader);
	static void WriteString(DataWriter^ writer, String^ string);
	static void WriteProperty(DataWriter^ writer, IPropertyValue^ propertyValue);
	static void WriteStringToObjectMap(DataWriter^ writer, IMap<String^, Object^>^ map);
	static void WriteObject(DataWriter^ writer, Object^ object);
};

/// <summary>
/// Fornisce l'accesso allo stato sessione globale per la sessione corrente.  Questo stato viene serializzato da
/// <see cref="SaveAsync"/> e ripristinato da <see cref="RestoreAsync"/>. È necessario utilizzare
/// uno dei valori seguenti: valori boxed interi inclusi, valori singoli e doppi a virgola mobile,
/// caratteri wide, valori booleani, valori String e Guid o Map<String^, Object^> dove i valori di Map sono
/// soggetti agli stessi vincoli.  Lo stato sessione deve essere compresso quanto più possibile.
/// </summary>
IMap<String^, Object^>^ SuspensionManager::SessionState()
{
	return _sessionState;
}

/// <summary>
/// Registra un'istanza di <see cref="Frame"/> per consentire il salvataggio e il ripristino della
/// cronologia di navigazione da <see cref="SessionState"/>.  I frame devono essere registrati una volta
/// subito dopo la creazione se partecipano alla gestione dello stato sessione.  Alla
/// registrazione, se lo stato è già stato ripristinato per la chiave specificata,
/// verrà subito ripristinata la cronologia di navigazione.  I successivi richiami di
/// <see cref="RestoreAsync(String)"/> ripristineranno anche la cronologia di navigazione.
/// </summary>
/// <param name="frame">Istanza la cui cronologia di navigazione deve essere gestita da
/// <see cref="SuspensionManager"/></param>
/// <param name="sessionStateKey">Chiave univoca in <see cref="SessionState"/> utilizzata per
/// archiviare le informazioni correlate alla navigazione.</param>
/// <param name="sessionBaseKey">Chiave facoltativa che identifica il tipo di sessione.
/// Può essere utilizzata per distinguere i diversi scenari di avvio dell'applicazione.</param>
void SuspensionManager::RegisterFrame(Frame^ frame, String^ sessionStateKey, String^ sessionBaseKey)
{
	if (frame->GetValue(FrameSessionStateKeyProperty) != nullptr)
	{
		throw ref new FailureException("Frames can only be registered to one session state key");
	}

	if (frame->GetValue(FrameSessionStateProperty) != nullptr)
	{
		throw ref new FailureException("Frames must be either be registered before accessing frame session state, or not registered at all");
	}

	if (sessionBaseKey != nullptr)
	{
		frame->SetValue(FrameSessionBaseKeyProperty, sessionBaseKey);
		sessionStateKey = sessionBaseKey + "_" + sessionStateKey;
	}

	// Utilizzare una proprietà di dipendenza per associare la chiave di sessione a un frame e conservare un elenco di frame di cui
	// deve essere gestito lo stato di navigazione
	frame->SetValue(FrameSessionStateKeyProperty, sessionStateKey);
	_registeredFrames.insert(_registeredFrames.begin(), WeakReference(frame));

	// Verificare se lo stato di navigazione può essere ripristinato
	RestoreFrameNavigationState(frame);
}

/// <summary>
/// Rimuove l'associazione a un oggetto <see cref="Frame"/> precedentemente registrato da <see cref="RegisterFrame"/>
/// da <see cref="SessionState"/>.  Qualsiasi stato di navigazione precedentemente acquisito verrà
/// rimosso.
/// </summary>
/// <param name="frame">Istanza di cui non deve più essere gestita la cronologia di
/// navigazione.</param>
void SuspensionManager::UnregisterFrame(Frame^ frame)
{
	// Rimuovere lo stato sessione e rimuovere il frame dall'elenco di frame di cui verrà salvato lo stato
	// di navigazione (insieme ai riferimenti deboli che non sono più raggiungibili)
	auto key = safe_cast<String^>(frame->GetValue(FrameSessionStateKeyProperty));
	if (SessionState()->HasKey(key))
	{
		SessionState()->Remove(key);
	}
	_registeredFrames.erase(
		std::remove_if(_registeredFrames.begin(), _registeredFrames.end(), [=](WeakReference& e)
	{
		auto testFrame = e.Resolve<Frame>();
		return testFrame == nullptr || testFrame == frame;
	}),
		_registeredFrames.end()
		);
}

/// <summary>
/// Fornisce l'archiviazione per lo stato sessione associato all'oggetto <see cref="Frame"/> specificato.
/// Per i frame precedentemente registrati con <see cref="RegisterFrame"/>, lo
/// stato sessione viene salvato e ripristinato automaticamente come parte dell'oggetto
/// <see cref="SessionState"/> globale.  I frame non registrati hanno uno stato di passaggio
/// che può tuttavia essere utile quando vengono ripristinate le pagine eliminate dalla
/// cache di navigazione.
/// </summary>
/// <remarks>Le applicazioni possono scegliere di basarsi su <see cref="NavigationHelper"/> per gestire
/// lo stato specifico della pagina anziché gestire direttamente lo stato sessione del frame.</remarks>
/// <param name="frame">Istanza per cui si desidera lo stato sessione.</param>
/// <returns>Raccolta di stati soggetti allo stesso meccanismo di serializzazione di
/// <see cref="SessionState"/>.</returns>
IMap<String^, Object^>^ SuspensionManager::SessionStateForFrame(Frame^ frame)
{
	auto frameState = safe_cast<IMap<String^, Object^>^>(frame->GetValue(FrameSessionStateProperty));

	if (frameState == nullptr)
	{
		auto frameSessionKey = safe_cast<String^>(frame->GetValue(FrameSessionStateKeyProperty));
		if (frameSessionKey != nullptr)
		{
			// I frame registrati riflettono lo stato sessione corrispondente
			if (!_sessionState->HasKey(frameSessionKey))
			{
				_sessionState->Insert(frameSessionKey, ref new Map<String^, Object^>());
			}
			frameState = safe_cast<IMap<String^, Object^>^>(_sessionState->Lookup(frameSessionKey));
		}
		else
		{
			// I frame non registrati hanno uno stato di passaggio
			frameState = ref new Map<String^, Object^>();
		}
		frame->SetValue(FrameSessionStateProperty, frameState);
	}
	return frameState;
}

void SuspensionManager::RestoreFrameNavigationState(Frame^ frame)
{
	auto frameState = SessionStateForFrame(frame);
	if (frameState->HasKey("Navigation"))
	{
		frame->SetNavigationState(safe_cast<String^>(frameState->Lookup("Navigation")));
	}
}

void SuspensionManager::SaveFrameNavigationState(Frame^ frame)
{
	auto frameState = SessionStateForFrame(frame);
	frameState->Insert("Navigation", frame->GetNavigationState());
}

/// <summary>
/// Salvare l'oggetto <see cref="SessionState"/> corrente.  Tutte le istanze di <see cref="Frame"/>
/// registrate con <see cref="RegisterFrame"/> conserveranno lo
/// stack di navigazione corrente, che a sua volta fornisce all'oggetto <see cref="Page"/> attivo la possibilità
/// di salvare lo stato.
/// </summary>
/// <returns>Attività asincrona che riflette il momento in cui è stato salvato lo stato sessione.</returns>
task<void> SuspensionManager::SaveAsync(void)
{
	// Salvare lo stato di navigazione per tutti i frame registrati
	for (auto && weakFrame : _registeredFrames)
	{
		auto frame = weakFrame.Resolve<Frame>();
		if (frame != nullptr) SaveFrameNavigationState(frame);
	}

	// Serializzare lo stato sessione in modo sincrono per evitare l'accesso asincrono allo stato
	// condiviso
	auto sessionData = ref new InMemoryRandomAccessStream();
	auto sessionDataWriter = ref new DataWriter(sessionData->GetOutputStreamAt(0));
	ObjectSerializeHelper::WriteObject(sessionDataWriter, _sessionState);

	// Dopo avere acquisito lo stato sessione in modo sincrono, iniziare il processo asincrono
	// di scrittura del risultato su disco
	return task<unsigned int>(sessionDataWriter->StoreAsync()).then([=](unsigned int)
	{
		return ApplicationData::Current->LocalFolder->CreateFileAsync(StringReference(sessionStateFilename),
			CreationCollisionOption::ReplaceExisting);
	})
		.then([=](StorageFile^ createdFile)
	{
		return createdFile->OpenAsync(FileAccessMode::ReadWrite);
	})
		.then([=](IRandomAccessStream^ newStream)
	{
		return RandomAccessStream::CopyAsync(
			sessionData->GetInputStreamAt(0), newStream->GetOutputStreamAt(0));
	})
		.then([=](UINT64 copiedBytes)
	{
		(void) copiedBytes; // Parametro non utilizzato
		return;
	});
}

/// <summary>
/// Ripristina l'oggetto <see cref="SessionState"/> precedentemente salvato.  Tutte le istanze di <see cref="Frame"/>
/// registrate con <see cref="RegisterFrame"/> ripristineranno anche lo stato di navigazione
/// precedente, che a sua volta fornisce all'oggetto <see cref="Page"/> attivo la possibilità di ripristinare il relativo
/// stato.
/// </summary>
/// <param name="sessionBaseKey">Chiave facoltativa che identifica il tipo di sessione.
/// Può essere utilizzata per distinguere i diversi scenari di avvio dell'applicazione.</param>
/// <returns>Attività asincrona che riflette quando lo stato sessione è stato letto.  Non
/// basarsi sul contenuto di <see cref="SessionState"/> finché questa attività
/// non viene completata.</returns>
task<void> SuspensionManager::RestoreAsync(String^ sessionBaseKey)
{
	_sessionState->Clear();

	task<StorageFile^> getFileTask(ApplicationData::Current->LocalFolder->GetFileAsync(StringReference(sessionStateFilename)));
	return getFileTask.then([=](StorageFile^ stateFile)
	{
		task<BasicProperties^> getBasicPropertiesTask(stateFile->GetBasicPropertiesAsync());
		return getBasicPropertiesTask.then([=](BasicProperties^ stateFileProperties)
		{
			auto size = unsigned int(stateFileProperties->Size);
			if (size != stateFileProperties->Size) throw ref new FailureException("Session state larger than 4GB");
			task<IRandomAccessStreamWithContentType^> openReadTask(stateFile->OpenReadAsync());
			return openReadTask.then([=](IRandomAccessStreamWithContentType^ stateFileStream)
			{
				auto stateReader = ref new DataReader(stateFileStream);
				return task<unsigned int>(stateReader->LoadAsync(size)).then([=](unsigned int bytesRead)
				{
					(void) bytesRead; // Parametro non utilizzato
					// Deserializzare lo stato sessione
					Object^ content = ObjectSerializeHelper::ReadObject(stateReader);
					_sessionState = (Map<String^, Object^>^)content;

					// Ripristinare tutti i frame registrati al relativo stato salvato
					for (auto && weakFrame : _registeredFrames)
					{
						auto frame = weakFrame.Resolve<Frame>();
						if (frame != nullptr && safe_cast<String^>(frame->GetValue(FrameSessionBaseKeyProperty)) == sessionBaseKey)
						{
							frame->ClearValue(FrameSessionStateProperty);
							RestoreFrameNavigationState(frame);
						}
					}
				}, task_continuation_context::use_current());
			});
		});
	});
}

#pragma region Object serialization for a known set of types

void ObjectSerializeHelper::WriteString(DataWriter^ writer, String^ string)
{
	writer->WriteByte(StringType);
	writer->WriteUInt32(writer->MeasureString(string));
	writer->WriteString(string);
}

void ObjectSerializeHelper::WriteProperty(DataWriter^ writer, IPropertyValue^ propertyValue)
{
	switch (propertyValue->Type)
	{
	case PropertyType::UInt8:
		writer->WriteByte(StreamTypes::UInt8Type);
		writer->WriteByte(propertyValue->GetUInt8());
		return;
	case PropertyType::UInt16:
		writer->WriteByte(StreamTypes::UInt16Type);
		writer->WriteUInt16(propertyValue->GetUInt16());
		return;
	case PropertyType::UInt32:
		writer->WriteByte(StreamTypes::UInt32Type);
		writer->WriteUInt32(propertyValue->GetUInt32());
		return;
	case PropertyType::UInt64:
		writer->WriteByte(StreamTypes::UInt64Type);
		writer->WriteUInt64(propertyValue->GetUInt64());
		return;
	case PropertyType::Int16:
		writer->WriteByte(StreamTypes::Int16Type);
		writer->WriteUInt16(propertyValue->GetInt16());
		return;
	case PropertyType::Int32:
		writer->WriteByte(StreamTypes::Int32Type);
		writer->WriteUInt32(propertyValue->GetInt32());
		return;
	case PropertyType::Int64:
		writer->WriteByte(StreamTypes::Int64Type);
		writer->WriteUInt64(propertyValue->GetInt64());
		return;
	case PropertyType::Single:
		writer->WriteByte(StreamTypes::SingleType);
		writer->WriteSingle(propertyValue->GetSingle());
		return;
	case PropertyType::Double:
		writer->WriteByte(StreamTypes::DoubleType);
		writer->WriteDouble(propertyValue->GetDouble());
		return;
	case PropertyType::Boolean:
		writer->WriteByte(StreamTypes::BooleanType);
		writer->WriteBoolean(propertyValue->GetBoolean());
		return;
	case PropertyType::Char16:
		writer->WriteByte(StreamTypes::Char16Type);
		writer->WriteUInt16(propertyValue->GetChar16());
		return;
	case PropertyType::Guid:
		writer->WriteByte(StreamTypes::GuidType);
		writer->WriteGuid(propertyValue->GetGuid());
		return;
	case PropertyType::String:
		WriteString(writer, propertyValue->GetString());
		return;
	default:
		throw ref new InvalidArgumentException("Unsupported property type");
	}
}

void ObjectSerializeHelper::WriteStringToObjectMap(DataWriter^ writer, IMap<String^, Object^>^ map)
{
	writer->WriteByte(StringToObjectMapType);
	writer->WriteUInt32(map->Size);
	for (auto && pair : map)
	{
		WriteObject(writer, pair->Key);
		WriteObject(writer, pair->Value);
	}
	writer->WriteByte(MapEndMarker);
}

void ObjectSerializeHelper::WriteObject(DataWriter^ writer, Object^ object)
{
	if (object == nullptr)
	{
		writer->WriteByte(NullPtrType);
		return;
	}

	auto propertyObject = dynamic_cast<IPropertyValue^>(object);
	if (propertyObject != nullptr)
	{
		WriteProperty(writer, propertyObject);
		return;
	}

	auto mapObject = dynamic_cast<IMap<String^, Object^>^>(object);
	if (mapObject != nullptr)
	{
		WriteStringToObjectMap(writer, mapObject);
		return;
	}

	throw ref new InvalidArgumentException("Unsupported data type");
}

String^ ObjectSerializeHelper::ReadString(DataReader^ reader)
{
	int length = reader->ReadUInt32();
	String^ string = reader->ReadString(length);
	return string;
}

IMap<String^, Object^>^ ObjectSerializeHelper::ReadStringToObjectMap(DataReader^ reader)
{
	auto map = ref new Map<String^, Object^>();
	auto size = reader->ReadUInt32();
	for (unsigned int index = 0; index < size; index++)
	{
		auto key = safe_cast<String^>(ReadObject(reader));
		auto value = ReadObject(reader);
		map->Insert(key, value);
	}
	if (reader->ReadByte() != StreamTypes::MapEndMarker)
	{
		throw ref new InvalidArgumentException("Invalid stream");
	}
	return map;
}

Object^ ObjectSerializeHelper::ReadObject(DataReader^ reader)
{
	auto type = reader->ReadByte();
	switch (type)
	{
	case StreamTypes::NullPtrType:
		return nullptr;
	case StreamTypes::UInt8Type:
		return reader->ReadByte();
	case StreamTypes::UInt16Type:
		return reader->ReadUInt16();
	case StreamTypes::UInt32Type:
		return reader->ReadUInt32();
	case StreamTypes::UInt64Type:
		return reader->ReadUInt64();
	case StreamTypes::Int16Type:
		return reader->ReadInt16();
	case StreamTypes::Int32Type:
		return reader->ReadInt32();
	case StreamTypes::Int64Type:
		return reader->ReadInt64();
	case StreamTypes::SingleType:
		return reader->ReadSingle();
	case StreamTypes::DoubleType:
		return reader->ReadDouble();
	case StreamTypes::BooleanType:
		return reader->ReadBoolean();
	case StreamTypes::Char16Type:
		return (char16_t) reader->ReadUInt16();
	case StreamTypes::GuidType:
		return reader->ReadGuid();
	case StreamTypes::StringType:
		return ReadString(reader);
	case StreamTypes::StringToObjectMapType:
		return ReadStringToObjectMap(reader);
	default:
		throw ref new InvalidArgumentException("Unsupported property type");
	}
}

#pragma endregion
