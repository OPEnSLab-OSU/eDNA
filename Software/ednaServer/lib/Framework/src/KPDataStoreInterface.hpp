#pragma once
#include <KPFoundation.hpp>

class KPDataStoreInterface : public KPComponent {
public:
	KPDataStoreInterface(const char * name) : KPComponent(name) {}

	// Returns the size remaining of the file
	virtual int loadContentOfFile(const char * filepath, char * buffer, size_t bufferSize,
								  int * charsRemainning = nullptr) = 0;

	virtual int saveContentToFile(const char * filepath, char * buffer, size_t bufferSize,
								  bool replaceContent = false) = 0;
};