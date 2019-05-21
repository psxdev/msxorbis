ifndef Ps4Sdk
ifdef ps4sdk
Ps4Sdk := $(ps4sdk)
endif
ifdef PS4SDK
Ps4Sdk := $(PS4SDK)
endif
ifndef Ps4Sdk
$(error Neither PS4SDK, Ps4Sdk nor ps4sdk set)
endif
endif

target ?= ps4_elf
TargetFile=homebrew.elf

include $(Ps4Sdk)/make/ps4sdk.mk
CompilerFlags += -DORBIS -DLSB_FIRST -DNO_ASM -DNO_HIRES_TIMERS -DNO_FILE_HISTORY -DNO_EMBEDDED_SAMPLES 
CompilerFlagsCpp +=  -std=c++11 -Wno-c++11-narrowing -fexceptions -frtti -DORBIS -DLSB_FIRST -DNO_ASM -DNO_HIRES_TIMERS -DNO_FILE_HISTORY -DNO_EMBEDDED_SAMPLES 
#CompilerFlagsCpp +=  -frtti -std=c++11 -Wno-write-strings -Wno-c++11-narrowing -DORBIS -DLSB_FIRST -DNO_ASM -DNO_HIRES_TIMERS -DNO_FILE_HISTORY -DNO_EMBEDDED_SAMPLES

LinkerFlags+= -lfreetype-gl -lfreetype -ldebugnet -lps4link -lorbisFile -lelfloader -lorbisKeyboard -lorbis2d -lpng -lz -lorbisGl -lorbisPad -lorbisAudio -lmod -lorbisAudio -lmod -lorbisFileBrowser -lorbisXbmFont -lsxmlc -lSceNet_stub -lScePigletv2VSH_stub -lSceSystemService_stub -lSceUserService_stub -lScePad_stub -lSceAudioOut_stub -lSceVideoOut_stub -lSceGnmDriver_stub -lSceIme_stub -lSceSysmodule_stub
IncludePath += -I$(Ps4Sdk)/include/freetype2 -I$(Ps4Sdk)/include/freetype-gl


