#include "MsxTypes.h"
#include "MediaDb.h"
#include "Crc32Calc.h"
#include "TokenExtract.h"
#include "StrcmpNoCase.h"
#include "ArchGlob.h"
#include "Board.h"
#include "Language.h"
#include "SHA1.h"
#include <string.h>
#include <sxmlc.h>
#include <sxmlsearch.h>
#include <stdlib.h>
#include <debugnet.h>
#include <orbisFile.h>
#include <fcntl.h>


#define MAXMAPPERS 10
                            /* ROM mapper types:             */
#define MAP_GEN8     0      /* Generic switch, 8kB pages     */
#define MAP_GEN16    1      /* Generic switch, 16kB pages    */
#define MAP_KONAMI5  2      /* Konami 5000/7000/9000/B000h   */
#define MAP_KONAMI4  3      /* Konami 4000/6000/8000/A000h   */
#define MAP_ASCII8   4      /* ASCII 6000/6800/7000/7800h    */
#define MAP_ASCII16  5      /* ASCII 6000/7000h              */
#define MAP_GMASTER2 6      /* Konami GameMaster2 cartridge  */
#define MAP_FMPAC    7      /* Panasonic FMPAC cartridge     */


int counter=0;
int found=0;
MediaDb *romdb=NULL;
MediaDb *casdb=NULL;
MediaDb *diskdb=NULL;
static RomType  romdbDefaultType = ROM_UNKNOWN;


const char * parseCountryCode(const char *code)
{
    if (strcmp(code,"SE")==0 || strcmp(code,"se")==0) return "Sweden";
    if (strcmp(code,"JP")==0 || strcmp(code,"jp")==0) return "Japan";
    if (strcmp(code,"KR")==0 || strcmp(code,"kr")==0) return "Korea";
    if (strcmp(code,"NL")==0 || strcmp(code,"nl")==0) return "The Netherlands";
    if (strcmp(code,"GB")==0 || strcmp(code,"gb")==0) return "England";
    if (strcmp(code,"FR")==0 || strcmp(code,"fr")==0) return "France";
    if (strcmp(code,"ES")==0 || strcmp(code,"es")==0) return "Spain";
    if (strcmp(code,"BR")==0 || strcmp(code,"br")==0) return "Brazil";
    if (strcmp(code,"MA")==0 || strcmp(code,"ma")==0) return "Arabic";

    return code;
}

RomType mediaDbStringToType(const char* romName)
{
    
    // Megaroms
    if (strcmp(romName, "ASCII16")==0)          return ROM_ASCII16;
    if (strcmp(romName, "ASCII16SRAM2")==0)     return ROM_ASCII16SRAM;
    if (strcmp(romName, "ASCII8")==0)           return ROM_ASCII8;
    if (strcmp(romName, "ASCII8SRAM8")==0)      return ROM_ASCII8SRAM;
    if (strcmp(romName, "KoeiSRAM8")==0)        return ROM_KOEI;
    if (strcmp(romName, "KoeiSRAM32")==0)       return ROM_KOEI;
    if (strcmp(romName, "Konami")==0)           return ROM_KONAMI4;
    if (strcmp(romName, "KonamiSCC")==0)        return ROM_KONAMI5;
    if (strcmp(romName, "MuPack")==0)           return ROM_MUPACK;
    if (strcmp(romName, "Manbow2")==0)          return ROM_MANBOW2;
    if (strcmp(romName, "Manbow2v2")==0)        return ROM_MANBOW2_V2;
    if (strcmp(romName, "HamarajaNight")==0)    return ROM_HAMARAJANIGHT;
    if (strcmp(romName, "MegaFlashRomScc")==0)  return ROM_MEGAFLSHSCC;
    if (strcmp(romName, "MegaFlashRomSccPlus")==0) return ROM_MEGAFLSHSCCPLUS;
    if (strcmp(romName, "Halnote")==0)          return ROM_HALNOTE;
    if (strcmp(romName, "HarryFox")==0)         return ROM_HARRYFOX;
    if (strcmp(romName, "Playball")==0)         return ROM_PLAYBALL;
    if (strcmp(romName, "Dooly")==0)            return ROM_DOOLY;
    if (strcmp(romName, "HolyQuran")==0)        return ROM_HOLYQURAN;
    if (strcmp(romName, "CrossBlaim")==0)       return ROM_CROSSBLAIM;
    if (strcmp(romName, "Zemina80in1")==0)      return ROM_KOREAN80;
    if (strcmp(romName, "Zemina90in1")==0)      return ROM_KOREAN90;
    if (strcmp(romName, "Zemina126in1")==0)     return ROM_KOREAN126;
    if (strcmp(romName, "Wizardry")==0)         return ROM_ASCII8SRAM;
    if (strcmp(romName, "GameMaster2")==0)      return ROM_GAMEMASTER2;
    if (strcmp(romName, "SuperLodeRunner")==0)  return ROM_LODERUNNER;
    if (strcmp(romName, "R-Type")==0)           return ROM_RTYPE;
    if (strcmp(romName, "Majutsushi")==0)       return ROM_MAJUTSUSHI;
    if (strcmp(romName, "Synthesizer")==0)      return ROM_KONAMISYNTH;
    if (strcmp(romName, "KeyboardMaster")==0)   return ROM_KONAMKBDMAS;
    if (strcmp(romName, "GenericKonami")==0)    return ROM_KONAMI4NF;
    if (strcmp(romName, "SuperPierrot")==0)     return ROM_ASCII16NF;
    if (strcmp(romName, "WordPro")==0)          return ROM_KONWORDPRO;
    if (strcmp(romName, "Normal")==0)           return ROM_STANDARD;
    if (strcmp(romName, "MatraInk")==0)         return ROM_MATRAINK;
    if (strcmp(romName, "NettouYakyuu")==0)     return ROM_NETTOUYAKYUU;

    // System roms
    if (strcmp(romName, "Bunsetsu")==0)     return ROM_BUNSETU;
    if (strcmp(romName, "CasPatch")==0)     return ROM_CASPATCH;
    if (strcmp(romName, "Coleco")==0)       return ROM_COLECO;
    if (strcmp(romName, "MegaCart")==0)     return ROM_CVMEGACART;
    if (strcmp(romName, "ActivisionPCB")==0) return ROM_ACTIVISIONPCB;
    if (strcmp(romName, "ActivisionPCB 2K")==0) return ROM_ACTIVISIONPCB_2K;
    if (strcmp(romName, "ActivisionPCB 16K")==0) return ROM_ACTIVISIONPCB_16K;
    if (strcmp(romName, "ActivisionPCB 256K")==0) return ROM_ACTIVISIONPCB_256K;
    if (strcmp(romName, "SG1000")==0)       return ROM_SG1000;
    if (strcmp(romName, "SC3000")==0)       return ROM_SC3000;
    if (strcmp(romName, "SG1000Castle")==0) return ROM_SG1000CASTLE;
    if (strcmp(romName, "SG1000RamA")==0)   return ROM_SG1000_RAMEXPANDER_A;
    if (strcmp(romName, "SG1000RamB")==0)   return ROM_SG1000_RAMEXPANDER_B;
    if (strcmp(romName, "SegaBasic")==0)    return ROM_SEGABASIC;
    if (strcmp(romName, "FMPAC")==0)        return ROM_FMPAC;
    if (strcmp(romName, "FMPAK")==0)        return ROM_FMPAK;
    if (strcmp(romName, "DiskPatch")==0)    return ROM_DISKPATCH;
    if (strcmp(romName, "Jisyo")==0)        return ROM_JISYO;
    if (strcmp(romName, "Kanji1")==0)       return ROM_KANJI;
    if (strcmp(romName, "Kanji12")==0)      return ROM_KANJI12;
    if (strcmp(romName, "MB8877A")==0)      return ROM_NATIONALFDC;
    if (strcmp(romName, "SVI707FDC")==0)    return ROM_SVI707FDC;
    if (strcmp(romName, "SVI738FDC")==0)    return ROM_SVI738FDC;
    if (strcmp(romName, "TC8566AF")==0)     return ROM_TC8566AF;
    if (strcmp(romName, "TC8566AFTR")==0)   return ROM_TC8566AF_TR;
    if (strcmp(romName, "WD2793")==0)       return ROM_PHILIPSFDC;
    if (strcmp(romName, "Microsol")==0)     return ROM_MICROSOL;
    if (strcmp(romName, "ARC")==0)          return ROM_ARC;
    if (strcmp(romName, "MoonSound")==0)    return ROM_MOONSOUND;

    if (strcmp(romName, "SunriseIDE")==0)   return ROM_SUNRISEIDE;
    if (strcmp(romName, "BeerIDE")==0)      return ROM_BEERIDE;
    if (strcmp(romName, "GIDE")==0)         return ROM_GIDE;
    if (strcmp(romName, "GoudaSCSI")==0)    return ROM_GOUDASCSI;

    if (strcmp(romName, "NMS1210")==0)      return ROM_NMS1210;

    if (strcmp(romName, "fsa1fm2")==0)      return ROM_PANASONIC8;
    if (strcmp(romName, "FSA1FM2")==0)      return ROM_PANASONIC8;
    if (strcmp(romName, "Panasonic8")==0)   return ROM_PANASONIC8;
    if (strcmp(romName, "PanasonicDram")==0)return ROM_DRAM;
    if (strcmp(romName, "PanasonicWx16")==0)return ROM_PANASONICWX16;
    if (strcmp(romName, "Panasonic16")==0)  return ROM_PANASONIC16;
    if (strcmp(romName, "Panasonic32")==0)  return ROM_PANASONIC32;
    if (strcmp(romName, "A1FMModem")==0)    return ROM_FSA1FMMODEM;
    if (strcmp(romName, "fsa1fm1")==0)      return ROM_FSA1FMMODEM;
    if (strcmp(romName, "FSA1FM1")==0)      return ROM_FSA1FMMODEM;
    if (strcmp(romName, "Standard16K")==0)  return ROM_MSXDOS2;
    if (strcmp(romName, "SVI328CART")==0)   return ROM_SVI328CART;
    if (strcmp(romName, "SVI328COL80")==0)  return ROM_SVI328COL80;
    if (strcmp(romName, "SVI727COL80")==0)  return ROM_SVI727COL80;
    if (strcmp(romName, "SVI707FDC")==0)    return ROM_SVI707FDC;
    if (strcmp(romName, "SVI738FDC")==0)    return ROM_SVI738FDC;
    if (strcmp(romName, "MSX-AUDIO")==0)    return ROM_MSXAUDIO;
    if (strcmp(romName, "MSX-MUSIC")==0)    return ROM_MSXMUSIC;
    if (strcmp(romName, "National")==0)     return ROM_NATIONAL;
    if (strcmp(romName, "CX5M-MUSIC")==0)   return ROM_YAMAHASFG05;
    if (strcmp(romName, "VMX80")==0)        return ROM_MICROSOL80;
    if (strcmp(romName, "HBI-V1")==0)       return ROM_SONYHBIV1;
    if (strcmp(romName, "SFG-01")==0)       return ROM_YAMAHASFG01;
    if (strcmp(romName, "SFG-05")==0)       return ROM_YAMAHASFG05;
    if (strcmp(romName, "NET")==0)          return ROM_YAMAHANET;
    if (strcmp(romName, "SF-7000IPL")==0)   return ROM_SF7000IPL;
    if (strcmp(romName, "FMDAS")==0)        return ROM_FMDAS;
    if (strcmp(romName, "Obsonet")==0)      return ROM_OBSONET;
    if (strcmp(romName, "Dumas")==0)        return ROM_DUMAS;
    if (strcmp(romName, "NoWind")==0)       return ROM_NOWIND;

    // Roms not supproted in this format in the db
    if (strcmp(romName, "0x4000")==0)       return ROM_0x4000;
    if (strcmp(romName, "0xC000")==0)       return ROM_0xC000;
    if (strcmp(romName, "auto")==0)         return ROM_PLAIN;
    if (strcmp(romName, "basic")==0)        return ROM_BASIC;

    if (strcmp(romName, "mirrored")==0)     return ROM_PLAIN;
    if (strcmp(romName, "forteII")==0)      return ROM_FORTEII;
    if (strcmp(romName, "msxdos2")==0)      return ROM_MSXDOS2;
    if (strcmp(romName, "konami5")==0)      return ROM_KONAMI5;
    if (strcmp(romName, "MuPack")==0)       return ROM_MUPACK;
    if (strcmp(romName, "konami4")==0)      return ROM_KONAMI4;
    if (strcmp(romName, "ascii8")==0)       return ROM_ASCII8;
    if (strcmp(romName, "halnote")==0)      return ROM_HALNOTE;
    if (strcmp(romName, "konamisynth")==0)  return ROM_KONAMISYNTH;
    if (strcmp(romName, "kbdmaster")==0)    return ROM_KONAMKBDMAS;
    if (strcmp(romName, "majutsushi")==0)   return ROM_MAJUTSUSHI;
    if (strcmp(romName, "ascii16")==0)      return ROM_ASCII16;
    if (strcmp(romName, "gamemaster2")==0)  return ROM_GAMEMASTER2;
    if (strcmp(romName, "ascii8sram")==0)   return ROM_ASCII8SRAM;
    if (strcmp(romName, "koei")==0)         return ROM_KOEI;
    if (strcmp(romName, "ascii16sram")==0)  return ROM_ASCII16SRAM;
    if (strcmp(romName, "konami4nf")==0)    return ROM_KONAMI4NF;
    if (strcmp(romName, "ascii16nf")==0)    return ROM_ASCII16NF;
    if (strcmp(romName, "snatcher")==0)     return ROM_SNATCHER;
    if (strcmp(romName, "sdsnatcher")==0)   return ROM_SDSNATCHER;
    if (strcmp(romName, "sccmirrored")==0)  return ROM_SCCMIRRORED;
    if (strcmp(romName, "sccexpanded")==0)  return ROM_SCCEXTENDED;
    if (strcmp(romName, "scc")==0)          return ROM_SCC;
    if (strcmp(romName, "sccplus")==0)      return ROM_SCCPLUS;
    if (strcmp(romName, "scc-i")==0)        return ROM_SCCPLUS;
    if (strcmp(romName, "scc+")==0)         return ROM_SCCPLUS;
    if (strcmp(romName, "pac")==0)          return ROM_PAC;
    if (strcmp(romName, "fmpac")==0)        return ROM_FMPAC;
    if (strcmp(romName, "fmpak")==0)        return ROM_FMPAK;
    if (strcmp(romName, "rtype")==0)        return ROM_RTYPE;
    if (strcmp(romName, "crossblaim")==0)   return ROM_CROSSBLAIM;
    if (strcmp(romName, "harryfox")==0)     return ROM_HARRYFOX;
    if (strcmp(romName, "loderunner")==0)   return ROM_LODERUNNER;
    if (strcmp(romName, "korean80")==0)     return ROM_KOREAN80;
    if (strcmp(romName, "korean90")==0)     return ROM_KOREAN90;
    if (strcmp(romName, "korean126")==0)    return ROM_KOREAN126;
    if (strcmp(romName, "holyquran")==0)    return ROM_HOLYQURAN;  
    if (strcmp(romName, "opcodesave")==0)   return ROM_OPCODESAVE;
    if (strcmp(romName, "opcodebios")==0)   return ROM_OPCODEBIOS;
    if (strcmp(romName, "opcodeslot")==0)   return ROM_OPCODESLOT;
    if (strcmp(romName, "opcodeega")==0)    return ROM_OPCODEMEGA;
    if (strcmp(romName, "coleco")==0)       return ROM_COLECO;
    if (strcmp(romName, "msxbus")==0)      return ROM_MSXBUS;

    // SG-1000 roms
    if (strcmp(romName, "sg1000")==0)       return ROM_SG1000;
    if (strcmp(romName, "castle")==0)       return ROM_SG1000CASTLE;
    if (strcmp(romName, "sg1000ramA")==0)   return ROM_SG1000_RAMEXPANDER_A;
    if (strcmp(romName, "sg1000ramB")==0)   return ROM_SG1000_RAMEXPANDER_B;
    if (strcmp(romName, "sg1000castle")==0) return ROM_SG1000CASTLE;


    return ROM_UNKNOWN;
}
const char* romTypeToString(RomType romType)
{
    switch (romType) {    
    case ROM_STANDARD:    return langRomTypeStandard();
    case ROM_MSXDOS2:     return langRomTypeMsxdos2();
    case ROM_KONAMI5:     return langRomTypeKonamiScc();
    case ROM_MUPACK:      return "Mu-Pack";
    case ROM_MANBOW2:     return langRomTypeManbow2();
    case ROM_HAMARAJANIGHT:return "Hamaraja Night";
    case ROM_MANBOW2_V2:  return "Manbow 2 v2";
    case ROM_MEGAFLSHSCC: return langRomTypeMegaFlashRomScc();
    case ROM_MEGAFLSHSCCPLUS: return "Mega Flash Rom SCC+";
    case ROM_OBSONET:     return langRomTypeObsonet();
    case ROM_DUMAS:       return langRomTypeDumas();
    case ROM_NOWIND:      return langRomTypeNoWind();
    case ROM_KONAMI4:     return langRomTypeKonami();
    case ROM_ASCII8:      return langRomTypeAscii8();
    case ROM_ASCII16:     return langRomTypeAscii16();
    case ROM_GAMEMASTER2: return langRomTypeGameMaster2();
    case ROM_ASCII8SRAM:  return langRomTypeAscii8Sram();
    case ROM_ASCII16SRAM: return langRomTypeAscii16Sram();
    case ROM_RTYPE:       return langRomTypeRtype();
    case ROM_CROSSBLAIM:  return langRomTypeCrossblaim();
    case ROM_HARRYFOX:    return langRomTypeHarryFox();
    case ROM_MAJUTSUSHI:  return langRomTypeMajutsushi();
    case ROM_KOREAN80:    return langRomTypeZenima80();
    case ROM_KOREAN90:    return langRomTypeZenima90();
    case ROM_KOREAN126:   return langRomTypeZenima126();
    case ROM_SCC:         return langRomTypeScc();
    case ROM_SCCPLUS:     return langRomTypeSccPlus();
    case ROM_SNATCHER:    return langRomTypeSnatcher();
    case ROM_SDSNATCHER:  return langRomTypeSdSnatcher();
    case ROM_SCCMIRRORED: return langRomTypeSccMirrored();
    case ROM_SCCEXTENDED: return langRomTypeSccExtended();
    case ROM_FMPAC:       return langRomTypeFmpac();
    case ROM_FMPAK:       return langRomTypeFmpak();
    case ROM_KONAMI4NF:   return langRomTypeKonamiGeneric();
    case ROM_ASCII16NF:   return langRomTypeSuperPierrot();
    case ROM_PLAIN:       return langRomTypeMirrored();
    case ROM_NETTOUYAKYUU:return "Jaleco Moero!! Nettou Yakyuu '88";
    case ROM_MATRAINK:    return "Matra INK";
    case ROM_FORTEII:     return "Forte II";
    case ROM_NORMAL:      return langRomTypeNormal();
    case ROM_DISKPATCH:   return langRomTypeDiskPatch();
    case ROM_CASPATCH:    return langRomTypeCasPatch();
    case ROM_TC8566AF:    return langRomTypeTc8566afFdc();
    case ROM_TC8566AF_TR: return langRomTypeTc8566afTrFdc();
    case ROM_MICROSOL:    return langRomTypeMicrosolFdc();
    case ROM_ARC:         return "Parallax ARC";
    case ROM_NATIONALFDC: return langRomTypeNationalFdc();
    case ROM_PHILIPSFDC:  return langRomTypePhilipsFdc();
    case ROM_SVI707FDC:   return langRomTypeSvi707Fdc();
    case ROM_SVI738FDC:   return langRomTypeSvi738Fdc();
    case RAM_MAPPER:      return langRomTypeMappedRam();
    case RAM_1KB_MIRRORED:return langRomTypeMirroredRam1k();
    case RAM_2KB_MIRRORED:return langRomTypeMirroredRam2k();
    case RAM_NORMAL:      return langRomTypeNormalRam();
    case ROM_KANJI:       return langRomTypeKanji();
    case ROM_HOLYQURAN:   return langRomTypeHolyQuran();
    case SRAM_MATSUCHITA: return langRomTypeMatsushitaSram();
    case SRAM_MATSUCHITA_INV: return langRomTypeMasushitaSramInv();
    case ROM_PANASONIC8:  return langRomTypePanasonic8();
    case ROM_PANASONICWX16:return langRomTypePanasonicWx16();
    case ROM_PANASONIC16: return langRomTypePanasonic16();
    case ROM_PANASONIC32: return langRomTypePanasonic32();
    case ROM_FSA1FMMODEM: return langRomTypePanasonicModem();
    case ROM_DRAM:        return langRomTypeDram();
    case ROM_BUNSETU:     return langRomTypeBunsetsu();
    case ROM_JISYO:       return langRomTypeJisyo();
    case ROM_KANJI12:     return langRomTypeKanji12();
    case ROM_NATIONAL:    return langRomTypeNationalSram();
    case SRAM_S1985:      return langRomTypeS1985();
    case ROM_S1990:       return langRomTypeS1990();
    case ROM_TURBORIO:    return langRomTypeTurborPause();
    case ROM_F4DEVICE:    return langRomTypeF4deviceNormal();
    case ROM_F4INVERTED:  return langRomTypeF4deviceInvert();
    case ROM_MSXMIDI:     return langRomTypeMsxMidi();
    case ROM_MSXMIDI_EXTERNAL: return langRomTypeMsxMidiExternal();
    case ROM_TURBORTIMER: return langRomTypeTurborTimer();
    case ROM_KOEI:        return langRomTypeKoei();
    case ROM_BASIC:       return langRomTypeBasic();
    case ROM_HALNOTE:     return langRomTypeHalnote();
    case ROM_LODERUNNER:  return langRomTypeLodeRunner();
    case ROM_0x4000:      return langRomTypeNormal4000();
    case ROM_0xC000:      return langRomTypeNormalC000();
    case ROM_KONAMISYNTH: return langRomTypeKonamiSynth();
    case ROM_KONAMKBDMAS: return langRomTypeKonamiKbdMast();
    case ROM_KONWORDPRO:  return langRomTypeKonamiWordPro();
    case ROM_PAC:         return langRomTypePac();
    case ROM_MEGARAM:     return langRomTypeMegaRam();
    case ROM_MEGARAM128:  return langRomTypeMegaRam128();
    case ROM_MEGARAM256:  return langRomTypeMegaRam256();
    case ROM_MEGARAM512:  return langRomTypeMegaRam512();
    case ROM_MEGARAM768:  return langRomTypeMegaRam768();
    case ROM_MEGARAM2M:   return langRomTypeMegaRam2mb();
    case ROM_EXTRAM:      return langRomTypeExtRam();
    case ROM_EXTRAM16KB:  return langRomTypeExtRam16();
    case ROM_EXTRAM32KB:  return langRomTypeExtRam32();
    case ROM_EXTRAM48KB:  return langRomTypeExtRam48();
    case ROM_EXTRAM64KB:  return langRomTypeExtRam64();
    case ROM_EXTRAM512KB: return langRomTypeExtRam512();
    case ROM_EXTRAM1MB:   return langRomTypeExtRam1mb();
    case ROM_EXTRAM2MB:   return langRomTypeExtRam2mb();
    case ROM_EXTRAM4MB:   return langRomTypeExtRam4mb();
    case ROM_MSXMUSIC:    return langRomTypeMsxMusic();
    case ROM_MSXAUDIO:    return langRomTypeMsxAudio();
    case ROM_MOONSOUND:   return langRomTypeMoonsound();
    case ROM_SVI328CART:  return langRomTypeSvi328Cart();
    case ROM_SVI328FDC:   return langRomTypeSvi328Fdc();
    case ROM_SVI328PRN:   return langRomTypeSvi328Prn();
    case ROM_SVI328RS232: return langRomTypeSvi328Uart();
    case ROM_SVI328COL80: return langRomTypeSvi328col80();
    case ROM_SVI328RSIDE: return langRomTypeSvi328RsIde();
    case ROM_SVI727COL80: return langRomTypeSvi727col80();
    case ROM_COLECO:      return langRomTypeColecoCart();
    case ROM_SG1000:      return langRomTypeSg1000Cart();
    case ROM_SC3000:      return langRomTypeSc3000Cart();
    case ROM_SG1000CASTLE:return langRomTypeTheCastle();
    case ROM_SG1000_RAMEXPANDER_A: return "Sega Ram Expander Type A";
    case ROM_SG1000_RAMEXPANDER_B: return "Sega Ram Expander Type B";
    case ROM_SEGABASIC:   return langRomTypeSegaBasic();
    case ROM_SONYHBI55:   return langRomTypeSonyHbi55();
    case ROM_MSXAUDIODEV: return langRomTypeY8950();
    case ROM_MSXPRN:      return langRomTypeMsxPrinter();
    case ROM_TURBORPCM:   return langRomTypeTurborPcm();
    case ROM_JOYREXPSG:   return "Joyrex PSG";
    case ROM_OPCODEPSG:   return "Opcode PSG";
    case ROM_GAMEREADER:  return langRomTypeGameReader();
    case ROM_SUNRISEIDE:  return langRomTypeSunriseIde();
    case ROM_BEERIDE:     return langRomTypeBeerIde();
    case ROM_NMS1210:     return "Philips NMS1210 Serial Interface";
    case ROM_GIDE:        return langRomTypeGide();
    case ROM_MICROSOL80:  return langRomTypeVmx80();
    case ROM_NMS8280DIGI: return langRomTypeNms8280Digitiz();
    case ROM_SONYHBIV1:   return langRomTypeHbiV1Digitiz();
    case ROM_PLAYBALL:    return langRomTypePlayBall();
    case ROM_DOOLY:       return "Dooly";
    case ROM_FMDAS:       return langRomTypeFmdas();
    case ROM_YAMAHASFG01: return langRomTypeSfg01();
    case ROM_YAMAHASFG05: return langRomTypeSfg05();
    case ROM_YAMAHANET:   return "Yamaha Net";
    case ROM_SF7000IPL:   return "SF-7000 IPL";
    case ROM_OPCODEBIOS:  return "ColecoVision Opcode Bios";
    case ROM_OPCODEMEGA:  return "ColecoVision Opcode MegaRam";
    case ROM_OPCODESAVE:  return "ColecoVision Opcode SaveRam";
    case ROM_OPCODESLOT:  return "ColecoVision Opcode Slot Manager";
    case ROM_CVMEGACART:  return "ColecoVision MegaCart(R)";
    case ROM_ACTIVISIONPCB:  return "Colecovision Activision PCB";
    case ROM_ACTIVISIONPCB_2K:  return "Colecovision Activision PCB 2K";
    case ROM_ACTIVISIONPCB_16K:  return "Colecovision Activision PCB 16K";
    case ROM_ACTIVISIONPCB_256K: return "Colecovision Activision PCB 256K";
    case SRAM_MEGASCSI:   return langRomTypeMegaSCSI();
    case SRAM_MEGASCSI128:return langRomTypeMegaSCSI128();
    case SRAM_MEGASCSI256:return langRomTypeMegaSCSI256();
    case SRAM_MEGASCSI512:return langRomTypeMegaSCSI512();
    case SRAM_MEGASCSI1MB:return langRomTypeMegaSCSI1mb();
    case SRAM_ESERAM:     return langRomTypeEseRam();
    case SRAM_ESERAM128:  return langRomTypeEseRam128();
    case SRAM_ESERAM256:  return langRomTypeEseRam256();
    case SRAM_ESERAM512:  return langRomTypeEseRam512();
    case SRAM_ESERAM1MB:  return langRomTypeEseRam1mb();
    case SRAM_WAVESCSI:   return langRomTypeWaveSCSI();
    case SRAM_WAVESCSI128:return langRomTypeWaveSCSI128();
    case SRAM_WAVESCSI256:return langRomTypeWaveSCSI256();
    case SRAM_WAVESCSI512:return langRomTypeWaveSCSI512();
    case SRAM_WAVESCSI1MB:return langRomTypeWaveSCSI1mb();
    case SRAM_ESESCC:     return langRomTypeEseSCC();
    case SRAM_ESESCC128:  return langRomTypeEseSCC128();
    case SRAM_ESESCC256:  return langRomTypeEseSCC256();
    case SRAM_ESESCC512:  return langRomTypeEseSCC512();
    case ROM_GOUDASCSI:   return langRomTypeGoudaSCSI();
    case ROM_MSXBUS:      return "msxbus";

    case ROM_UNKNOWN:     return langTextUnknown();
    }

    return langTextUnknown();
}

const char* romTypeToShortString(RomType romType) 
{
    switch (romType) {
    case ROM_STANDARD:    return "STANDARD";
    case ROM_MSXDOS2:     return "MSXDOS2";
    case ROM_KONAMI5:     return "KONAMI SCC";
    case ROM_MUPACK:      return "MU-PACK";
    case ROM_MANBOW2:     return "MANBOW 2";
    case ROM_HAMARAJANIGHT:return "HAMARAJANGT";
    case ROM_MANBOW2_V2:  return "MANBOW 2 v2";
    case ROM_MEGAFLSHSCC: return "MEGAFLSHSCC";
    case ROM_MEGAFLSHSCCPLUS: return "MEGAFLSHSCC+";
    case ROM_OBSONET:     return "OBSONET";
    case ROM_DUMAS:       return "DUMAS";
    case ROM_NOWIND:      return "NOWIND";
    case ROM_KONAMI4:     return "KONAMI";
    case ROM_ASCII8:      return "ASCII8";
    case ROM_ASCII16:     return "ASCII16";
    case ROM_GAMEMASTER2: return "GMASTER2";
    case ROM_ASCII8SRAM:  return "ASCII8SRAM";
    case ROM_ASCII16SRAM: return "ASCII16SRAM";
    case ROM_RTYPE:       return "R-TYPE";
    case ROM_CROSSBLAIM:  return "CROSSBLAIM";
    case ROM_HARRYFOX:    return "HARRYFOX";
    case ROM_KOREAN80:    return "ZEM 80IN1";
    case ROM_KOREAN126:   return "ZEM 126IN1";
    case ROM_KOREAN90:    return "ZEM 90IN1";
    case ROM_SCC:         return "SCC";
    case ROM_SCCPLUS:     return "SCC-I";
    case ROM_SNATCHER:    return "SNATCHER";
    case ROM_SDSNATCHER:  return "SDSNATCHER";
    case ROM_SCCMIRRORED: return "SCCMIRRORED";
    case ROM_SCCEXTENDED: return "SCCEXTENDED";
    case ROM_FMPAC:       return "FMPAC";
    case ROM_FMPAK:       return "FMPAK";
    case ROM_KONAMI4NF:   return "KONAMI GEN";
    case ROM_ASCII16NF:   return "SUPERPIERR";
    case ROM_PLAIN:       return "MIRRORED";
    case ROM_NETTOUYAKYUU:return "NETTOU YAKYUU";
    case ROM_MATRAINK:    return "MATRA INK";
    case ROM_FORTEII:     return "FORTE II";
    case ROM_NORMAL:      return "NORMAL";
    case ROM_DRAM:        return "DRAM";
    case ROM_DISKPATCH:   return "DISKPATCH";
    case ROM_CASPATCH:    return "CASPATCH";
    case ROM_TC8566AF:    return "TC8566AF";
    case ROM_TC8566AF_TR: return "TC8566AF";
    case ROM_MICROSOL:    return "MICROSOL";
    case ROM_ARC:         return "ARC";
    case ROM_NATIONALFDC: return "NATNL FDC";
    case ROM_PHILIPSFDC:  return "PHILIPSFDC";
    case ROM_SVI707FDC:   return "SVI707 FDC";
    case ROM_SVI738FDC:   return "SVI738 FDC";
    case RAM_MAPPER:      return "MAPPED RAM";
    case RAM_1KB_MIRRORED:return "1K MIR RAM";
    case RAM_2KB_MIRRORED:return "2K MIR RAM";
    case RAM_NORMAL:      return "NORMAL RAM";
    case ROM_KANJI:       return "KANJI";
    case ROM_HOLYQURAN:   return "HOLYQURAN";
    case SRAM_MATSUCHITA:     return "MATSUSHITA";
    case SRAM_MATSUCHITA_INV: return "MATSUS INV";
    case ROM_PANASONICWX16:   return "PANASON 16";
    case ROM_PANASONIC16: return "PANASON 16";
    case ROM_PANASONIC32: return "PANASON 32";
    case ROM_BUNSETU:     return "BUNSETSU";
    case ROM_JISYO:       return "JISYO";
    case ROM_KANJI12:     return "KANJI12";
    case ROM_NATIONAL:    return "NATIONAL";
    case SRAM_S1985:      return "S1985";
    case ROM_S1990:       return "S1990";
    case ROM_TURBORIO:    return "TR PAUSE";
    case ROM_F4DEVICE:    return "F4NORMAL";
    case ROM_F4INVERTED:  return "F4INV";
    case ROM_MSXMIDI:     return "MSX-MIDI";
    case ROM_MSXMIDI_EXTERNAL: return "MSX-MIDIEX";
    case ROM_TURBORTIMER: return "TURBORTMR";
    case ROM_KOEI:        return "KOEI";
    case ROM_BASIC:       return "BASIC";
    case ROM_HALNOTE:     return "HALNOTE";
    case ROM_LODERUNNER:  return "LODERUNNER";
    case ROM_0x4000:      return "4000h";
    case ROM_0xC000:      return "C000h";
    case ROM_KONAMISYNTH: return "KONSYNTH";
    case ROM_KONAMKBDMAS: return "KBDMASTER";
    case ROM_KONWORDPRO:  return "KONWORDPRO";
    case ROM_MAJUTSUSHI:  return "MAJUTSUSHI";
    case ROM_PAC:         return "PAC";
    case ROM_MEGARAM:     return "MEGARAM";
    case ROM_MEGARAM128:  return "MEGARAM128";
    case ROM_MEGARAM256:  return "MEGARAM256";
    case ROM_MEGARAM512:  return "MEGARAM512";
    case ROM_MEGARAM768:  return "MEGARAM768";
    case ROM_MEGARAM2M:   return "MEGARAM2MB";
    case ROM_EXTRAM:      return "EXTERN RAM";
    case ROM_EXTRAM16KB:  return "EXTRAM 16";
    case ROM_EXTRAM32KB:  return "EXTRAM 32";
    case ROM_EXTRAM48KB:  return "EXTRAM 48";
    case ROM_EXTRAM64KB:  return "EXTRAM 64";
    case ROM_EXTRAM512KB: return "EXTRAM 512";
    case ROM_EXTRAM1MB:   return "EXTRAM 2MB";
    case ROM_EXTRAM2MB:   return "EXTRAM 1MB";
    case ROM_EXTRAM4MB:   return "EXTRAM 4MB";
    case ROM_MSXMUSIC:    return "MSXMUSIC";
    case ROM_MSXAUDIO:    return "MSXAUDIO";
    case ROM_MOONSOUND:   return "MOONSOUND";
    case ROM_SVI328CART:  return "SVI328CART";
    case ROM_SVI328FDC:   return "SVI328FDC";
    case ROM_SVI328PRN:   return "SVI328PRN";
    case ROM_SVI328RS232: return "SVI328RS232";
    case ROM_SVI328COL80: return "SVI328COL80";
    case ROM_SVI328RSIDE: return "SVI328RSIDE";
    case ROM_SVI727COL80: return "SVI727COL80";
    case ROM_COLECO:      return "COLECO";
    case ROM_SG1000:      return "SG-1000";
    case ROM_SC3000:      return "SC-3000";
    case ROM_SG1000CASTLE:return "THECASTLE";
    case ROM_SEGABASIC:   return "SEGABASIC";
    case ROM_SG1000_RAMEXPANDER_A: return "SEGARAM A";
    case ROM_SG1000_RAMEXPANDER_B: return "SEGARAM B";
    case ROM_SONYHBI55:   return "HBI-55";
    case ROM_MSXAUDIODEV: return "MSXAUDIO";
    case ROM_MSXPRN:      return "MSXPRN";
    case ROM_TURBORPCM:   return "TURBOR PCM";
    case ROM_JOYREXPSG:   return "JOYREX PSG";
    case ROM_OPCODEPSG:   return "OPCODE PSG";
    case ROM_OPCODEBIOS:  return "OPCODE BIOS";
    case ROM_OPCODEMEGA:  return "OPCODE MEGA";
    case ROM_OPCODESAVE:  return "OPCODE SAVE";
    case ROM_OPCODESLOT:  return "OPCODE SLOT";
    case ROM_GAMEREADER:  return "GAMEREADER";
    case ROM_SUNRISEIDE:  return "SUNRISEIDE";
    case ROM_BEERIDE:     return "BEER IDE";
    case ROM_NMS1210:     return "NMS1210";
    case ROM_GIDE:        return "GIDE";
    case ROM_MICROSOL80:  return "MICROSOL80";
    case ROM_NMS8280DIGI: return "8280 DIGI";
    case ROM_SONYHBIV1:   return "SONY HBI-V1";
    case ROM_PLAYBALL:    return "PLAYBALL";
    case ROM_DOOLY:       return "DOOLY";
    case ROM_FMDAS:       return "FM-DAS";
    case ROM_YAMAHASFG01: return "SFG-01";
    case ROM_YAMAHASFG05: return "SFG-05";
    case ROM_YAMAHANET:   return "YAMAHA NET";
    case ROM_SF7000IPL:   return "SF-7000 IPL";
    case ROM_CVMEGACART:  return "MEGACART";
    case ROM_ACTIVISIONPCB:     return "ACTIVISION";
    case ROM_ACTIVISIONPCB_2K:  return "ACTIVISN2";
    case ROM_ACTIVISIONPCB_16K: return "ACTIVISI16";
    case ROM_ACTIVISIONPCB_256K:return "ACTIVISI256";
    case SRAM_MEGASCSI:   return "MEGASCSI";
    case SRAM_MEGASCSI128:return "MEGASCSI128";
    case SRAM_MEGASCSI256:return "MEGASCSI256";
    case SRAM_MEGASCSI512:return "MEGASCSI512";
    case SRAM_MEGASCSI1MB:return "MEGASCSI1MB";
    case SRAM_ESERAM:     return "ESE-RAM";
    case SRAM_ESERAM128:  return "ESE-RAM128";
    case SRAM_ESERAM256:  return "ESE-RAM256";
    case SRAM_ESERAM512:  return "ESE-RAM512";
    case SRAM_ESERAM1MB:  return "ESE-RAM1MB";
    case SRAM_WAVESCSI:   return "WAVESCSI";
    case SRAM_WAVESCSI128:return "WAVESCSI128";
    case SRAM_WAVESCSI256:return "WAVESCSI256";
    case SRAM_WAVESCSI512:return "WAVESCSI512";
    case SRAM_WAVESCSI1MB:return "WAVESCSI1MB";
    case SRAM_ESESCC:     return "ESE-SCC";
    case SRAM_ESESCC128:  return "ESE-SCC128";
    case SRAM_ESESCC256:  return "ESE-SCC256";
    case SRAM_ESESCC512:  return "ESE-SCC512";
    case ROM_GOUDASCSI:   return "GOUDA SCSI";

    case ROM_UNKNOWN:     return "UNKNOWN";
    }

    return "UNKNOWN";
}

int romTypeIsRom(RomType romType) {
    switch (romType) {
    case ROM_SCC:         return 1;
    case ROM_SCCPLUS:     return 1;
    case ROM_MOONSOUND:   return 1;
    case ROM_SNATCHER:    return 1;
    case ROM_SDSNATCHER:  return 1;
    case ROM_SCCMIRRORED: return 1;
    case ROM_SCCEXTENDED: return 1;
    case ROM_PLAIN:       return 1;
    case ROM_NETTOUYAKYUU:return 1;
    case ROM_MATRAINK:    return 1;
    case ROM_FORTEII:     return 1;
    case ROM_FMPAK:       return 1;
    case ROM_NORMAL:      return 1;
    case ROM_DRAM:        return 1;
    case ROM_DISKPATCH:   return 1;
    case ROM_CASPATCH:    return 1;
    case ROM_MICROSOL:    return 1;
    case ROM_ARC:         return 1;
    case ROM_NATIONALFDC: return 1;
    case ROM_PHILIPSFDC:  return 1;
    case ROM_SVI707FDC:   return 1;
    case ROM_SVI738FDC:   return 1;
    case ROM_HOLYQURAN:   return 1;
    case SRAM_MATSUCHITA: return 1;
    case SRAM_MATSUCHITA_INV: return 1;
    case ROM_BASIC:       return 1;
    case ROM_0x4000:      return 1;
    case ROM_0xC000:      return 1;
    case ROM_KONAMISYNTH: return 1;
    case ROM_KONAMKBDMAS: return 1;
    case ROM_KONWORDPRO:  return 1;
    case ROM_MICROSOL80:  return 1;
    case ROM_SONYHBIV1:   return 1;
    case ROM_PLAYBALL:    return 1;
    case ROM_DOOLY:       return 1;
    case ROM_FMDAS:       return 1;
    case ROM_YAMAHASFG01: return 1;
    case ROM_YAMAHASFG05: return 1;
    case ROM_SF7000IPL:   return 1;
    case ROM_YAMAHANET:   return 1;
    case ROM_EXTRAM16KB:  return 1;
    case ROM_EXTRAM32KB:  return 1;
    case ROM_EXTRAM48KB:  return 1;
    case ROM_EXTRAM64KB:  return 1;
    }
    return 0;
}

int romTypeIsMegaRom(RomType romType) {
    switch (romType) {
    case ROM_STANDARD:    return 1;
    case ROM_MSXDOS2:     return 1;
    case ROM_KONAMI5:     return 1;
    case ROM_MUPACK:      return 1;
    case ROM_MANBOW2:     return 1;
    case ROM_HAMARAJANIGHT: return 1;
    case ROM_MANBOW2_V2:  return 1;
    case ROM_MEGAFLSHSCC: return 1;
    case ROM_MEGAFLSHSCCPLUS: return 1;
    case ROM_OBSONET:     return 1;
    case ROM_DUMAS:       return 1;
    case ROM_NOWIND:      return 1;
    case ROM_KONAMI4:     return 1;
    case ROM_ASCII8:      return 1;
    case ROM_ASCII16:     return 1;
    case ROM_GAMEMASTER2: return 1;
    case ROM_ASCII8SRAM:  return 1;
    case ROM_TC8566AF:    return 1;
    case ROM_TC8566AF_TR: return 1;
    case ROM_ASCII16SRAM: return 1;
    case ROM_RTYPE:       return 1;
    case ROM_CROSSBLAIM:  return 1;
    case ROM_HARRYFOX:    return 1;
    case ROM_KOREAN80:    return 1;
    case ROM_KOREAN126:   return 1;
    case ROM_KONAMI4NF:   return 1;
    case ROM_ASCII16NF:   return 1;
    case ROM_HOLYQURAN:   return 1;
    case ROM_MAJUTSUSHI:  return 1;
    case ROM_KOEI:        return 1;
    case ROM_HALNOTE:     return 1;
    case ROM_LODERUNNER:  return 1;
    case ROM_MSXAUDIO:    return 1;
    case ROM_KOREAN90:    return 1;
    case ROM_SONYHBI55:   return 1;
    case ROM_EXTRAM512KB: return 1;
    case ROM_EXTRAM1MB:   return 1;
    case ROM_EXTRAM2MB:   return 1;
    case ROM_EXTRAM4MB:   return 1;
    case ROM_GAMEREADER:  return 1;
    case ROM_SUNRISEIDE:  return 1;
    case ROM_BEERIDE:     return 1;
    case SRAM_MEGASCSI:   return 1;
    case SRAM_MEGASCSI128:return 1;
    case SRAM_MEGASCSI256:return 1;
    case SRAM_MEGASCSI512:return 1;
    case SRAM_MEGASCSI1MB:return 1;
    case SRAM_ESERAM:     return 1;
    case SRAM_ESERAM128:  return 1;
    case SRAM_ESERAM256:  return 1;
    case SRAM_ESERAM512:  return 1;
    case SRAM_ESERAM1MB:  return 1;
    case SRAM_WAVESCSI:   return 1;
    case SRAM_WAVESCSI128:return 1;
    case SRAM_WAVESCSI256:return 1;
    case SRAM_WAVESCSI512:return 1;
    case SRAM_WAVESCSI1MB:return 1;
    case SRAM_ESESCC:     return 1;
    case SRAM_ESESCC128:  return 1;
    case SRAM_ESESCC256:  return 1;
    case SRAM_ESESCC512:  return 1;
    }
    return 0;
}

int romTypeIsMegaRam(RomType romType) {
    switch (romType) {
    case ROM_MEGARAM:     return 1;
    case ROM_MEGARAM128:  return 1;
    case ROM_MEGARAM256:  return 1;
    case ROM_MEGARAM512:  return 1;
    case ROM_MEGARAM768:  return 1;
    case ROM_MEGARAM2M:   return 1;
    }
    return 0;
}

int romTypeIsFmPac(RomType romType) {
    switch (romType) {
    case ROM_FMPAC:       return 1;
    }
    return 0;
}
RomType mediaDbGetRomType(MediaType* mediaType)
{
    return mediaType->romType;
}

const char* mediaDbGetTitle(MediaType* mediaType)
{
    return mediaType->title;
}

const char* mediaDbGetYear(MediaType* mediaType)
{
    return mediaType->year;
}

const char* mediaDbGetCompany(MediaType* mediaType)
{
    return mediaType->company;
}

const char* mediaDbGetRemark(MediaType* mediaType)
{
    return mediaType->remark;
}
void mediaDbSetDefaultRomType(RomType romType)
{
    romdbDefaultType = romType;
}

MediaType* mediaDbLookup(MediaDb* mediaDb, const char *buffer,int size)
{
    int i;
    MediaType *it=NULL;
    for(i=0;i<counter;i++)
    {
        if(strcmp(buffer,mediaDb[i].digest)==0)
        {
            debugNetPrintf(DEBUG,"[MSXORBIS] %s found %d\n",__FUNCTION__,i);
            it=mediaDb[i].media;
            break;
        }
    }
    return it;
}

void mediaDbAddItem(int media,char *genmsxid,int romType,char * system,char *title,char * company,char * country,char *year,bool original,char *originalData, char *remark,char *digest,char *startData)
{
    MediaType *mediaItem=malloc(sizeof(MediaType));
    //MediaType mediaItem;
    //MediaDb romdb;
    
    //debugNetPrintf(DEBUG,"[MSXORBIS] %s sizeof mediaItem %lu\n",__FUNCTION__,sizeof(mediaItem));
    //debugNetPrintf(DEBUG,"[MSXORBIS] %s sizeof MediaType %lu\n",__FUNCTION__,sizeof(MediaType));
    //debugNetPrintf(DEBUG,"[MSXORBIS] %s sizeof MediaDb %lu\n",__FUNCTION__,sizeof(MediaDb));
    //debugNetPrintf(DEBUG,"[MSXORBIS] %s sizeof MediaDb %lu\n",__FUNCTION__,sizeof(romdb));
    if(strcmp(system,"MSX")==0)
    {
        mediaItem->genmsxid=atoi(genmsxid);
        mediaItem->romType=romType;
        mediaItem->original=original;
        sprintf(mediaItem->system,"%s",system);
        sprintf(mediaItem->title,"%s",title);
        sprintf(mediaItem->company,"%s",company);
        sprintf(mediaItem->country,"%s",country);
        sprintf(mediaItem->year,"%s",year);
        sprintf(mediaItem->remark,"%s",remark);
        sprintf(mediaItem->originalData,"%s",originalData);
        sprintf(mediaItem->startData,"%s",startData);
    
    
    switch(media)
    {
        case 0:
        /*  debugNetPrintf(DEBUG,"[MSXORBIS] %s digest=%s\n",__FUNCTION__,digest);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s rom\n",__FUNCTION__);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s genmsxid=%d\n",__FUNCTION__,atoi(genmsxid));
            debugNetPrintf(DEBUG,"[MSXORBIS] %s romType=%d\n",__FUNCTION__,romType);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s media=%d\n",__FUNCTION__,media);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s original=%d\n",__FUNCTION__,original);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s system=%s\n",__FUNCTION__,system);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s title=%s\n",__FUNCTION__,title);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s company=%s\n",__FUNCTION__,company);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s country=%s\n",__FUNCTION__,country);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s year=%s\n",__FUNCTION__,year);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s remark=%s\n",__FUNCTION__,remark);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s originalData=%s\n",__FUNCTION__,originalData);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s startData=%s\n",__FUNCTION__,startData);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s ---------------------------\n",__FUNCTION__);*/
            if(romdb!=NULL && mediaItem!=NULL)
            {
                sprintf(romdb[counter].digest,"%s",digest);
                romdb[counter].media=mediaItem;
               // if(strcmp(digest,"2ae97f3c8152f56695c3d3a2dedaa781bf0a15f1")==0)
               // {
               //     found=counter;
               // }
                counter++;
            }
            else
            {
                debugNetPrintf(DEBUG,"[MSXORBIS] %s error malloc\n",__FUNCTION__);
            }
            break;
        case 1:
         /* debugNetPrintf(DEBUG,"[MSXORBIS] %s digest=%s\n",__FUNCTION__,digest);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s systerom\n",__FUNCTION__);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s genmsxid=%d\n",__FUNCTION__,atoi(genmsxid));
            debugNetPrintf(DEBUG,"[MSXORBIS] %s romType=%d\n",__FUNCTION__,romType);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s media=%d\n",__FUNCTION__,media);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s original=%d\n",__FUNCTION__,original);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s system=%s\n",__FUNCTION__,system);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s title=%s\n",__FUNCTION__,title);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s company=%s\n",__FUNCTION__,company);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s country=%s\n",__FUNCTION__,country);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s year=%s\n",__FUNCTION__,year);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s remark=%s\n",__FUNCTION__,remark);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s originalData=%s\n",__FUNCTION__,originalData);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s startData=%s\n",__FUNCTION__,startData);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s ---------------------------\n",__FUNCTION__);*/
            break;
        case 2:
        /*  debugNetPrintf(DEBUG,"[MSXORBIS] %s digest=%s\n",__FUNCTION__,digest);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s dsk\n",__FUNCTION__);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s genmsxid=%d\n",__FUNCTION__,atoi(genmsxid));
            debugNetPrintf(DEBUG,"[MSXORBIS] %s romType=%d\n",__FUNCTION__,romType);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s media=%d\n",__FUNCTION__,media);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s original=%d\n",__FUNCTION__,original);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s system=%s\n",__FUNCTION__,system);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s title=%s\n",__FUNCTION__,title);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s company=%s\n",__FUNCTION__,company);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s country=%s\n",__FUNCTION__,country);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s year=%s\n",__FUNCTION__,year);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s remark=%s\n",__FUNCTION__,remark);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s originalData=%s\n",__FUNCTION__,originalData);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s startData=%s\n",__FUNCTION__,startData);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s ---------------------------\n",__FUNCTION__);*/
            break;
        case 3:
        /*  debugNetPrintf(DEBUG,"[MSXORBIS] %s digest=%s\n",__FUNCTION__,digest);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s cas\n",__FUNCTION__);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s genmsxid=%d\n",__FUNCTION__,atoi(genmsxid));
            debugNetPrintf(DEBUG,"[MSXORBIS] %s romType=%d\n",__FUNCTION__,romType);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s media=%d\n",__FUNCTION__,media);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s original=%d\n",__FUNCTION__,original);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s system=%s\n",__FUNCTION__,system);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s title=%s\n",__FUNCTION__,title);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s company=%s\n",__FUNCTION__,company);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s country=%s\n",__FUNCTION__,country);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s year=%s\n",__FUNCTION__,year);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s remark=%s\n",__FUNCTION__,remark);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s originalData=%s\n",__FUNCTION__,originalData);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s startData=%s\n",__FUNCTION__,startData);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s ---------------------------\n",__FUNCTION__);*/
            break;
        case 4:
        /*  debugNetPrintf(DEBUG,"[MSXORBIS] %s digest=%s\n",__FUNCTION__,digest);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s caswav\n",__FUNCTION__);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s genmsxid=%d\n",__FUNCTION__,atoi(genmsxid));
            debugNetPrintf(DEBUG,"[MSXORBIS] %s romType=%d\n",__FUNCTION__,romType);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s media=%d\n",__FUNCTION__,media);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s original=%d\n",__FUNCTION__,original);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s system=%s\n",__FUNCTION__,system);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s title=%s\n",__FUNCTION__,title);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s company=%s\n",__FUNCTION__,company);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s country=%s\n",__FUNCTION__,country);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s year=%s\n",__FUNCTION__,year);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s remark=%s\n",__FUNCTION__,remark);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s originalData=%s\n",__FUNCTION__,originalData);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s startData=%s\n",__FUNCTION__,startData);
            debugNetPrintf(DEBUG,"[MSXORBIS] %s ---------------------------\n",__FUNCTION__);*/
            break;
        default:
            debugNetPrintf(ERROR,"[MSXORBIS] %s error\n",__FUNCTION__);
            break;
    }
    }
}

void mediaDbAddDump(XMLNode* dmpchild,char * genmsxid, char *system,char *title,char * company,char * country,char *year,bool original,char *originalData)
{
    int i;
    RomType romType;
    char startData[22]="";
    char remark[181]="";
    char digest[41];
    XMLNode *romchild;
    XMLNode *caschild;
    XMLNode *dskchild;
    bool flagStart,flagRemark;
    if(strcmp(dmpchild->tag,"megarom") == 0 || strcmp(dmpchild->tag,"systemrom") == 0 || strcmp(dmpchild->tag,"rom") == 0) 
    {
        RomType romType=strcmp(dmpchild->tag,"rom") == 0 ? ROM_PLAIN : ROM_UNKNOWN;
        
        for(i=0;i<dmpchild->n_children;i++)
        {
            romchild=XMLNode_get_child(dmpchild,i);
        
            if(strcmp(romchild->tag,"type")==0)
            {
                romType=mediaDbStringToType(romchild->text);
            }
            if(strcmp(romchild->tag,"start")==0)
            {
                sprintf(startData,"%s",romchild->text);
            }
            if(strcmp(romchild->tag,"remark")==0)
            {
                sprintf(remark,"%s",romchild->text);
            }
            if(strcmp(romchild->tag,"hash")==0)
            {
                sprintf(digest,"%s",romchild->text);
            }
            
        }
        if(romType!=ROM_CVMEGACART && romType!=ROM_ACTIVISIONPCB && romType!=ROM_ACTIVISIONPCB_2K && romType!=ROM_ACTIVISIONPCB_16K && romType!=ROM_ACTIVISIONPCB_256K)
        {
            if(strcmpnocase(system,"coleco")==0)
            {
                romType=ROM_COLECO;
            }
        }
        if(strcmpnocase(system,"svi")==0)
        {
            if(romType!=ROM_SVI328COL80)
            {
                romType=ROM_SVI328CART;
            }
        }
        
        if(romType!= ROM_SG1000CASTLE && romType!=ROM_SEGABASIC && romType!=ROM_SG1000_RAMEXPANDER_A && romType!=ROM_SG1000_RAMEXPANDER_B) 
        {
            if(strcmpnocase(system,"sg1000")==0)
            {
                romType=ROM_SG1000;
            }
            if(strcmpnocase(system,"sc3000")==0 || strcmpnocase(system,"sf7000")==0)
            {
                romType=ROM_SC3000;
            }
        }
        // For standard roms, a start tag is used to specify start address
        if(romType==ROM_STANDARD)
        {
            if (strcmp(startData,"")!=0) 
            {
                if (strcmp(startData,"0x0000")==0)
                {
                    romType=ROM_STANDARD;
                }
                if(strcmp(startData,"0x4000")==0)
                {
                    romType=ROM_0x4000;
                }
                if(strcmp(startData,"0x8000")==0)
                {
                    romType=ROM_BASIC;
                }
                if(strcmp(startData,"0xC000")==0) 
                {
                    romType=ROM_0xC000;
                }
            }
        }
        mediaDbAddItem(0,genmsxid,romType,system,title,company,country,year,original,originalData,remark,digest,startData);
            
    }
    if(strcmp(dmpchild->tag, "sccpluscart") == 0)
    {
        RomType romType=ROM_SCC;
        for(i=0;i<dmpchild->n_children;i++)
        {
            romchild=XMLNode_get_child(dmpchild,i);
            if(strcmp(romchild->tag,"boot")==0)
            {
                if(strcmp(romchild->text,"scc+")==0)
                {
                    romType = ROM_SCCPLUS;
                }
            }
            if(strcmp(romchild->tag,"remark")==0)
            {
                sprintf(remark,"%s",romchild->text);
            }
            if(strcmp(romchild->tag,"hash")==0)
            {
                sprintf(digest,"%s",romchild->text);
            }
        }
        
        mediaDbAddItem(0,genmsxid,romType,system,title,company,country,year,original,originalData,remark,digest,startData);
        
    }
    if(strcmp(dmpchild->tag, "cas") == 0)
    {
        flagStart=false;
        flagRemark=false;
        XMLNode *child;
        int type;
        printf("children cas %d\n",dmpchild->n_children);
        for(i=0;i<dmpchild->n_children;i++)
        {
            caschild=XMLNode_get_child(dmpchild,i);
            if(strcmp(caschild->tag,"start")==0)
            {
                child=XMLNode_get_child(caschild,0);
                if(strcmp(child->tag,"text")==0)
                {
                    sprintf(startData,"%s",child->text);
                    
                }
                flagStart=true;
            }
            if(strcmp(caschild->tag,"remark")==0)
            {
                flagRemark=true;
                child=XMLNode_get_child(caschild,0);
                if(strcmp(child->tag,"text")==0)
                {
                    sprintf(remark,"%s",child->text);
                }
                
            }
        }
        for(i=0;i<dmpchild->n_children;i++)
        {
            caschild=XMLNode_get_child(dmpchild,i);
            
            if(strcmp(caschild->tag,"format")==0)
            {
                char digestcas[41];
                
                child=XMLNode_get_child(caschild,0);
                SXML_CHAR *value;
                type=0;
                XMLNode_get_attribute(caschild, C2SX("type"), &value);
                if(strcmp(value,"cas")==0)
                {
                    type=0;
                }
                if(strcmp(value,"wav")==0)
                {
                    type=1;
                }       
                if(strcmp(child->tag,"hash")==0)
                {
                    printf("%d %s\n",type,child->text);
                    sprintf(digestcas,"%s",child->text);
                }
            
            
                
                mediaDbAddItem(type?4:3,genmsxid,ROM_UNKNOWN,system,title,company,country,year,original,originalData,flagRemark?remark:"",digestcas,flagStart?startData:"");
                
            }
        }
    }
    if (strcmp(dmpchild->tag, "dsk") == 0)
    {
        XMLNode *child;
        for(i=0;i<dmpchild->n_children;i++)
        {
            dskchild=XMLNode_get_child(dmpchild,i);
            if(strcmp(dskchild->tag,"start")==0)
            {
                child=XMLNode_get_child(dskchild,0);
                if(strcmp(child->tag,"text")==0)
                {
                    sprintf(startData,"%s",child->text);
                }
                
            }
            if(strcmp(dskchild->tag,"format")==0)
            {
                child=XMLNode_get_child(dskchild,0);
                SXML_CHAR value[20];
                if(strcmp(child->tag,"hash")==0)
                {
                    sprintf(digest,"%s",child->text);
                }
                
            }
            if(strcmp(dskchild->tag,"remark")==0)
            {
                child=XMLNode_get_child(dskchild,0);
                if(strcmp(child->tag,"text")==0)
                {
                    sprintf(remark,"%s",child->text);
                }
                
            }
        }
        mediaDbAddItem(2,genmsxid,ROM_UNKNOWN,system,title,company,country,year,original,originalData,remark,digest,startData);
        
    }
    
}
void mediaDbAddFromXmlFile(const char *filename)
{
    int ret;
    int i,j,k,l, len;
    
    SXML_CHAR* p;
    int f = orbisOpen(filename,O_RDONLY,0);
    if(f<=0)
    {
        debugNetPrintf(ERROR,"[MSXORBIS] %s error opening xml db filename %s\n",__FUNCTION__,filename);
        return;
    }

    len = orbisLseek(f,0,SEEK_END);
    orbisLseek(f,0,SEEK_SET);
    p = (SXML_CHAR*)malloc(len+sizeof(SXML_CHAR));
    if(p==NULL)
    {
        debugNetPrintf(ERROR,"[MSXORBIS] %s error in malloc\n",__FUNCTION__);
        orbisClose(f);
        return;
    }
    if(orbisRead(f,p,len)!=len)
    {
        debugNetPrintf(ERROR,"[MSXORBIS] %s error reading xml db filename %s\n",__FUNCTION__,filename);
        orbisClose(f);
        free(p);
        return;
    }
    orbisClose(f);
    p[len] = 0;
    XMLDoc doc;
    XMLSearch search;
    XMLSearch search1;
    
    XMLNode* nodesoftwaredb;
    XMLNode* childnodesoftware;
    XMLNode* childnode;
    XMLNode* dumpchildnode;
    XMLNode* groupchildnode;
    
    SXML_CHAR* xpath = NULL;
    
    XMLDoc_init(&doc);
    XMLSearch_init(&search);
    XMLSearch_search_set_tag(&search, C2SX("softwaredb"));
    
    XMLSearch_init(&search1);
    XMLSearch_search_set_tag(&search1, C2SX("software"));
    
    XMLDoc_parse_buffer_DOM(p, NULL, &doc);
    debugNetPrintf(DEBUG,"[MSXORBIS] %s Start search '%s'\n",__FUNCTION__,XMLSearch_get_XPath_string(&search, &xpath, C2SX('\"')));     
    free(xpath);
    int numchild;
    nodesoftwaredb = XMLDoc_root(&doc); //doc.nodes[doc.i_root];
    
    
    if(XMLSearch_node_matches(nodesoftwaredb, &search))
    {
        //debugNetPrintf(DEBUG,"[MSXORBIS] %s gotcha! children %d %d %d\n",__FUNCTION__,nodesoftwaredb->n_children,nodesoftwaredb->init_value,XML_INIT_DONE);
        //debugNetPrintf(DEBUG,"[MSXORBIS] %s valid=%d\n",__FUNCTION__,XMLNode_is_valid(nodesoftwaredb));
        //debugNetPrintf(DEBUG,"[MSXORBIS] %s gotcha! children %d\n",__FUNCTION__,XMLNode_get_children_count(nodesoftwaredb));
        
        
        for(i=0;i<nodesoftwaredb->n_children;i++)
        {
            char system[13];
            char genmsxid[5];
            char title[256];
            char company[39];
            char country[3];
            char year[10];
            bool original;
            SXML_CHAR *value;
            bool flagGenmsxid=false;
            //debugNetPrintf(DEBUG,"[MSXORBIS] %s %s:%s\n",__FUNCTION__,node->attributes[0].name,node->attributes[0].value);
            
            childnodesoftware=XMLNode_get_child(nodesoftwaredb,i);
        //  debugNetPrintf(DEBUG,"[MSXORBIS] %s %d\n",__FUNCTION__,XMLNode_is_valid(childnodesoftware));
            
        //  debugNetPrintf(DEBUG,"[MSXORBIS] %s numchild=%d %d\n",__FUNCTION__,XMLNode_get_children_count(childnodesoftware),childnodesoftware->n_children);
            
            
            //if(XMLSearch_node_matches(node->children[i], &search1))
            if(XMLSearch_node_matches(childnodesoftware, &search1))
            {
                for(j=0;j<childnodesoftware->n_children;j++)
                {
                    childnode=XMLNode_get_child(childnodesoftware,j);
                    if(strcmp(childnode->tag,"system")==0)
                    {
                        sprintf(system,"%s",childnode->text?childnode->text:"");
                        //debugNetPrintf(DEBUG,"[MSXORBIS] %s system=%s %lu\n",__FUNCTION__,system,strlen(system));
                    }
                    if(strcmp(childnode->tag,"genmsxid")==0)
                    {
                        sprintf(genmsxid,"%s",childnode->text?childnode->text:"");
                        //debugNetPrintf(DEBUG,"[MSXORBIS] %s genmsxid=%s %lu\n",__FUNCTION__,genmsxid,strlen(genmsxid));
                        flagGenmsxid=true;
                    }
                    if(strcmp(childnode->tag,"title")==0)
                    {
                        sprintf(title,"%s",childnode->text?childnode->text:"");
                        //debugNetPrintf(DEBUG,"[MSXORBIS] %s title=%s %lu\n",__FUNCTION__,title,strlen(title));
                    }
                    if(strcmp(childnode->tag,"company")==0)
                    {
                        sprintf(company,"%s",childnode->text?childnode->text:"");
                        //debugNetPrintf(DEBUG,"[MSXORBIS] %s company=%s %lu\n",__FUNCTION__,company,strlen(company));
                    }
                    if(strcmp(childnode->tag,"country")==0)
                    {
                        sprintf(country,"%s",childnode->text?childnode->text:"");
                        //debugNetPrintf(DEBUG,"[MSXORBIS] %s country=%s %lu\n",__FUNCTION__,country,strlen(country));
                        
                    }
                    if(strcmp(childnode->tag,"year")==0)
                    {
                        
                        sprintf(year,"%s",childnode->text?childnode->text:"");
                        //debugNetPrintf(DEBUG,"[MSXORBIS] %s year=%s %lu\n",__FUNCTION__,year,strlen(year));
                        
                    }
                    if(strcmp(childnode->tag,"dump")==0)
                    {
                        char originalData[12];
                        
                        bool original=false;
                        bool flagOriginal=false;
                        //debugNetPrintf(DEBUG,"[MSXORBIS] %s %s numchild=%d\n",__FUNCTION__,childnode->tag,childnode->n_children);
                        for(k=0;k<childnode->n_children;k++)
                        {
                            
                            dumpchildnode=XMLNode_get_child(childnode,k);
                            if(strcmp(dumpchildnode->tag,"original")==0)
                            {
                                
                                //debugNetPrintf(DEBUG,"[MSXORBIS] %s %d\n",__FUNCTION__,XMLNode_get_attribute(dumpchildnode, C2SX("value"), &value));
                                
                                sprintf(originalData,"%s",dumpchildnode->text?dumpchildnode->text:"");
                                
                                //debugNetPrintf(DEBUG,"[MSXORBIS] %s originalData=%s %lu\n",__FUNCTION__,originalData,strlen(originalData));
                                original=strcmp(value,"true")==0;
                                //debugNetPrintf(DEBUG,"[MSXORBIS] %s originalData=%s original=%d %s %lu\n",__FUNCTION__,originalData,original,value,strlen(originalData));
                                
                                flagOriginal=true;
                                continue;
                            }
                            if(strcmp(dumpchildnode->tag,"group")==0)
                            {
                                for(l=0;l<dumpchildnode->n_children;l++)
                                {
                                    groupchildnode=XMLNode_get_child(dumpchildnode,l);
                                    if(strcmp(groupchildnode->tag,"original")==0)
                                    {
                                        XMLNode_get_attribute(groupchildnode, C2SX("value"), &value);
                                        sprintf(originalData,"%s",groupchildnode->text?groupchildnode->text:"");
                                        //debugNetPrintf(DEBUG,"[MSXORBIS] %s originalData=%s %lu\n",__FUNCTION__,originalData,strlen(originalData));
                                        original=strcmp(value,"true")==0;
                                        //debugNetPrintf(DEBUG,"[MSXORBIS] %s originalData=%s original=%d %lu\n",__FUNCTION__,originalData,original,strlen(originalData));
                                        flagOriginal=true;
                                        continue;
                                    }
                                    if(strcmp(groupchildnode->tag,"cas")==0 || strcmp(groupchildnode->tag,"dsk")==0)
                                    {
                                        mediaDbAddDump(groupchildnode,flagGenmsxid?genmsxid:"",system,title,company,country,year,flagOriginal?original:false,flagOriginal?originalData:"");
                                    }
                                }
                                continue;
                            }
                            mediaDbAddDump(dumpchildnode,flagGenmsxid?genmsxid:"",system,title,company,country,year,flagOriginal?original:false,flagOriginal?originalData:"");
                        }
                        
                        
                    }
                    
                    
                    //XMLNode_print(childnode, stdout, NULL, NULL, false, 0, 0);
                    //printf("\n-------\n");
                    
                }
                //printf("%d: numattr=%d\n",childnode->n_attributes);
                //XMLNode_print(node->children[i], stdout, NULL, NULL, false, 0, 0);
                //printf("%s:%s\n",childnode->attributes[0].name,childnode->attributes[0].value);
                
            }
            
            
        }
    }

    /*while ((node = XMLSearch_next(node, &search)) != NULL) {
        printf("Found match: ");
        XMLNode_print(node, stdout, NULL, NULL, false, 0, 0);
        printf("\n");
    }*/
    //XMLSearch_search_add_attribute(&search, "presence", NULL);
    //XMLSearch_search_add_attribute(&search, "value", "a*");
    debugNetPrintf(DEBUG,"[MSXORBIS] %s End search\n",__FUNCTION__);
    //XMLDoc_print(&doc, stdout, NULL, NULL, false, 0, 0);

    XMLSearch_free(&search, false);
    XMLSearch_free(&search1, false);

    free(p);
    XMLDoc_free(&doc);
}


MediaType* mediaDbGuessRom(const void *buffer,int size) 
{
    static MediaType staticMediaType;
    staticMediaType.romType=ROM_UNKNOWN;
    staticMediaType.genmsxid=0;
    staticMediaType.original=false;
    sprintf(staticMediaType.system,"%s","");
    sprintf(staticMediaType.title,"%s","Unknown MSX rom)");
    sprintf(staticMediaType.company,"%s","");
    sprintf(staticMediaType.country,"%s","");
    sprintf(staticMediaType.year,"%s","");
    sprintf(staticMediaType.remark,"%s","");
    sprintf(staticMediaType.originalData,"%s","");
    sprintf(staticMediaType.startData,"%s","");

    const UInt8* romData = (const UInt8*)buffer;
    int i;
    int mapper;
    UInt32 counters[7] = { 0, 0, 0, 0, 0, 0, 0 };

    staticMediaType.romType = romdbDefaultType;

    if (size < 128) {
        return &staticMediaType;
    }

    MediaType* mediaType = mediaDbLookupRom(buffer, size);
    if (mediaType == NULL) {
        mediaType = &staticMediaType;
    }
    debugNetPrintf(DEBUG,"[MSXORBIS] %s mediaDbLookupRom return %d\n",__FUNCTION__,mediaType->romType);

    if (mediaType->romType!=ROM_UNKNOWN) {
        return mediaType;
    }

    BoardType boardType = boardGetType();

    switch (boardType) {
    case BOARD_SVI:
        staticMediaType.romType = ROM_SVI328CART;
        return &staticMediaType;
    case BOARD_COLECO:
    case BOARD_COLECOADAM:
        staticMediaType.romType = ROM_COLECO;
        return &staticMediaType;
    case BOARD_SG1000:
        staticMediaType.romType = ROM_SG1000;
        return &staticMediaType;
    case BOARD_SC3000:
    case BOARD_SF7000:
        staticMediaType.romType = ROM_SC3000;
        return &staticMediaType;
    case BOARD_MSX_FORTE_II:
        break;
    case BOARD_MSX:
        break;
    }


    if (size <= 0x10000) {
        if (size == 0x10000) {
            if (romData[0x4000] == 'A' && romData[0x4001] == 'B') mediaType->romType = ROM_PLAIN;
            else mediaType->romType = ROM_ASCII16;
            return mediaType;
        } 
        
        if (size <= 0x4000 && romData[0] == 'A' && romData[1] == 'B') {
            UInt16 init = romData[2] + 256 * romData[3];
            UInt16 text = romData[8] + 256 * romData[9];
//          if (init == 0 && (text & 0xc000) == 0x8000) {
            if ((text & 0xc000) == 0x8000) {
                mediaType->romType = ROM_BASIC;
                return mediaType;
            }
        }
        mediaType->romType = ROM_PLAIN;
        return mediaType;
    }
    
    const char ManbowTag[] = "Mapper: Manbow 2";
    UInt32 tagLength = strlen(ManbowTag);
    for (i = 0; i < (int)(size - tagLength); i++) {
        if (romData[i] == ManbowTag[0]) {
            if (memcmp(romData + i, ManbowTag, tagLength) == 0) {
                mediaType->romType = ROM_MANBOW2;
                return mediaType;
            }
        }
    }

    /* Count occurences of characteristic addresses */
#if 1
    for (i = 0; i < size - 3; i++) {
        if (romData[i] == 0x32) {
            UInt32 value = (UInt32)romData[i + 1] + ((UInt32)romData[i + 2] << 8);
            switch(value) {
            case 0x4000: 
            case 0x8000: 
            case 0xa000: 
                counters[3]++;
                break;

            case 0x5000: 
            case 0x9000: 
            case 0xb000: 
                counters[2]++;
                break;

            case 0x6000: 
                counters[3]++;
                counters[4]++;
                counters[5]++;
                counters[6]++;
                break;

            case 0x6800: 
            case 0x7800: 
                counters[4]++;
                break;

            case 0x7000: 
                counters[2]++;
                counters[4]++;
                counters[5]++;
                counters[6]++;
                break;
            case 0x77ff: 
                counters[5]++;
                break;
            default:
                value = value & 0xf800;
                switch (value) {
                    case 0x6000:
                        counters[3]++;
                        counters[5]++;
                        counters[4]++;
                        break;
                    case 0x7000:
                        counters[2]++;
                        counters[5]++;
                        counters[4]++;
                        break;
                    case 0x6800:
                    case 0x7800:
                        counters[4]++;
                }
            }
        }
    }
    int kon4 = 0, kon5 = 0, asc8 = 0, asc16 = 0, kor126 = 0, b, w, c;
    for (i = 0; i < size - 3; i++) 
    {
        if (romData[i] == 0x32)
        {
            b = romData[i+2];
            w = romData[i+1] == 0 ? 1 : 0;
            c = 0;
            if (b == 0x50 || b == 0x90 || b == 0xb0)
                kon5+=w;
            else if (b == 0x40 || b == 0x80 || b == 0xa0)
                kon4+=w;
            else if (((b & 0xf8) == 0x68 || (b & 0xf8) == 0x78))
                asc8+=w;
            else if ((b == 0x60 || b == 0x70) && w == 0)
            {
                counters[6]++;
            }
            else if (b == 0x60)
            {
                kon4+=w;
                asc8+=w;
                asc16+=w;
            }
            else if (b == 0x70)
            {
                kon5+=w;
                asc8+=w;
                asc16+=w;
            }
            else if (romData[i+2] == 0x77 && romData[i+1] == 0xff)
                asc16+=w;
            else
                c = 1;
            //if (c == 0) printf("0x%08x, 0x%04x, %d, %d, %d, %d\n", i, romData[i+2]*256 + romData[i+1], kon5, kon4, asc8, asc16);
        }
    }
    if (asc8 > 0) asc8--;
    counters[2] += kon5;
    counters[3] += kon4;
    counters[4] += asc8;
    counters[5] += asc16;
#endif  

    /* Find which mapper type got more hits */
    mapper = 0;

    //if (counters[4] > 0)        counters[4]--;

    for (i = 0; i <= 5; i++) {
        if (counters[i] > 0 && counters[i] >= counters[mapper]) {
            mapper = i;
        }
    }

    if (mapper == 5 && counters[0] == counters[5]) {
        mapper = 0;
    }
    
    if (counters[6] == 12)
        mapper = 6;

    debugNetPrintf(DEBUG,"[MSXORBIS] %s mapper:%d, [%d],[%d],[%d],[%d],[%d],[%d],[%d]\n",__FUNCTION__,mapper, counters[0], counters[1], counters[2], counters[3], counters[4], counters[5], counters[6]);
    switch (mapper) {
    default:
    case 0: mediaType->romType = ROM_STANDARD; break;
    case 1: mediaType->romType = ROM_MSXDOS2; break;
    case 2: mediaType->romType = ROM_KONAMI5; break;
    case 3: mediaType->romType = ROM_KONAMI4; break;
    case 4: mediaType->romType = ROM_ASCII8SRAM; break;
    case 5: mediaType->romType = ROM_ASCII16SRAM; break;
    case 6: mediaType->romType = ROM_KOREAN126; break;
    }
    
    return mediaType;
}
const char* mediaDbGetPrettyString(MediaType* mediaType)
{
    static char prettyString[512];

    prettyString[0] = 0;

    if (mediaType != NULL) 
    {
        sprintf(prettyString,"%s - %s %s %s : %s [%s]",mediaType->title,mediaType->company,mediaType->year,mediaType->remark,mediaType->startData);
       
    }

    return prettyString;
}
MediaType* mediaDbLookupRom(const void *buffer, int size)
{
    if(romdb==NULL) 
    {
        return NULL;
    }
    MediaType* mediaType=mediaDbLookup(romdb,buffer,size);
    return mediaType;
}

MediaType* mediaDbLookupDisk(const void *buffer,int size)
{
    if(diskdb==NULL)
    {
        return NULL;
    }
    return mediaDbLookup(diskdb,buffer,size);  
}

MediaType* mediaDbLookupCas(const void *buffer,int size)
{
    if(casdb==NULL)
    {
        return NULL;
    }
    return mediaDbLookup(casdb,buffer,size);
}

void mediaDbCreateRomdb()
{
    if(romdb==NULL)
    {
        romdb=malloc(sizeof(MediaDb)*4000);
        if(romdb==NULL)
        {
            debugNetPrintf(ERROR,"[MSXORBIS] %s error in malloc\n",__FUNCTION__);
            return;
        }
        debugNetPrintf(DEBUG,"[MSXORBIS] %s romdb malloc done\n",__FUNCTION__);

    }
}
void mediaDbCreateDiskdb()
{
    if(diskdb==NULL)
    {
        //dskdb=malloc(sizeof(MediaDb)*4000);
        if(diskdb==NULL)
        {
            debugNetPrintf(ERROR,"[MSXORBIS] %s error in malloc\n",__FUNCTION__);
            return;
        }
        debugNetPrintf(DEBUG,"[MSXORBIS] %s diskdb malloc done\n",__FUNCTION__);


    }
}
void mediaDbCreateCasdb()
{
    if(casdb==NULL)
    {
        //casdb=malloc(sizeof(MediaDb)*4000);
        if(casdb==NULL)
        {
            debugNetPrintf(ERROR,"[MSXORBIS] %s error in malloc\n",__FUNCTION__);
            return;
        }
        debugNetPrintf(DEBUG,"[MSXORBIS] %s casdb malloc done\n",__FUNCTION__);

    }
}
void mediaDbLoad()
{
    
    mediaDbCreateRomdb();
    mediaDbCreateDiskdb();
    mediaDbCreateCasdb();
    debugNetPrintf(DEBUG,"[MSXORBIS] %s before load msxromdb\n",__FUNCTION__);

    mediaDbAddFromXmlFile("host0:app/msxorbis/Databases/msxromdb.xml");
    debugNetPrintf(ERROR,"[MSXORBIS] %s after load msxromdb\n",__FUNCTION__);

    mediaDbAddFromXmlFile("host0:app/msxorbis/Databases/msxsysromdb.xml");
    debugNetPrintf(ERROR,"[MSXORBIS] %s after load msxsysromdb\n",__FUNCTION__);

    //mediaDbAddFromXmlFile("host0:app/msxorbis/Databases/msxcaswavdb.xml");
    //mediaDbAddFromXmlFile("host0:app/msxorbis/Databases/msxdskdb.xml");
    
}

