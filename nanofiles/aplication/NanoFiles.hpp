#pragma once

#include "util/FileDatabase.hpp"

class NanoFiles {
public:
	inline static const std::string DEFAULT_SHARED_DIRNAME = "nf-shared";
	inline static std::string sharedDirname = "nf-shared";
	inline static FileDatabase db = FileDatabase("nf-shared");
	/**
	 * Flag para pruebas iniciales, desactivado una vez que la comunicación
	 * cliente-directorio está implementada y probada.
	 */
	inline static bool testMode = false;
};