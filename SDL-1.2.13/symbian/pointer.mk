ZDIR=$(EPOCROOT)\epoc32\data\z
TARGETDIR=$(ZDIR)\resource\apps
TARGETFILE=$(TARGETDIR)\$(FNAME).mbm
SOURCE_PATH=./data
SOURCE=$(SOURCE_PATH)/pointer
FNAME=sdlexe
RESOURCE :
	bmconv /h$(EPOCROOT)\epoc32\include\$(FNAME).mbg \
	$(TARGETFILE) \
	/c16$(SOURCE).bmp /8$(SOURCE)-alpha.bmp


  
	    	
do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)
FINAL : do_nothing
	 
