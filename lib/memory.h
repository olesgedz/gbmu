#ifndef INCLUDED_MEMORY_H
#define INCLUDED_MEMORY_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <map>
struct Dromaius;

#define MEMORY_MAX_SYMBOL_SIZE 100

struct Memory
{
	// Up-reference
	Dromaius *emu;

	// Hardcoded GameBoy bios
	static  uint8_t bios[256];

	enum class MBC {
		NONE,
		MBC1,
		MBC2,
		MBC3,
		MBC4,
		MBC5,
		MMM01,
		OTHER,
	} mbc;

	inline std::string mbcAsString() {
		switch (mbc) {
			case MBC::NONE: return "NONE";
			case MBC::MBC1: return "MBC1";
			case MBC::MBC2: return "MBC2";
			case MBC::MBC3: return "MBC3";
			case MBC::MBC4: return "MBC4";
			case MBC::MBC5: return "MBC5";
			case MBC::MMM01: return "MMM01";
			case MBC::OTHER: return "OTHER";
			default: return "(invalid mbc)";
		}
	}

	typedef struct romheader_s {
		char	gamename[15];
		uint8_t	colorbyte;		// 0x80 = yes
		uint16_t newlicensee;
		uint8_t	sgbfeatures;	// 0x03 = yes
		uint8_t	type;
		uint8_t	romsize;
		uint8_t	ramsize;
		uint8_t	country;		// 0x00 = japan, 0x01 = other
		uint8_t	oldlicensee;
		uint8_t	headersum;
		uint16_t romsum;
	} romheader_t;

	// Dynamic buffer for ROM image
	uint8_t *rom;
	size_t romLen;
	size_t ramSize;

	uint8_t workram[0x2000]; // 8kb
	uint8_t extram[0x2000];
	uint8_t zeropageram[128];

	bool ramEnabled;
	uint8_t bankMode; // 0 = ROM, 1 = RAM
	uint8_t ramBank;
	uint8_t romBank;
	uint8_t rtcReg;
	uint8_t rtc[5];

	// Flags
	bool initialized = false;
	bool romLoaded = false;
	bool biosLoaded = true;

	// map<<pageNr, addr>, symbol>
	std::map<std::pair<uint8_t, uint16_t>, std::string> addrToSymbol;
	// map<symbol, <pageNr, addr>>
	std::map<std::string, std::pair<uint8_t, uint16_t>> symbolToAddr;

	~Memory();

	// TODO: operator[]() overload?
	uint8_t readByte(uint16_t addr);
	uint16_t readWord(uint16_t addr);

	void writeByte(uint8_t b, uint16_t addr);
	void writeWord(uint16_t w, uint16_t addr);

	std::string getRegionName(uint16_t addr);
	std::string getCartridgeTypeString(uint8_t type);
	std::string getCartridgeRomSizeString(uint8_t size);
	std::string getCartridgeRamSizeString(uint8_t size);
	


	void dumpToFile(std::string const &filename);
	void tryParseSymbolsFile(std::string filename);


	std::string getSymbolFromAddress(uint8_t bank, uint16_t addr);
	std::pair<uint8_t, uint16_t> getAddressFromSymbol(uint8_t bank, std::string &symbol);

	bool loadRom(std::string const &filename);
	void unloadRom();
	void initialize();
	void freeBuffers();

};

#endif